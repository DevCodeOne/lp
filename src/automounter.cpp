#include "automounter.h"

#include "udevpp.h"
#include "logger.h"

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
        // Error udev_monitor coulnd't be created
    }

    while (!_thread_stop) {
        auto device = device_monitor.receive_device();

        if (device) {
            auto result = device->get_device_attributes();

            for (const auto &[name, value] : result) {
                logger::get()->info("udev usb {} = {}", name, value);
            }
        } else {
            std::this_thread::sleep_for(Constants::no_device_sleep);
        }
    }
}
