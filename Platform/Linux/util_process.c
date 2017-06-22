#include "util_process.h"
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define PROC_NAME_LINE 1
#define PROC_PID_LINE 5
#define PROC_UID_LINE 8
#define BUFF_LEN 1024 
#define CPU_USAGE_INFO_LINE 1
#define MEM_TOTAL_LINE 1
#define MEM_FREE_LINE 2

typedef struct statstruct_proc {
  int           pid;                      /** The process id. **/
  char          exName [_POSIX_PATH_MAX]; /** The filename of the executable **/
  char          state; /** 1 **/          /** R is running, S is sleeping, 
			   D is sleeping in an uninterruptible wait,
			   Z is zombie, T is traced or stopped **/
  unsigned      euid,                      /** effective user id **/
                egid;                      /** effective group id */					     
  int           ppid;                     /** The pid of the parent. **/
  int           pgrp;                     /** The pgrp of the process. **/
  int           session;                  /** The session id of the process. **/
  int           tty;                      /** The tty the process uses **/
  int           tpgid;                    /** (too long) **/
  unsigned int	flags;                    /** The flags of the process. **/
  unsigned int	minflt;                   /** The number of minor faults **/
  unsigned int	cminflt;                  /** The number of minor faults with childs **/
  unsigned int	majflt;                   /** The number of major faults **/
  unsigned int  cmajflt;                  /** The number of major faults with childs **/
  int           utime;                    /** user mode jiffies **/
  int           stime;                    /** kernel mode jiffies **/
  int		cutime;                   /** user mode jiffies with childs **/
  int           cstime;                   /** kernel mode jiffies with childs **/
  int           counter;                  /** process's next timeslice **/
  int           priority;                 /** the standard nice value, plus fifteen **/
  unsigned int  timeout;                  /** The time in jiffies of the next timeout **/
  unsigned int  itrealvalue;              /** The time before the next SIGALRM is sent to the process **/
  int           starttime; /** 20 **/     /** Time the process started after system boot **/
  unsigned int  vsize;                    /** Virtual memory size **/
  unsigned int  rss;                      /** Resident Set Size **/
  unsigned int  rlim;                     /** Current limit in bytes on the rss **/
  unsigned int  startcode;                /** The address above which program text can run **/
  unsigned int	endcode;                  /** The address below which program text can run **/
  unsigned int  startstack;               /** The address of the start of the stack **/
  unsigned int  kstkesp;                  /** The current value of ESP **/
  unsigned int  kstkeip;                 /** The current value of EIP **/
  int		signal;                   /** The bitmap of pending signals **/
  int           blocked; /** 30 **/       /** The bitmap of blocked signals **/
  int           sigignore;                /** The bitmap of ignored signals **/
  int           sigcatch;                 /** The bitmap of catched signals **/
  unsigned int  wchan;  /** 33 **/        /** (too long) **/
  int		sched, 		  /** scheduler **/
                sched_priority;		  /** scheduler priority **/
		
} procinfo;

procinfo m_procInfo;

PROCESSENTRY32_NODE * m_curNode = NULL;

bool util_process_launch(char * appPath)
{
	bool bRet = true;
    	pid_t pid = fork();
	if ( 0 == pid ) {/* Child process */
		exit(execlp("/bin/sh", "sh", "-c", appPath, NULL));
	} else if (pid < 0){ /* fork() failed */
        	bRet = false;    
	}
	return bRet;
}

HANDLE util_process_cmd_launch(char * cmdline)
{
	if(cmdline == NULL) return 0;
	pid_t pid = fork();
	if (0 == pid) /* Child process */
	{
		execlp("/bin/sh", "sh", "-c", cmdline, NULL);
	}
	if (-1 == pid)
		return pid;//Means failed -- like 'FALSE'
	else
	{
		int status;
		waitpid(pid, &status, 0);             /* Wait for the child to terminate. */
		return pid;
	}
}

