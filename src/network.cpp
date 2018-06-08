#include "posix.h"

#include <signal.h>
#include <string.h>
#include <wait.h>

#include <fstream>
#include <string>

#include "logger.h"
#include "network.h"

std::optional<network_control> network_control::open_control(const fs::path &config_path) {
    if (!lock_file(config_path)) {
        return {};
    }

    return network_control(config_path);
}

network_control::network_control(const fs::path &config_path)
    : m_config_path(config_path), m_hostapd_pid(0) {
    read_values_from_config();
    start_network();
}

network_control::network_control(network_control &&other)
    : m_config_path(other.m_config_path),
      m_values(other.m_values),
      m_hostapd_pid(other.m_hostapd_pid) {
    other.m_config_path = "";
    other.m_hostapd_pid = 0;
    other.m_values.clear();
}

network_control::~network_control() {
    if (m_config_path != "") {
        unlock_file(m_config_path);
    }
}

network_control &network_control::operator=(network_control &&other) {
    swap(other);

    return *this;
}

void network_control::swap(network_control &other) {
    using std::swap;

    swap(m_config_path, other.m_config_path);
    swap(m_hostapd_pid, other.m_hostapd_pid);
    swap(m_values, other.m_values);
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
        int log_fd = open(hostapd_default_log_file, O_RDWR | O_CLOEXEC | O_CREAT,
                          S_IRUSR | S_IWUSR | S_IROTH);

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

std::optional<std::string> network_control::get_value(const std::string &key) const {
    auto result = m_values.find(key);

    if (result == m_values.cend()) {
        return {};
    }

    return result->second;
}

void network_control::set_value(const std::string &key, const std::string &value) {
    m_values[key] = value;
}

bool network_control::apply_config() {
    logger::get()->info("Writing config to disk");
    write_values_to_config();
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

bool network_control::read_values_from_config() {
    std::string current_line, current_key, current_value;
    std::ifstream stream(m_config_path);

    while (std::getline(stream, current_line)) {
        if (current_line.find_first_of("#") < current_line.find_first_of("=") ||
            current_line.find_first_of("=") == current_line.size() || current_line.size() == 0) {
            continue;
        }

        std::istringstream line(current_line);

        // Config file is malformed key without value
        if (!std::getline(line, current_key, '=')) {
            return false;
        }

        std::getline(line, current_value, '=');

        // Config file is malformed because a key was encountered multiple times
        if (m_values.find(current_key) != m_values.cend()) {
            return false;
        }

        m_values[current_key] = current_value;
    }

    return true;
}

bool network_control::write_values_to_config() {
    std::ofstream stream(m_config_path, std::ios_base::out | std::ios_base::trunc);

    for (const auto &value : m_values) {
        logger::get()->info("{} = {}", value.first, value.second);
        stream << value.first << "=" << value.second << '\n';
    }

    stream << std::flush;

    return true;
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
