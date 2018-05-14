#include <cstddef>

#include "spdlog/spdlog.h"

#include "logger.h"

std::shared_ptr<spdlog::logger> logger::get() {
    if (_logger_instance == nullptr) {
        _logger_instance = spdlog::stdout_color_mt("console");
    }

    return _logger_instance;
}
