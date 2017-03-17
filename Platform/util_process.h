#ifndef _UTIL_PROCESS_H
#define _UTIL_PROCESS_H 

#include <stdbool.h>
#include "export.h"
#ifdef WIN32
#include <tlhelp32.h>
#else
#ifndef HANDLE
typedef long HANDLE;
#endif

typedef struct _SYSTEM_INFO{
	unsigned long dwNumberOfProcessors;
}SYSTEM_INFO, *LPSYSTEM_INFO;

//FILETIME is jiffies
typedef struct _FILETIME{
	unsigned long dwLowDateTIme;
	unsigned long dwHighDateTime;
}FILETIME, *PFILETIME,*LPFILETIME;

typedef struct tagPROCESSENTRY32
{
	unsigned long dwSize;
	unsigned long th32ProcessID;
	unsigned long dwUID;
	char szExeFile[260];
}PROCESSENTRY32, *PPROCESSENTRY32, *LPPROCESSENTRY32;

typedef struct PROCESSENTRY32_NODE
{
	PROCESSENTRY32 prcMonInfo;
	struct PROCESSENTRY32_NODE * next;
}PROCESSENTRY32_NODE, *PPROCESSENTRY32_NODE;

typedef struct _MEMORYSTATUSEX {
	unsigned long dwLength;
	unsigned long long ullTotalPhys;
	unsigned long long ullAvailPhys;
}MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
#endif

#ifdef __cplusplus
extern "C" {
#endif

WISEPLATFORM_API bool util_process_launch(char * appPath);

WISEPLATFORM_API HANDLE util_process_cmd_launch(char * cmdline);

WISEPLATFORM_API HANDLE util_process_cmd_launch_no_wait(char * cmdline);

WISEPLATFORM_API void util_process_wait_handle(HANDLE hProcess);

WISEPLATFORM_API void util_process_kill_handle(HANDLE hProcess);

WISEPLATFORM_API bool util_process_as_user_launch(char * cmdLine, bool isAppNameRun, bool isShowWindow, unsigned long * newPrcID);

WISEPLATFORM_API bool util_process_kill(char * processName);

WISEPLATFORM_API int util_process_id_get(void);

WISEPLATFORM_API bool util_process_username_get(HANDLE hProcess, char * userNameBuf, int bufLen); 

WISEPLATFORM_API bool util_process_check(char * processName);

WISEPLATFORM_API bool util_process_get_logon_users(char * logonUserList, int *logonUserCnt ,int maxLogonUserCnt,int maxLogonUserNameLen);

WISEPLATFORM_API HANDLE util_process_create_Toolhelp32Snapshot(unsigned long dwFlags, unsigned long th32ProcessID);

WISEPLATFORM_API bool util_process_close_Toolhelp32Snapshot_handle(HANDLE hSnapshot);

WISEPLATFORM_API bool util_process_Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);

WISEPLATFORM_API bool util_process_Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);

WISEPLATFORM_API bool util_process_GlobalMemoryStatusEx(LPMEMORYSTATUSEX lpBuffer);

WISEPLATFORM_API bool util_process_GetSystemTimes(LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime);

WISEPLATFORM_API bool util_process_GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime);

WISEPLATFORM_API void util_process_GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);

#ifdef __cplusplus
}
#endif

#endif
