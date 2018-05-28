#include "dispmanx.h"

// dispmanx_rect
dispmanx_rect::dispmanx_rect(uint32_t off_x, uint32_t off_y, uint32_t width, uint32_t height)
    : m_width(width), m_height(height), m_off_x(off_x), m_off_y(off_y) {
    update_rect();
}

dispmanx_rect &dispmanx_rect::width(uint32_t width) {
    m_width = width;
    update_rect();

    return *this;
}

dispmanx_rect &dispmanx_rect::height(uint32_t height) {
    m_width = height;
    update_rect();

    return *this;
}

dispmanx_rect &dispmanx_rect::off_x(uint32_t off_x) {
    m_off_x = off_x;
    update_rect();

    return *this;
}

dispmanx_rect &dispmanx_rect::off_y(uint32_t off_y) {
    m_off_y = off_y;
    update_rect();

    return *this;
}

uint32_t dispmanx_rect::width() const { return m_width; }

uint32_t dispmanx_rect::height() const { return m_height; }

uint32_t dispmanx_rect::off_x() const { return m_off_x; }

uint32_t dispmanx_rect::off_y() const { return m_off_y; }

void dispmanx_rect::update_rect() {
    vc_dispmanx_rect_set(&m_rect, m_off_x, m_off_y, m_width, m_height);
}

const VC_RECT_T &dispmanx_rect::rect() const { return m_rect; }

// dispmanx_modeinfo
dispmanx_modeinfo::dispmanx_modeinfo(const DISPMANX_MODEINFO_T &modeinfo) : m_modeinfo(modeinfo) {}

// dispmanx_display_input_format
dispmanx_display_input_format dispmanx_modeinfo::input_format() const {
    return (dispmanx_display_input_format)m_modeinfo.input_format;
}

// dispmanx_resource
std::optional<dispmanx_resource> dispmanx_resource::create_resource(
    const VC_IMAGE_TYPE_T &image_type, uint32_t width, uint32_t height) {
    uint32_t native_image_handle = 0;
    DISPMANX_RESOURCE_HANDLE_T handle =
        vc_dispmanx_resource_create(image_type, width, height, &native_image_handle);

    if (handle == 0) {
        return {};
    }

    return dispmanx_resource(handle, image_type, width, height);
}

dispmanx_resource::dispmanx_resource(const DISPMANX_RESOURCE_HANDLE_T &handle,
                                     const VC_IMAGE_TYPE_T &image_type, uint32_t width,
                                     uint32_t height)
    : m_handle(handle), m_image_type(image_type), m_dimensions(0, 0, width, height) {}

dispmanx_resource::dispmanx_resource(dispmanx_resource &&other)
    : m_handle(std::move(other.m_handle)),
      m_image_type(other.m_image_type),
      m_dimensions(other.m_dimensions) {
    other.m_handle = 0;
}

dispmanx_resource::~dispmanx_resource() { vc_dispmanx_resource_delete(m_handle); }

const DISPMANX_RESOURCE_HANDLE_T &dispmanx_resource::handle() const { return m_handle; }

const dispmanx_rect &dispmanx_resource::dimensions() const { return m_dimensions; }

// dispmanx_transform
dispmanx_transform::dispmanx_transform(const orientation &orient, const flipped &flip,
                                       const snapshot_behaviour &behaviour)
    : m_orient(orient), m_flip(flip), m_behaviour(behaviour) {}

DISPMANX_TRANSFORM_T dispmanx_transform::native_transform() const {
    return (DISPMANX_TRANSFORM_T)((int)m_orient | (int)m_flip | (int)m_behaviour);
}

// dispmanx_alpha
dispmanx_alpha::dispmanx_alpha(DISPMANX_FLAGS_ALPHA_T alpha_flags, uint32_t opacity,
                               const dispmanx_resource *resource)
    : m_flags(alpha_flags), m_opacity(opacity), m_resource(resource) {}

DISPMANX_FLAGS_ALPHA_T dispmanx_alpha::flags() const { return m_flags; }

uint32_t dispmanx_alpha::opacity() const { return m_opacity; }

