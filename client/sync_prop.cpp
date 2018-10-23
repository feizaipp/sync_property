/*
 * sync_prop.c
 */

#define LOG_TAG "SYNC_PROP"

#include <cutils/log.h>
#include <binder/IServiceManager.h>
#include <syncprop_services/ISyncPropService.h>
#include <utils/String8.h>
#include <utils/String16.h>
#include <cutils/properties.h>
#include <fission_cutils/container.h>

using namespace android;

const char *vp_get_host_prop_name[] = {
	"ril.iccid.sim1",
	"ril.iccid.sim2",
	"gsm.baseband.capability",
	"gsm.gcf.testmode",
	"gsm.project.baseband.2",
	"gsm.project.baseband",
	"gsm.ril.ct3g.2",
	"gsm.ril.ct3g",
	"gsm.ril.eboot",
	"gsm.ril.init",
	"gsm.ril.uicctype.2",
	"gsm.ril.uicctype",
	"gsm.serial",
	"ril.active.md",
	"ril.ready.sim",
	"ril.ipo.radiooff",
	"gsm.sim.ril.phbready",
	"gsm.sim.ril.phbready.2",
	"mtk.md1.status",
	"ril.cdma.emdstatus.send",
	"gsm.network.type",
	"gsm.operator.alpha",
	"gsm.operator.iso-country",
	"gsm.operator.isroaming",
	"gsm.sim.operator.default-name",
	"gsm.sim.operator.default-name.2",
	"persist.radio.simswitch",
};

const char *host_notify_vp_prop_name[] = {
	"ril.ready.sim",
	"ril.active.md",
};

const char *vp_notify_host_prop_name[] = {
	"gsm.sim.operator.numeric"
};

const char *host_get_vp_prop_name[] = {
	
};

enum {
	VP_GET_HOST_PROP = 0,
	HOST_GET_VP_PROP,
	SYNC_PROP,
	SYNC_PROP_NUM,
};

static sp<ISyncPropService> get_srv_sps(void)
{
	sp<ISyncPropService> srv_sps = NULL;
	
	sp<IBinder> binder = defaultServiceManager()->checkService(String16("SyncPropService"));
	if (binder != NULL) {
		srv_sps = interface_cast<ISyncPropService>(binder);
	} else {
		ALOGE("get SyncPropService failed");
	}

	return srv_sps;
}

int main(int argc, char **argv)
{
	unsigned int i = 0;
	unsigned int cmd = 0;
	sp<ISyncPropService> srv_sps = NULL;
	unsigned int mode;

	if (argc < 2) {
        ALOGE("input error!\n");
        return -1;
    }
	cmd = atoi(argv[1]);
	if (cmd >= SYNC_PROP_NUM) {
        ALOGE("input error! cmd = %d\n", cmd);
        return -1;
    }

	get_container_mode(&mode);
	ALOGV("mode:%d cmd:%d", mode, cmd);
	srv_sps = get_srv_sps();
	if (srv_sps == NULL) {
		if (SYNC_PROP == cmd) {
			char value[PROPERTY_VALUE_MAX];
			property_get(argv[2], value, "");
			ALOGE("name:%s, value:%s line:%d", argv[2], value, __LINE__);
		}
		return -1;
	}
	if (cmd == VP_GET_HOST_PROP) {
		if ((mode == ENUM_SYS_GENERAY) || (mode == ENUM_SYS_SECURITY)) {
			for (i=0; i<sizeof(vp_get_host_prop_name)/sizeof(char *); i++) {
				String16 name = String16(vp_get_host_prop_name[i]);
				String16 value = srv_sps->get_host_property(name);
				property_set(String8(name).string(), String8(value).string());
				ALOGV("name:%s value:%s line:%d", String8(name).string(), String8(value).string(), __LINE__);
			}
		}
	} else if (cmd == SYNC_PROP) {
		char value[PROPERTY_VALUE_MAX];
		property_get(argv[2], value, "");
		String16 _name = String16(argv[2]);
		String16 _value = String16(value);
		if (mode == ENUM_SYS_HOST) {
			srv_sps->sync_prop_host_to_vp(_name, _value);
		} else if (mode == ENUM_SYS_GENERAY) {
			srv_sps->sync_prop_vp_to_host(_name, _value);
		}
		ALOGV("name:%s value:%s line:%d", String8(_name).string(), String8(_value).string(), __LINE__);
	}

	return 0;
}
