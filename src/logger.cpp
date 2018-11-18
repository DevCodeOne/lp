#include <cstddef>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ansicolor_sink.h"

#include "logger.h"

std::shared_ptr<spdlog::logger> logger::get() {
    if (_logger_instance == nullptr) {
        _logger_instance = spdlog::create<spdlog::sinks::ansicolor_stdout_sink_mt>("console");
    }

    return _logger_instance;
}
