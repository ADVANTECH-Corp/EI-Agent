/****************************************************************************/
/* Copyright(C) : Advantech Technologies, Inc.								*/
/* Create Date  : 2017/02/09 by Scott Chang								    */
/* Modified Date: 2017/02/09 by Scott Chang									*/
/* Abstract     : WISE-PaaS 2.0 definition for RMM 4.x						*/
/* Reference    : None														*/
/****************************************************************************/

#ifndef _WISEPAAS_02_DEF_H_
#define _WISEPAAS_02_DEF_H_

#include "wise/wisepaas_base_def.h"

#define DEF_GENERAL_HANDLER					"\"handlerName\":\"general\""
#define DEF_SERVERREDUNDANCY_HANDLER		"\"handlerName\":\"ServerRedundancy\""
#define DEF_GENERAL_REQID					"\"requestID\":109"
#define DEF_WISE_COMMAND					"\"commCmd\":%d"
#define DEF_SERVERCTL_STATUS				"\"statuscode\":%d"
#define DEF_WISE_TIMESTAMP					"\"sendTS\":%d"

#define DEF_WILLMSG_TOPIC					"/wisepaas/%s/device/%s/willmessage"	/*publish*/
#define DEF_INFOACK_TOPIC					"/wisepaas/%s/device/%s/agentinfoack"	/*publish*/
#define DEF_AGENTREPORT_TOPIC				"/wisepaas/%s/device/%s/devinfoack"	/*publish*/

#define DEF_AGENTINFO_JSON					"{\"content\":{\"parentID\":\"%s\",\"hostname\":\"%s\",\"sn\":\"%s\",\"mac\":\"%s\",\"version\":\"%s\",\"type\":\"%s\",\"product\":\"%s\",\"manufacture\":\"%s\",\"account\":\"%s\",\"passwd\":\"%s\",\"status\":%d,\"tag\":\"%s\"},\"commCmd\":1,\"agentID\":\"%s\",\"handlerName\":\"general\",\"sendTS\":{\"$date\":%lld}}"
#define DEF_PRESERVE_PRODUCT_NAME			"device"
#define DEF_AGENTACT_TOPIC					"/wisepaas/%s/%s/%s/agentactionack"	/*publish "/wisepaas/<tenantId>/<productTag>/<devId>/agentactionack"*/
#define DEF_EVENTNOTIFY_TOPIC				"/wisepaas/%s/%s/%s/eventnotifyack"	/*publish "/wisepaas/<tenantId>/<productTag>/<devId>/eventnotifyack"*/
#define DEF_CALLBACKREQ_TOPIC				"/wisepaas/%s/%s/%s/agentactionreq"	/*Subscrib "/wisepaas/<tenantId>/<productTag>/<devId>/agentactionreq"e*/
//#define DEF_ACTIONACK_TOPIC					"/wisepaas/%s/device/%s/agentactionreq"	/*Subscribe*/
#define DEF_AGENTCONTROL_TOPIC				"/wisepaas/%s/device/+/agentctrlreq"	/*Subscribe*/
#define DEF_HEARTBEAT_TOPIC					"/wisepaas/%s/device/%s/notifyack"	/*publish*/
#define DEF_ACTION_RESULT_SESSION_JSON		"{\"agentID\":\"%s\",\"commCmd\":%d,\"handlerName\":\"general\",\"content\":{\"result\":\"%s\"},\"sessionID\":\"%s\",\"sendTS\":{\"$date\":%lld}}"
#define DEF_ACTION_RESULT_JSON				"{\"agentID\":\"%s\",\"commCmd\":%d,\"handlerName\":\"general\",\"content\":{\"result\":\"%s\"},\"sendTS\":{\"$date\":%lld}}"
#define DEF_AUTOREPORT_JSON					"{\"agentID\":\"%s\",\"commCmd\":2055,\"handlerName\":\"general\",\"content\":%s,\"sendTS\":{\"$date\":%lld}}"
#define DEF_ACTION_RESPONSE_JSON			"{\"agentID\":\"%s\",\"commCmd\":%d,\"handlerName\":\"%s\",\"content\":%s,\"sendTS\":{\"$date\":%lld}}"
#define DEF_HEARTBEAT_MESSAGE_JSON			"{\"hb\":{\"devID\":\"%s\"}}"
#define DEF_HEARTBEATRATE_RESPONSE_SESSION_JSON	"{\"agentID\":\"%s\",\"commCmd\":%d,\"handlerName\":\"general\",\"content\":{\"heartbeatrate\":%d},\"sessionID\":\"%s\",\"sendTS\":{\"$date\":%lld}}"

#endif