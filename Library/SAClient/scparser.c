#include "scparser.h"
#include <stdlib.h>
#include <cJSON.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "WISEPlatform.h" //for strdup wrapping
#include "util_string.h"
#include "sys/time.h"

#define BASICINFO_BODY_STRUCT	"susiCommData"
#define BASICINFO_HANDLERNAME	"handlerName"
#define BASICINFO_CMDTYPE		"commCmd"
#define BASICINFO_AGENTID		"agentID"
#define BASICINFO_TIMESTAMP		"sendTS"
#define BASICINFO_CONTENT		"content"
/*
#define AGENTINFO_DEVID			"devID"
#define AGENTINFO_HOSTNAME		"hostname"
#define AGENTINFO_SN			"sn"
#define AGENTINFO_MAC			"mac"
#define AGENTINFO_VERSION		"version"
#define AGENTINFO_TYPE			"type"
#define AGENTINFO_PRODUCT		"product"
#define AGENTINFO_MANUFACTURE	"manufacture"
#define AGENTINFO_STATUS		"status"
#define AGENTINFO_USERNAME		"account"
#define AGENTINFO_PASSWORD		"passwd"

#define GLOBAL_OS_INFO                   "osInfo"
#define GLOBAL_OS_VERSION                "osVersion"
#define GLOBAL_AGENT_VERSION             "cagentVersion"
#define GLOBAL_AGENT_TYPE             "cagentType"
#define GLOBAL_BIOS_VERSION              "biosVersion"
#define GLOBAL_PLATFORM_NAME             "platformName"
#define GLOBAL_PROCESSOR_NAME            "processorName"
#define GLOBAL_OS_ARCH                   "osArch"
#define GLOBAL_SYS_TOTAL_PHYS_MEM        "totalPhysMemKB"
#define GLOBAL_SYS_MACS					 "macs"
#define GLOBAL_SYS_IP					 "IP"

#define AGNET_INFO_CMD				1 
#define glb_get_init_info_rep		116
*/
#ifdef WIN32
	#if _MSC_VER < 1900
struct timespec {
	time_t   tv_sec;        /* seconds */
	long     tv_nsec;       /* nanoseconds */
};
	#else 
#include <time.h> // VS2015 or WIN_IOT
	#endif // WIN_IOT
#endif

char *scparser_utf8toansi(const char* str)
{
	int len = 0;
	char *strOutput = NULL;
	if(!IsUTF8(str))
	{
		len = strlen(str)+1;
		strOutput = (char *)malloc(len);
		memcpy(strOutput, str, len);
		
	}
	else
	{
		char * tempStr=UTF8ToANSI(str);
		len = strlen(tempStr)+1;
		strOutput = (char *)malloc(len);
		memcpy(strOutput, tempStr, len);
		free(tempStr);
		tempStr = NULL;
	}
	return strOutput;	
}

char * scparser_ansitoutf8(char* wText)
{
	char * utf8RetStr = NULL;
	int tmpLen = 0;
	if(!wText)
		return utf8RetStr;
	if(!IsUTF8(wText))
	{
		utf8RetStr = ANSIToUTF8(wText);
		tmpLen = !utf8RetStr ? 0 : strlen(utf8RetStr);
		if(tmpLen == 1)
		{
			if(utf8RetStr) free(utf8RetStr);
			utf8RetStr = UnicodeToUTF8((wchar_t *)wText);
		}
	}
	else
	{
		tmpLen = strlen(wText)+1;
		utf8RetStr = (char *)malloc(tmpLen);
		memcpy(utf8RetStr, wText, tmpLen);
	}
	return utf8RetStr;
}

char *scparser_print(PJSON item)
{
	if(item == NULL)
		return NULL;
	return cJSON_Print(item);
}

char *scparser_unformatted_print(PJSON item)
{
	if(item == NULL)
		return NULL;
	return cJSON_PrintUnformatted(item);
}

void scparser_free(PJSON ptr)
{
	cJSON *pAgentInfo = ptr;
	cJSON_Delete(pAgentInfo);
}

