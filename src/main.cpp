#include <cstdlib>

#include <QApplication>

#include "logger.h"
#include "network.h"
#include "secondary_display.h"

int main(int argc, char *argv[]) {
    auto logger = logger::get();
    bcm_host::initialize();

    char *env = getenv("VC_DISPLAY");

    logger->info("{}", env);

    auto network = network_control::open_control(network_control::hostapd_default_conf_path);

    if (!network) {
        logger->critical("Couldn't start network exiting");
        return EXIT_FAILURE;
    }

    QApplication a(argc, argv);

    secondary_display display(20);
    display.setMinimumSize(800, 480);
    display.showFullScreen();

    return a.exec();
}
