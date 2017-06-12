#include <jni.h>
#include <stdio.h>
#include "WISECoreJNI.h"
#include "WISECoreEx.h"

WiCore_t* g_wise = NULL;
typedef struct {
	JavaVM *vm;
	jint version;
	jobject cbIface;
	jmethodID connect_cb;
	jmethodID lostconnect_cb;
	jmethodID disconnect_cb;
	jmethodID msgrecv_cb;
	jmethodID getcap_cb;
	jmethodID reportstart_cb;
	jmethodID reportstop_cb;
	jmethodID rename_cb;
	jmethodID update_cb;
	jmethodID reconnect_cb;
	jmethodID timetick_cb;
	jmethodID heartbeatquery_cb;
	jmethodID heartbeatupdate_cb;
}core_jni_contex_t;
core_jni_contex_t* g_ctx = NULL;

JNIEnv* AttachThread(JavaVM *vm, jint version)
{
	JNIEnv* env = NULL;
	int getEnvStat = (*vm)->GetEnv(vm, (void **)&env, version);
	if (getEnvStat == JNI_EDETACHED) {
		printf("GetEnv: not attached\n");
		if ((*vm)->AttachCurrentThread(vm, (void **)&env, NULL) != 0) {
			printf("Failed to attach\n");
		}
	}
	else if (getEnvStat == JNI_OK) {
		//
	}
	else if (getEnvStat == JNI_EVERSION) {
		printf("GetEnv: version not supported\n");
	}
	return env;
}

void DetachThread(JavaVM *vm)
{
	(*vm)->DetachCurrentThread(vm);
}

void on_connect_cb(void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	printf("CB_Connected \n");
	(*env)->ExceptionClear(env);
	if (ctx->connect_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->connect_cb,NULL);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling CB_Connected \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	DetachThread(ctx->vm);
}

void on_lostconnect_cb(void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	printf("CB_Lostconnect %s\n", core_ex_error_string_get(g_wise));
	(*env)->ExceptionClear(env);
	if (ctx->lostconnect_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->lostconnect_cb, NULL);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling CB_Lostconnect \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	DetachThread(ctx->vm);
}

void on_disconnect_cb(void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	printf("CB_Disconnect \n");
	(*env)->ExceptionClear(env);
	if (ctx->disconnect_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->disconnect_cb, NULL);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling CB_Disconnect \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	DetachThread(ctx->vm);
}

void on_msgrecv(const char* topic, const void *pkt, const long pktlength, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrTopic, jstrPacket;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrTopic = (*env)->NewStringUTF(env, topic);
	jstrPacket = (*env)->NewStringUTF(env, pkt);

	printf("Packet received:\n [%s],\n %s\n", topic, pkt);
	(*env)->ExceptionClear(env);
	if (ctx->msgrecv_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->msgrecv_cb, jstrTopic, jstrPacket, pktlength);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling CB_Disconnect \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrPacket);
	(*env)->DeleteLocalRef(env, jstrTopic);
	DetachThread(ctx->vm);
}

