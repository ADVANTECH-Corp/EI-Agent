/****************************************************************************/
/* Copyright(C) : Advantech Technologies, Inc.								*/
/* Create Date  : 2017/01/20 by Scott Chang								    */
/* Modified Date: 2017/01/20 by Scott Chang									*/
/* Abstract     : Mosquitto Carrier Extend API definition					*/
/* Reference    : None														*/
/****************************************************************************/
#include "network.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mosquitto.h"
#include "susiaccess_def.h"
#include "WiseCarrierEx_MQTT.h"
#include "topic.h"
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

static char g_version[32] = {0};

struct mqttmsg
{
	int mid;
	pthread_mutex_t waitlock;
	pthread_cond_t waitresponse;
	struct mqttmsg* next;
};

struct msg_queue {
	struct mqttmsg* root;
	int size;
};

typedef struct{
	struct mosquitto *mosq;

	/*Connection Info*/
	char strClientID[DEF_DEVID_LENGTH];
	char strServerIP[DEF_MAX_STRING_LENGTH];
	int iServerPort;
	char strAuthID[DEF_USER_PASS_LENGTH];
	char strAuthPW[DEF_USER_PASS_LENGTH];
	int iKeepalive;

	char strWillTopic[DEF_MAX_PATH];
	char* pWillPayload;
	int iWillLen;

	tls_type tlstype;
	/*TLS*/
	char strCaFile[DEF_MAX_PATH];
	char strCaPath[DEF_MAX_PATH];
	char strCertFile[DEF_MAX_PATH];
	char strKeyFile[DEF_MAX_PATH];
	char strCerPasswd[DEF_USER_PASS_LENGTH];
	/*pre-shared-key*/
	char strPsk[DEF_USER_PASS_LENGTH];
	char strIdentity[DEF_USER_PASS_LENGTH];
	char strCiphers[DEF_MAX_CIPHER];

	void *pUserData;

	WICAR_CONNECT_CB on_connect_cb;
	WICAR_DISCONNECT_CB on_disconnect_cb;
	WICAR_LOSTCONNECT_CB on_lostconnect_cb;
	topic_entry_st* pSubscribeTopics;

	pthread_mutex_t publishlock;
	pthread_t reconnthr;

	mc_err_code iErrorCode;
	struct msg_queue msg_queue;
}mosq_car_t;

#pragma region MQTT MESSAGE QUEUE

struct mqttmsg* MQTT_LastMsgQueue(struct msg_queue* msg_queue)
{
	struct mqttmsg* msg = msg_queue->root;
	struct mqttmsg* target = NULL;
	if(msg == NULL)
		return target;
	else
		target = msg;

	while(target->next)
	{
		target = target->next;
	}
	return target;	
}

void MQTT_InitMsgQueue(struct msg_queue* msg_queue)
{
	memset(msg_queue, 0, sizeof(struct msg_queue));
}

void MQTT_UninitMsgQueue(struct msg_queue* msg_queue)
{
	struct mqttmsg* msg = msg_queue->root;
	struct mqttmsg* target = NULL;
	while(msg)
	{
		target = msg;
		msg = msg->next;
		pthread_mutex_destroy(&target->waitlock);
		pthread_cond_destroy(&target->waitresponse);
		free(target);
		msg_queue->size--;
	}
}

struct mqttmsg* MQTT_AddMsgQueue(struct msg_queue* msg_queue, int mid)
{
	struct mqttmsg* msg = NULL;
	struct mqttmsg* newmsg = malloc(sizeof(struct mqttmsg));
	memset(newmsg, 0, sizeof(struct mqttmsg));
	newmsg->mid = mid;
	pthread_mutex_init(&newmsg->waitlock, NULL);
	pthread_cond_init(&newmsg->waitresponse, NULL);

