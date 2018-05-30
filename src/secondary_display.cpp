#include <algorithm>

#include <QPainter>

#include "logger.h"
#include "secondary_display.h"

secondary_display::secondary_display(int refresh_rate, QWidget *parent)
    : QWidget(parent), m_pixmap(1280, 720) {
    auto display = dispmanx_display::open_display((dispmanx_display::ids)6);

    if (!display) {
        logger::get()->critical("Couldn't open display");
        return;
    }

    m_display = std::make_unique<dispmanx_display>(std::move(*display));
    dispmanx_modeinfo mode = m_display->current_mode();

    logger::get()->info("modeinfo : width={}, height={}", mode.width(), mode.height());
    auto resource = dispmanx_resource::create_resource(VC_IMAGE_RGBA32, 1280, 720);

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
}

void secondary_display::timerEvent(QTimerEvent *) { update_display(); }

void secondary_display::update_display() {
    // TODO add check if content has changed
    dispmanx_rect rect(0, 0, 1280, 720);

    QImage content = grab().toImage();
    QImage result = content.convertToFormat(QImage::Format::Format_RGBA8888);

    for (int i = 0; i < 720; ++i) {
        const int32_t *data = (const int32_t *)result.constScanLine(i);

        int32_t *const pixel_iterator = m_pixmap[i];
        for (int j = 0; j < 1280; ++j) {
            pixel_iterator[j] = data[j];
        }
    }

    m_resource->write_data(VC_IMAGE_RGBA32, m_pixmap, rect);

    auto update = m_display->start_update(0);

    if (!update) {
        logger::get()->warn("Couldn't update display");
        return;
    }

    auto dimensions = m_resource->dimensions();
    if (!m_element) {
        auto element_handle = update->add_element(
            2000, dimensions, *m_resource, dimensions, dispmanx_protection::NONE,
            dispmanx_alpha(DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, nullptr), dispmanx_clamp(),
            dispmanx_transform());

        if (!element_handle) {
            logger::get()->critical("Couldn't add element to update");
            return;
        }

        m_element = std::make_unique<dispmanx_element_handle>(std::move(*element_handle));
    }

    update->change_element_source(*m_element, *m_resource);

    update->submit_update();
}
