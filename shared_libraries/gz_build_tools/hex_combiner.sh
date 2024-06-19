#!/bin/bash
set -e

# NOTE: srecord is required for this script to run. Run sudo apt install srecord, sudo brew install srecord,
# or choco install srecord before running.
# NOTE: crc32 is required for this script to run. On Windows, you will need to install crc32 executable in
# your PATH before running.
# srec_cat command examples page http://srecord.sourceforge.net/man/man1/srec_examples.html

# WARNING: If the bootloader and application .hex file names are swapped, the computed crc32 will be different.
# This is because the Start Liner Address record (05) at the end of the .hex files for the the bootloader and
# hex files are different and srec_cat uses the start address of the first .hex file passed to it. We always
# want to use the bootloader starting address, so ensure that the -b option is always set to the bootloader
# .hex file.

############################################################
# Help                                                     #
############################################################
Help()
{
   # Display Help
   echo
   echo "Combines two hex files into one and appends the crc32 to the filename."
   echo
   echo "Syntax: ./hex_combiner.sh [-h|-b] bootloader.hex -a application.hex -o outfile"
   echo "options:"
   echo "a     Name of one of the application hex file"
   echo "b     Name of one of the bootloader file"
   echo "h     Print this Help."
   echo "l     Length of the rows in the output file (optional, default is 43)"
   echo "o     Name of the output file with extension and without the crc32 appended."
   echo "d     If this is mppt an offset.bin will be generated"
   echo "c     Name of the image we want to have the checksum calculated"
   echo
}

############################################################
############################################################
# Main program                                             #
############################################################
############################################################

# Set variables
echo
# Get the path that this script is located in, not where it is executed from as pwd alone provides
script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
if [[ $OSTYPE == 'darwin'* ]]; then
  echo "MacOS detected..."
  os="MacOS"
  os_deps_dir="/usr/bin/"
  srec_cat_path="/opt/homebrew/bin/srec_cat"
elif [[ $OSTYPE == 'linux'* ]]; then
  echo "Linux detected..."
  os="Linux"
  os_deps_dir="/usr/bin/"
  srec_cat_path="/usr/bin/srec_cat"
else
  echo "Windows detected..."
  os="Windows"
  os_deps_dir="${script_dir}/deps/windows/"
  srec_cat_path="${os_deps_dir}srec_cat"
fi

bootloader=""
application=""
outfile=""
linelen="43"
device=""
offset=""
flash_end=""
image=""

mppt_offset="0x6000"
mppt_flash_end="0x2BDFC"
pcu_combined_truncated_flash_offset="0x1007FE00"
pcu_combined_truncated_flash_end="0x14000000"

############################################################
# Process the input options. Add options as needed.        #
############################################################
# function defines
append_crc () {
  local crc=$(${os_deps_dir}crc32 $1)

  if [[ $os == "Windows" ]]; then
    # Strip the 0x and the file size from the output
    crc=${crc:2:8}
    # Convert the hexadecimal string to lowercase
    crc="${crc,,}"
  fi

  # Remove the file extension from the filename
  new_outfile=$(echo "$1" | sed -e 's/\.[^.]*$//')
  file_extension=$(echo "$1" | sed 's/.*\.//')
  # Append the crc32 and .hex to the output filename
  new_outfile="${new_outfile}.${crc}.${file_extension}"

  # Rename the output filename
  cp $1 $new_outfile

  echo
  echo "The combined output filename is ${new_outfile}"
  echo
}

# Get the options
while getopts ":a:b:hl:o:d:c:" option; do
  case $option in
  a) # application .hex file
    application="$OPTARG";;
  b) # bootloader .hex file
    bootloader="$OPTARG";;
  h) # display Help
    Help
    exit 0;;
  l) # outfile line length
    linelen="$OPTARG";;
  o) # output file
    outfile="$OPTARG";;
  d)
    device="$OPTARG";;  
  c)
    image="$OPTARG";;
  \?) # Invalid option
    echo "Error: Invalid option."
    Help
    exit -1;;
  esac
done

# If no output filename was passed, bail
if [ -z "${outfile}" ]; then
  Help
  exit -1
fi

if [ "${image}" ]; then
  echo
  echo "Calculating the checksum and append it to the output file"
  mv $image $outfile
  append_crc $outfile
  rm $outfile
  exit 0
fi

#if the input filenames were not passed, bail
if [ -z "${application}" ]; then
  Help
  exit -1
fi
if [ -z "${bootloader}" ]; then
  Help
  exit -1
fi
echo
echo "Creating catenated app hex file..." 
echo "$srec_cat_path $application -Intel -exclude -within $bootloader -o app_cat.hex"
$srec_cat_path $application -Intel -exclude -within $bootloader -Intel -o app_cat.hex -Intel

#only create offset.bin if mppt
if [ "${device}" == "mppt" ]; then
  exit_before_truncated="true"
  offset=$mppt_offset
  flash_end=$mppt_flash_end
  echo
  echo "Creating offset app binary file..."
  echo "$srec_cat_path app_cat.hex -Intel -crop $offset $flash_end -offset -$offset -o offset.bin -binary" 
  $srec_cat_path app_cat.hex -Intel -crop $offset $flash_end -offset -$offset -o offset.bin -binary
fi

echo
echo "Combining bootloader and catenated app hex files..." 
echo "$srec_cat_path $bootloader -Intel app_cat.hex -Intel -output $outfile -Intel -line-length=$linelen"
$srec_cat_path $bootloader -Intel app_cat.hex -Intel -output $outfile -Intel -line-length=$linelen

if [[ $os == "Windows" ]]; then
  # Convert the dos line endings to unix line endings
  $(${os_deps_dir}dos2unix $outfile &>/dev/null)
fi

append_crc $outfile

if [ "${exit_before_truncated}" == "true" ]; then
  exit 0
fi

echo "Generating truncated combined file that preserves manufacturing data..." 
# Remove the file extension from the filename
truncated_combined_output=$(echo "$outfile" | sed -e 's/\.[^.]*$//')
file_extension=$(echo "$outfile" | sed 's/.*\.//')
# Annotate that this file preserves manufacturing data
truncated_combined_output="${truncated_combined_output}.preserve-mfg-data.${file_extension}"
echo "$srec_cat_path $outfile -Intel -exclude $pcu_combined_truncated_flash_offset $pcu_combined_truncated_flash_end -output $truncated_combined_output -Intel -line-length=$linelen"
$srec_cat_path $outfile -Intel -exclude $pcu_combined_truncated_flash_offset $pcu_combined_truncated_flash_end -output $truncated_combined_output -Intel -line-length=$linelen

if [[ $os == "Windows" ]]; then
  # Convert the dos line endings to unix line endings
  $(${os_deps_dir}dos2unix $truncated_combined_output &>/dev/null)
fi

append_crc $truncated_combined_output

# Cleanup the temporary files
rm $outfile
rm $truncated_combined_output