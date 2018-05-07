#pragma once

#include "wpa_ctrl.h"

#include <experimental/filesystem>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace network {

    class wpa_device;

    class wpa_manager {
       public:
        wpa_manager(const wpa_device &device);
        ~wpa_manager();

        static std::vector<wpa_device> get_devices(const fs::path &ctrl_dir);

        static inline constexpr char default_control_location[] = "/var/run/hostapd";

       private:
    };

    class wpa_device {
       public:
        ~wpa_device() = default;
        wpa_device(const wpa_device &device) = delete;
        wpa_device(wpa_device &&device) noexcept;

       private:
        wpa_device(fs::path ctrl_dir, fs::path device);

        const fs::path m_ctrl_dir;
        const fs::path m_device;
        wpa_ctrl *m_ctrl;

        friend std::vector<wpa_device> wpa_manager::get_devices(const fs::path &);
    };

}  // namespace network
