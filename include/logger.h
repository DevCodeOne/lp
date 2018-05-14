#pragma once

#include <memory>

#include "spdlog/spdlog.h"

class logger {
    public:
        static std::shared_ptr<spdlog::logger> get();
    private:

        static inline std::shared_ptr<spdlog::logger> _logger_instance = nullptr;
};
