#pragma once

#include <memory>
#include <optional>

#include "bcm_host_wrapper.h"

/** \brief This class is a wrapper for the dispmanx struct VC_RECT_T it is used, when a region has
 * to be specified.
 */
class dispmanx_rect {
   public:
    /** \brief Constructs a dispmanx_rect with off_x and off_y being the origin of the rectangle,
     * the width and height being the side lengths of the rectangle
     */
    dispmanx_rect(uint32_t off_x, uint32_t off_y, uint32_t width, uint32_t height);
    /** \brief Constructs a dispmanx_rect which is a exact copy of the provided instance.
     */
    dispmanx_rect(const dispmanx_rect &other) = default;
    /** \brief Move constructs a dispmanx_rect.
     */
    dispmanx_rect(dispmanx_rect &&other) = default;

    /** \brief Sets the width of the rectangle.
     */
    dispmanx_rect &width(uint32_t width);
    /** \brief Sets the height of the rectangle.
     */
    dispmanx_rect &height(uint32_t height);
    /** \brief Sets the x coordinate of the origin of the rectangle.
     */
    dispmanx_rect &off_x(uint32_t off_x);
    /** \brief Sets the y coordinate of the origin of the rectangle.
     */
    dispmanx_rect &off_y(uint32_t off_y);

    /** \brief Observer for the width of the rectangle.
     */
    uint32_t width() const;
    /** \brief Observer for the height of the rectangle.
     */
    uint32_t height() const;
    /** \brief Observer for the x coordinate of the origin of the rectangle.
     */
    uint32_t off_x() const;
    /** \brief Observer for the x coordinate of the origin of the rectangle.
     */
    uint32_t off_y() const;

    /** \brief Returns the underlying struct which can be used by the vc_dispman* calls.
     */
    const VC_RECT_T &rect() const;

   private:
    /** \brief Updates the underlying structure with the current values.
     * This structure is lazily updated, which means it is only updated when needed e.g. retrieved
     * by the rect funtion.
     */
    void update_rect();

    VC_RECT_T m_rect;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_off_x;
    uint32_t m_off_y;
    /** \brief Flag which indicates that the values of this instance differ from the values in the
     * underlying structure.
     */
    bool m_changed;
};

/** \brief This class is used when raw pixel values should be loaded into a dispmanx_resource.
 * This class automaticly chooses a suitable size for the underlying array. It also calculates the
 * correct pitch. If the underlying array doesn't have dimensions which aren't divisable by 16 the
 * function which uploades the pixels to a dispmanx_resource won't finish
 */
template<typename color_type, typename size_type = size_t>
class dispmanx_pixmap {
   public:
    /** \brief Constructs a dispmanx_pixmap with dimensions which are at least widthxheight.
     * The class will round the dimensions up to numbers which are divisable by 16 so that the
     * functions which upload the data from the pixmap to the resource will work.
     */
    dispmanx_pixmap(size_type width, size_type height);

    /** \brief Returns row with index.
     */
    const color_type *operator[](size_type index) const;
    /** \brief Returns row with index.
     */
    color_type *operator[](size_type index);

    /** \brief Returns the whole underlying array.
     */
    const color_type *data() const;

    /** \brief Returns the real width of the pixmap.
     */
    size_type width() const;
    /** \brief Returns the real pitch of the pixmap which is the number of entries until the next
     * row of the pixmap.
     */
    size_type pitch() const;
    /** \brief Returns the real height of the pixmap.
     */
    size_type height() const;

   private:
    size_type m_width;
    size_type m_height;
    size_type m_pitch;
    std::unique_ptr<color_type[]> m_data;

    /** \brief Function which calculates the next number which is greater or equael to this value
     * and is divisable by 16.
     */
    template<size_type align_to>
    static constexpr size_type align(const size_type length);
};

template<typename color_type, typename size_type>
dispmanx_pixmap<color_type, size_type>::dispmanx_pixmap(size_type width, size_type height)
    : m_width(width), m_height(height), m_pitch(align<16>(width)) {
    m_data = std::make_unique<color_type[]>(m_pitch * m_height);
}

template<typename color_type, typename size_type>
template<size_type align_to>
constexpr size_type dispmanx_pixmap<color_type, size_type>::align(const size_type length) {
    return (length + (align_to - 1)) & (~(align_to - 1));
}

template<typename color_type, typename size_type>
size_type dispmanx_pixmap<color_type, size_type>::width() const {
    return m_width;
}

template<typename color_type, typename size_type>
size_type dispmanx_pixmap<color_type, size_type>::height() const {
    return m_height;
}