	msg = MQTT_LastMsgQueue(msg_queue);
	if(msg==NULL)
	{
		msg_queue->size = 1;
		msg_queue->root = newmsg;
	}
	else
	{
		msg_queue->size++;
		msg->next = newmsg;
	}
	return newmsg;
}

struct mqttmsg* MQTT_FindMsgQueue(struct msg_queue* msg_queue, int mid)
{
	struct mqttmsg* msg = msg_queue->root;
	struct mqttmsg* target = NULL;

	while(msg)
	{
		if(msg->mid == mid)
		{
			target = msg;
			break;
		}
		msg = msg->next;
	}
	return target;
}

void MQTT_FreeMsgQueue(struct msg_queue* msg_queue, int mid)
{
	struct mqttmsg* msg = msg_queue->root;
	struct mqttmsg* target = NULL;
	if(msg == NULL)
		return;
	if(msg->mid == mid)
	{
		msg_queue->root = msg->next;
		msg_queue->size--;
		target = msg;
		goto FREE_MSG;
	}

	while(msg->next)
	{
		if(msg->next->mid == mid)
		{
			target = msg->next;
			msg->next = target->next;
			msg_queue->size--;
			break;
		}
		msg = msg->next;
	}
FREE_MSG:
	if(target == NULL) return;
	pthread_mutex_destroy(&msg->waitlock);
	pthread_cond_destroy(&msg->waitresponse);
	free(msg);
}

void MQTT_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{	
	struct mqttmsg* msg = NULL;
	mosq_car_t* pmosq = NULL;
	if(obj == NULL)
		return;

	pmosq = (mosq_car_t*)obj;
	msg = MQTT_FindMsgQueue(&pmosq->msg_queue, mid);
	if(msg == NULL) return;
	pthread_cond_signal(&msg->waitresponse);
}

#pragma endregion MQTT MESSAGE QUEUE

void* thread_reconnect(void* args)
{
	struct mosquitto *mosq = NULL;
	int sleep_time = 30;
	if(args == NULL)
		return;
	mosq = (struct mosquitto*)args;
	srand(time(NULL));
	sleep_time += (10 - (rand() % 20));

	sleep(sleep_time);

	mosquitto_reconnect_async(mosq);

	pthread_exit(0);
	return 0;
}

void on_connect_callback(struct mosquitto *mosq, void *userdata, int rc)
{
	mosq_car_t* pmosq = NULL;
	if(userdata == NULL)
		return;
	pmosq = (mosq_car_t*)userdata;
	pmosq->iErrorCode = mc_conn_accept + rc;
	if(rc == 0)
	{
		if(pmosq->reconnthr != NULL)
		{
			pthread_cancel(pmosq->reconnthr);
			pthread_join(pmosq->reconnthr, NULL);
			pmosq->reconnthr = NULL;	
		}

		if(pmosq->on_connect_cb)
			pmosq->on_connect_cb(pmosq->pUserData);
	}
	else
	{
		if(pmosq == NULL)
		{
			if(pthread_create(&pmosq->reconnthr, NULL, thread_reconnect, mosq)!=0)
				pmosq->reconnthr = NULL;				
		}

		if(pmosq->on_lostconnect_cb)
			pmosq->on_lostconnect_cb(pmosq->pUserData);
	}
}

void on_disconnect_callback(struct mosquitto *mosq, void *userdata, int rc)
{
	mosq_car_t* pmosq = NULL;
	if(userdata == NULL)
		return;
	pmosq = (mosq_car_t*)userdata;
	pmosq->iErrorCode = mc_conn_accept + rc;
	if(rc == 0)
	{
		if(pmosq->reconnthr != NULL)
		{
			pthread_cancel(pmosq->reconnthr);
			pthread_join(pmosq->reconnthr, NULL);
			pmosq->reconnthr = NULL;	
		}

		if(pmosq->on_disconnect_cb)
			pmosq->on_disconnect_cb(pmosq->pUserData);
	}
	else
	{
		if(pmosq->reconnthr == NULL)
		{
			if(pthread_create(&pmosq->reconnthr, NULL, thread_reconnect, mosq)!=0)
				pmosq->reconnthr = NULL;				
		}

		if(pmosq->on_lostconnect_cb)
			pmosq->on_lostconnect_cb(pmosq->pUserData);
	}
}

