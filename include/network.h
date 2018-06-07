#pragma once

#include "posix.h"

#include <experimental/filesystem>
#include <map>
#include <mutex>
#include <optional>

namespace fs = std::experimental::filesystem;

class network_control {
   public:
    static std::optional<network_control> open_control(const fs::path &config_path);

    network_control(const network_control &other) = delete;
    network_control(network_control &&other);
    ~network_control();

    network_control &operator=(const network_control &other) = delete;
    network_control &operator=(network_control &&other);
    void swap(network_control &other);

    std::optional<std::string> get_value(const std::string &key) const;
    bool set_value(const std::string &key, const std::string &value) const;
    bool apply_config();

    const fs::path &config_path() const;

    static inline constexpr char hostapd_default_conf_path[] = "/etc/hostapd/hostapd.conf";
    static inline constexpr char hostapd_default_wpa_conf_path[] = "/etc/hostapd/hostapd.wpa_psk";
    static inline constexpr char hostapd_default_log_file[] = "/tmp/lp_network.log";

   private:
    static bool lock_file(const fs::path &config_path);
    static bool unlock_file(const fs::path &config_path);

    network_control(const fs::path &config_path);
    bool start_network();

    static inline std::map<fs::path, bool> _lock_table;
    static inline std::mutex _lock_table_mutex;
    static inline constexpr char hostapd_path[] = "/usr/bin/hostapd";
    static inline constexpr const char *const hostapd_argument = hostapd_default_conf_path;

    fs::path m_config_path;
    std::map<std::string, std::string> m_values;
    pid_t m_hostapd_pid;
};

void swap(network_control &lhs, network_control &rhs);