HANDLE util_process_cmd_launch_no_wait(char * cmdline)
{
	if(cmdline == NULL) return 0;
	pid_t pid = fork();
	if (0 == pid) /* Child process */
	{
		execlp("/bin/sh", "sh", "-c", cmdline, NULL);
	}
	return pid;
}

WISEPLATFORM_API bool util_is_process_running(HANDLE hProcess)
{
	if (-1 == hProcess)
		return false;
	else
	{
		int result = kill(hProcess, 0);
		return result==0;
	}
}

void util_process_wait_handle(HANDLE hProcess)
{
	if (-1 == hProcess)
		return;
	else
	{
		int status;
		waitpid(hProcess, &status, 0);             /* Wait for the child to terminate. */
	}
}

void util_process_kill_handle(HANDLE hProcess)
{
	if (-1 == hProcess)
		return;
	else
	{
		int status;
		kill(hProcess, SIGABRT);
		wait(&status);             /* Wait for the child to terminate. */
	}
}

unsigned long getPIDByName(char * prcName)
{
	FILE *fopen = NULL;
	unsigned int pid = 0;	
	char buf[16];
	char cmdLine[256];
	sprintf(cmdLine,"pidof -s %s",prcName);
	if((fopen = popen(cmdLine,"r"))==NULL)
		return 0;
	if(!fgets(buf,sizeof(buf),fopen))
	{
		pclose(fopen);
		return 0;
	}
	sscanf(buf,"%ud",&pid);
	pclose(fopen);
	return pid;
}

bool GetSysLogonUserName(char * userNameBuf, unsigned int bufLen)
{
	int i = 0;
	FILE * fp = NULL;
	char cmdline[128];
	char cmdbuf[12][32]={{0}};

	if (userNameBuf == NULL || bufLen == 0) return false;

	sprintf(cmdline,"last|grep still");//for opensusi kde desktop
	fp = popen(cmdline,"r");
	if(NULL != fp)
	{
		char buf[512]={0};
		if (fgets(buf, sizeof(buf), fp))
    		{
        		sscanf(buf,"%31s",cmdbuf[0]);
		}
	}
	
    	pclose(fp);

	i = strlen(cmdbuf[0]);
	if(i>0 && i< bufLen)
		strcpy(userNameBuf, cmdbuf[0]);
	else 
		return false;
	return true;
}

bool util_process_as_user_launch(char * cmdLine, bool isAppNameRun, bool isShowWindow, unsigned long * newPrcID)
{
	char logonUserName[32] = {0};
	if(!cmdLine) return false;
	if(GetSysLogonUserName(logonUserName, sizeof(logonUserName)))
	{
		FILE *fp = NULL;
		char cmdBuf[256] = {0};
		//sprintf(cmdBuf,"su - %s -c %s &",logonUserName,cmdLine);
		//sprintf(cmdBuf,"DISPLAY=:0 su -c %s %s &",cmdLine,logonUserName);
		sprintf(cmdBuf,"DISPLAY=:0 su -c 'xterm -e /bin/bash -c '%s'' %s &",cmdLine,logonUserName);
		if((fp=popen(cmdBuf,"r"))==NULL)
		{
			//printf("restart process failed,%s",cmdBuf);
			pclose(fp);
			return false;	
		}
		pclose(fp);
	}
	if(newPrcID != NULL)
		*newPrcID = getPIDByName(cmdLine);
	return true;
}

