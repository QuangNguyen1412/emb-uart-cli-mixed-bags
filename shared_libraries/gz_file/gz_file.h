/** @file gz_file.h
* 
* @brief helpers for file handling
*
* Authors:
* - Quang Nguyen
*
* @par
* COPYRIGHT NOTICE: (c) 2023 Goal Zero.
* All rights reserved.
*/

#ifndef GZ_FILE_H
#define GZ_FILE_H

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>
#include "gz_std_types.h"
#include <stdio.h>

typedef struct {
  FILE *fp;
  const char *path;
} GZFile;

/**
 * @brief check for file existence
 * @return true exist
 * @return false non-exist
 * **/
bool gz_file_exists(const char* filePath);

/**
 * @brief delete file with a given path
 * @return true successfully deleted
 * @return false non-exist or fail to delete
 * **/
bool gz_file_delete(const char* filePath);

/**
 * @brief returns file size
 * **/
uint32_t gz_file_size(const char* filePath);

/**
 * @brief move 'filePath' to 'fileDes'
 * **/
void gz_file_mv(const char* fileDes, const char* filePath);

#ifndef CY8C6116BZI_F54
/**
 * @brief Like strstr but for file names. Calls your callback and passes the file name for each file whose
 * name contains the target string.
 * 
 * @param targetString Optional. The sub-string you wish to search file name matches somewhere in the file name (including the extension). 
 * If NULL, files can only match purely based on the fileExtension.
 * @param searchDir Optional. The path to the directory you wish to search. If NULL, the directory shall be "."
 * @param fileExtension Optional. The file extension you wish to match. If NULL, matches are not limited to the end of the file name (Eg. my_bin.txt would match search for bin).
 * @param fileFound_cb Optional. A callback function that gets called for each matching file found. 
 * The file name is passed to your callback. Can be NULL.
 * @return int The number of files found with matching names
 */
int gz_file_search(const char* targetString, const char* searchDir, const char* fileExtension, v_fp_c_chp_t fileFound_cb);

/**
 * @brief Opens a file as GZFile
 * @param path The path to the file to open
 * @param mode The mode to open the file in. See fopen for details
 * @return GZFile* A pointer to the GZFile struct. NULL if the file could not be opened
 * WARNING: Caller must free the allocated pointer!
 */
GZFile * gzfopen(const char *path, const char *mode);

/**
 * @brief Closes a GZFile
 * @param gzfp The GZFile to close
 * @return int 0 if the file was closed successfully. -1 if the file could not be closed
*/
int gzfclose(GZFile *gzfp);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GZ_FILE_H */
