#include "util_path.h"
#include <windows.h>
#include <stdio.h>
#include  <io.h>

WISEPLATFORM_API void util_split_path_file(char const *filepath, char* path, char* file) 
{
	const char *slash = filepath, *next;
	while ((next = strpbrk(slash + 1, "\\/"))) slash = next;
	if (filepath != slash) slash++;
	strncpy(path, filepath, slash - filepath);
	strcpy(file, slash);
}

WISEPLATFORM_API void util_path_combine(char* destination, const char* path1, const char* path2)
{
	if(path1 == NULL && path2 == NULL) {
		strcpy(destination, "");
	}
	else if(path2 == NULL || strlen(path2) == 0) {
		strcpy(destination, path1);
	}
	else if(path1 == NULL || strlen(path1) == 0) {
		strcpy(destination, path2);
	} 
	else {
		char directory_separator[] = {FILE_SEPARATOR, 0};
		const char *last_char, *temp = path1;
		const char *skip_char = path2;
		int append_directory_separator = 0;

		while(*temp != '\0')
		{
			last_char = temp;
			temp++;        
		}

		if(strcmp(last_char, directory_separator) != 0) {
			append_directory_separator = 1;
		}
		strncpy(destination, path1, strlen(path1));
		if(append_directory_separator)
		{
			if(strncmp(path2, directory_separator, strlen(directory_separator)) != 0)
				strncat(destination, directory_separator, strlen(directory_separator));
		}
		else
		{
			if(*skip_char == FILE_SEPARATOR)
				skip_char++;   
		}
		strncat(destination, skip_char, strlen(skip_char));
	}
}

WISEPLATFORM_API int util_module_path_get(char * moudlePath)
{
	int iRet = 0;
	char * lastSlash = NULL;
	char tempPath[MAX_PATH] = {0};
	if(NULL == moudlePath) return iRet;
	if(ERROR_SUCCESS != GetModuleFileName(NULL, tempPath, sizeof(tempPath)))
	{
		lastSlash = strrchr(tempPath, FILE_SEPARATOR);
		if(NULL != lastSlash)
		{
			strncpy(moudlePath, tempPath, lastSlash - tempPath + 1);
			iRet = lastSlash - tempPath + 1;
		}
	}
	return iRet;
}

WISEPLATFORM_API bool util_create_directory(char* path)
{
	return CreateDirectory(path,NULL)==TRUE?true:false;
}

WISEPLATFORM_API unsigned long util_temp_path_get(char* lpBuffer, int nBufferLength)
{
	return GetTempPath (nBufferLength, lpBuffer);
}

WISEPLATFORM_API bool util_is_file_exist(char const *filepath)
{
	if(NULL == filepath) return false;
	return _access(filepath, 0) == 0 ? true : false;
}

WISEPLATFORM_API void util_remove_file(char const *filepath)
{
	if(filepath && strlen(filepath))
	{
		remove(filepath);
	}
}

WISEPLATFORM_API bool util_copy_file(char const *srcFilepath, char const *destFilepath)
{
	if(srcFilepath == NULL || destFilepath == NULL)
		return false;
	return CopyFile(srcFilepath, destFilepath, false)?true:false;	 
}

WISEPLATFORM_API long util_file_size_get(char const *filepath)
{
	long size = 0;
	FILE *fp = NULL;
	fp = fopen(filepath, "r");
	if(fp == NULL)
		return size;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	return size;
}

WISEPLATFORM_API long util_file_read(char const *filepath, char* buff, long length)
{
	long size = 0;
	FILE *fp = NULL;
	fp = fopen(filepath, "r");
	if(fp == NULL)
		return size;
	size = fread(buff, length, 1, fp);
	return size;
}