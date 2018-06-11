#pragma once

#include "posix.h"

#include <experimental/filesystem>
#include <map>
#include <mutex>
#include <optional>

namespace fs = std::experimental::filesystem;

class network_control {
   public:
    /** \brief Constructs a network_control class, also locks the configuration file
     * Tries to construct a network_control class, if it fails it returns an empty std::optional
     * which signals the failure. This happens if the config file is already used by another
     * network_control instance
     */
    static std::optional<network_control> open_control(const fs::path &config_path);

    network_control(const network_control &other) = delete;
    /** \brief Move constructs a network_control instance
     * Also clears the values of the other network_control instance
     */
    network_control(network_control &&other);
    /** \brief Deconstructs the network_control instance
     * Deconstructs the network_control instance and unlocks the configuration file so it can be
     * used by another network_control instance again
     */
    ~network_control();

    network_control &operator=(const network_control &other) = delete;
    network_control &operator=(network_control &&other);
    void swap(network_control &other);

    /** \brief Retrieve value of property key
     */
    std::optional<std::string> get_value(const std::string &key) const;
    /** \brief Set value of property key
     * If the key already exists it will override the value of that key otherwise it will create a
     * new entry
     */
    void set_value(const std::string &key, const std::string &value);
    /** \brief Applies configuration changes
     * Changes that were made by the user get written in the provided configuration file of this
     * instance The associated hostapd process is then terminated and restarted after the
     * configuration has been written
     */
    bool apply_config();
    bool terminate_network();

    const fs::path &config_path() const;

    static inline constexpr char hostapd_default_conf_path[] = "/etc/hostapd/hostapd.conf";
    static inline constexpr char hostapd_default_wpa_conf_path[] = "/etc/hostapd/hostapd.wpa_psk";
    static inline constexpr char hostapd_default_log_file[] = "/var/log/lp_network.log";

   private:
    static bool lock_file(const fs::path &config_path);
    static bool unlock_file(const fs::path &config_path);

    /** \brief Constructs a network_control instance with the specified config_path
     * The instantiation of a network_control instance will also start a hostapd process with the
     * specified config
     */
    network_control(const fs::path &config_path);
    /** \brief start a hostapd process with the provided config
     */
    bool start_network();
    /** \brief Populates the key value map with the properties of the provided configuration file
     */
    bool read_values_from_config();
    /** \brief Writes the values in the configuration file
     */
    bool write_values_to_config();

    static inline std::map<fs::path, bool> _lock_table;
    static inline std::mutex _lock_table_mutex;
    static inline constexpr char hostapd_path[] = "/usr/bin/hostapd";
    static inline constexpr const char *const hostapd_argument = hostapd_default_conf_path;

    fs::path m_config_path;
    std::map<std::string, std::string> m_values;
    pid_t m_hostapd_pid;
};

void swap(network_control &lhs, network_control &rhs);
