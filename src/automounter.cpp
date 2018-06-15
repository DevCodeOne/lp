#include "posix.h"

#include "sys/mount.h"
#include "sys/stat.h"

#include "automounter.h"
#include "logger.h"
#include "udevpp.h"

automounter::automounter() {
    std::lock_guard<std::recursive_mutex> guard(_instance_mutex);
    ++_instance_count;
}

automounter::~automounter() {
    std::lock_guard<std::recursive_mutex> guard(_instance_mutex);

    --_instance_count;

    if (_instance_count == 0) {
        stop();
    }
}

void automounter::start() {
    std::lock_guard<std::recursive_mutex> guard(_instance_mutex);

    if (!_thread_started) {
        _usb_watch_thread = std::thread(automounter::automounter_thread);
        _thread_started = true;
    }
}

void automounter::stop() {
    std::lock_guard<std::recursive_mutex> guard(_instance_mutex);

    if (_thread_started) {
        _thread_stop = true;

        if (_usb_watch_thread.joinable()) {
            _usb_watch_thread.join();
            _thread_started = false;
        } else {
            // Couldn't join thread
        }
    }
}

void automounter::automounter_thread() {
    udevpp::UDev udev;

    if (!udev) {
        // Error udev is not valid
    }

    udevpp::UDevMonitor device_monitor(udev, udevpp::UDevMonitor::Mode::NONBLOCKING);

    if (!device_monitor) {
        // Error udev_monitor couldn't be created
    }

    while (!_thread_stop) {
        auto device = device_monitor.receive_device();

        if (!device) {
            std::this_thread::sleep_for(no_device_sleep);
            continue;
        }

        auto attributes = device->get_device_attributes();
        auto result = attributes.find("DEVNAME");

        if (result == attributes.cend()) {
            // attribute "DEVNAME" not found
            continue;
        }

        std::string device_name = result->second;

        if (!std::regex_match(device_name, _device_name_regex)) {
            // This is not an event for a partition
            continue;
        }

        result = attributes.find("ID_FS_TYPE");

        if (result == attributes.cend()) {
            // attribute "ID_FS_TYPE" not found
            continue;
        }

        std::string filesystem_type = result->second;

        // TODO mount partition

        fs::path mount_point(mount_point_base);
        mount_point /= fs::path(device_name.substr(device_name.find_last_of("/") + 1));

        result = attributes.find("ACTION");

        if (result == attributes.cend()) {
            continue;
        }

        std::string action = result->second;

        if (action == "add") {
            logger::get()->info("Mounting {} with filesystem {} to {}", device_name,
                                filesystem_type, mount_point.c_str());
            int dircreat_result = mkdir(mount_point.c_str(), S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);

            if (dircreat_result) {
                logger::get()->info("Failed to create directory {}", mount_point.c_str());
                continue;
            }

            int mount_result = mount(device_name.c_str(), mount_point.c_str(),
                                     filesystem_type.c_str(), MS_NOEXEC | MS_NOSUID, "");

            if (mount_result) {
                logger::get()->info("Failed to mount {}", device_name);
            }

            // TODO enter mount information in table so it can be deleted once the device gets unmounted
        }
    }
}
