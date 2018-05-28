#include <algorithm>

#include <QPainter>

#include "logger.h"
#include "secondary_display.h"

secondary_display::secondary_display(int refresh_rate, QWidget *parent) : QWidget(parent) {
    auto display = dispmanx_display::open_display(dispmanx_display::ids::FORCE_LCD);

    if (!display) {
        logger::get()->critical("Couldn't open display");
        return;
    }

    m_display = std::make_unique<dispmanx_display>(std::move(*display));
    dispmanx_modeinfo mode = m_display->current_mode();

    logger::get()->info("modeinfo : width={}, height={}", mode.width(), mode.height());
    auto resource =
        dispmanx_resource::create_resource(VC_IMAGE_RGBA32, mode.width(), mode.height());

    if (!resource) {
        logger::get()->critical("Couldn't create resource");
        return;
    }

    m_resource = std::make_unique<dispmanx_resource>(std::move(*resource));

    startTimer(1000 / refresh_rate);
}

void secondary_display::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.fillRect(0, 0, 1280, 720, QColor(255, 0, 20, 255));
    painter.drawText(100, 100, "Hello World");
    logger::get()->info("Update");
}

void secondary_display::timerEvent(QTimerEvent *) { update_display(); }

void secondary_display::update_display() {
    // TODO add check if content has changed
    auto update = m_display->start_update();

    if (!update) {
        logger::get()->warn("Couldn't update display");
        return;
    } else {
        logger::get()->info("Updating display");
    }

    auto dimensions = m_resource->dimensions();
    if (!m_element) {
        auto element_handle = update->add_element(
            1, dimensions, *m_resource, dimensions, dispmanx_protection::NONE,
            dispmanx_alpha(DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, nullptr), dispmanx_clamp(),
            dispmanx_transform());

        if (!element_handle) {
            logger::get()->critical("Couldn't add element to update");
            return;
        }

        m_element = std::make_unique<dispmanx_element_handle>(std::move(*element_handle));
    }

    QImage content = grab().toImage();

    int pixels[100*100];
    memset((void *) pixels, 255, 100*100);
    m_resource->write_data(VC_IMAGE_RGBA32, pixels, dimensions);
    update->change_element_source(*m_element, *m_resource.get());

    update->submit_update();
}
