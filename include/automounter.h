#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

class automounter {
   public:
    automounter();
    ~automounter();

    void start();
    void stop();

    class Constants {
       public:
        Constants() = delete;

        static inline constexpr std::chrono::milliseconds no_device_sleep{15};
    };

   private:
    static void automounter_thread();

    static inline std::atomic_bool _thread_started = false;
    static inline std::atomic_bool _thread_stop = false;
    static inline std::thread _usb_watch_thread;
    static inline std::recursive_mutex _instance_mutex;
    static inline std::atomic_int _instance_count = 0;
};
