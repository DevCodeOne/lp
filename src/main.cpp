#include <cstdlib>

#include "logger.h"
#include "network.h"

int main() {

    auto logger = logger::get();

    auto interface_paths = network::wpa_manager::get_interfaces();

    for (auto &interface_path : interface_paths) {
        auto interface = interface_path.open_interface();

        if (interface) {
            logger->info("{}", interface.status());
        } else {
            logger->critical("Interface is not valid");
        }
    }

    return EXIT_SUCCESS;
}
