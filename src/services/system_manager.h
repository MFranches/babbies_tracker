#pragma once

namespace Services {
    class SystemManager {
      public:
        // Delete copy constructor and assignment operator to enforce singleton pattern
        SystemManager(const SystemManager &)            = delete;
        SystemManager &operator=(const SystemManager &) = delete;
        static SystemManager &getInstance();
        int init();

      private:
        SystemManager();
    };
} // namespace Services