void on_message_recv_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg)
{	
	struct topic_entry * target = NULL;
	mosq_car_t* pmosq = NULL;

	if(userdata == NULL)
		return;
	pmosq = (mosq_car_t*)userdata;

	target = topic_find(pmosq->pSubscribeTopics, msg->topic);
	
	if(target != NULL)
	{
		if(target->callback_func)
			((WICAR_MESSAGE_CB)target->callback_func)(msg->topic, msg->payload, msg->payloadlen, pmosq->pUserData);
	}
}

int on_password_check(char *buf, int size, int rwflag, void *userdata)
{
	int length = 0;
	mosq_car_t* pmosq = NULL;

	if(!buf)
		return 0;

	if(userdata == NULL)
		return 0;

	pmosq = (mosq_car_t*)userdata;

	length = strlen(pmosq->strCerPasswd);

	memset(buf, 0, size);
	if(length+1 >= size)
	{
		strncpy(buf,pmosq->strCerPasswd,size);
		return size;
	}
	else
	{
		pthread_t reconnthr = 0;
		if(pthread_create(&reconnthr, NULL, thread_reconnect, NULL)==0)
			pthread_detach(reconnthr);

		strncpy(buf, pmosq->strCerPasswd, length+1);
		return length;
	}
}

struct mosquitto * _initialize(mosq_car_t* pmosq, char const * devid)
{
	struct mosquitto *mosq = NULL;

	mosquitto_lib_init();
	mosq = mosquitto_new(devid, true, pmosq);
	if (!mosq)
	{
		pmosq->iErrorCode = mc_err_init_fail;
		return NULL;
	}

	mosquitto_connect_callback_set(mosq, on_connect_callback);
	mosquitto_disconnect_callback_set(mosq, on_disconnect_callback);
	mosquitto_message_callback_set(mosq, on_message_recv_callback);

	MQTT_InitMsgQueue(&pmosq->msg_queue);
	mosquitto_publish_callback_set(mosq, MQTT_publish_callback);

	pmosq->mosq = mosq;
	return mosq;
}

