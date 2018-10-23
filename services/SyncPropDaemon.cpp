#define LOG_TAG "SyncPropDaemon"
#define LOG_NDEBUG 0

#include <utils/Log.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <syncprop_services/SyncPropService.h>

using namespace android;

int main(int argc, char **argv)
{
	sp<IServiceManager> sm(defaultServiceManager());
	ALOGD("start SyncProp service");

    sp<SyncPropService> sps = new SyncPropService();
	sm->addService(String16(SyncPropService::getServiceName()), sps, false);

	sps->run();

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
}