PJSON scparser_packet_create(susiaccess_packet_body_t const * pPacket)
{
	/*
{"commCmd":271,"requestID":103, XXX}
	*/
   cJSON* root = NULL;
   cJSON* content = NULL;
   cJSON* pReqInfoHead = NULL;
   cJSON* datetime = NULL;
   long long tick = 0;

   if(!pPacket) return NULL;
   if(pPacket->content)
   {
	   char* strContent = scparser_ansitoutf8(pPacket->content);
	   content = cJSON_Parse(strContent);
	   free(strContent);
	   strContent = NULL;
   }
   if(pPacket->type == pkt_type_custom)
	   return content;

   if(!content)
	   content = cJSON_CreateObject();
   
   if(pPacket->type == pkt_type_susiaccess)
   {
	   root = cJSON_CreateObject();
	   cJSON_AddItemToObject(root, BASICINFO_BODY_STRUCT, content);
	   pReqInfoHead = content;
   }
   else
   {
	   root = cJSON_CreateObject();
	   cJSON_AddItemToObject(root, BASICINFO_CONTENT, content);
	   pReqInfoHead = root;
   }

   cJSON_AddNumberToObject(pReqInfoHead, BASICINFO_CMDTYPE, pPacket->cmd);

   cJSON_AddStringToObject(pReqInfoHead, BASICINFO_AGENTID, pPacket->devId);
   cJSON_AddStringToObject(pReqInfoHead, BASICINFO_HANDLERNAME, pPacket->handlerName);

   {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		tick = (long long)tv.tv_sec*1000 + (long long)tv.tv_usec/1000;
   }
   datetime = cJSON_CreateObject();
   cJSON_AddItemToObject(pReqInfoHead, "sendTS", datetime);
   cJSON_AddNumberToObject(datetime, "$date", tick);

   return root;
}

char* scparser_packet_print(susiaccess_packet_body_t const * pPacket)
{
	PJSON pPacketJSON = NULL;
	char* pPayload = NULL;

	pPacketJSON = scparser_packet_create(pPacket);

	pPayload = scparser_unformatted_print(pPacketJSON);
	
	scparser_free(pPacketJSON);

	return pPayload;
}

int scparser_message_parse(const void* data, int datalen, susiaccess_packet_body_t * pkt)
{
	/*{"susiCommData":{"commCmd":251,"catalogID":4,"requestID":10}}*/
	//char* strInput = NULL;
	cJSON* root = NULL;
	cJSON* body = NULL;
	cJSON* target = NULL;
	cJSON* content = NULL;
	bool bRMMSupport = true;
	if(!data) return false;

	if(!pkt) return false;

	memset(pkt, 0 , sizeof(susiaccess_packet_body_t));
	pkt->type = pkt_type_custom;
	//strInput = scparser_utf8toansi(data);

	//root = cJSON_Parse(strInput);
	root = cJSON_Parse(data);
	
	//free(strInput);
	//strInput = NULL;
	
	if(!root) return false;

	body = cJSON_GetObjectItem(root, BASICINFO_BODY_STRUCT);
	if(!body)
	{
		bRMMSupport = false;
		body = root;
	}

	target = body->child;
	while (target)
	{
		if(!strcmp(target->string, BASICINFO_CMDTYPE))
		{
			pkt->type = bRMMSupport?pkt_type_susiaccess:pkt_type_wisepaas;
			pkt->cmd = target->valueint;
		}
		else if(!strcmp(target->string, BASICINFO_AGENTID))
		{
			pkt->type = bRMMSupport?pkt_type_susiaccess:pkt_type_wisepaas;
			strncpy(pkt->devId, target->valuestring, sizeof(pkt->devId));
		}
		else if(!strcmp(target->string, BASICINFO_HANDLERNAME))
		{
			pkt->type = bRMMSupport?pkt_type_susiaccess:pkt_type_wisepaas;
			strncpy(pkt->handlerName, target->valuestring, sizeof(pkt->handlerName));
		}
		else if(!strcmp(target->string, BASICINFO_CONTENT))
		{
			pkt->type = pkt_type_wisepaas;
			content = cJSON_Duplicate(target,true);
		}
		else
		{
			if(!content)
				content = cJSON_CreateObject();

			cJSON_AddItemToObject(content, target->string, cJSON_Duplicate(target,true));
		}
		target = target->next;
	}

	if(content)
	{
		char* strInput = NULL;
		char* strcontent = cJSON_PrintUnformatted(content);
		cJSON_Delete(content);
		strInput = scparser_utf8toansi(strcontent);
		pkt->content = strdup(strInput);
		free(strcontent);
		free(strInput);
		strInput = NULL;
	}

	cJSON_Delete(root);
	return true;
}
