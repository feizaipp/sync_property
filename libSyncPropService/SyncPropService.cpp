#define LOG_TAG "SyncPropService"
#define LOG_NDEBUG 0

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <utils/Trace.h>
#include <binder/Parcel.h>
#include <utils/String16.h>
#include <utils/threads.h>
#include <utils/Atomic.h>
#include <syncprop_services/SyncPropService.h>
#include <fission_cutils/container.h>

namespace android {

SyncPropService::SyncPropService() : BnSyncPropService()
{
	isReady1 = false;
	isReady2 = false;
	srv_fss = NULL;
	srv_fns = NULL;
	obj = 0;
	started = false;
	pthread_mutex_init(&s_startupMutex, NULL);
	pthread_cond_init(&s_startupCond, NULL);
	srv_fhs = get_srv_fhs();
	while (srv_fhs == NULL) {
		sleep(1);
		srv_fhs = get_srv_fhs();
	}
}

void SyncPropService::setProperty(sp<IFissionHostService> &srv_fhs, String16 &key, String16 &value)
{
	String16 value1;
	value1 = srv_fhs->getProperty(key);
	if (!value.compare(value1)) {
		ALOGD("property equals");
		return;
	}
	srv_fhs->setProperty(key, value);
}

void SyncPropService::setProperty(sp<IFissionGenerayService> &srv_fns, String16 &key, String16 &value)
{
	String16 value1;
	value1 = srv_fns->getProperty(key);
	if (!value.compare(value1)) {
		ALOGD("property equals");
		return;
	}
	srv_fns->setProperty(key, value);
}

void SyncPropService::setProperty(sp<IFissionSecurityService> &srv_fss, String16 &key, String16 &value)
{
	String16 value1;
	value1 = srv_fss->getProperty(key);
	if (!value.compare(value1)) {
		ALOGD("property equals");
		return;
	}
	srv_fss->setProperty(key, value);
}

String16 SyncPropService::getProperty(sp<IFissionHostService> &srv_fhs, String16 &key)
{
	String16 value;
	value = srv_fhs->getProperty(key);

	return value;
}

String16 SyncPropService::getProperty(sp<IFissionGenerayService> &srv_fns, String16 &key)
{
	String16 value;
	value = srv_fns->getProperty(key);

	return value;
}

String16 SyncPropService::getProperty(sp<IFissionSecurityService> &srv_fss, String16 &key)
{
	String16 value;
	value = srv_fss->getProperty(key);

	return value;
}

void SyncPropService::runThread()
{
	unsigned int i = 0;
	if (this->obj == 0) {
		pthread_mutex_lock(&s_startupMutex);
		started = true;
		pthread_cond_broadcast(&s_startupCond);
		pthread_mutex_unlock(&s_startupMutex);
		while (srv_fss == NULL) {
			srv_fss = get_srv_fss();
			sleep(1);
		}
		/* sync prop */
		Mutex::Autolock _l(mLockh2vp);
		for (i=0; i<kvhost2vp.size(); i++) {
			String16 key = kvhost2vp.keyAt(i);
			String16 value = kvhost2vp.valueAt(i);
			setProperty(srv_fss, key, value);
		}
		isReady1 = true;
	} else if (this->obj == 1) {
		pthread_mutex_lock(&s_startupMutex);
		started = true;
		pthread_cond_broadcast(&s_startupCond);
		pthread_mutex_unlock(&s_startupMutex);
		while (srv_fns == NULL) {
			srv_fns = get_srv_fns();
			sleep(1);
		}
		/* sync prop */
		Mutex::Autolock _l(mLockh2vp);
		for (i=0; i<kvhost2vp.size(); i++) {
			String16 key = kvhost2vp.keyAt(i);
			String16 value = kvhost2vp.valueAt(i);
			setProperty(srv_fns, key, value);
		}
		isReady2 = true;
	}
}

void *SyncPropService::threadStart(void *obj)
{
	SyncPropService *me = reinterpret_cast<SyncPropService *>(obj);
	me->runThread();
	return NULL;
}

sp<IFissionGenerayService> SyncPropService::get_srv_fns(void)
{
	sp<IFissionGenerayService> srv_fns = NULL;

	sp<IBinder> binder = defaultServiceManager()->checkService(String16("FissionGeneraySvc"));
	if (binder != NULL) {
		srv_fns = interface_cast<IFissionGenerayService>(binder);
	} else {
		ALOGE("get FissionGeneraySvc failed");
	}

	return srv_fns;
}

sp<IFissionSecurityService> SyncPropService::get_srv_fss(void)
{
	sp<IFissionSecurityService> srv_fss = NULL;

	sp<IBinder> binder = defaultServiceManager()->checkService(String16("FissionSecuritySvc"));
	if (binder != NULL) {
		srv_fss = interface_cast<IFissionSecurityService>(binder);
	} else {
		ALOGE("get FissionSecuritySvc failed");
	}

	return srv_fss;
}

sp<IFissionHostService> SyncPropService::get_srv_fhs(void)
{
	sp<IFissionHostService> srv_fhs = NULL;

	sp<IBinder> binder = defaultServiceManager()->checkService(String16("FissionHostSvc"));
	if (binder != NULL) {
		srv_fhs = interface_cast<IFissionHostService>(binder);
	} else {
		ALOGE("get FissionHostSvc failed");
	}

	return srv_fhs;
}

void SyncPropService::run()
{
	pthread_attr_t attr;
	int ret;
	int fission_mode = get_fission_mode();
	
	if (fission_mode == FISSION_MODE_DOUBLE) {
		started = false;
	    pthread_mutex_lock(&s_startupMutex);
	    pthread_attr_init(&attr);
	    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		this->obj = 0;
		ret = pthread_create(&mThread1, &attr, SyncPropService::threadStart, this);
		if (ret != 0) {
	        ALOGE("Failed to create dispatch thread: %s", strerror(ret));
	        goto done1;
	    }
		while (started == false) {
		    pthread_cond_wait(&s_startupCond, &s_startupMutex);
		}
done1:
		pthread_mutex_unlock(&s_startupMutex);
	}
	started = false;
    pthread_mutex_lock(&s_startupMutex);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	this->obj = 1;
	ret = pthread_create(&mThread2, NULL, SyncPropService::threadStart, this);
	if (ret != 0) {
        ALOGE("Failed to create dispatch thread: %s", strerror(ret));
        goto done2;
    }
	while (started == false) {
	    pthread_cond_wait(&s_startupCond, &s_startupMutex);
	}

done2:
    pthread_mutex_unlock(&s_startupMutex);

}


void SyncPropService::sync_prop_host_to_vp(String16 &key, String16 &value)
{
	Mutex::Autolock _l(mLockh2vp);
	int fission_mode = get_fission_mode();

	if (fission_mode == FISSION_MODE_DOUBLE) {
		if (!isReady1 || !isReady2) {
			ALOGD("sync_prop_host_to_vp add %s:%s", String8(key).string(), String8(value).string());
			kvhost2vp.add(key, value);
		}
	} else {
		if (!isReady2) {
			ALOGD("sync_prop_host_to_vp add %s:%s", String8(key).string(), String8(value).string());
			kvhost2vp.add(key, value);
		}
	}

	if (fission_mode == FISSION_MODE_DOUBLE) {
		if (isReady1) {
			/* sync prop */
			setProperty(srv_fss, key, value);
		}
	}

	if (isReady2) {
		/* sync prop */
		setProperty(srv_fns, key, value);
	}
}

void SyncPropService::sync_prop_vp_to_host(String16 &key, String16 &value)
{
	Mutex::Autolock _l(mLockvp2h);

	if (srv_fhs == NULL) {
		ALOGD("srv_fhs is NULL");
		return;
	}
	setProperty(srv_fhs, key, value);
}

String16 SyncPropService::get_host_property(String16 &key)
{
	Mutex::Autolock _l(mLockh2vp);
	String16 value;
	
	value = getProperty(srv_fhs, key);

	return value;
}


int SyncPropService::get_count()
{
	Mutex::Autolock _l(mLockh2vp);

	return kvhost2vp.size();
}

SyncPropService::~SyncPropService()
{

}

}; // namespace android
