#include "util_process.h"
#include <Windows.h>
#include <process.h>
#include <string.h>
#include <stdio.h>

WISEPLATFORM_API bool util_process_launch(char * appPath)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	if(NULL == appPath) return -1;

	memset(&si, 0, sizeof(si));
	si.dwFlags = STARTF_USESHOWWINDOW;  
	si.wShowWindow = SW_HIDE;
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));

	if(CreateProcess(appPath, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW , NULL, NULL, &si, &pi))
	{
		return true;
	}
	else
	{
		return false;
	}
}

WISEPLATFORM_API HANDLE util_process_cmd_launch(char * cmdline)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	if(cmdline == NULL) return NULL;
	memset(&si, 0, sizeof(si));
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	if(CreateProcess(NULL, ( char*)cmdline, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		//CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	return pi.hProcess;
}

WISEPLATFORM_API HANDLE util_process_cmd_launch_no_wait(char * cmdline)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	if(cmdline == NULL) return NULL;
	memset(&si, 0, sizeof(si));
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	if(CreateProcess(NULL, ( char*)cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		return pi.hProcess;
	}
	else
	{
		return NULL;
	}
}

WISEPLATFORM_API bool util_is_process_running(HANDLE hProcess)
{
	unsigned long long ret = 0;
	if(hProcess == NULL)
		return false;

	ret = WaitForSingleObject(hProcess, 0);
    return ret == WAIT_TIMEOUT;
}

WISEPLATFORM_API void util_process_wait_handle(HANDLE hProcess)
{
	if(hProcess == NULL)
		return;
	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);
}

WISEPLATFORM_API void util_process_kill_handle(HANDLE hProcess)
{
	if(hProcess == NULL)
		return;
	TerminateProcess(hProcess, 0);    //asynchronous
	WaitForSingleObject(hProcess, 5000);
	CloseHandle(hProcess);
}

bool GetTokenByName(HANDLE * hToken, char * prcName)
{
	bool bRet = false;
	HANDLE hPrc = NULL;
	PROCESSENTRY32 pe;
	HANDLE hSnapshot=NULL;
	if(NULL == prcName || NULL == hToken) return bRet;
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
		return bRet;
	while(true)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==false)
			break;
		if(_stricmp(pe.szExeFile, prcName)==0)
		{	
			hPrc = OpenProcess(PROCESS_QUERY_INFORMATION, false, pe.th32ProcessID);
			bRet = OpenProcessToken(hPrc,TOKEN_ALL_ACCESS,hToken);
			CloseHandle(hPrc);
			break;
		}
	}
	if(hSnapshot) CloseHandle(hSnapshot);
	return bRet;
}

WISEPLATFORM_API bool util_process_as_user_launch(char * cmdLine, bool isAppNameRun, bool isShowWindow, unsigned long * newPrcID)
{
	bool bRet = false;
	if(NULL == cmdLine) return bRet;
	{
		HANDLE hToken;
		if(!GetTokenByName(&hToken,"EXPLORER.EXE"))
		{
			return bRet;
		}
		else
		{
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			DWORD dwCreateFlag = CREATE_NO_WINDOW;
			memset(&si, 0, sizeof(si));
			si.dwFlags = STARTF_USESHOWWINDOW; 
			si.wShowWindow = SW_HIDE;
			if(isShowWindow)
			{
				si.wShowWindow = SW_SHOW;
				dwCreateFlag = CREATE_NEW_CONSOLE;
			}
			si.cb = sizeof(si);
			memset(&pi, 0, sizeof(pi));
			if(isAppNameRun)
			{
				bRet = CreateProcessAsUserA(hToken, cmdLine, NULL,  NULL ,NULL,
					false, dwCreateFlag, NULL, NULL, &si, &pi);
			}
			else
			{
				bRet = CreateProcessAsUserA(hToken, NULL, cmdLine, NULL ,NULL,
					false, dwCreateFlag, NULL, NULL, &si, &pi);
			}

			if(!bRet)
			{
				printf("error code: %s  %d\n", cmdLine, GetLastError());
			}
			else
			{
				if(newPrcID != NULL) *newPrcID = pi.dwProcessId;
			}
			CloseHandle(hToken);
		}
	}
	return bRet;
}

