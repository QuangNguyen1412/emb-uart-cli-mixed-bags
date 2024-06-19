// script that take a filepath as input, creates a directory on file system for each key under object iNodes and iNodes

// import libraries for file system
const fs = require('fs');
const path = require('path');
// import libraries for cli arguments
const argv = require('minimist')(process.argv.slice(2));
const SECOND_NODEID_POSTFIX = "-2";
const FIRST_NODEID_POSTFIX = "-1";
const OTA_IMAGE_POSTFIX_BIN = ".bin";
const OTA_IMAGE_POSTFIX_ZIP = ".zip";
const NODE_ID_PREFIX = "N-";
const OUTPUT_DIR_NAME = "s3-firmware-images";
const WMU_IMAGE_PREFIX = "wmu-v";
const PCU_IMAGE_PREFIX = "y6g-pcu-offset.v";
const BMS_IMAGE_PREFIX = "y6g-bms-hp-offset-0x3000-v";
const MPPT_IMAGE_PREFIX = "y6g-mppt-offset-0x3000-v";
const INV_LV_IMAGE_PREFIX = "y6g-inv-lv-120v-offset-0x3800-v";
const INV_HV_IMAGE_PREFIX = "y6g-inv-hv-120v-standalone-v";
const INV_120V_DIR = "INV_120V";

// a function that takes a file name as input and returns true if the file name ends with ".bin" or ".zip"
function isBinOrZipFile(file) {
  return file.endsWith(OTA_IMAGE_POSTFIX_BIN) || file.endsWith(OTA_IMAGE_POSTFIX_ZIP);
}

// Check to see if user has provided a HostId and firmware directory
if (argv._.length < 2) {
  console.log('Usage: node s3-firmware-images.js <Host ID> <firmware directory>');
  process.exit(1);
}
// get the first cli argument as a file path
const hostId = argv._[0];
const fwDir = argv._[1];
const outputDir = path.join(fwDir, OUTPUT_DIR_NAME);
const HOSTID_DIR_PATH = path.join(fwDir, OUTPUT_DIR_NAME, hostId);

// parse host id "hyphen separated" to a dictionary that has sn as the second key, pcuBoardId as the third key, bmsBoardId as the fourth key, mpptBoardId as the fifth key, and invBoardId as the sixth key
const hostIdDict = hostId.split('-');
// get the second key of hostIdDict as sn
const sn = hostIdDict[1];
// get the third key of hostIdDict as pcuBoardId
const pcuWmuBoardId = hostIdDict[2];
// get the fourth key of hostIdDict as bmsBoardId
const bmsBoardId = hostIdDict[3];
// get the fifth key of hostIdDict as mpptBoardId
const mpptBoardId = hostIdDict[4];
// get the sixth key of hostIdDict as invBoardId
const invBoardId = hostIdDict[5];

