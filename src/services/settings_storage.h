#pragma once
// Standard modules
#include <array>
#include <cstddef>
#include <cstdint>
#include <errno.h>
#include <string_view>
#include <vector>
// Zephyr modules
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

namespace Services {
    class SettingsStorage {
      public:
        using key_t                          = std::string_view;
        constexpr static key_t KEY_CELL_APN  = "cell/apn";
        constexpr static key_t KEY_CELL_PASS = "cell/pass";

      // private:
      //   struct storageElement {
      //       key_t key;
      //       size_t size;
      //       // type_t type;
      //   };
      //   std::vector<storageElement> storageElementsVector;

      // public:
        // Delete copy constructor and assignment operator to enforce singleton pattern
        SettingsStorage(const SettingsStorage &)            = delete;
        SettingsStorage &operator=(const SettingsStorage &) = delete;
        static SettingsStorage &getInstance() {
            static SettingsStorage instance;
            return instance;
        };
        int init();

        int SetKey(key_t key, void *data, size_t size);
        int GetKey(key_t key, void *data, size_t size);
        const bool isInitialized() const { return initialized; }

      private:
        SettingsStorage();
        bool initialized = false;
    };
} // namespace Services