#include <string.h>
#include <pthread.h>

#include <jni.h>
#include <android/log.h>

#include <antistrophe.h>

#define LOG_TAG "antistrophe-jni"

struct java_object_t
{
	jfieldID native_id;

	jmethodID on_connect_id;
	jmethodID on_disconnect_id;
	jmethodID on_error_id;

	jmethodID on_receive_roster_id;
};

static struct java_object_t java_object;

typedef struct {
	xmpp_conn_t *conn;
	xmpp_ctx_t *ctx;
	JNIEnv *env;
	jobject thiz;
} xmpp_native_client;

static int roster_handler(xmpp_conn_t * const conn, xmpp_request_status_t status, void * const userdata)
{
	xmpp_native_client *native = (xmpp_native_client*)userdata;
	JNIEnv *env = native->env;
	int i, len;
	xmpp_user_t **users;
	jclass jcls;
	jobjectArray jnames, jjids; 

	xmpp_lock_users(conn);
	users = xmpp_get_users(conn, &len);

	jcls = (*env)->FindClass(env, "java/lang/String");
	jnames = (*env)->NewObjectArray(env, len, jcls, NULL);
	jjids = (*env)->NewObjectArray(env, len, jcls, NULL);

	for (i = 0; i < len; ++i) {
		(*env)->SetObjectArrayElement(env, jnames, i, (*env)->NewStringUTF(env, users[i]->name));
		(*env)->SetObjectArrayElement(env, jjids, i, (*env)->NewStringUTF(env, users[i]->jid));
		(*native->env)->CallVoidMethod(native->env, native->thiz, java_object.on_receive_roster_id, jnames, jjids);
	}

	xmpp_unlock_users(conn);

	return 0;
}

static void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status,
                  const int error, xmpp_stream_error_t * const stream_error,
                  void * const userdata) {

	xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
	xmpp_native_client *native = (xmpp_native_client*)userdata;

	if (status == XMPP_CONN_CONNECT) {
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "connected");
		(*native->env)->CallVoidMethod(native->env, native->thiz, java_object.on_connect_id, NULL);
    } else if (status == XMPP_CONN_DISCONNECT) {
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "disconnected");
		(*native->env)->CallVoidMethod(native->env, native->thiz, java_object.on_disconnect_id, NULL);
        xmpp_stop(ctx);
    } else {
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "error");
		(*native->env)->CallVoidMethod(native->env, native->thiz, java_object.on_error_id, NULL);
        xmpp_stop(ctx);
	}
}


static void android_xmpp_log_handler(void * const userdata, const xmpp_log_level_t level, const char * const area,
							 const char * const msg) 

{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, msg);
}

static void set_xmpp_native(JNIEnv *env, jobject thiz, xmpp_native_client *native)
{
	(*env)->SetIntField(env, thiz, java_object.native_id, (jint)native);
}

static xmpp_native_client* get_xmpp_native(JNIEnv *env, jobject thiz) 
{
	return (xmpp_native_client*)(*env)->GetIntField(env, thiz, java_object.native_id);
}

void Java_jp_mixi_xmpp_antistrophe_XMPPClient_initialize(JNIEnv* env, jclass jclazz)
{
    xmpp_initialize();

	java_object.native_id = (*env)->GetFieldID(env, jclazz, "mNative", "I");

	java_object.on_connect_id = (*env)->GetMethodID(env, jclazz, "onConnect", "()V");
	java_object.on_disconnect_id = (*env)->GetMethodID(env, jclazz, "onDisconnect", "()V");
	java_object.on_error_id = (*env)->GetMethodID(env, jclazz, "onError", "()V");

	java_object.on_receive_roster_id = (*env)->GetMethodID(env, jclazz, "onReceiveRoster",
														   "([Ljava/lang/String;[Ljava/lang/String;)V");
}

void Java_jp_mixi_xmpp_antistrophe_XMPPClient_destroy(JNIEnv* env, jclass jclazz)
{
	xmpp_shutdown();
}

