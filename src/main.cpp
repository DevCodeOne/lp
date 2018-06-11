#include <cstdlib>

#include <QApplication>

#include "logger.h"
#include "network.h"
#include "automounter.h"
#include "secondary_display.h"

int main(int argc, char *argv[]) {
    auto logger = logger::get();
    bcm_host::initialize();

    automounter mounter;
    auto network = network_control::open_control(network_control::hostapd_default_conf_path);

    if (!network) {
        logger->critical("Couldn't start network exiting");
        return EXIT_FAILURE;
    }

    mounter.start();

    QApplication a(argc, argv);

    secondary_display display(50);

    return a.exec();
}
