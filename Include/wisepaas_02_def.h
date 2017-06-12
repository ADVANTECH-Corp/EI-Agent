/****************************************************************************/
/* Copyright(C) : Advantech Technologies, Inc.								*/
/* Create Date  : 2017/02/09 by Scott Chang								    */
/* Modified Date: 2017/02/09 by Scott Chang									*/
/* Abstract     : WISE-PaaS 2.0 definition for RMM 4.x						*/
/* Reference    : None														*/
/****************************************************************************/

#ifndef _WISEPAAS_02_DEF_H_
#define _WISEPAAS_02_DEF_H_

#define DEF_FILENAME_LENGTH			32
#define DEF_DEVID_LENGTH			37
#define DEF_FW_DESCRIPTION_LENGTH	128
#define DEF_PROCESSOR_NAME_LEN		64

#define DEF_MARK_LENGTH				8
#define DEF_ACCOUNT_LENGTH			32
#define DEF_TYPE_LENGTH				32
#define DEF_HOSTNAME_LENGTH			42
#define DEF_SN_LENGTH				32
#define DEF_MAC_LENGTH				32
#define DEF_LAL_LENGTH				20
#define DEF_VERSION_LENGTH			32
#define DEF_MAX_STRING_LENGTH		128
#define DEF_RUN_MODE_LENGTH			32
#define DEF_ENABLE_LENGTH			8
#define DEF_USER_PASS_LENGTH		256
#define DEF_PORT_LENGTH				8
#define DEF_KVM_MODE_LENGTH			16
#define MAX_TOPIC_LEN               32
#define DEF_OSVERSION_LEN			64
#define DEF_MAX_PATH				260
#define DEF_MAX_CIPHER				4095

#define AGENT_STATUS_OFFLINE		0  /**< Agent offline flag */
#define AGENT_STATUS_ONLINE			1  /**< Agent online  flag: Server responsed */
#define AGENT_STATUS_CONNECTION_FAILED	2  /**< Agent connect failed flag */

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
#define DEF_ACTION_RESULT_SESSION_JSON		"{\"commCmd\":%d,\"handlerName\":\"general\",\"content\":{\"result\":\"%s\"},\"sessionID\":\"%s\",\"sendTS\":{\"$date\":%lld}}"
#define DEF_ACTION_RESULT_JSON				"{\"commCmd\":%d,\"handlerName\":\"general\",\"content\":{\"result\":\"%s\"},\"sendTS\":{\"$date\":%lld}}"
#define DEF_ACTION_RESPONSE_JSON			"{\"commCmd\":%d,\"handlerName\":\"%s\",\"content\":\"%s\",\"sendTS\":{\"$date\":%lld}}"
#define DEF_HEARTBEAT_MESSAGE_JSON			"{\"hb\":{\"devID\":\"%s\"}}"
#define DEF_HEARTBEATRATE_RESPONSE_SESSION_JSON	"{\"commCmd\":%d,\"handlerName\":\"general\",\"content\":{\"heartbeatrate\":%d},\"sessionID\":\"%s\",\"sendTS\":{\"$date\":%lld}}"

#endif