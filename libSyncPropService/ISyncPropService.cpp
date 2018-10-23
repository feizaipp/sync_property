#define LOG_TAG "ISyncPropService"
#define LOG_NDEBUG 0
#include <utils/Log.h>

#include <stdint.h>
#include <sys/types.h>

#include <binder/Parcel.h>
#include <syncprop_services/ISyncPropService.h>


namespace android {

enum {
	SYNC_PROP_HOST_TO_VP = IBinder::FIRST_CALL_TRANSACTION,
	SYNC_PROP_VP_TO_HOST,
	SYNC_PROP_VP_GET_HOST,
	GET_COUNT,
};

class BpSyncPropService : public BpInterface<ISyncPropService>
{
public:
	BpSyncPropService(const sp<IBinder>& impl)
	    : BpInterface<ISyncPropService>(impl)
	{
	}

	virtual void sync_prop_host_to_vp(String16 &key, String16 &value)
	{
		Parcel data, reply;

		data.writeInterfaceToken(ISyncPropService::getInterfaceDescriptor());
		data.writeString16(key);
		data.writeString16(value);
		remote()->transact(SYNC_PROP_HOST_TO_VP, data, &reply);
	}

	virtual void sync_prop_vp_to_host(String16 &key, String16 &value)
	{
		Parcel data, reply;

		data.writeInterfaceToken(ISyncPropService::getInterfaceDescriptor());
		data.writeString16(key);
		data.writeString16(value);
		remote()->transact(SYNC_PROP_VP_TO_HOST, data, &reply);
	}

	virtual String16 get_host_property(String16 &key)
	{
		Parcel data, reply;

		data.writeInterfaceToken(ISyncPropService::getInterfaceDescriptor());
		data.writeString16(key);
		remote()->transact(SYNC_PROP_VP_GET_HOST, data, &reply);
		String16 value = reply.readString16();
		return value;
	}

	virtual int get_count()
	{
		Parcel data, reply;

		data.writeInterfaceToken(ISyncPropService::getInterfaceDescriptor());
		remote()->transact(GET_COUNT, data, &reply);
		return reply.readInt32();
	}
};


IMPLEMENT_META_INTERFACE(SyncPropService, "android.radio.ISyncPropService");

// ----------------------------------------------------------------------

status_t BnSyncPropService::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	//ALOGV("zongpeng func:%s code:%d", __func__, code);
	switch (code) {
	case SYNC_PROP_HOST_TO_VP: {
		CHECK_INTERFACE(ISyncPropService, data, reply);
		String16 key = data.readString16();
		String16 value = data.readString16();
		sync_prop_host_to_vp(key, value);
		return NO_ERROR;
	}

	case SYNC_PROP_VP_TO_HOST: {
		CHECK_INTERFACE(ISyncPropService, data, reply);
		String16 key = data.readString16();
		String16 value = data.readString16();
		sync_prop_vp_to_host(key, value);
		return NO_ERROR;
	}

	case SYNC_PROP_VP_GET_HOST: {
		CHECK_INTERFACE(ISyncPropService, data, reply);
		String16 key = data.readString16();
		String16 value = get_host_property(key);
		reply->writeString16(value);
		return NO_ERROR;
	}

	case GET_COUNT: {
		int ret = -1;
		CHECK_INTERFACE(ISyncPropService, data, reply);
		ret = get_count();
		reply->writeInt32(ret);
		return NO_ERROR;
	}

	default:
	        return BBinder::onTransact(code, data, reply, flags);
	}
}

// ----------------------------------------------------------------------------

}; // namespace android