//create a directory even if it already exists
fs.mkdirSync(path.join(fwDir, OUTPUT_DIR_NAME), { recursive: true });
fs.mkdirSync(HOSTID_DIR_PATH, { recursive: true });
const PCU_DIR_NAME = NODE_ID_PREFIX + sn + "-" + pcuWmuBoardId + FIRST_NODEID_POSTFIX;
const WMU_DIR_NAME = NODE_ID_PREFIX + sn + "-" + pcuWmuBoardId + SECOND_NODEID_POSTFIX;
const BMS_DIR_NAME = NODE_ID_PREFIX + sn + "-" + bmsBoardId + FIRST_NODEID_POSTFIX;
const MPPT_DIR_NAME = NODE_ID_PREFIX + sn + "-" + mpptBoardId + FIRST_NODEID_POSTFIX;
const INV_LV_DIR_NAME = NODE_ID_PREFIX + sn + "-" + invBoardId + FIRST_NODEID_POSTFIX;
const INV_HV_DIR_NAME = NODE_ID_PREFIX + sn + "-" + invBoardId + SECOND_NODEID_POSTFIX;
fs.mkdirSync(path.join(outputDir, PCU_DIR_NAME), { recursive: true }); // PCU "N-37500-A20-1"
fs.mkdirSync(path.join(outputDir, WMU_DIR_NAME), { recursive: true }); // WMU "N-37500-A20-2"
fs.mkdirSync(path.join(outputDir, BMS_DIR_NAME), { recursive: true }); // BMS "N-37500-B2-1"
fs.mkdirSync(path.join(outputDir, MPPT_DIR_NAME), { recursive: true }); // MPPT "N-37500-C20-1"
fs.mkdirSync(path.join(outputDir, INV_LV_DIR_NAME), { recursive: true }); // INV "N-37500-D30-1"
fs.mkdirSync(path.join(outputDir, INV_HV_DIR_NAME), { recursive: true }); // INV "N-37500-D30-2"
// for each file in the outputDir directory, print out the file name that ends with ".bin" or ".zip"
fs.readdirSync(fwDir, {withFileTypes: true}).forEach(fsDir => {
  const file = fsDir.name;
  if (isBinOrZipFile(file)) {
    if (file.startsWith(WMU_IMAGE_PREFIX)) {
      // extract version from file name that start with the prefix "wmu-v" and ends with ".", then separate each digit by "."
      const version = file.substring(WMU_IMAGE_PREFIX.length, file.indexOf("."));
      var versionString = version[0] + "." + version[1] + "." + version[2];
      console.log("WMU version: ", versionString);
      const WMU_DIR_PATH = path.join(outputDir, WMU_DIR_NAME);
      const DEST_FILE_NAME = versionString + ".zip";
      // copy this file to a WMU_DIR_PATH directory, destination file named DEST_FILE_NAME
      fs.copyFileSync(path.join(fwDir, file), path.join(WMU_DIR_PATH, DEST_FILE_NAME));
    } else if (file.startsWith(PCU_IMAGE_PREFIX)) {
      // extract version from file name that start with the prefix "pcu-v" and ends with ".", then separate each digit by "."
      const version = file.substring(PCU_IMAGE_PREFIX.length, file.indexOf(".", PCU_IMAGE_PREFIX.length));
      var versionString = version[0] + "." + version[1] + "." + version[2];
      console.log("PCU version: ", versionString);
      const PCU_DIR_PATH = path.join(outputDir, PCU_DIR_NAME);
      const DEST_FILE_NAME = versionString + ".bin";
      // copy this file to a PCU_DIR_PATH directory, destination file named DEST_FILE_NAME
      fs.copyFileSync(path.join(fwDir, file), path.join(PCU_DIR_PATH, DEST_FILE_NAME));
    } else if (file.startsWith(BMS_IMAGE_PREFIX)) {
      const version = file.substring(BMS_IMAGE_PREFIX.length, file.indexOf(".", BMS_IMAGE_PREFIX.length));
      var versionString = version[0] + "." + version[1] + "." + version[2];
      console.log("BMS version: ", versionString);
      const BMS_DIR_PATH = path.join(outputDir, BMS_DIR_NAME);
      const DEST_FILE_NAME = versionString + ".bin";
      // copy this file to a BMS_DIR_PATH directory, destination file named DEST_FILE_NAME
      fs.copyFileSync(path.join(fwDir, file), path.join(BMS_DIR_PATH, DEST_FILE_NAME));
    } else if (file.startsWith(MPPT_IMAGE_PREFIX)) {
      const version = file.substring(MPPT_IMAGE_PREFIX.length, file.indexOf(".", MPPT_IMAGE_PREFIX.length));
      var versionString = version[0] + "." + version[1] + "." + version[2];
      console.log("MPPT version: ", versionString);
      const MPPT_DIR_PATH = path.join(outputDir, MPPT_DIR_NAME);
      const DEST_FILE_NAME = versionString + ".bin";
      // copy this file to a MPPT_DIR_PATH directory, destination file named DEST_FILE_NAME
      fs.copyFileSync(path.join(fwDir, file), path.join(MPPT_DIR_PATH, DEST_FILE_NAME));
    }
  } else if (file == INV_120V_DIR) {
    fs.readdirSync(fwDir + "/" + file).forEach(invFile => {
      if (isBinOrZipFile(invFile)) {
        if (invFile.startsWith(INV_LV_IMAGE_PREFIX)) {
          // get version from inverter file
          const version = invFile.substring(INV_LV_IMAGE_PREFIX.length, invFile.indexOf(".", INV_LV_IMAGE_PREFIX.length));
          var versionString = version[0] + "." + version[1] + "." + version[2];
          console.log("INV LV version: ", versionString);
          const INVLV_DIR_PATH = path.join(outputDir, INV_LV_DIR_NAME);
          const DEST_FILE_NAME = versionString + ".bin";
          // copy this file to a INVLV_DIR_PATH directory, destination file named DEST_FILE_NAME
          fs.copyFileSync(path.join(fwDir, file, invFile), path.join(INVLV_DIR_PATH, DEST_FILE_NAME));
        } else if (invFile.startsWith(INV_HV_IMAGE_PREFIX)) {
          // get version from inverter file
          const version = invFile.substring(INV_HV_IMAGE_PREFIX.length, invFile.indexOf(".", INV_HV_IMAGE_PREFIX.length));
          var versionString = version[0] + "." + version[1] + "." + version[2];
          console.log("INV HV version: ", versionString);
          const INVHV_DIR_PATH = path.join(outputDir, INV_HV_DIR_NAME);
          const DEST_FILE_NAME = versionString + ".bin";
          // copy this file to a INVHV_DIR_PATH directory, destination file named DEST_FILE_NAME
          fs.copyFileSync(path.join(fwDir, file, invFile), path.join(INVHV_DIR_PATH, DEST_FILE_NAME));
        }
      }
    });
  }
});