void _uninitialize(mosq_car_t* pmosq)
{
	struct mosquitto *mosq = NULL;
	if(pmosq == NULL)
		return;

	mosq = pmosq->mosq;
	if(mosq == NULL)
		return;

	mosquitto_connect_callback_set(mosq, NULL);
	mosquitto_disconnect_callback_set(mosq, NULL);
	mosquitto_message_callback_set(mosq, NULL);

	mosquitto_publish_callback_set(mosq, NULL);
	MQTT_UninitMsgQueue(&pmosq->msg_queue);

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

bool _tls_set(mosq_car_t *pmosq)
{
	int result = MOSQ_ERR_SUCCESS;
	if(pmosq == NULL)
		return false;

	mosquitto_tls_insecure_set(pmosq->mosq, true);
	result = mosquitto_tls_set(pmosq->mosq, strlen(pmosq->strCaFile)>0?pmosq->strCaFile:NULL, strlen(pmosq->strCaPath)>0?pmosq->strCaPath:NULL, strlen(pmosq->strCertFile)>0?pmosq->strCertFile:NULL, strlen(pmosq->strKeyFile)>0?pmosq->strKeyFile:NULL, on_password_check);
	pmosq->iErrorCode = result;
	return result==MOSQ_ERR_SUCCESS?true:false;
}

bool _psk_set(mosq_car_t *pmosq)
{
	int result = MOSQ_ERR_SUCCESS;
	if(pmosq == NULL)
		return false;

	result = mosquitto_tls_psk_set(pmosq->mosq, strlen(pmosq->strPsk)>0?pmosq->strPsk:NULL, strlen(pmosq->strIdentity)>0?pmosq->strIdentity:NULL, strlen(pmosq->strCiphers)>0?pmosq->strCiphers:NULL);
	pmosq->iErrorCode = result;
	return result==MOSQ_ERR_SUCCESS?true:false;
}

bool _connect(mosq_car_t *pmosq)
{
	bool bAsync = true;
	int result = MOSQ_ERR_SUCCESS;
	if(pmosq == NULL)
		return false;

	if( strlen(pmosq->strAuthID)>0 && strlen(pmosq->strAuthPW)>0)
		mosquitto_username_pw_set(pmosq->mosq,pmosq->strAuthID,pmosq->strAuthPW);

	mosquitto_will_clear(pmosq->mosq);

	if(pmosq->pWillPayload != NULL) {
		mosquitto_will_set(pmosq->mosq, pmosq->strWillTopic, pmosq->iWillLen , pmosq->pWillPayload, 0, false);
	}
	mosquitto_loop_start(pmosq->mosq); /*Must call before mosquitto_connect_async*/
	if(bAsync)
		result = mosquitto_connect_async(pmosq->mosq, pmosq->strServerIP, pmosq->iServerPort, pmosq->iKeepalive);
	else
		result = mosquitto_connect(pmosq->mosq, pmosq->strServerIP, pmosq->iServerPort, pmosq->iKeepalive);
	pmosq->iErrorCode = result;

	return result==MOSQ_ERR_SUCCESS?true:false;
}

bool _reconnect(mosq_car_t *pmosq)
{
	int result = MOSQ_ERR_SUCCESS;
	if(pmosq == NULL)
		return false;

	result = mosquitto_reconnect(pmosq->mosq);
	pmosq->iErrorCode = result;

	return result==MOSQ_ERR_SUCCESS?true:false;
}

bool _disconnect(mosq_car_t *pmosq, bool bForce)
{
	int result = MOSQ_ERR_SUCCESS;
	if(pmosq == NULL)
		return false;
	if(!bForce)
		mosquitto_loop(pmosq->mosq, 0, 1);	
	result = mosquitto_disconnect(pmosq->mosq);
	if(!bForce)
		mosquitto_loop(pmosq->mosq, 0, 1);	
	mosquitto_loop_stop(pmosq->mosq, bForce); //disable force for linux.

	pmosq->iErrorCode = mc_err_success;
	return result==MOSQ_ERR_SUCCESS?true:false;
}


WISE_CARRIER_API const char * WiCarEx_MQTT_LibraryTag()
{
	int major=0, minor=0, revision=0;
	mosquitto_lib_version(&major, &minor, &revision);
	sprintf(g_version, "mosquitto_%d.%d.%d", major, minor, revision);
	return g_version;
}

WISE_CARRIER_API WiCar_t WiCarEx_MQTT_Init(WICAR_CONNECT_CB on_connect, WICAR_DISCONNECT_CB on_disconnect, void *userdata)
{
	mosq_car_t* pmosq = calloc(1, sizeof(mosq_car_t));
	
	if(pmosq == NULL)
		return NULL;

	pmosq->on_connect_cb = on_connect;
	pmosq->on_disconnect_cb = on_disconnect;
	pmosq->pUserData = userdata;
	pmosq->iKeepalive = 10; //default 10 sec.
	pmosq->iErrorCode = mc_err_success;
	return (WiCar_t)pmosq;
}

WISE_CARRIER_API void WiCarEx_MQTT_Uninit(WiCar_t pmosq)
{
	if(pmosq)
	{
		mosq_car_t* mosq = (mosq_car_t*)pmosq;
		struct topic_entry *iter_topic = NULL;
		struct topic_entry *tmp_topic = NULL;

		iter_topic = topic_first(mosq->pSubscribeTopics);
		while(iter_topic != NULL)
		{
			tmp_topic = iter_topic->next;
			topic_remove(&mosq->pSubscribeTopics, iter_topic->name);
			iter_topic = tmp_topic;
		}

		if(mosq->reconnthr != NULL)
		{
			pthread_cancel(mosq->reconnthr);
			pthread_join(mosq->reconnthr, NULL);
			mosq->reconnthr = NULL;	
		}
		//g_mosq->pSubscribeTopics = NULL;
		if(mosq->mosq)
			_uninitialize(mosq);

		if(mosq->pWillPayload!=NULL)
			free(mosq->pWillPayload);
		mosq->pWillPayload = NULL;
		free(mosq);
	}
	pmosq = NULL;
}

WISE_CARRIER_API bool WiCarEx_MQTT_SetWillMsg(WiCar_t pmosq, const char* topic, const void *msg, int msglen)
{
	mosq_car_t* mosq = NULL;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;

	strncpy(mosq->strWillTopic, topic, sizeof(mosq->strWillTopic));
	if(mosq->pWillPayload)
		free(mosq->pWillPayload);
	mosq->pWillPayload = calloc(1, msglen+1);
	if(mosq->pWillPayload)
	{
		memset(mosq->pWillPayload, 0, msglen+1);
		strcpy(mosq->pWillPayload, msg);
		mosq->iWillLen = msglen;
	}
	else{
		mosq->iErrorCode = mc_err_malloc_fail;
		return false;
	}
	mosq->iErrorCode = mc_err_success;
	return true;

}

WISE_CARRIER_API bool WiCarEx_MQTT_SetAuth(WiCar_t pmosq, char const * username, char const * password)
{
	mosq_car_t* mosq = NULL;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	strncpy(mosq->strAuthID, username, sizeof(mosq->strAuthID));
	strncpy(mosq->strAuthPW, password, sizeof(mosq->strAuthPW));
	mosq->iErrorCode = mc_err_success;
	return true;
}

WISE_CARRIER_API bool WiCarEx_MQTT_SetKeepLive(WiCar_t pmosq, int keepalive)
{
	mosq_car_t* mosq = NULL;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	mosq->iKeepalive = keepalive;
	mosq->iErrorCode = mc_err_success;
	return true;
}

WISE_CARRIER_API bool WiCarEx_MQTT_SetTls(WiCar_t pmosq, const char *cafile, const char *capath, const char *certfile, const char *keyfile, const char* password)
{
	mosq_car_t* mosq = NULL;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;

	mosq->tlstype = tls_type_tls;
	if(cafile)
		strncpy(mosq->strCaFile, cafile, sizeof(mosq->strCaFile));
	if(capath)
		strncpy(mosq->strCaPath, capath, sizeof(mosq->strCaPath));
	if(certfile)
		strncpy(mosq->strCertFile, certfile, sizeof(mosq->strCertFile));
	if(keyfile)
		strncpy(mosq->strKeyFile, keyfile, sizeof(mosq->strKeyFile));
	if(password)
		strncpy(mosq->strCerPasswd, password, sizeof(mosq->strCerPasswd));
	mosq->iErrorCode = mc_err_success;
	return true;
}

WISE_CARRIER_API bool WiCarEx_MQTT_SetTlsPsk(WiCar_t pmosq, const char *psk, const char *identity, const char *ciphers)
{
	mosq_car_t* mosq = NULL;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	mosq->tlstype = tls_type_psk;
	if(psk)
		strncpy(mosq->strPsk, psk, sizeof(mosq->strPsk));
	if(identity)
		strncpy(mosq->strIdentity, identity, sizeof(mosq->strIdentity));
	if(ciphers)
		strncpy(mosq->strCiphers, ciphers, sizeof(mosq->strCiphers));
	mosq->iErrorCode = mc_err_success;
	return true;
}

WISE_CARRIER_API bool WiCarEx_MQTT_Connect(WiCar_t pmosq, const char* address, int port, const char* clientId, WICAR_LOSTCONNECT_CB on_lostconnect)
{
	mosq_car_t* mosq = NULL;
	struct mosquitto *mymosq = NULL;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	if(mosq->mosq)
	{
		_uninitialize(mosq);
		mosq->mosq = NULL;
	}
	
	mymosq = _initialize(mosq, clientId);
	if (!mymosq)
	{
		mosq->iErrorCode = mc_err_init_fail;
		return false;
	}

	switch (mosq->tlstype)
	{
	case tls_type_none:
		break;
	case tls_type_tls:
		if(!_tls_set(mosq)) return false;
		break;
	case tls_type_psk:
		if(!_psk_set(mosq)) return false;
		break;
	}

	
	pthread_mutex_init(&mosq->publishlock, NULL);

	strncpy(mosq->strServerIP, address, sizeof(mosq->strServerIP));
	mosq->iServerPort = port;
	strncpy(mosq->strClientID, clientId, sizeof(mosq->strClientID));
	mosq->on_lostconnect_cb = on_lostconnect;
	return _connect(mosq);

}

WISE_CARRIER_API bool WiCarEx_MQTT_Reconnect(WiCar_t pmosq)
{
	mosq_car_t* mosq = NULL;
	bool bRet = false;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	pthread_mutex_destroy(&mosq->publishlock);
	bRet = _reconnect(mosq);
	pthread_mutex_init(&mosq->publishlock, NULL);
	if(bRet)
	{
		struct topic_entry *iter_topic = NULL;

		iter_topic = topic_first(mosq->pSubscribeTopics);
		while(iter_topic != NULL)
		{
			mosquitto_subscribe(mosq->mosq, NULL, iter_topic->name, 0);
			iter_topic = iter_topic->next;
		}
	}
	return bRet;
}

WISE_CARRIER_API bool WiCarEx_MQTT_Disconnect(WiCar_t pmosq, int force)
{
	mosq_car_t* mosq = NULL;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	if(_disconnect(mosq, force>0?true:false))
	{
		struct topic_entry *iter_topic = NULL;
		struct topic_entry *tmp_topic = NULL;
		
		pthread_mutex_destroy(&mosq->publishlock);

		iter_topic = topic_first(mosq->pSubscribeTopics);
		while(iter_topic != NULL)
		{
			tmp_topic = iter_topic->next;
			topic_remove(&mosq->pSubscribeTopics, iter_topic->name);
			iter_topic = tmp_topic;
		}
		//g_mosq->pSubscribeTopics = NULL;
		_uninitialize(mosq);
		mosq->mosq = NULL;
		return true;
	}
	else
		return false;
}

WISE_CARRIER_API bool WiCarEx_MQTT_Publish(WiCar_t pmosq, const char* topic, const void *msg, int msglen, int retain, int qos)
{
	mosq_car_t* mosq = NULL;
	int result = MOSQ_ERR_SUCCESS;
	struct mqttmsg* mqttmsg = NULL;
	int mid = 0;
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	pthread_mutex_lock(&mosq->publishlock);
	result = mosquitto_publish(mosq->mosq, &mid, topic, msglen, msg, qos, retain>0?true:false);
	pthread_mutex_unlock(&mosq->publishlock);
	mosq->iErrorCode = result;
	if(result == MOSQ_ERR_SUCCESS)
	{
		if(qos>0)
			mqttmsg = MQTT_AddMsgQueue(&mosq->msg_queue, mid);
	}

	if(mqttmsg)
	{
		struct timespec time;
		struct timeval tv;
		int ret = 0;
		gettimeofday(&tv, NULL);
		time.tv_sec = tv.tv_sec + 3;
		time.tv_nsec = tv.tv_usec;
		ret = pthread_cond_timedwait(&mqttmsg->waitresponse, &mqttmsg->waitlock, &time);
		if(ret != 0)
		{
			printf("Publish wait mid:%d timeout", mid);
			result = MOSQ_ERR_INVAL;
		}
		MQTT_FreeMsgQueue(&mosq->msg_queue, mid);
	}

	return result==MOSQ_ERR_SUCCESS?true:false;
}

WISE_CARRIER_API bool WiCarEx_MQTT_Subscribe(WiCar_t pmosq, const char* topic, int qos, WICAR_MESSAGE_CB on_recieve)
{
	mosq_car_t* mosq = NULL;
	int result = MOSQ_ERR_SUCCESS;
	topic_entry_st *ptopic = NULL;

	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	result = mosquitto_subscribe(mosq->mosq, NULL, topic, 0);
	if(result==MOSQ_ERR_SUCCESS)
	{
		ptopic = topic_find(mosq->pSubscribeTopics, topic);
		if(ptopic == NULL)
		{
			ptopic = topic_add(&mosq->pSubscribeTopics, topic, (void*)on_recieve);
			if(ptopic == NULL)
			{
				mosquitto_unsubscribe(mosq->mosq, NULL, topic);
				mosq->iErrorCode = mc_err_malloc_fail;
				return false;
			}
		}
		else
			ptopic->callback_func = on_recieve;
	}
	mosq->iErrorCode = result;
	return result==MOSQ_ERR_SUCCESS?true:false;
}

WISE_CARRIER_API bool WiCarEx_MQTT_UnSubscribe(WiCar_t pmosq, const char* topic)
{
	mosq_car_t* mosq = NULL;
	int result = MOSQ_ERR_SUCCESS;
	topic_entry_st *ptopic = NULL;

	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	result = mosquitto_unsubscribe(mosq->mosq, NULL, topic);

	topic_remove(&mosq->pSubscribeTopics, (char*)topic);

	mosq->iErrorCode = result;
	return result==MOSQ_ERR_SUCCESS?true:false;
}

WISE_CARRIER_API bool WiCarEx_MQTT_GetLocalIP(WiCar_t pmosq, char *address)
{
	mosq_car_t* mosq = NULL;
	int sockfd = -1;
	char strAddr[16] = {0};
	if(!pmosq)
		return false;
	mosq = (mosq_car_t*)pmosq;
	sockfd = mosquitto_socket(mosq->mosq);

	if(network_local_ip_get(sockfd, strAddr, sizeof(strAddr))==0)
	{
		mosq->iErrorCode = mc_err_success;
		strncpy(address, strAddr, sizeof(strAddr));
		return true;
	}
	else
	{
		mosq->iErrorCode = mc_err_no_connect;
		return false; 
	}
}

WISE_CARRIER_API const char *WiCarEx_MQTT_GetCurrentErrorString(WiCar_t pmosq)
{
	mosq_car_t* mosq = NULL;
	if(!pmosq)
		return "No initialized.";
	mosq = (mosq_car_t*)pmosq;
	switch(mosq->iErrorCode){
		case mc_err_success:
			return "No error.";
		case mc_err_invalid_param:
			return "Invalided parameters.";
		case mc_err_no_init:
			return "No initialized.";
		case mc_err_init_fail:
			return "Initialize failed";
		case mc_err_already_init:
			return "Already initialized.";
		case mc_err_no_connect:
			return "No connected.";
		case mc_err_malloc_fail:
			return "Memory allocate failed";
		case mc_err_not_support:
			return "Not support.";
		default:
			{
				if(mosq->iErrorCode > mc_conn_accept &&  mosq->iErrorCode < mc_err_invalid_param)
				{
					int rc = mosq->iErrorCode - mc_conn_accept;
					return mosquitto_connack_string(rc);
				}
				else
					return mosquitto_strerror(mosq->iErrorCode);
			}
	}
}