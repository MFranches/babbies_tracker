// Zephyr modules
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <errno.h>
//App modules
#include "settings_storage.h"

using Services::SettingsStorage;

/* This module will show an example of how to use Zephyr's Settings Subsystem
 * with the NVS (Non-Volatile Storage) backend to store and retrieve
 * configuration data such as cellular APN and credentials.
 */
// Storage partition defined in the DTS
#define STORAGE_PARTITION	nvs_storage
#define STORAGE_PARTITION_ID	FIXED_PARTITION_ID(STORAGE_PARTITION)
#define FAIL_MSG "fail (err %d)\n"
#define SECTION_BEGIN_LINE \
	"\n=================================================\n"

LOG_MODULE_REGISTER(settings_storage, LOG_LEVEL_INF);

char apn[32] = "my_apn";
char pass[32] = "my_password";

/**< This gets called when the value is loaded from persistent storage with settings_load(),
 * or when using settings_runtime_set() from the runtime backend.
 */
static int cellRootHandleSet(const char *name, size_t length,
                settings_read_cb readCallBack, void *callBackArguments)
{
    const char *next;
    size_t next_len;
    int rc;
    printk("cellRootHandleSet: name=%s length=%zu", name, length);

	if (settings_name_steq(name, "apn", &next) && !next) {
		if (length != sizeof(apn)) {
			return -EINVAL;
		}
		rc = readCallBack(callBackArguments, &apn, sizeof(apn));
		printk("<cell/apn> = %s\n", apn);
		return 0;
	}

	next_len = settings_name_next(name, &next);

	if (!next) {
		return -ENOENT;
	}

	if (!strncmp(name, "pass", next_len)) {
		next_len = settings_name_next(name, &next);

		if (!next) {
			rc = readCallBack(callBackArguments, &pass, sizeof(pass));
			printk("<cell/pass> = %s\n", pass);
			return 0;
		}
		
		return -ENOENT;
	}

	return -ENOENT;
}

/**< This gets called after the settings have been loaded in full. 
 * Sometimes you don’t want an individual setting value to take effect right away, 
 * for example if there are multiple settings which are interdependent.
 */
static int cellRootHandleCommit(void)
{
	LOG_INF("Loading all settings under <cell> handler is done\n"
	"apn: %s, pass: %s\n", apn, pass);	
	return 0;
}

/**< This gets called to write all current settings. This happens when settings_save()
 * tries to save the settings or transfer to any user-implemented back-end.
 */
static int cellRootHandleExport(int (*cb)(const char *name,
			       const void *value, size_t val_len))
{
	LOG_INF("Export keys under <cell> handler\n");
	(void)cb("cell/apn", &apn, sizeof(apn));
	(void)cb("cell/pass", &pass, sizeof(pass));

	return 0;
}

/**<
 * We must register handlers to be called by the Settings API
 * They can be  dinamically or statically defined dinamically.
 */
SETTINGS_STATIC_HANDLER_DEFINE(cellRootHandle, "cell", nullptr,
			       cellRootHandleSet, cellRootHandleCommit,
			       cellRootHandleExport);


SettingsStorage::SettingsStorage() {}

/**< Initialize the settings subsystem */
int SettingsStorage::init() 
{
    int error = settings_subsys_init();
    if (error) {
        LOG_ERR("Failed to initialize settings subsystem: %d", error);
        return error;
    }

    // Register handlers
    error = settings_register(&cellRootHandle);
	if (error) {
        LOG_ERR("Failed to register cellular data handler: %d", error);
        return error;
    }
    
	error = settings_load();
	if (error) {
		LOG_ERR("Failed to load settings: %d", error);
		return error;
	}
    return 0;
}

int SettingsStorage::SetKey(key_t key, void* data, size_t size) {
	int error = settings_save_one(key.data(), data, size);
	if (error) {
		LOG_ERR("Failed to set key %s: %d", key.data(), error);
		return error;
	}
	return 0;
}