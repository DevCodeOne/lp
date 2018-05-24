#pragma once

#include <mutex>

#include "bcm_host.h"

class bcm_host {
   public:
    static void initialize();

   private:
    static inline bool _is_initialized = false;
    static inline std::mutex _bcm_mutex;
};
