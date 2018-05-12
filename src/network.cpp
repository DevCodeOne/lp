#include <algorithm>
#include <utility>

#include <logger.h>
#include "network.h"

namespace network {

    wpa_network_id::wpa_network_id(int32_t id) : m_id(id) {}

    int32_t wpa_network_id::id() const { return m_id; }

    bool operator==(const wpa_network_id &lhs, const wpa_network_id &rhs) {
        return lhs.m_id == rhs.m_id;
    }

    std::ostream &operator<<(std::ostream &ostream, const wpa_network_id &id) {
        ostream << id.id();
        return ostream;
    }

    bool operator!=(const wpa_network_id &lhs, const wpa_network_id &rhs) { return !(lhs == rhs); }

    wpa_interface_path::wpa_interface_path(fs::path ctrl_dir, fs::path interface)
        : m_ctrl_dir(std::move(ctrl_dir)), m_interface(std::move(interface)) {}

    fs::path wpa_interface_path::path() const { return m_interface; }

    wpa_interface wpa_interface_path::open_interface() const { return wpa_interface(*this); }

    wpa_interface::wpa_interface(wpa_interface &&device) noexcept
        : m_interface_path(std::move(device.m_interface_path)), m_ctrl(std::move(device.m_ctrl)) {}

    wpa_interface::wpa_interface(wpa_interface_path interface_path)
        : m_interface_path(std::move(interface_path)),
          m_ctrl(wpa_ctrl_open(m_interface_path.path().c_str()), wpa_ctrl_close) {}

    wpa_interface::operator bool() const { return m_ctrl != nullptr; }

    wpa_interface &wpa_interface::operator=(wpa_interface &&other) noexcept {
        swap(other);

        return *this;
    }

    void wpa_interface::swap(wpa_interface &other) noexcept {
        using fs::swap;
        using std::swap;

        swap(m_interface_path, other.m_interface_path);
        swap(m_ctrl, other.m_ctrl);
    }

    std::string wpa_interface::status() {
        return wpa_command::execute_command<wpa_commands::STATUS>(*this);
    }

    std::string wpa_interface::verbose_status() {
        return wpa_command::execute_command<wpa_commands::STATUS_VERBOSE>(*this);
    }

    void wpa_interface::logon() { return wpa_command::execute_command<wpa_commands::LOGON>(*this); }

    void wpa_interface::logoff() {
        return wpa_command::execute_command<wpa_commands::LOGOFF>(*this);
    }

    void wpa_interface::reassociate() {
        return wpa_command::execute_command<wpa_commands::REASSOCIATE>(*this);
    }

    void wpa_interface::reconnect() {
        return wpa_command::execute_command<wpa_commands::RECONNECT>(*this);
    }

    void wpa_interface::preauth(const wpa_bssid &bssid) {
        // return wpa_command::execute_command<wpa_commands::PREAUTH>(*this, bssid);
    }

    void wpa_interface::attach() {
        return wpa_command::execute_command<wpa_commands::ATTACH>(*this);
    }
    void wpa_interface::detach() {
        return wpa_command::execute_command<wpa_commands::DETACH>(*this);
    }
    void wpa_interface::reconfigure() {
        return wpa_command::execute_command<wpa_commands::RECONFIGURE>(*this);
    }
    void wpa_interface::terminate() {
        return wpa_command::execute_command<wpa_commands::TERMINATE>(*this);
    }

    void set_bssid(const wpa_network_id &id, const wpa_bssid &bssid);
    std::vector<wpa_network> list_networks();
    void disconnect();
    void scan_async();
    void scan_sync();
    void select_network(const wpa_network_id &id);

    void wpa_interface::enable_network(const wpa_network_id &id) {
        return wpa_command::execute_command<wpa_commands::ENABLE_NETWORK>(*this, id);
    }

    void disable_network(const wpa_network_id &id);
    wpa_network_id add_network();

    void swap(wpa_interface &lhs, wpa_interface &rhs) { lhs.swap(rhs); }

    std::vector<wpa_interface_path> wpa_manager::get_interfaces(const fs::path &ctrl_dir) {
        std::vector<wpa_interface_path> devices;

        for (auto &current_device : fs::directory_iterator(ctrl_dir)) {
            devices.emplace_back(ctrl_dir, current_device.path());
        }

        return devices;
    }

}  // namespace network