bool util_process_kill(char * processName)
{
	pid_t p;
	size_t i, j;
	char* s = (char*)malloc(264);
	char buf[128];
	FILE* st;
	DIR* d = opendir("/proc");
	if (d == NULL) { free(s); return false; }
	struct dirent* f;
	while ((f = readdir(d)) != NULL) {
		if (f->d_name[0] == '.') continue;
		for (i = 0; isdigit(f->d_name[i]); i++);
		if (i < strlen(f->d_name)) continue;
		strcpy(s, "/proc/");
		strcat(s, f->d_name);
		strcat(s, "/status");
		st = fopen(s, "r");
		if (st == NULL) { closedir(d); free(s); return false; }
		do {
			if (fgets(buf, 128, st) == NULL) { fclose(st); closedir(d); free(s); return -1; }
		} while (strncmp(buf, "Name:", 5));
		fclose(st);
		for (j = 5; isspace(buf[j]); j++);
		*strchr(buf, '\n') = 0;
		if (!strcmp(&(buf[j]), processName)) {
			sscanf(&(s[6]), "%d", &p);
			kill(p, SIGKILL);
		}
	}
	closedir(d);
	free(s);
	return true;
}

int util_process_id_get(void)
{
   return getpid();
}

bool util_process_username_get(HANDLE hProcess, char * userNameBuf, int bufLen)
{
	if(!hProcess) return false;
	if (m_curNode == NULL || m_curNode->prcMonInfo.th32ProcessID != hProcess) return false;
	
	struct passwd *pw;
	pw = getpwuid(m_curNode->prcMonInfo.dwUID);
	if(!pw) return false;
	unsigned int strLength = strlen((char *)(pw->pw_name));
	if(strLength >= bufLen) return false;
	strcpy(userNameBuf,(char *)(pw->pw_name));
	return true;

}

bool util_process_check(char * processName)
{
	bool isFind = false;
	if (processName && strlen(processName) > 0)
	{
		FILE *fd = NULL;
		char buf[BUFSIZ];

		sprintf(buf, "ps -ely | grep %s | grep -v grep", processName);
		fd = popen(buf, "r");
		while (fgets(buf, sizeof(buf), fd))
		{
			if (strstr(buf, processName) && !strstr(buf, "<defunct>"))//not a zombie process
			{
				isFind = true;
				break;
			}
		}
		pclose(fd);
	}
	return isFind;
}

