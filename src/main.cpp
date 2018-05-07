#include <cstdlib>

#include <iostream>

#include "spdlog/spdlog.h"

#include <network.h>

int main() {

    auto logger = spdlog::stdout_color_mt("console");

    // if (logger == nullptr) {
    //     std::cout << "Couldn't open logger" << std::endl;
    // }

    // wpa_ctrl *ctrl = wpa_ctrl_open("/var/run/hostapd/wlan0");
    // wpa_ctrl *logger_ctrl = wpa_ctrl_open("/var/run/hostapd/wlan0");

    // if (logger_ctrl == nullptr) {
    //     logger->critical("Couldn't get a wpa_ctrl handle");
    //     exit(EXIT_FAILURE);
    // } else {
    //     wpa_ctrl_attach(logger_ctrl);
    //     wpa_ctrl_detach(logger_ctrl);
    //     wpa_ctrl_close(logger_ctrl);
    // }

    // if (ctrl == nullptr) {
    //     logger->critical("Couldn't get a wpa_ctrl handle");
    //     exit(EXIT_FAILURE);
    // }

    // wpa_ctrl_close(ctrl);
    auto devices = network::wpa_manager::get_devices(network::wpa_manager::default_control_location);

    return EXIT_SUCCESS;
}