void on_get_capability(const void *pkt, const long pktlength, const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrPacket, jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrPacket = (*env)->NewStringUTF(env, pkt);
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("on_get_capability \n");
	(*env)->ExceptionClear(env);
	if (ctx->getcap_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->getcap_cb, jstrPacket, pktlength, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_get_capability \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrPacket);
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

void on_start_report(const void *pkt, const long pktlength, const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrPacket, jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrPacket = (*env)->NewStringUTF(env, pkt);
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("on_start_report \n");
	(*env)->ExceptionClear(env);
	if (ctx->reportstart_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->reportstart_cb, jstrPacket, pktlength, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_get_capability \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrPacket);
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

void on_stop_report(const void *pkt, const long pktlength, const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrPacket, jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrPacket = (*env)->NewStringUTF(env, pkt);
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("on_stop_report \n");
	(*env)->ExceptionClear(env);
	if (ctx->reportstop_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->reportstop_cb, jstrPacket, pktlength, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_get_capability \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrPacket);
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

void on_rename(const char* name, const int cmdid, const char* sessionid, const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrName, jstrSession, jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrName = (*env)->NewStringUTF(env, name);
	jstrSession = (*env)->NewStringUTF(env, sessionid);
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("rename to: %s\n", name);
	(*env)->ExceptionClear(env);
	if (ctx->rename_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->rename_cb, jstrName, cmdid, jstrSession, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_rename \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrName);
	(*env)->DeleteLocalRef(env, jstrSession);
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

void on_update(const char* loginID, const char* loginPW, const int port, const char* path, const char* md5, const int cmdid, const char* sessionid, const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrID, jstrPW, jstrPath, jstrMD5, jstrSession, jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrID = (*env)->NewStringUTF(env, loginID);
	jstrPW = (*env)->NewStringUTF(env, loginPW);
	jstrPath = (*env)->NewStringUTF(env, path);
	jstrMD5 = (*env)->NewStringUTF(env, md5);
	jstrSession = (*env)->NewStringUTF(env, sessionid);
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("Update: %s, %s, %d, %s, %s\n", loginID, loginPW, port, path, md5);
	(*env)->ExceptionClear(env);
	if (ctx->update_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->update_cb, jstrID, jstrPW, port, jstrPath, jstrMD5, cmdid, jstrSession, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_rename \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrID);
	(*env)->DeleteLocalRef(env, jstrPW);
	(*env)->DeleteLocalRef(env, jstrPath);
	(*env)->DeleteLocalRef(env, jstrMD5);
	(*env)->DeleteLocalRef(env, jstrSession);
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

void on_server_reconnect(const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("on_server_reconnect \n");
	(*env)->ExceptionClear(env);
	if (ctx->reconnect_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->reconnect_cb, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_rename \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

long long get_timetick(void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	long long tick = 0;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return 0;
	}
	printf("get_timetick \n");
	(*env)->ExceptionClear(env);
	if(ctx->timetick_cb)
		tick = (*env)->CallLongMethod(env, ctx->cbIface, ctx->timetick_cb, NULL);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling get_timetick \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	DetachThread(ctx->vm);
	return tick;
}

void on_heartbeatrate_query(const char* sessionid, const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrSession, jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrSession = (*env)->NewStringUTF(env, sessionid);
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("on_heartbeatrate_query \n");
	(*env)->ExceptionClear(env);
	if(ctx->heartbeatquery_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->heartbeatquery_cb, jstrSession, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_rename \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrSession);
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

void on_heartbeatrate_update(const int heartbeatrate, const char* sessionid, const char* tenantid, const char* devid, void* userdata)
{
	core_jni_contex_t* ctx = (core_jni_contex_t*)userdata;
	jstring jstrSession, jstrTenant, jstrDevID;
	JNIEnv *env = AttachThread(ctx->vm, ctx->version);
	if (env == NULL)
	{
		printf("AttachJNIEnv fail \n");
		return;
	}
	jstrSession = (*env)->NewStringUTF(env, sessionid);
	jstrTenant = (*env)->NewStringUTF(env, tenantid);
	jstrDevID = (*env)->NewStringUTF(env, devid);
	printf("Heartbeat Rate Update: %d, %s, %s\n", heartbeatrate, sessionid, devid);
	(*env)->ExceptionClear(env);
	if(ctx->heartbeatupdate_cb)
		(*env)->CallVoidMethod(env, ctx->cbIface, ctx->heartbeatupdate_cb, heartbeatrate, jstrSession, jstrTenant, jstrDevID);
	if ((*env)->ExceptionCheck(env)) {
		//panic! Light fires! The British are coming!!!
		printf("Exception on calling on_rename \n");
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	(*env)->DeleteLocalRef(env, jstrSession);
	(*env)->DeleteLocalRef(env, jstrTenant);
	(*env)->DeleteLocalRef(env, jstrDevID);
	DetachThread(ctx->vm);
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1initialize (JNIEnv *env, jobject thisObj, jstring strTenantID, jstring strClientID, jstring strHostName, jstring strMAC) {
	
	const char *nativeTenantID = (*env)->GetStringUTFChars(env, strTenantID, 0);
	const char *nativeClientID = (*env)->GetStringUTFChars(env, strClientID, 0);
	const char *nativeHostName = (*env)->GetStringUTFChars(env, strHostName, 0);
	const char *nativeMAC = (*env)->GetStringUTFChars(env, strMAC, 0);

	/*for attach debug*/
	//while (true)
	//	Sleep(1);

	g_ctx = calloc(1, sizeof(core_jni_contex_t));
	(*env)->GetJavaVM(env, &g_ctx->vm);
	g_ctx->version = (*env)->GetVersion(env);
	g_wise = core_ex_initialize(nativeTenantID, nativeClientID, nativeHostName, nativeMAC, g_ctx);
	(*env)->ReleaseStringUTFChars(env, strTenantID, nativeTenantID);
	(*env)->ReleaseStringUTFChars(env, strClientID, nativeClientID);
	(*env)->ReleaseStringUTFChars(env, strHostName, nativeHostName);
	(*env)->ReleaseStringUTFChars(env, strMAC, nativeMAC);
	printf("core_ex_initialize %s\n", g_wise != NULL ? "true" : "false");
	return g_wise != NULL?JNI_TRUE:JNI_FALSE;
}

JNIEXPORT void JNICALL Java_WISECoreJNI_core_1uninitialize(JNIEnv *env, jobject thisObj)
{
	if (g_wise)
	{
		core_ex_uninitialize(g_wise);
	}
	if (g_ctx)
	{
		if (g_ctx->cbIface)
			(*env)->DeleteGlobalRef(env, g_ctx->cbIface);
		free(g_ctx);
	}
	g_ctx = NULL;
	printf("core_ex_uninitialize\n");
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1tag_1set(JNIEnv *env, jobject thisObj, jstring strTag)
{
	bool bRet = false;
	const char *nativeTag = (*env)->GetStringUTFChars(env, strTag, 0);
	if (g_wise)
		bRet = core_ex_tag_set(g_wise, nativeTag);
	printf("core_ex_tag_set %s %s\n", nativeTag, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, strTag, nativeTag);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1product_1info_1set(JNIEnv *env, jobject thisObj, jstring strSerialNum, jstring strParentID, jstring strVersion, jstring strType, jstring strProduct, jstring strManufacture)
{
	bool bRet = false;
	const char *nativeSerialNum = (*env)->GetStringUTFChars(env, strSerialNum, 0);
	const char *nativeParentID = (*env)->GetStringUTFChars(env, strParentID, 0);
	const char *nativeVersion = (*env)->GetStringUTFChars(env, strVersion, 0);
	const char *nativeType = (*env)->GetStringUTFChars(env, strType, 0);
	const char *nativeProduct = (*env)->GetStringUTFChars(env, strProduct, 0);
	const char *nativeManufacture = (*env)->GetStringUTFChars(env, strManufacture, 0);
	if (g_wise)
		bRet = core_ex_product_info_set(g_wise, nativeSerialNum, nativeParentID, nativeVersion, nativeType, nativeProduct, nativeManufacture);
	printf("core_ex_product_info_set %s %s\n", nativeSerialNum, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, strSerialNum, nativeSerialNum);
	(*env)->ReleaseStringUTFChars(env, strParentID, nativeParentID);
	(*env)->ReleaseStringUTFChars(env, strVersion, nativeVersion);
	(*env)->ReleaseStringUTFChars(env, strType, nativeType);
	(*env)->ReleaseStringUTFChars(env, strProduct, nativeProduct);
	(*env)->ReleaseStringUTFChars(env, strManufacture, nativeManufacture);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1account_1bind(JNIEnv *env, jobject thisObj, jstring strLoginID, jstring strLoginPW)
{
	bool bRet = false;
	const char *nativeLoginID = (*env)->GetStringUTFChars(env, strLoginID, 0);
	const char *nativeLoginPW = (*env)->GetStringUTFChars(env, strLoginPW, 0);
	if (g_wise)
		bRet = core_ex_account_bind(g_wise, nativeLoginID, nativeLoginPW);
	printf("core_ex_account_bind %s %s\n", nativeLoginID, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, strLoginID, nativeLoginID);
	(*env)->ReleaseStringUTFChars(env, strLoginPW, nativeLoginPW);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1callback_1set(JNIEnv *env, jobject thisObj, jobject cbIface)
{
	bool bRet = false;
	jclass objclass ;
	jmethodID connect_cb;
	jmethodID lostconnect_cb;
	jmethodID disconnect_cb;
	jmethodID msgrecv_cb;
	jmethodID getcap_cb;
	jmethodID reportstart_cb;
	jmethodID reportstop_cb;
	jmethodID rename_cb;
	jmethodID update_cb;
	jmethodID reconnect_cb;
	jmethodID timetick_cb;
	jmethodID heartbeatquery_cb;
	jmethodID heartbeatupdate_cb;

	g_ctx->cbIface = (*env)->NewGlobalRef(env, cbIface);
	objclass = (*env)->GetObjectClass(env, cbIface);
	connect_cb = (*env)->GetMethodID(env, objclass, "on_connected", "()V");
	if (connect_cb != 0) {
		g_ctx->connect_cb = connect_cb;
	}

	lostconnect_cb = (*env)->GetMethodID(env, objclass, "on_lostconnected", "()V");
	if (lostconnect_cb != 0) {
		g_ctx->lostconnect_cb = lostconnect_cb;
	}

	disconnect_cb = (*env)->GetMethodID(env, objclass, "on_disconnected", "()V");
	if (disconnect_cb != 0) {
		g_ctx->disconnect_cb = disconnect_cb;
	}

	msgrecv_cb = (*env)->GetMethodID(env, objclass, "on_msgrecv", "(Ljava/lang/String;Ljava/lang/String;J)V");
	if (msgrecv_cb != 0) {
		g_ctx->msgrecv_cb = msgrecv_cb;
	}

	getcap_cb = (*env)->GetMethodID(env, objclass, "on_get_capablity", "(Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;)V");
	if (getcap_cb != 0) {
		g_ctx->getcap_cb = getcap_cb;
	}

	reportstart_cb = (*env)->GetMethodID(env, objclass, "on_autoreport_start", "(Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;)V");
	if (reportstart_cb != 0) {
		g_ctx->reportstart_cb = reportstart_cb;
	}

	reportstop_cb = (*env)->GetMethodID(env, objclass, "on_autoreport_stop", "(Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;)V");
	if (reportstop_cb != 0) {
		g_ctx->reportstop_cb = reportstop_cb;
	}

	rename_cb = (*env)->GetMethodID(env, objclass, "on_rename", "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (rename_cb != 0) {
		g_ctx->rename_cb = rename_cb;
	}

	update_cb = (*env)->GetMethodID(env, objclass, "on_update", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (update_cb != 0) {
		g_ctx->update_cb = update_cb;
	}

	reconnect_cb = (*env)->GetMethodID(env, objclass, "on_server_reconnect", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (reconnect_cb != 0) {
		g_ctx->reconnect_cb = reconnect_cb;
	}

	timetick_cb = (*env)->GetMethodID(env, objclass, "get_timetick", "()J");
	if (timetick_cb != 0) {
		g_ctx->timetick_cb = timetick_cb;
	}

	heartbeatquery_cb = (*env)->GetMethodID(env, objclass, "on_heartbeatrate_query", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (heartbeatquery_cb != 0) {
		g_ctx->heartbeatquery_cb = heartbeatquery_cb;
	}

	heartbeatupdate_cb = (*env)->GetMethodID(env, objclass, "on_heartbeatrate_update", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (heartbeatupdate_cb != 0) {
		g_ctx->heartbeatupdate_cb = heartbeatupdate_cb;
	}

	if (g_wise)
	{
		bRet = core_ex_connection_callback_set(g_wise, on_connect_cb, on_lostconnect_cb, on_disconnect_cb, on_msgrecv);

		bRet &= core_ex_iot_callback_set(g_wise, on_get_capability, on_start_report, on_stop_report);

		bRet &= core_ex_action_callback_set(g_wise, on_rename, on_update);

		bRet &= core_ex_server_reconnect_callback_set(g_wise, on_server_reconnect);

		bRet &= core_ex_time_tick_callback_set(g_wise, get_timetick);

		bRet &= core_ex_heartbeat_callback_set(g_wise, on_heartbeatrate_query, on_heartbeatrate_update);
	}
		
	printf("core_ex_callback_set %s\n", bRet ? "true" : "false");

	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1action_1response(JNIEnv *env, jobject thisObj, jint cmdid, jstring sessoinid, jboolean success, jstring tenantid, jstring devid)
{
	bool bRet = false;
	const char *nativeSessionID = (*env)->GetStringUTFChars(env, sessoinid, 0);
	const char *nativeTenantID = (*env)->GetStringUTFChars(env, tenantid, 0);
	const char *nativeDevID = (*env)->GetStringUTFChars(env, devid, 0);
	if (g_wise)
		bRet = core_ex_action_response(g_wise, cmdid, nativeSessionID, success, nativeTenantID, nativeDevID);
	printf("core_ex_action_response %d %s\n", cmdid, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, sessoinid, nativeSessionID);
	(*env)->ReleaseStringUTFChars(env, tenantid, nativeTenantID);
	(*env)->ReleaseStringUTFChars(env, devid, nativeDevID);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1heartbeatratequery_1response(JNIEnv *env, jobject thisObj, jint heartbeatrate, jstring sessoinid, jstring tenantid, jstring devid)
{
	bool bRet = false;
	const char *nativeSessionID = (*env)->GetStringUTFChars(env, sessoinid, 0);
	const char *nativeTenantID = (*env)->GetStringUTFChars(env, tenantid, 0);
	const char *nativeDevID = (*env)->GetStringUTFChars(env, devid, 0);
	if (g_wise)
		bRet = core_heartbeatratequery_response(g_wise, heartbeatrate, nativeSessionID, nativeTenantID, nativeDevID);
	printf("core_heartbeatratequery_response %d %s\n", heartbeatrate, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, sessoinid, nativeSessionID);
	(*env)->ReleaseStringUTFChars(env, tenantid, nativeTenantID);
	(*env)->ReleaseStringUTFChars(env, devid, nativeDevID);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1tls_1set(JNIEnv *env, jobject thisObj, jstring cafile, jstring capath, jstring certfile, jstring keyfile, jstring password)
{
	bool bRet = false;
	const char *nativeCAFile = (*env)->GetStringUTFChars(env, cafile, 0);
	const char *nativeCAPath = (*env)->GetStringUTFChars(env, capath, 0);
	const char *nativeCertFile = (*env)->GetStringUTFChars(env, certfile, 0);
	const char *nativeKeyFile = (*env)->GetStringUTFChars(env, keyfile, 0);
	const char *nativePassWD = (*env)->GetStringUTFChars(env, password, 0);
	if (g_wise)
		bRet = core_ex_tls_set(g_wise, nativeCAFile, nativeCAPath, nativeCertFile, nativeKeyFile, nativePassWD);
	printf("core_ex_tls_set %s\n", bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, cafile, nativeCAFile);
	(*env)->ReleaseStringUTFChars(env, capath, nativeCAPath);
	(*env)->ReleaseStringUTFChars(env, certfile, nativeCertFile);
	(*env)->ReleaseStringUTFChars(env, keyfile, nativeKeyFile);
	(*env)->ReleaseStringUTFChars(env, password, nativePassWD);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1tls_1psk_1set(JNIEnv *env, jobject thisObj, jstring psk, jstring identity, jstring ciphers)
{
	bool bRet = false;
	const char *nativePSK = (*env)->GetStringUTFChars(env, psk, 0);
	const char *nativeIdentity = (*env)->GetStringUTFChars(env, identity, 0);
	const char *nativeCiphers = (*env)->GetStringUTFChars(env, ciphers, 0);
	if (g_wise)
		bRet = core_ex_tls_psk_set(g_wise, nativePSK, nativeIdentity, nativeCiphers);
	printf("core_ex_tls_psk_set %s\n", bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, psk, nativePSK);
	(*env)->ReleaseStringUTFChars(env, identity, nativeIdentity);
	(*env)->ReleaseStringUTFChars(env, ciphers, nativeCiphers);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1connect(JNIEnv *env, jobject thisObj, jstring strServerIP, jint iServerPort, jstring strConnID, jstring strConnPW)
{
	bool bRet = false;
	const char *nativeServerIP = (*env)->GetStringUTFChars(env, strServerIP, 0);
	const char *nativeConnID = (*env)->GetStringUTFChars(env, strConnID, 0);
	const char *nativeConnPW = (*env)->GetStringUTFChars(env, strConnPW, 0);
	if (g_wise)
		bRet = core_ex_connect(g_wise, nativeServerIP, iServerPort, nativeConnID, nativeConnPW);
	printf("core_ex_connect %s\n", bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, strServerIP, nativeServerIP);
	(*env)->ReleaseStringUTFChars(env, strConnID, nativeConnID);
	(*env)->ReleaseStringUTFChars(env, strConnPW, nativeConnPW);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_WISECoreJNI_core_1disconnect(JNIEnv *env, jobject thisObj, jboolean bForce)
{
	if (g_wise)
		core_ex_disconnect(g_wise, bForce);
	printf("core_ex_disconnect \n");
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1device_1register(JNIEnv *env, jobject thisObj)
{
	bool bRet = false;
	if (g_wise)
		bRet = core_ex_device_register(g_wise);
	printf("core_ex_device_register \n");
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1heartbeat_1send(JNIEnv *env, jobject thisObj)
{
	bool bRet = false;
	if (g_wise)
		bRet = core_ex_heartbeat_send(g_wise);
	printf("core_ex_heartbeat_send \n");
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1publish(JNIEnv *env, jobject thisObj, jstring topic, jstring pkt, jlong pktlength, jint retain, jint qos)
{
	bool bRet = false;
	const char *nativeTopic = (*env)->GetStringUTFChars(env, topic, 0);
	const char *nativePacket = (*env)->GetStringUTFChars(env, pkt, 0);
	if (g_wise)
		bRet = core_ex_publish(g_wise, nativeTopic, nativePacket, pktlength, retain, qos);
	printf("core_ex_publish %s %s %s\n", nativeTopic, nativePacket, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, topic, nativeTopic);
	(*env)->ReleaseStringUTFChars(env, pkt, nativePacket);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1subscribe(JNIEnv *env, jobject thisObj, jstring topic, jint qos)
{
	bool bRet = false;
	const char *nativeTopic = (*env)->GetStringUTFChars(env, topic, 0);
	if (g_wise)
		bRet = core_ex_subscribe(g_wise, nativeTopic, qos);
	printf("core_ex_subscribe %s %s\n", nativeTopic, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, topic, nativeTopic);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_WISECoreJNI_core_1unsubscribe(JNIEnv *env, jobject thisObj, jstring topic)
{
	bool bRet = false;
	const char *nativeTopic = (*env)->GetStringUTFChars(env, topic, 0);
	if (g_wise)
		bRet = core_ex_unsubscribe(g_wise, nativeTopic);
	printf("core_ex_unsubscribe %s %s\n", nativeTopic, bRet ? "true" : "false");
	(*env)->ReleaseStringUTFChars(env, topic, nativeTopic);
	return bRet ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_WISECoreJNI_core_1address_1get(JNIEnv *env, jobject thisObj)
{
	char address[16] = { 0 };
	bool bRet = false;
	jstring jstrAddress = NULL;
	if (g_wise)
		bRet = core_ex_address_get(g_wise, address);
	printf("core_ex_address_get %s %s\n", address, bRet ? "true" : "false");
	if(bRet)
		jstrAddress = (*env)->NewStringUTF(env, address);
	return jstrAddress;
}