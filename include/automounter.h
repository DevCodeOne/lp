#pragma once

#include <atomic>
#include <chrono>
#include <experimental/filesystem>
#include <mutex>
#include <regex>
#include <thread>

namespace fs = std::experimental::filesystem;

/** \brief This class mounts usb thumbdrives automaticly.
 */
class automounter {
   public:
    /** \brief Constructs one instance of an automounter.
     * All instances automounters work with the same data.
     */
    automounter();
    ~automounter();

    /** \brief Starts the thread which mounts the usb devices.
     * Only one thread is started further calls of this method won't start another thread.
     */
    void start();
    /** \brief Stops the automounting thread.
     * This method will block until the thread is terminated.
     */
    void stop();

    /** \brief Time to sleep if no device is found.
     */
    static inline constexpr std::chrono::milliseconds no_device_sleep{15};

   private:
    /** \brief unmount all by this automounter mounted devices.
     */
    static void unmount_all();
    /** \brief Method which will be run by the automounting thread.
     */
    static void automounter_thread();

    static inline std::atomic_bool _thread_started = false;
    static inline std::atomic_bool _thread_stop = false;
    static inline std::thread _usb_watch_thread;
    static inline std::recursive_mutex _instance_mutex;
    /** \brief Number of automounter instances.
     * If the instance count goes to zero the automounting thread will be terminated
     */
    static inline std::atomic_int _instance_count = 0;
    /** \brief The mount table of the automounter.
     * One entry corresponds to one mountpoint. The key consists of the device path e.g. /dev/sda
     * and the value will be the resulting mount point.
     */
    static inline std::map<std::string, std::string> _mount_table;
    /** \brief Regex for valid device names.
     */
    static inline std::regex _device_name_regex{"/dev/sd[a-z][1-9]"};
    /** \brief Mount base points all devices will be mounted in a directory named after their device name located in this directory.
     */
    static inline const char mount_point_base[] = "/mnt";
};