template<typename color_type, typename size_type>
size_type dispmanx_pixmap<color_type, size_type>::pitch() const {
    return m_pitch;
}

template<typename color_type, typename size_type>
const color_type *dispmanx_pixmap<color_type, size_type>::operator[](size_type index) const {
    return m_data.get() + index * pitch();
}

template<typename color_type, typename size_type>
color_type *dispmanx_pixmap<color_type, size_type>::operator[](size_type index) {
    return m_data.get() + index * pitch();
}

template<typename color_type, typename size_type>
const color_type *dispmanx_pixmap<color_type, size_type>::data() const {
    return m_data.get();
}

/** \brief This enum lists possible orientations which can be used to rotate the display.
 */
enum struct orientation {
    NO_ROTATION = DISPMANX_NO_ROTATE,
    ROTATE_90 = DISPMANX_ROTATE_90,
    ROTATE_180 = DISPMANX_ROTATE_180,
    ROTATE_270 = DISPMANX_ROTATE_270
};

// TODO Make chainable
/** \brief This enum contains flags which signal if the display should by flipped or not.
 */
enum struct flipped { NO = 0, HORIZONTALLY = DISPMANX_FLIP_HRIZ, VERITCALLY = DISPMANX_FLIP_VERT };

// TODO Make chainable
enum struct snapshot_behaviour { NONE };

// TODO Maybe implement other flags as well
/** \brief This class contains all the information necessary to describe the alignment of the
 * display It contains information such as display rotation, or snapshot_behaviour.
 */
class dispmanx_transform {
   public:
    dispmanx_transform(const orientation &orient = orientation::NO_ROTATION,
                       const flipped &flip = flipped::NO,
                       const snapshot_behaviour &behaviour = snapshot_behaviour::NONE);
    /** \brief Returns the underlying structure which is used by the vc_dispman* calls.
     */
    DISPMANX_TRANSFORM_T native_transform() const;

   private:
    orientation m_orient;
    flipped m_flip;
    snapshot_behaviour m_behaviour;
};

/** \brief Class which contains the image data to display on the screen.
 * This class is used in conjunction with dispmanx_element. With those two together it is possible
 * to display something on the screen
 */
class dispmanx_resource {
   public:
    /** \brief Move construct an instance from another.
     */
    dispmanx_resource(dispmanx_resource &&other);
    ~dispmanx_resource();
    /** \brief Creates a new resource with the provided parameters.
     * If the creation of the dispmanx_resource fails for any reason no instance will be returned
     */
    static std::optional<dispmanx_resource> create_resource(const VC_IMAGE_TYPE_T &image_type,
                                                            uint32_t width, uint32_t height);
    /** \brief Writes the data of the pixmap into the resource.
     */
    template<typename T>
    void write_data(VC_IMAGE_TYPE_T src_type, const dispmanx_pixmap<T> &pixmap,
                    const dispmanx_rect &region);
    /** \brief Reads data from the resource into the pixmap.
     */
    template<typename T>
    void read_data(const dispmanx_rect &region, dispmanx_pixmap<T> &pixmap);

    /** \brief Returns the dimensions (widthxheight) of the resource.
     */
    const dispmanx_rect &dimensions() const;

   private:
    dispmanx_resource(const DISPMANX_RESOURCE_HANDLE_T &handle, const VC_IMAGE_TYPE_T &image_type,
                      uint32_t width, uint32_t height);

    const DISPMANX_RESOURCE_HANDLE_T &handle() const;

    /** \brief Underlying structure to interface with vc_dispman* call.
     */
    DISPMANX_RESOURCE_HANDLE_T m_handle;
    const VC_IMAGE_TYPE_T m_image_type;
    const dispmanx_rect m_dimensions;

    friend class dispmanx_update;
};

template<typename T>
void dispmanx_resource::write_data(VC_IMAGE_TYPE_T src_type, const dispmanx_pixmap<T> &pixmap,
                                   const dispmanx_rect &region) {
    vc_dispmanx_resource_write_data(m_handle, src_type, pixmap.pitch() * sizeof(T),
                                    (void *)pixmap.data(), &region.rect());
}

template<typename T>
void dispmanx_resource::read_data(const dispmanx_rect &region, dispmanx_pixmap<T> &pixmap) {
    vc_dispmanx_resource_read_data(m_handle, &region.rect(), (void *)pixmap.data(), pixmap.pitch());
}

/** \brief This class is used to identify an element which is used by an dispmanx_update.
 * It doesn't contain any data of the element itself instead it only contains an identifier with
 * which it is possible to modify an dispmanx_element inside an dispmanx_update
 */
