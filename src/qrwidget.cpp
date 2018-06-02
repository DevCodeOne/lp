#include <algorithm>

#include <QPainter>

#include "qrwidget.h"

qrwidget::qrwidget(const std::string &message) : m_message(message) { update_qrcode(); }

void qrwidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setBrush(QColor("white"));
    painter.setBrush(Qt::SolidPattern);
    painter.setPen(Qt::NoPen);

    painter.drawRect(0, 0, width(), height());

    painter.setBrush(QColor("black"));

    float square_size = std::min(width(), height()) / (float)m_side_length;

    for (int i = 0; i < m_side_length; ++i) {
        for (int j = 0; j < m_side_length; ++j) {
            if ((m_data[i * m_side_length + j] & 1) == 1) {
                painter.drawRect(j * square_size, i * square_size, (j + 1) * square_size, (i + 1) * square_size);
            }
        }
    }
}

void qrwidget::update_qrcode() {
    QRcode *qrcode = nullptr;
    QRcode_List *qrcodes =
        QRcode_encodeStringStructured(m_message.c_str(), 1, QR_ECLEVEL_H, QR_MODE_8, 1);
    QRcode_List *entry = qrcodes;

    while (entry != nullptr) {
        qrcode = entry->code;
        m_data = std::make_unique<unsigned char[]>(qrcode->width * qrcode->width);
        m_side_length = qrcode->width;

        memcpy(m_data.get(), qrcode->data, qrcode->width * qrcode->width);
    }

    QRcode_List_free(entry);
}

const std::string &qrwidget::message() const { return m_message; }

qrwidget &qrwidget::message(const std::string &message) {
    m_message = message;
    update_qrcode();
    repaint();
    return *this;
}
