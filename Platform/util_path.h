#ifndef _UTIL_PATH_H
#define _UTIL_PATH_H 

#include <stdbool.h>
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#define MAX_PATH	260
#endif

WISEPLATFORM_API void util_split_path_file(char const *filepath, char* path, char* file);

WISEPLATFORM_API void util_path_combine(char* destination, const char* path1, const char* path2);

WISEPLATFORM_API int util_module_path_get(char * moudlePath);

WISEPLATFORM_API bool util_create_directory(char* path);

WISEPLATFORM_API unsigned long util_temp_path_get(char* lpBuffer, int nBufferLength);

WISEPLATFORM_API bool util_is_file_exist(char const *filepath);

WISEPLATFORM_API void util_remove_file(char const *filepath);

WISEPLATFORM_API bool util_copy_file(char const *srcFilepath, char const *destFilepath);

WISEPLATFORM_API long util_file_size_get(char const *filepath);

WISEPLATFORM_API long util_file_read(char const *filepath, char* buff, long length);

#ifdef __cplusplus
}
#endif


#endif
