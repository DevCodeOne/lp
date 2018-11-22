#include <cstdlib>

#include <QApplication>

#include "bcm_host_wrapper.h"

#include "automounter.h"
#include "logger.h"
#include "network.h"
#include "secondary_display.h"

int main(int argc, char *argv[]) {
    auto logger = logger::get();
    auto bcm_lib_instance = bcm_host_wrapper::bcm_host::instance();

    automounter mounter;
    auto network = network_control::open_control(network_control::hostapd_default_conf_path);

    if (!network) {
        logger->critical("Couldn't start network exiting");
        return EXIT_FAILURE;
    }

    network->set_value("wpa_passphrase", network_control::random_password(8));
    logger::get()->info("Passphrase : {}", *network->get_value("wpa_passphrase"));
    network->apply_config();

    mounter.start();

    QApplication a(argc, argv);

    secondary_display display(50);

    return a.exec();
}
