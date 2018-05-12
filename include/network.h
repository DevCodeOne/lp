#pragma once

#include <array>
#include <experimental/filesystem>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "wpa_common.h"
#include "wpa_ctrl.h"

namespace fs = std::experimental::filesystem;

namespace network {

    class wpa_interface;

    // taken from wpa_debug.h
    enum struct wpa_debug_level : uint8_t { EXCESSIVE, MSGDUMP, DEBUG, INFO, WARNING, ERROR };

    class wpa_bssid {
       private:
        static inline constexpr size_t bssid_len = ETH_ALEN;
        std::array<uint8_t, bssid_len> bssid;
    };

    std::ostream operator<<(std::ostream &os, const wpa_bssid &bssid);

    class wpa_ssid {};

    std::ostream operator<<(std::ostream &os, const wpa_ssid &ssid);

    class wpa_network_id {
       public:
        wpa_network_id(int32_t id);

        int32_t id() const;

       private:
        int32_t m_id;

        friend bool operator==(const wpa_network_id &, const wpa_network_id &);
    };

    bool operator==(const wpa_network_id &, const wpa_network_id &);
    std::ostream &operator<<(std::ostream &ostream, const wpa_network_id &id);

    class wpa_network {
       public:
        wpa_network(const wpa_network_id &network_id, const wpa_ssid &ssid, const wpa_bssid &bssid);
        wpa_network(const wpa_network &other);
        wpa_network(wpa_network &&other);

       private:
        const wpa_network_id m_network_id;
        const wpa_ssid m_ssid;
        const wpa_bssid m_bssid;
    };

    bool operator==(const wpa_network_id &lhs, const wpa_network_id &rhs);
    bool operator!=(const wpa_network_id &lhs, const wpa_network_id &rhs);

    enum struct wpa_commands : uint8_t {
        PING,
        STATUS,
        STATUS_VERBOSE,
        SET,
        LOGON,
        LOGOFF,
        REASSOCIATE,
        RECONNECT,
        PREAUTH,
        ATTACH,
        DETACH,
        RECONFIGURE,
        TERMINATE,
        SET_BSSID,
        LIST_NETWORKS,
        DISCONNECT,
        SCAN,
        SCAN_RESULTS,
        SELECT_NETWORK,
        ENABLE_NETWORK,
        DISABLE_NETWORK,
        ADD_NETWORK,
        REMOVE_NETWORK,
        SET_NETWORK,
        GET_NETWORK,
        SAVE_CONFIG
    };

    template<wpa_commands Command>
    struct wpa_command_types {
        using Argument_Tuple = std::tuple<>;
        using Return_Type = void;
    };

    template<>
    struct wpa_command_types<wpa_commands::STATUS> {
        using Argument_Tuple = std::tuple<>;
        using Return_Type = std::string;
    };

    template<>
    struct wpa_command_types<wpa_commands::STATUS_VERBOSE> {
        using Argument_Tuple = std::tuple<>;
        using Return_Type = std::string;
    };

    template<>
    struct wpa_command_types<wpa_commands::LIST_NETWORKS> {
        using Argument_Tuple = std::tuple<>;
        using Return_Type = std::vector<wpa_network>;
    };

    template<>
    struct wpa_command_types<wpa_commands::PREAUTH> {
        using Argument_Tuple = std::tuple<wpa_bssid>;
        using Return_Type = void;
    };

    template<>
    struct wpa_command_types<wpa_commands::SET_BSSID> {
        using Argument_Tuple = std::tuple<wpa_network_id, wpa_bssid>;
        using Return_Type = void;
    };

    template<>
    struct wpa_command_types<wpa_commands::SELECT_NETWORK> {
        using Argument_Tuple = std::tuple<wpa_network_id>;
        using Return_Type = void;
    };

    template<>
    struct wpa_command_types<wpa_commands::ENABLE_NETWORK> {
        using Argument_Tuple = std::tuple<wpa_network_id>;
        using Return_Type = void;
    };
    template<>
    struct wpa_command_types<wpa_commands::DISABLE_NETWORK> {
        using Argument_Tuple = std::tuple<wpa_network_id>;
        using Return_Type = void;
    };
    template<>
    struct wpa_command_types<wpa_commands::ADD_NETWORK> {
        using Argument_Tuple = std::tuple<wpa_network_id>;
        using Return_Type = wpa_network_id;
    };

    template<>
    struct wpa_command_types<wpa_commands::REMOVE_NETWORK> {
        using Argument_Tuple = std::tuple<wpa_network_id>;
        using Return_Type = void;
    };

    template<>
    struct wpa_command_types<wpa_commands::SET_NETWORK> {
        // TODO consider implementing own values. Because of this value changes have to be made in
        // this wrapper too
        using Argument_Tuple = std::tuple<wpa_network_id, std::string, std::string>;
    };

    template<>
    struct wpa_command_types<wpa_commands::GET_NETWORK> {
        // TODO consider implementing own values. Because of this value changes have to be made in
        // this wrapper too
        using Argument_Tuple = std::tuple<wpa_network_id, std::string>;
    };

    class wpa_interface_path {
       public:
        wpa_interface_path(fs::path ctrl_dir, fs::path interface);

        fs::path path() const;
        wpa_interface open_interface() const;

       private:
        fs::path m_ctrl_dir;
        fs::path m_interface;
    };

    class wpa_manager {
       public:
        static std::vector<wpa_interface_path> get_interfaces(
            const fs::path &ctrl_dir = DEFAULT_CONTROL_LOCATION);

        static inline constexpr char DEFAULT_CONTROL_LOCATION[] = "/var/run/hostapd";
        static inline constexpr size_t MAX_ANSWER_LENGTH = 2048;
    };