bool util_process_get_logon_users(char * logonUserList, int *logonUserCnt ,int maxLogonUserCnt,int maxLogonUserNameLen)
{
	FILE * fp = NULL;
	char cmdline[128];
	if (logonUserList == NULL || logonUserCnt == NULL) return false;
	*logonUserCnt = 0;
	sprintf(cmdline,"last|grep still");
	fp = popen(cmdline,"r");
	if(NULL != fp){
	    char buf[512] = {0};
		while (fgets(buf, sizeof(buf), fp))
    		{
			int i = 0;
			char cmdbuf[12][32]={{0}};
			bool isAddIn = false;
        	sscanf(buf,"%31s",cmdbuf[0]);
			for (i = 0; i < *logonUserCnt && i< maxLogonUserCnt; i++)
			{
				if(!strcmp(&(logonUserList[maxLogonUserNameLen * i]),cmdbuf[0]))
				{
					isAddIn = true;
					break;
				}
			}
			if(!isAddIn)
			{
				if(*logonUserCnt < maxLogonUserCnt)
				{
					strcpy(&(logonUserList[maxLogonUserNameLen * (*logonUserCnt)]),cmdbuf[0]);
					(*logonUserCnt)++;
				}
			}
			
		}
	}
    	pclose(fp);
//Wei.gang add to debug
//	int i = 0;
//	for (i = 0; i< *logonUserCnt; i++)
//		printf("[common] app_os_GetSysLogonUserList. get user %d: %s\n",i, &(logonUserList[maxLogonUserNameLen * i]));
//Wei.Gang add end
	return true;
}
/*
bool read_line(FILE* fp,char* buff,int b_l,int l)
{
    char line_buff[b_l];
    int i;

    if (!fp)
        return false;
    
    for (i = 0; i < l-1; i++)
    {
        if (!fgets (line_buff, sizeof(line_buff), fp))
        {
            return false;
        }
    }

    if (!fgets (line_buff, sizeof(line_buff), fp))
    {
        return false;
    }

    memcpy(buff,line_buff,b_l);

    return true;
}
*/
bool util_process_read_proc(PROCESSENTRY32* info,const char* c_pid)
{
    FILE* fp = NULL;
    char file[512] = {0};
    char line_buff[BUFF_LEN] = {0};
    char name[32] = {0};
    unsigned long dwID_tmp = 0;    

    sprintf(file,"/proc/%s/status",c_pid);
    if (!(fp = fopen(file,"r")))
    {
        return false;
    }
    if (read_line(fp,line_buff,BUFF_LEN,PROC_NAME_LINE))
    {
        sscanf(line_buff,"%31s %259s",name,(info->szExeFile));
    }
    else
    {
        fclose(fp);
	return false;
    }

    fseek(fp,0,SEEK_SET);
    if (read_line(fp,line_buff,BUFF_LEN,PROC_PID_LINE))
    {
        sscanf(line_buff,"%31s %lu",name,&dwID_tmp);
		if(!strcmp("Pid:",name))
			info->th32ProcessID = dwID_tmp;
		else
		{
			fseek(fp,0,SEEK_SET);
			if (read_line(fp,line_buff,BUFF_LEN,PROC_PID_LINE-1))
			{
				sscanf(line_buff,"%31s %lu",name,&dwID_tmp);
				if(!strcmp("Pid:",name))
					info->th32ProcessID = dwID_tmp;
				else
				{
					fclose(fp);
					return false;
				}
			}
		}
    }
    else
    {
        fclose(fp);
	return false;
    }

    dwID_tmp = 0;
    fseek(fp,0,SEEK_SET);
    if (read_line(fp,line_buff,BUFF_LEN,PROC_UID_LINE))
    {
        sscanf(line_buff,"%31s %lu",name,&dwID_tmp);
		if(!strcmp("Uid:",name))
			info->dwUID = dwID_tmp;
		else
		{
			fseek(fp,0,SEEK_SET);
			if (read_line(fp,line_buff,BUFF_LEN,PROC_UID_LINE-1))
			{
				sscanf(line_buff,"%31s %lu",name,&dwID_tmp);
				if(!strcmp("Uid:",name))
					info->dwUID = dwID_tmp;
				else
				{
					fclose(fp);
					return false;
				}
			}
		}
    }
    else
    {
        fclose(fp);
	return false;
    }

    fclose(fp);
    return true;
}

HANDLE util_process_create_Toolhelp32Snapshot(unsigned long dwFlags, unsigned long th32ProcessID)
{
	PROCESSENTRY32_NODE * headNodeSnapshot = NULL;
	PROCESSENTRY32_NODE * curInfoNode = NULL;
	PROCESSENTRY32_NODE * prcInfoNode = NULL;
    	DIR *dir;
    	struct dirent *ptr;
	if (!(dir = opendir("/proc")))
        	return 0;
	headNodeSnapshot = (PROCESSENTRY32_NODE *)malloc(sizeof(PROCESSENTRY32_NODE));
	memset(headNodeSnapshot, 0, sizeof(PROCESSENTRY32_NODE));
	curInfoNode = headNodeSnapshot;

    	while (ptr = readdir(dir))
    	{
        	if (ptr->d_name[0] > '0' && ptr->d_name[0] <= '9')
        	{
			prcInfoNode = (PROCESSENTRY32_NODE *)malloc(sizeof(PROCESSENTRY32_NODE));
			memset(prcInfoNode, 0, sizeof(PROCESSENTRY32_NODE));
            		if(!util_process_read_proc(&(prcInfoNode->prcMonInfo),ptr->d_name))
			{
				free(prcInfoNode);
				continue;
			}
			curInfoNode->next = prcInfoNode;
			curInfoNode = prcInfoNode;
		//printf("[ProcessMonitorHandler] proc name %s pid %d uid %d.\n",prcInfoNode->prcMonInfo.szExeFile, prcInfoNode->prcMonInfo.th32ProcessID, prcInfoNode->prcMonInfo.dwUID);
        	}
    	}
	curInfoNode = headNodeSnapshot;
	closedir(dir);
	return (HANDLE)headNodeSnapshot;
}

