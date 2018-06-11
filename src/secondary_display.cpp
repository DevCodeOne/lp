#include <algorithm>

#include <QPainter>

#include "logger.h"
#include "secondary_display.h"

secondary_display::secondary_display(int refresh_rate)
    : QTabWidget(nullptr),
      m_pixmap(16, 16),
      m_qrwidget("Hello World fjdslfgsdjfsdklgsdj"),
      m_button("TestButton", nullptr) {
    auto display = dispmanx_display::open_display(dispmanx_display::ids::FORCE_LCD);

    if (!display) {
        logger::get()->critical("Couldn't open display");
        return;
    }

    m_display = std::make_unique<dispmanx_display>(std::move(*display));
    dispmanx_modeinfo mode = m_display->current_mode();

    setMinimumSize(QSize(mode.width(), mode.height()));
    showFullScreen();

    logger::get()->info("modeinfo : width={}, height={}", mode.width(), mode.height());
    auto resource =
        dispmanx_resource::create_resource(VC_IMAGE_RGBA32, mode.width(), mode.height());
    m_pixmap = dispmanx_pixmap<int32_t>(mode.width(), mode.height());

    if (!resource) {
        logger::get()->critical("Couldn't create resource");
        return;
    }

    m_resource = std::make_unique<dispmanx_resource>(std::move(*resource));
    m_button.setMaximumHeight(40);
    m_button.setMaximumWidth(200);

    addTab(&m_qrwidget, "Control Tab");
    addTab(&m_button, "Test Tab");

    startTimer(1000 / refresh_rate);
}

void secondary_display::paintEvent(QPaintEvent *) {
    if (!m_is_grabbing) {
        m_update_needed = true;
    }

    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 255));
}

QImage secondary_display::pixels() {
    m_is_grabbing = true;
    QImage content = grab().toImage();
    m_is_grabbing = false;
    return content.convertToFormat(QImage::Format::Format_RGBA8888);
}

void secondary_display::timerEvent(QTimerEvent *) { update_display(); }

void secondary_display::update_display() {
    if (m_update_needed) {
        dispmanx_rect rect(m_resource->dimensions());

        auto screen_contents = pixels();

        for (size_t i = 0; i < rect.height(); ++i) {
            memcpy(m_pixmap[i], screen_contents.constScanLine(i), sizeof(int32_t) * rect.width());
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
                dispmanx_alpha(DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, nullptr),
                dispmanx_clamp(), dispmanx_transform());

            if (!element_handle) {
                logger::get()->critical("Couldn't add element to update");
                return;
            }

            m_element = std::make_unique<dispmanx_element_handle>(std::move(*element_handle));
        }

        update->change_element_source(*m_element, *m_resource);

        update->submit_update();
        m_update_needed = false;
    }
}
