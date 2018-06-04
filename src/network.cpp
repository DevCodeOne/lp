#include <string.h>
#include <signal.h>

#include "network.h"

std::optional<network_control> network_control::open_control(const fs::path &config_path) {
    if (!lock_file(config_path)) {
        return {};
    }

    return network_control(config_path);
}

network_control::network_control(const fs::path &config_path) : m_config_path(config_path) {
    start_network();
}

network_control::network_control(network_control &&other)
    : m_config_path(other.m_config_path), m_hostapd_pid(other.m_hostapd_pid) {
    other.m_config_path = "";
    other.m_hostapd_pid = 0;
}

network_control::~network_control() {
    if (m_config_path != "") {
        unlock_file(m_config_path);
    }
}

network_control &network_control::operator=(network_control &&other) {
    network_control tmp(std::move(other));
    swap(tmp);

    return *this;
}

void network_control::swap(network_control &other) {
    using std::swap;

    swap(m_config_path, other.m_config_path);
    swap(m_hostapd_pid, other.m_hostapd_pid);
}

const fs::path &network_control::config_path() const { return m_config_path; }

bool network_control::start_network() {
    int ret = fork();
    if (ret == -1) {
        return false;
    }

    // child
    if (ret > 0) {
        m_hostapd_pid = ret;
    } else if (ret == 0) {
        // Set streams so the output can be written to the logs
        char *arguments[] = {strdup(hostapd_path), strdup(hostapd_argument), nullptr};

        if (execv(hostapd_path, arguments) == -1) {
            // Error executing hostapd
        }
    }

    return false;
}

bool network_control::apply_config() {
    return kill(m_hostapd_pid, SIGHUP) != -1;
}

bool network_control::lock_file(const fs::path &config_path) {
    std::lock_guard<std::mutex> lock_guard{_lock_table_mutex};
    // file can be used if it isn't found in the lock table or it is found but not locked
    auto result = _lock_table.find(config_path);
    bool lockable = result == _lock_table.cend() || !result->second;

    if (lockable) {
        _lock_table[config_path] = true;
    }

    return lockable;
}

bool network_control::unlock_file(const fs::path &config_path) {
    auto result = _lock_table.find(config_path);

    bool unlockable = result != _lock_table.cend() || result->second;

    if (!unlockable) {
        _lock_table[config_path] = false;
    }

    return unlockable;
}

void swap(network_control &lhs, network_control &rhs) { lhs.swap(rhs); }