bool adjust_privileges() 
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize=sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		if (GetLastError()==ERROR_CALL_NOT_IMPLEMENTED) return true;
		else return false;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) 
	{
		CloseHandle(hToken);
		return false;
	}

	memset(&tp, 0, sizeof(tp));
	tp.PrivilegeCount=1;
	tp.Privileges[0].Luid=luid;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) 
	{
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);
	return true;
}

WISEPLATFORM_API bool util_process_kill(char * processName)
{
	bool bRet = false;
	PROCESSENTRY32 pe;
	HANDLE hSnapshot=NULL;
	if(NULL == processName) return bRet;
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
	{
		CloseHandle(hSnapshot);
		return bRet;
	}
	while(true)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==false)
			break;
		if(strcmp(pe.szExeFile,processName)==0)
		{
			HANDLE hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
			if(hPrc == NULL) 
			{
				DWORD dwRet = GetLastError();          
				if(dwRet == 5)
				{
					if(adjust_privileges())
					{
						hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
					}
				}
			}

			if(hPrc)
			{
				TerminateProcess(hPrc, 0);    //asynchronous
				WaitForSingleObject(hPrc, 5000);
				bRet = true;
				CloseHandle(hPrc);
			}

			break;
		}
	}
	CloseHandle(hSnapshot);
	return bRet;
}

WISEPLATFORM_API int util_process_id_get(void)
{
	return _getpid();
}

WISEPLATFORM_API bool util_process_username_get(HANDLE hProcess, char * userNameBuf, int bufLen)
{
	bool bRet = false;
	if(hProcess == NULL || userNameBuf == NULL || bufLen <= 0) return bRet;
	{
		HANDLE hToken;
		if (OpenProcessToken(hProcess,TOKEN_QUERY,&hToken))
		{
			DWORD dwNeedLen = 0;
			GetTokenInformation(hToken, TokenUser, NULL, 0, &dwNeedLen);
			if(dwNeedLen > 0)
			{
				TOKEN_USER * pTokenUser = (TOKEN_USER *)malloc(dwNeedLen);
				if(GetTokenInformation(hToken, TokenUser, pTokenUser, dwNeedLen, &dwNeedLen))
				{
					SID_NAME_USE sn;
					char szDomainName[MAX_PATH] = {0};
					DWORD dwDmLen = MAX_PATH;
					if(LookupAccountSid(NULL, pTokenUser->User.Sid, userNameBuf, (LPDWORD)&bufLen, szDomainName, &dwDmLen, &sn))
					{
						bRet = true;
					}
				}
				if(pTokenUser)
				{
					free(pTokenUser);
					pTokenUser = NULL;
				}
			}
			CloseHandle(hToken);
		}
	}

	return bRet;
}

WISEPLATFORM_API bool util_process_check(char * processName)
{
	bool bRet = false;
	PROCESSENTRY32 pe;
	HANDLE hSnapshot=NULL;
	if(NULL == processName) return bRet;
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
	{
		CloseHandle(hSnapshot);
		return bRet;
	}
	while(true)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==false)
			break;
		if(_stricmp(pe.szExeFile,processName)==0)
		{
			bRet = true;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return bRet;
}

bool util_process_adjust_privileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize=sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		if (GetLastError()==ERROR_CALL_NOT_IMPLEMENTED) return true;
		else return false;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) 
	{
		CloseHandle(hToken);
		return false;
	}

	memset(&tp, 0, sizeof(tp));
	tp.PrivilegeCount=1;
	tp.Privileges[0].Luid=luid;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, false, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) 
	{
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);
	return true;
}

