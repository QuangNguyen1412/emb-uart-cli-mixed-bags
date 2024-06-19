/** @file gz_file.c
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef CY8C6116BZI_F54
#include <dirent.h>
#endif
#include <string.h>
#include "gz_file.h"

static bool _fileExtPresent(const char* fileName, const char* fileExtension) {
  if (fileName && fileExtension) {
    size_t nameLength = strlen(fileName);
    size_t extLength = strlen(fileExtension);

    if (nameLength >= extLength) {
      const char *extension = fileName + (nameLength - extLength);
      if (strcmp(extension, fileExtension) == 0) {
        return true;
      }
    }
  }
  return false;
}

bool gz_file_exists(const char* filePath) {
  if (!filePath) return false;
  FILE *file = fopen(filePath, "r");
  if (file == NULL) {
    return false;
  } else {
    fclose(file);
    return true;
  }
}

bool gz_file_delete(const char* filePath) {
  if (filePath) {
    return (remove(filePath) == 0);
  }
  return false;
}

uint32_t gz_file_size(const char* filePath) {
  uint32_t fileSize = 0;
  int fd = open(filePath, O_RDONLY);
  if (fd < 0) {
    return fileSize;
  }
  struct stat finfo;
  if(!fstat(fd, &finfo)) {
    fileSize = finfo.st_size;
  }
  close(fd);
  return fileSize;
}

#ifndef CY8C6116BZI_F54
int gz_file_search(const char* targetString, const char* searchDir, const char* fileExtension, v_fp_c_chp_t fileFound_cb) {
  int rv = 0;
  char* subString = NULL;
  if (targetString == NULL && fileExtension == NULL) {
    return rv;
  }

  if (searchDir == NULL) {
    searchDir = ".";
  }

  DIR *dir = opendir(searchDir);
  if (dir == NULL) {
    return rv;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    // Most file systems use DT_REG for regular files and this should normally be checked, but mgos LFS uses DT_UNKNOWN (0).
    // if (entry->d_type == DT_REG) {}
    if (targetString) {
      if (fileExtension) { // target string and file extension specified must be present to match
        if (_fileExtPresent(entry->d_name, fileExtension)) {
          if (strstr(entry->d_name, targetString)) {
            if (fileFound_cb && *fileFound_cb) {
              fileFound_cb(entry->d_name);
            }
            rv++;
            // printf("Found: %s, Type: %d\n", entry->d_name, entry->d_type);
          }
        } 
      } else { // only target string specified must be present to match
        if (strstr(entry->d_name, targetString)) {
          if (fileFound_cb && *fileFound_cb) {
            fileFound_cb(entry->d_name);
          }
          rv++;
          // printf("Found: %s, Type: %d\n", entry->d_name, entry->d_type);
        }
      } 
    } else if (fileExtension) { // only file extension specified must be present to match
      if (_fileExtPresent(entry->d_name, fileExtension)) {
        if (fileFound_cb && *fileFound_cb) {
          fileFound_cb(entry->d_name);
        }
        rv++;
        // printf("Found: %s, Type: %d\n", entry->d_name, entry->d_type);
      }
    }
  }

  closedir(dir);
  return rv;
}

GZFile * gzfopen(const char *path, const char *mode) {
  FILE *fp = fopen(path, mode);
  if (fp == NULL) {
    return NULL;
  }
  GZFile *gzfile = malloc(sizeof(GZFile));
  if (gzfile) {
    gzfile->fp = fp;
    gzfile->path = path;
  }
  return gzfile;
}

int gzfclose(GZFile *gzfp) {
  int r = EOF;
  if (gzfp == NULL) {
    return r;
  }
  if (gzfp->fp != NULL) {
    r = fclose(gzfp->fp);
    if (r != 0) {
      return r;
    }
  }
  gzfp->fp = NULL;
  gzfp->path = NULL;
  free(gzfp);
  return 0;
}

#endif