bool util_process_close_Toolhelp32Snapshot_handle(HANDLE hSnapshot)
{
	if(!hSnapshot) return false;
	PROCESSENTRY32_NODE * headInfoNode = (PROCESSENTRY32_NODE *)hSnapshot;
	PROCESSENTRY32_NODE * curInfoNode = NULL;
	PROCESSENTRY32_NODE * nextInfoNode = NULL;
	curInfoNode = headInfoNode->next;
	while(curInfoNode != NULL)
	{
		nextInfoNode = curInfoNode->next;
		free(curInfoNode);
		curInfoNode = nextInfoNode;
	}
	free(headInfoNode);
	headInfoNode = NULL;
	m_curNode = NULL;
	return true;
}

bool util_process_Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
	if(!hSnapshot) return false;
	PROCESSENTRY32_NODE * headInfoNode = (PROCESSENTRY32_NODE *)hSnapshot;
	m_curNode=headInfoNode->next;
	if(m_curNode != NULL)
		memcpy(lppe, &(m_curNode->prcMonInfo), sizeof(PROCESSENTRY32));
	else
		return false;
	//printf("[ProcessMonitorHandler]0 proc name %s pid %d\n",lppe->szExeFile, lppe->th32ProcessID);
	//lppe = &(curNode->prcMonInfo);
	return true;
}

bool util_process_Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
	//printf("7\n");
	if(!hSnapshot) return false;
	m_curNode=m_curNode->next;
	if(m_curNode != NULL)
		memcpy(lppe, &(m_curNode->prcMonInfo), sizeof(PROCESSENTRY32));
	else
		return false;
	//printf("[ProcessMonitorHandler]1 proc name %s pid %d\n",m_curNode->prcMonInfo.szExeFile, m_curNode->prcMonInfo.th32ProcessID);
	//printf("[ProcessMonitorHandler]2 proc name %s pid %d\n",lppe->szExeFile, lppe->th32ProcessID);
	//lppe = &(m_curNode->prcMonInfo);
	return true;
}

bool util_process_GlobalMemoryStatusEx(LPMEMORYSTATUSEX lpBuffer)
{
	char proc_pic_path[128]={0};
	FILE * fp = NULL;
	if(!lpBuffer) return false;
	sprintf(proc_pic_path,"/proc/meminfo");
	
	fp = fopen(proc_pic_path,"r");
	if(NULL != fp)
	{
		char buf[256]={0};
		if (read_line(fp,buf,sizeof(buf),MEM_TOTAL_LINE))
    		{
			char name[32]={0};
        		sscanf(buf,"%s %llu",name,&(lpBuffer->ullTotalPhys));
    		} 
		fseek(fp,0,SEEK_SET);
		if (read_line(fp,buf,sizeof(buf),MEM_FREE_LINE))
    		{
			char name[32]={0};
        		sscanf(buf,"%s %llu",name,&(lpBuffer->ullAvailPhys));
    		} 
	}
    	fclose(fp);
	return true;
}