const dispmanx_resource *dispmanx_alpha::mask() const { return m_resource; }

// dispmanx_element_handle
dispmanx_element_handle::dispmanx_element_handle(const DISPMANX_ELEMENT_HANDLE_T &handle)
    : m_handle(handle) {}

dispmanx_element_handle::dispmanx_element_handle(dispmanx_element_handle &&other)
    : m_handle(other.handle()) {
    other.m_handle = 0;
}

const DISPMANX_ELEMENT_HANDLE_T &dispmanx_element_handle::handle() const { return m_handle; }

// dispmanx_update
// TODO implement and use clamp
std::optional<dispmanx_element_handle> dispmanx_update::add_element(
    int32_t layer, const dispmanx_rect &dest_region, const dispmanx_resource &resource,
    const dispmanx_rect &src_region, const dispmanx_protection &protection,
    const dispmanx_alpha &alpha_wrapper, const dispmanx_clamp &clamp,
    const dispmanx_transform &transform) {
    VC_DISPMANX_ALPHA_T alpha{alpha_wrapper.flags(), alpha_wrapper.opacity(),
                              alpha_wrapper.mask() ? alpha_wrapper.mask()->handle() : 0};
    DISPMANX_ELEMENT_HANDLE_T handle = vc_dispmanx_element_add(
        m_update, m_display.handle(), layer, &dest_region.rect(), resource.handle(),
        &src_region.rect(), (DISPMANX_PROTECTION_T)protection, &alpha, nullptr,
        transform.native_transform());

    if (handle == 0) {
        return {};
    }

    return dispmanx_element_handle(handle);
}

dispmanx_update::dispmanx_update(DISPMANX_UPDATE_HANDLE_T &update, dispmanx_display &display)
    : m_update(update), m_display(display) {}

dispmanx_update::~dispmanx_update() {
    if (!m_submitted) {
        submit_update();
    }
}

void dispmanx_update::change_element_source(const dispmanx_element_handle &element,
                                            dispmanx_resource &resource) {
    vc_dispmanx_element_change_source(m_update, element.handle(), resource.handle());
}

void dispmanx_update::change_element_layer(const dispmanx_element_handle &element, int32_t layer) {
    vc_dispmanx_element_change_layer(m_update, element.handle(), layer);
}

void dispmanx_update::signal_element_modification(const dispmanx_element_handle &element,
                                                  const dispmanx_rect &region) {
    vc_dispmanx_element_modified(m_update, element.handle(), &region.rect());
}

void dispmanx_update::remove_element(const dispmanx_element_handle &element) {
    vc_dispmanx_element_remove(m_update, element.handle());
}

void dispmanx_update::submit_update() {
    if (m_submitted) {
        return;
    }

    m_submitted = true;
    vc_dispmanx_update_submit_sync(m_update);
}

// dispmanx_display
std::optional<dispmanx_display> dispmanx_display::open_display(ids display_id) {
    auto display = vc_dispmanx_display_open((uint8_t)display_id);

    if (display == 0) {
        return {};
    }

    return dispmanx_display{display};
}

dispmanx_display::dispmanx_display(DISPMANX_DISPLAY_HANDLE_T &handle) : m_handle(handle) {}

dispmanx_display::dispmanx_display(dispmanx_display &&other) : m_handle(other.m_handle) {
    other.m_handle = 0;
}

dispmanx_display::~dispmanx_display() {
    if (m_handle != 0) {
        vc_dispmanx_display_close(m_handle);
    }
}

dispmanx_modeinfo dispmanx_display::current_mode() const {
    DISPMANX_MODEINFO_T modeinfo;
    vc_dispmanx_display_get_info(m_handle, &modeinfo);

    return dispmanx_modeinfo{modeinfo};
}

const DISPMANX_DISPLAY_HANDLE_T &dispmanx_display::handle() const { return m_handle; }

std::optional<dispmanx_update> dispmanx_display::start_update(int32_t priority) {
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(priority);

    if (update == 0) {
        return {};
    }

    return dispmanx_update(update, *this);
}
