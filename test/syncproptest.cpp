#define LOG_TAG "SyncPropTest"
#define LOG_NDEBUG 0

#include <utils/Log.h>
#include <binder/IServiceManager.h>
#include <syncprop_services/ISyncPropService.h>


using namespace android;

int main(int argc, char **argv)
{
	int ret = -1;
	sp<IServiceManager> sm = defaultServiceManager();
	sp<IBinder> binder = sm->getService(String16("SyncPropService"));
	if (binder == 0)
	{
		printf("cant get service.\n");
		return -1;
	}
	sp<ISyncPropService> service = interface_cast<ISyncPropService>(binder);
	ret = service->get_count();
	printf("keyvector count is %d\n", ret);

	return 0;
}
