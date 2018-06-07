#include <algorithm>

#include <QPainter>

#include "logger.h"
#include "qrwidget.h"

qrwidget::qrwidget(const std::string &message) : m_message(message) {
    update_qrcode();
    setMaximumWidth(m_side_length * 10);
    setMaximumHeight(m_side_length * 10);
}

void qrwidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(),
                     QBrush(QColor("white"), Qt::BrushStyle::SolidPattern));
    QBrush brush = QBrush(QColor("black"), Qt::BrushStyle::SolidPattern);

    int square_size = std::floor(std::min(width(), height()) / m_side_length);

    for (int i = 0; i < m_side_length; ++i) {
        for (int j = 0; j < m_side_length; ++j) {
            if ((m_data[i * m_side_length + j] & 1) == 1) {
                painter.fillRect(j * square_size, i * square_size, square_size, square_size, brush);
            }
        }
    }

    painter.end();
}

void qrwidget::update_qrcode() {
    QRcode *qrcode = QRcode_encodeString8bit(m_message.c_str(), 1, QR_ECLEVEL_H);
    if (qrcode->width != m_side_length || !m_data) {
        m_data = std::make_unique<unsigned char[]>(qrcode->width * qrcode->width);
    }

    m_side_length = qrcode->width;

    memcpy(m_data.get(), qrcode->data, qrcode->width * qrcode->width);

    free(qrcode);
}

const std::string &qrwidget::message() const { return m_message; }

qrwidget &qrwidget::message(const std::string &message) {
    m_message = message;
    update_qrcode();
    repaint();
    return *this;
}
