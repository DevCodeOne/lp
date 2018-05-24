#include "bcm_host_wrapper.h"

void bcm_host::initialize() {
    std::lock_guard<std::mutex> guard(_bcm_mutex);

    if (!_is_initialized) {
        bcm_host_init();
    }
}