    class wpa_interface {
       public:
        ~wpa_interface() = default;
        wpa_interface(const wpa_interface &device) = delete;
        wpa_interface(wpa_interface &&device) noexcept;

        explicit operator bool() const;
        wpa_interface &operator=(const wpa_interface &) = delete;
        wpa_interface &operator=(wpa_interface &&) noexcept;

        void swap(wpa_interface &other) noexcept;

        std::string status();
        std::string verbose_status();
        void logon();
        void logoff();
        void reassociate();
        void reconnect();
        void preauth(const wpa_bssid &bssid);
        void attach();
        void detach();
        void reconfigure();
        void terminate();
        void set_bssid(const wpa_network_id &id, const wpa_bssid &bssid);
        std::vector<wpa_network> list_networks();
        void disconnect();
        void scan_async();
        void scan_sync();
        void select_network(const wpa_network_id &id);
        void enable_network(const wpa_network_id &id);
        void disable_network(const wpa_network_id &id);
        wpa_network_id add_network();
        void remove_network(const wpa_network_id &id);
        // TODO variable and value
        void set_network(const wpa_network_id &id);
        // TODO variable
        void get_network(const wpa_network_id &id);
        void save_config();

       private:
        wpa_interface(wpa_interface_path interface);

        std::string execute_command(const std::string &command);

        wpa_interface_path m_interface_path;
        std::unique_ptr<wpa_ctrl, std::add_pointer_t<decltype(wpa_ctrl_close)>> m_ctrl;

        friend class wpa_interface_path;
        friend class wpa_command;
    };

    void swap(wpa_interface &lhs, wpa_interface &rhs);

    class wpa_command {
       public:
        template<wpa_commands Command, typename... Arguments>
        static typename wpa_command_types<Command>::Return_Type execute_command(
            wpa_interface &interface, Arguments... args);

       private:
        template<wpa_commands Command, typename... Arguments>
        static std::optional<std::string> build_command(Arguments... args);

        template<size_t Index, typename T>
        static void append_arguments(std::stringstream &command, const T &arguments);

        const inline static std::map<wpa_commands, std::string> command_to_string{
            {wpa_commands::PING, "PING"},
            {wpa_commands::STATUS, "STATUS"},
            {wpa_commands::STATUS_VERBOSE, "STATUS-VERBOSE"},
            {wpa_commands::SET, "SET"},
            {wpa_commands::LOGON, "LOGON"},
            {wpa_commands::LOGOFF, "LOGOFF"},
            {wpa_commands::REASSOCIATE, "REASSOCIATE"},
            {wpa_commands::RECONNECT, "RECONNECT"},
            {wpa_commands::PREAUTH, "PREAUTH"},
            {wpa_commands::ATTACH, "ATTACH"},
            {wpa_commands::DETACH, "DETACH"},
            {wpa_commands::RECONFIGURE, "RECONFIGURE"},
            {wpa_commands::TERMINATE, "TERMINATE"},
            {wpa_commands::SET_BSSID, "SET_BSSID"},
            {wpa_commands::LIST_NETWORKS, "LIST_NETWORKS"},
            {wpa_commands::DISCONNECT, "DISCONNECT"},
            {wpa_commands::SCAN, "SCAN"},
            {wpa_commands::SCAN_RESULTS, "SCAN_RESULTS"},
            {wpa_commands::SELECT_NETWORK, "SELECT_NETWORK"},
            {wpa_commands::ENABLE_NETWORK, "ENABLE_NETWORK"},
            {wpa_commands::DISABLE_NETWORK, "DISABLE_NETWORK"},
            {wpa_commands::ADD_NETWORK, "ADD_NETWORK"},
            {wpa_commands::REMOVE_NETWORK, "REMOVE_NETWORK"},
            {wpa_commands::SET_NETWORK, "SET_NETWORK"},
            {wpa_commands::GET_NETWORK, "GET_NETWORK"},
            {wpa_commands::SAVE_CONFIG, "SAVE_CONFIG"}};
    };

    template<wpa_commands Command, typename... Arguments>
    typename wpa_command_types<Command>::Return_Type wpa_command::execute_command(
        wpa_interface &interface, Arguments... args) {
        using return_type = typename wpa_command_types<Command>::Return_Type;
        auto command = *wpa_command::build_command<Command>(args...);
        char result[wpa_manager::MAX_ANSWER_LENGTH];
        size_t message_length = wpa_manager::MAX_ANSWER_LENGTH;

        int ret = wpa_ctrl_request(interface.m_ctrl.get(), command.c_str(), command.length(),
                                   result, &message_length, logger::wpa_callback);

        if (ret != 0) {
            logger::get()->critical("An error occured trying to execute a command");
        }

        if constexpr (!std::is_same_v<return_type, void>) {
            return return_type(std::string(result, message_length));
        }
    }

    template<wpa_commands Command, typename... Arguments>
    std::optional<std::string> wpa_command::build_command(Arguments... args) {
        using tuple_type = const typename wpa_command_types<Command>::Argument_Tuple;
        const tuple_type arguments{args...};

        auto command = command_to_string.find(Command);

        if (command == command_to_string.cend()) {
            return {};
        }

        std::stringstream result;
        result << command->second;
        append_arguments<0, tuple_type>(result, arguments);

        return result.str();
    }

    template<size_t Index, typename T>
    void wpa_command::append_arguments(std::stringstream &command, const T &arguments) {
        if constexpr (Index != std::tuple_size_v<T>) {
            command << " " << std::get<Index>(arguments);
            append_arguments<Index + 1>(command, arguments);
        }
    }

}  // namespace network
