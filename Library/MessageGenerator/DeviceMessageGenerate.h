#ifndef _DEVICE_MESSAGE_GENERATE_H_
#define _DEVICE_MESSAGE_GENERATE_H_
#include "MsgGenerator.h"
#include <stdbool.h>
#include "wisepaas_02_def.h"
#include "susiaccess_def.h"

#ifdef __cplusplus
extern "C" {
#endif

	bool DEV_CreateAgentInfo(susiaccess_agent_profile_body_t const * pProfile, int status, long long tick, char* strInfo, int length);

	bool DEV_CreateWillMessage(susiaccess_agent_profile_body_t const * pProfile, long long tick, char* strInfo, int length);

	bool DEV_CreateOSInfo(susiaccess_agent_profile_body_t* profile, long long tick, char* strInfo, int length);

	char*  DEV_CreateHandlerList(char* devID, char** handldelist, int count);

	char* DEV_CreateEventNotify(char* subtype, char* message, char * extMsg);

	char* DEV_CreateFullEventNotify(char* devID, int severity, char* handler, char* subtype, char* message, char * extMsg);

	char* DEV_CreateFullMessage(char* devID, char* handler, int cmdId, char* msg);

	bool DEV_GetAgentInfoTopic_Tenant(char* tenantID, char* devID, char * topic, int length);
#define DEV_GetAgentInfoTopic(devID, topic, length) DEV_GetAgentInfoTopic_Tenant("general", devID, topic, length)

	bool DEV_GetWillMessageTopic_Tenant(char* tenantID, char* devID, char * topic, int length);
#define DEV_GetWillMessageTopic(devID, topic, length) DEV_GetWillMessageTopic_Tenant("general", devID, topic, length)

	bool DEV_GetActionReqTopic_Tenant(char* tenantID, char* devID, char * topic, int length);
#define DEV_GetActionReqTopic(devID, topic, length) DEV_GetActionReqTopic_Tenant("general", devID, topic, length)

	bool DEV_GetEventNotifyTopic_Tenant(char* tenantID, char* devID, char * topic, int length);
#define DEV_GetEventNotifyTopic(devID, topic, length) DEV_GetEventNotifyTopic_Tenant("general", devID, topic, length)

#define DEV_GetOSInfoTopic_Tenant(tenantID, devID, topic, length) DEV_GetActionReqTopic_Tenant(tenantID, devID, topic, length)
#define DEV_GetOSInfoTopic(devID, topic, length) DEV_GetActionReqTopic(devID, topic, length)

#define DEV_GetHandlerListTopic_Tenant(tenantID, devID, topic, length) DEV_GetActionReqTopic_Tenant(tenantID, devID, topic, length)
#define DEV_GetHandlerListTopic(devID, topic, length) DEV_GetActionReqTopic(devID, topic, length)

	void DEV_ReleaseBuffer(char* buff);

	char* DEV_PrintPacket(susiaccess_packet_body_t const * pPacket);
	bool DEV_ParseMessage(const void* data, int datalen, susiaccess_packet_body_t * pkt);

#ifdef __cplusplus
}
#endif
#endif