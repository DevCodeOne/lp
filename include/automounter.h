#pragma once

#include <atomic>
#include <chrono>
#include <experimental/filesystem>
#include <mutex>
#include <regex>
#include <thread>

namespace fs = std::experimental::filesystem;

class automounter {
   public:
    automounter();
    ~automounter();

    void start();
    void stop();

    static inline constexpr std::chrono::milliseconds no_device_sleep{15};

   private:
    static void unmount_all();
    static void automounter_thread();

    static inline std::atomic_bool _thread_started = false;
    static inline std::atomic_bool _thread_stop = false;
    static inline std::thread _usb_watch_thread;
    static inline std::recursive_mutex _instance_mutex;
    static inline std::atomic_int _instance_count = 0;
    static inline std::map<std::string, std::string> _mounted_device;
    static inline std::regex _device_name_regex{"/dev/sd[a-z][1-9]"};
    static inline const char mount_point_base[] = "/mnt";
};
