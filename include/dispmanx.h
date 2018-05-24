#pragma once

#include <optional>

#include "bcm_host_wrapper.h"

class dispmanx_rect {
   public:
    dispmanx_rect(uint32_t off_x, uint32_t off_y, uint32_t width, uint32_t height);
    dispmanx_rect(const dispmanx_rect &other);
    dispmanx_rect(dispmanx_rect &&other);

    dispmanx_rect &width(uint32_t width);
    dispmanx_rect &height(uint32_t height);
    dispmanx_rect &off_x(uint32_t off_x);
    dispmanx_rect &off_y(uint32_t off_y);

    uint32_t width() const;
    uint32_t height() const;
    uint32_t off_x() const;
    uint32_t off_y() const;

    const VC_RECT_T &rect() const;

   private:
    // maybe use this struct lazy so only update when needed ?
    VC_RECT_T m_rect;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_off_x;
    uint32_t m_off_y;
    bool m_changed;
};

class dispmanx_resource {};

class dispmanx_element {};

class dispmanx_transform {};

enum struct dispmanx_display_input_format {
    INVALID = VCOS_DISPLAY_INPUT_FORMAT_INVALID,
    RGB888 = VCOS_DISPLAY_INPUT_FORMAT_RGB888,
    RGB565 = VCOS_DISPLAY_INPUT_FORMAT_RGB565
};

class dispmanx_mode_info {
   public:
    dispmanx_mode_info(const dispmanx_mode_info &other) = default;
    dispmanx_mode_info(dispmanx_mode_info &&other) = default;

    decltype(DISPMANX_MODEINFO_T::width) width() const;
    decltype(DISPMANX_MODEINFO_T::height) height() const;
    decltype(DISPMANX_MODEINFO_T::display_num) display_number() const;
    dispmanx_display_input_format input_format() const;

   private:
    dispmanx_mode_info(const DISPMANX_MODEINFO_T &mode_info);
    DISPMANX_MODEINFO_T m_mode;
};

class dispmanx_display {
   public:
    dispmanx_display(const dispmanx_display &other) = delete;
    dispmanx_display(dispmanx_display &&other) = default;
    ~dispmanx_display();

    static std::optional<dispmanx_display> open_display(uint8_t display_id);

    dispmanx_mode_info current_mode() const;

   private:
    dispmanx_display(DISPMANX_DISPLAY_HANDLE_T display_handle);

    DISPMANX_DISPLAY_HANDLE_T display_handle;
};
