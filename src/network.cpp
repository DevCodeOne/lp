#include "posix.h"

#include <signal.h>
#include <string.h>
#include <wait.h>

#include "logger.h"
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
        int log_fd =
            open(hostapd_default_log_file, O_RDWR | O_CLOEXEC | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);

        if (log_fd != -1) {
            if (ftruncate(log_fd, 0) == -1) {
                logger::get()->warn("Couldn't truncate the log file");
            }

            int ret = dup2(log_fd, STDOUT_FILENO);

            if (ret == -1) {
                logger::get()->warn("Couldn't change the fd of stdout to point to log_fd");
            }
            ret = dup2(log_fd, STDERR_FILENO);

            if (ret == -1) {
                logger::get()->warn("Couldn't change the fd of stderr to point to log_fd");
            }
        } else {
            logger::get()->warn("Couldn't create logging file for hostapd to write in");
        }

        char *arguments[] = {strdup(hostapd_path), strdup(hostapd_argument), strdup("-dd"),
                             nullptr};

        if (execv(hostapd_path, arguments) == -1) {
            // Error executing hostapd
        }
    }

    return false;
}

bool network_control::apply_config() {
    logger::get()->info("Sending signal to {}", m_hostapd_pid);
    int result = kill(m_hostapd_pid, SIGTERM);

    if (result == -1) {
        logger::get()->critical("Couldn't terminate hostapd process");

        return false;
    }

    int status = 0;
    result = waitpid(m_hostapd_pid, &status, 0);

    if (result == -1) {
        logger::get()->critical("Couldn't wait for the termination of hostapd");
    }

    return start_network();
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
