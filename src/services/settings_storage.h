#pragma once
// Zephyr modules
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <errno.h>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Services {
    class SettingsStorage {
      public:
        using key_t = std::string_view;
        // Delete copy constructor and assignment operator to enforce singleton pattern
        SettingsStorage(const SettingsStorage &)            = delete;
        SettingsStorage &operator=(const SettingsStorage &) = delete;
        static SettingsStorage &getInstance() {
            static SettingsStorage instance;
            return instance;
        };
        int init();

        void SetKey(key_t key, void* data, size_t size);
        void GetKey(key_t key, void* data, size_t size);

      private:
        SettingsStorage();
    };
} // namespace Services