// App modules
#include "system_manager.h"
// Zephyr modules
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(system_manager, LOG_LEVEL_INF);

using Services::SystemManager;

SystemManager& SystemManager::getInstance() {
    static SystemManager instance;
    return instance;
}

int SystemManager::init() {
    LOG_INF("SystemManager initialized.");
    return 0;
}