bool util_process_GetSystemTimes(LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
{
	char proc_pic_path[128]={0};
	FILE * fp = NULL;

	if(!lpIdleTime || !lpKernelTime || !lpUserTime) return false;
	sprintf(proc_pic_path,"/proc/stat");
	fp = fopen(proc_pic_path,"r");
	if(NULL != fp)
	{
		char buf[256]={0};
		if (read_line(fp,buf,sizeof(buf),CPU_USAGE_INFO_LINE))
    		{
			char name[32]={0};
			long long niceTime;
        		sscanf(buf,"%s %lld %lld %lld %lld",name,lpUserTime,&niceTime,lpKernelTime,lpIdleTime);
			//printf("[app_os_GetSystemTime] name:%s, utime:%lld, NTime:%lld, ktime:%lld, itime:%lld.\n",name,*lpUserTime,niceTime,*lpKernelTime,*lpIdleTime);
			//*(long long *)lpUserTime =*(long long *)lpUserTime + niceTime;
    		} 
	}
	fclose(fp);
	return true;
}

static int get_proc_info(pid_t pid, procinfo * pinfo)
{
  char szFileName [_POSIX_PATH_MAX],
    szStatStr [2048],
    *s, *t;
  FILE *fp;
  struct stat st;
  
  if (NULL == pinfo) {
    errno = EINVAL;
    return -1;
  }

  sprintf (szFileName, "/proc/%u/stat", (unsigned) pid);
  
  if (-1 == access (szFileName, R_OK)) {
    return (pinfo->pid = -1);
  } /** if **/

  if (-1 != stat (szFileName, &st)) {
  	pinfo->euid = st.st_uid;
  	pinfo->egid = st.st_gid;
  } else {
  	pinfo->euid = pinfo->egid = -1;
  }
  
  
  if ((fp = fopen (szFileName, "r")) == NULL) {
    return (pinfo->pid = -1);
  } /** IF_NULL **/
  
  if ((s = fgets (szStatStr, 2048, fp)) == NULL) {
    fclose (fp);
    return (pinfo->pid = -1);
  }

  /** pid **/
  sscanf (szStatStr, "%u", &(pinfo->pid));
  s = strchr (szStatStr, '(') + 1;
  t = strchr (szStatStr, ')');
  strncpy (pinfo->exName, s, t - s);
  pinfo->exName [t - s] = '\0';
  
  sscanf (t + 2, "%c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
	  /*       1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33*/
	  &(pinfo->state),
	  &(pinfo->ppid),
	  &(pinfo->pgrp),
	  &(pinfo->session),
	  &(pinfo->tty),
	  &(pinfo->tpgid),
	  &(pinfo->flags),
	  &(pinfo->minflt),
	  &(pinfo->cminflt),
	  &(pinfo->majflt),
	  &(pinfo->cmajflt),
	  &(pinfo->utime),
	  &(pinfo->stime),
	  &(pinfo->cutime),
	  &(pinfo->cstime),
	  &(pinfo->counter),
	  &(pinfo->priority),
	  &(pinfo->timeout),
	  &(pinfo->itrealvalue),
	  &(pinfo->starttime),
	  &(pinfo->vsize),
	  &(pinfo->rss),
	  &(pinfo->rlim),
	  &(pinfo->startcode),
	  &(pinfo->endcode),
	  &(pinfo->startstack),
	  &(pinfo->kstkesp),
	  &(pinfo->kstkeip),
	  &(pinfo->signal),
	  &(pinfo->blocked),
	  &(pinfo->sigignore),
	  &(pinfo->sigcatch),
	  &(pinfo->wchan));
  fclose (fp);
  return 0;
}

bool util_process_GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
{
	if(!hProcess || !lpCreationTime || !lpKernelTime || !lpUserTime) return false;
	//procinfo procInfo;
	int ret = get_proc_info(hProcess, &m_procInfo);
	if(ret < 0) return false;
	//printf("[app_os_GetProcessTimes]  procid:%d,realid%d, utime: %d, stime: %d, start_time: %d\n",hPRocess, m_procInfo.pid ,m_procInfo.utime, m_procInfo.stime, m_procInfo.starttime);
	
	*((long long *)lpCreationTime) = m_procInfo.starttime;
	*((long long *)lpKernelTime) = m_procInfo.stime;
	*((long long *)lpUserTime) = m_procInfo.utime;
	return true;
}

void util_process_GetSystemInfo(LPSYSTEM_INFO lpSystemInfo)
{
	if (lpSystemInfo == NULL) return; 
	lpSystemInfo->dwNumberOfProcessors = sysconf(_SC_NPROCESSORS_CONF);
	return;
}