void util_process_get_handles(HANDLE *eplHandleList, char *processName, int *logonUserCnt , int maxLogonUserCnt)
{
	HANDLE hPrc = NULL;
	PROCESSENTRY32 pe;
	HANDLE hSnapshot=NULL;
	if(NULL == eplHandleList || NULL == processName || NULL == logonUserCnt) return ;
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
		return ;
	*logonUserCnt = 0;
	while(TRUE)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==FALSE)
			break;
		if(strcmp(pe.szExeFile, processName)==0)
		{
			hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);

			if(hPrc == NULL) 
			{
				DWORD dwRet = GetLastError();          
				if(dwRet == 5)
				{
					if(util_process_adjust_privileges())
					{
						hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
					}
				}
			}
			if(*logonUserCnt < maxLogonUserCnt)
			{
				eplHandleList[*logonUserCnt] = hPrc;
				(*logonUserCnt)++;
			}
			//break;
		}
	}
	if(hSnapshot) CloseHandle(hSnapshot);
	//return hPrc;
}


WISEPLATFORM_API bool util_process_get_logon_users(char * logonUserList, int *logonUserCnt ,int maxLogonUserCnt,int maxLogonUserNameLen)
{
	if(logonUserList == NULL || logonUserCnt == NULL) return false;
	{
		//HANDLE eplHandleList[maxLogonUserCnt] = {0};
		HANDLE *eplHandleList = (HANDLE *)malloc(sizeof(HANDLE) * maxLogonUserCnt);
		memset(eplHandleList,0,sizeof(HANDLE) * maxLogonUserCnt);
		util_process_get_handles(eplHandleList,"explorer.exe", logonUserCnt ,maxLogonUserCnt);
		if(*logonUserCnt > 0)
		{
			int i = 0;
			for(i = 0; i< *logonUserCnt; i++)
			{
				/*if(app_os_GetProcessUsername(eplHandleList[i], logonUserList[i], maxLogonUserNameLen))
				{
					continue
				}*/
				util_process_username_get(eplHandleList[i], &(logonUserList[i*maxLogonUserNameLen]), maxLogonUserNameLen);
				if(eplHandleList[i]) CloseHandle(eplHandleList[i]);
			}
		}
		free(eplHandleList);
		//if(eplHandle) app_os_CloseHandle(eplHandle);
	}
	return true;
}

WISEPLATFORM_API HANDLE util_process_create_Toolhelp32Snapshot(unsigned long dwFlags, unsigned long th32ProcessID)
{
	return CreateToolhelp32Snapshot(dwFlags,th32ProcessID);
}

WISEPLATFORM_API bool util_process_close_Toolhelp32Snapshot_handle(HANDLE hSnapshot)
{
	return CloseHandle(hSnapshot);
}

WISEPLATFORM_API bool util_process_Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
	return Process32First(hSnapshot, lppe);
}

WISEPLATFORM_API bool util_process_Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
	return Process32Next(hSnapshot, lppe);
}

WISEPLATFORM_API bool util_process_GlobalMemoryStatusEx(__out LPMEMORYSTATUSEX lpBuffer)
{
	return GlobalMemoryStatusEx(lpBuffer);
}

WISEPLATFORM_API bool util_process_GetSystemTimes(__out_opt LPFILETIME lpIdleTime, __out_opt LPFILETIME lpKernelTime, __out_opt LPFILETIME lpUserTime)
{
	return GetSystemTimes(lpIdleTime,lpKernelTime,lpUserTime);
}

WISEPLATFORM_API bool util_process_GetProcessTimes(__in  HANDLE hProcess, __out LPFILETIME lpCreationTime, __out LPFILETIME lpExitTime, __out LPFILETIME lpKernelTime, __out LPFILETIME lpUserTime)
{
	return GetProcessTimes(hProcess,lpCreationTime,lpExitTime,lpKernelTime,lpUserTime);
}

WISEPLATFORM_API void util_process_GetSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
	GetSystemInfo(lpSystemInfo);
}