static int connect(xmpp_native_client *native, jstring jhost, jstring jjid, jstring jpass)
{
	JNIEnv *env = native->env;
	const char *host = (*env)->GetStringUTFChars(env, jhost, NULL);
	const char *jid = (*env)->GetStringUTFChars(env, jjid, NULL);
	const char *pass = (*env)->GetStringUTFChars(env, jpass, NULL);
	int ret;

    xmpp_conn_set_jid(native->conn, jid);
    xmpp_conn_set_pass(native->conn, pass);

    ret = xmpp_connect_client(native->conn, host, 0, conn_handler, native);
	if (ret < 0)
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Failed to connect to %s", host);

	(*env)->ReleaseStringUTFChars(env, jhost, host);
	(*env)->ReleaseStringUTFChars(env, jjid, jid);
	(*env)->ReleaseStringUTFChars(env, jpass, pass);

	return ret;
}

int Java_jp_mixi_xmpp_antistrophe_XMPPClient_connect(JNIEnv *env, jobject thiz,
												 jstring jhost, jstring jjid, jstring jpass, jboolean debug)
{
    xmpp_conn_t *conn;
	xmpp_ctx_t *ctx;
    xmpp_log_t *log = NULL;
	xmpp_native_client *native;
	int ret = -1;

	if (debug == JNI_TRUE) {
		log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
		log->handler = android_xmpp_log_handler;
	}
	
    ctx = xmpp_ctx_new(NULL, log);
	if (!ctx) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to allocate xmpp_ctx_t");
		goto err_ctx_new;
	}

    conn = xmpp_conn_new(ctx);
	if (!conn) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to allocate xmpp_conn_t");
		goto err_conn_new;
	}

	native = malloc(sizeof(xmpp_native_client));
	native->ctx = ctx;
	native->conn = conn;
	native->env = env;
	native->thiz = thiz;

	ret = connect(native, jhost, jjid, jpass);
	if (ret < 0)
		goto err_connect;
	
	set_xmpp_native(env, thiz, native);
	ret = 0;
	xmpp_run(ctx);
	set_xmpp_native(env, thiz, NULL);

err_connect:
	free(native);
	xmpp_conn_release(conn);
err_conn_new:
    xmpp_ctx_free(ctx);	
err_ctx_new:
	return ret;
}

void Java_jp_mixi_xmpp_antistrophe_XMPPClient_sendRawString(JNIEnv *env, jobject thiz, jstring jstr)
{
	const char *str = (*env)->GetStringUTFChars(env, jstr, NULL);
	xmpp_native_client *native = get_xmpp_native(env, thiz);

    xmpp_send_raw_string(native->conn, str);

	(*env)->ReleaseStringUTFChars(env, jstr, str);
}

void Java_jp_mixi_xmpp_antistrophe_XMPPClient_close(JNIEnv *env, jobject thiz)
{
}

void Java_jp_mixi_xmpp_antistrophe_XMPPClient_sendMessage(JNIEnv *env, jobject thiz, jstring jmsg, jstring jto)
{
	const char *msg = (*env)->GetStringUTFChars(env, jmsg, NULL);
	const char *to = (*env)->GetStringUTFChars(env, jto, NULL);
	xmpp_native_client *native = get_xmpp_native(env, thiz);

	xmpp_send_message(native->conn, msg, to);

	(*env)->ReleaseStringUTFChars(env, jmsg, msg);
	(*env)->ReleaseStringUTFChars(env, jto, to);
}

void Java_jp_mixi_xmpp_antistrophe_XMPPClient_sendRoster(JNIEnv *env, jobject thiz)
{
	xmpp_native_client *native = get_xmpp_native(env, thiz);
	xmpp_send_roster(native->conn, roster_handler, native);
}

void Java_jp_mixi_xmpp_antistrophe_XMPPClient_sendPrecense(JNIEnv *env, jobject thiz, jint jshow, jstring jstatus)
{
	const char *status = (*env)->GetStringUTFChars(env, jstatus, NULL);
	xmpp_native_client *native = get_xmpp_native(env, thiz);

	xmpp_send_presence(native->conn, jshow, status);

	(*env)->ReleaseStringUTFChars(env, jstatus, status);
}
