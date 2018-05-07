#include <iostream>

#include "network.h"

namespace network {

    wpa_device::wpa_device(wpa_device &&device) noexcept
        : m_ctrl_dir(std::move(device.m_ctrl_dir)), m_device(std::move(device.m_device)) {}

    wpa_device::wpa_device(fs::path ctrl_dir, fs::path device)
        : m_ctrl_dir(std::move(ctrl_dir)), m_device(std::move(device)) {}

    std::vector<wpa_device> wpa_manager::get_devices(const fs::path &ctrl_dir) {
        std::vector<wpa_device> devices;
        for (auto &current_device : fs::directory_iterator(ctrl_dir)) {
            devices.emplace_back(wpa_device(ctrl_dir, current_device.path()));
        }

        return devices;
    }

}  // namespace network