class dispmanx_element_handle {
   public:
    dispmanx_element_handle(dispmanx_element_handle &&other);
    dispmanx_element_handle(const dispmanx_element_handle &other) = delete;

   private:
    dispmanx_element_handle(const DISPMANX_ELEMENT_HANDLE_T &handle);
    const DISPMANX_ELEMENT_HANDLE_T &handle() const;

    DISPMANX_ELEMENT_HANDLE_T m_handle;

    friend class dispmanx_update;
};

enum struct dispmanx_protection {
    MAX = DISPMANX_PROTECTION_MAX,
    NONE = DISPMANX_PROTECTION_NONE,
    HDCP = DISPMANX_PROTECTION_HDCP
};

class dispmanx_alpha {
   public:
    dispmanx_alpha(DISPMANX_FLAGS_ALPHA_T alpha_flags, uint32_t opacity);
    dispmanx_alpha(DISPMANX_FLAGS_ALPHA_T alpha_flags, uint32_t opacity,
                   const dispmanx_resource *resource);

    DISPMANX_FLAGS_ALPHA_T flags() const;
    uint32_t opacity() const;
    const dispmanx_resource *mask() const;

   private:
    DISPMANX_FLAGS_ALPHA_T m_flags;
    uint32_t m_opacity;
    const dispmanx_resource *m_resource;

    friend class dispmanx_update;
};

// TODO : Implement if needed
class dispmanx_clamp {};

class dispmanx_display;

class dispmanx_update {
   public:
    dispmanx_update(const dispmanx_update &) = delete;
    dispmanx_update(dispmanx_update &&);
    ~dispmanx_update();

    std::optional<dispmanx_element_handle> add_element(
        int32_t layer, const dispmanx_rect &dest_region, const dispmanx_resource &resource,
        const dispmanx_rect &src_region, const dispmanx_protection &protection,
        const dispmanx_alpha &alpha, const dispmanx_clamp &clamp,
        const dispmanx_transform &transform);
    void change_element_source(const dispmanx_element_handle &element, dispmanx_resource &resource);
    void change_element_layer(const dispmanx_element_handle &element, int32_t layer);
    void signal_element_modification(const dispmanx_element_handle &element,
                                     const dispmanx_rect &region);
    void remove_element(const dispmanx_element_handle &element);
    void submit_update();

   private:
    dispmanx_update(DISPMANX_UPDATE_HANDLE_T &update_handle, dispmanx_display &display);

    DISPMANX_UPDATE_HANDLE_T m_update;
    dispmanx_display &m_display;
    bool m_submitted = false;

    friend class dispmanx_display;
};

enum struct dispmanx_display_input_format {
    INVALID = VCOS_DISPLAY_INPUT_FORMAT_INVALID,
    RGB888 = VCOS_DISPLAY_INPUT_FORMAT_RGB888,
    RGB565 = VCOS_DISPLAY_INPUT_FORMAT_RGB565
};

class dispmanx_modeinfo {
   public:
    dispmanx_modeinfo(const dispmanx_modeinfo &other) = default;
    dispmanx_modeinfo(dispmanx_modeinfo &&other) = default;

    inline auto width() const { return m_modeinfo.width; }
    inline auto height() const { return m_modeinfo.height; }
    inline auto display_number() const { return m_modeinfo.display_num; }
    dispmanx_display_input_format input_format() const;

   private:
    dispmanx_modeinfo(const DISPMANX_MODEINFO_T &modeinfo);
    DISPMANX_MODEINFO_T m_modeinfo;

    friend class dispmanx_display;
};

class dispmanx_display {
   public:
    enum struct ids : uint8_t {
        MAIN_LCD = DISPMANX_ID_MAIN_LCD,
        AUX_LCD = DISPMANX_ID_AUX_LCD,
        HDMI = DISPMANX_ID_HDMI,
        SDTV = DISPMANX_ID_SDTV,
        FORCE_LCD = DISPMANX_ID_FORCE_LCD,
        FORCE_TV = DISPMANX_ID_FORCE_TV,
        FORCE_OTHER = DISPMANX_ID_FORCE_OTHER
    };

    dispmanx_display(const dispmanx_display &other) = delete;
    dispmanx_display(dispmanx_display &&other);
    ~dispmanx_display();

    static std::optional<dispmanx_display> open_display(ids display_id);

    std::optional<dispmanx_update> start_update(int32_t priority = 0);

    dispmanx_modeinfo current_mode() const;

   private:
    dispmanx_display(DISPMANX_DISPLAY_HANDLE_T &display_handle);
    const DISPMANX_DISPLAY_HANDLE_T &handle() const;

    DISPMANX_DISPLAY_HANDLE_T m_handle;

    friend class dispmanx_update;
};
