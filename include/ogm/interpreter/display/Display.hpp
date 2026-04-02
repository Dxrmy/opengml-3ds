#pragma once

#include "TextureStore.hpp"
#include "ogm/common/types.hpp"
#include "ogm/interpreter/serialize.hpp"
#include "ogm/geometry/Vector.hpp"

#include <cstdint>
#include <map>
#include <array>

class GLFWwindow;

namespace ogm {

// forward declarations
namespace asset {
    class AssetFont;
}

namespace interpreter {

using namespace ogm;

typedef uint8_t ogm_keycode_t;

struct Texture;

struct VertexFormatAttribute
{
    enum DataType
    {
        F1,
        F2,
        F3,
        F4,
        RGBA, // colour
        U4, // 4 unsigned bytes
    } m_type;

    enum Destination
    {
        POSITION,
        COLOUR,
        NORMAL,
        TEXCOORD,

        // colour blending
        BW,
    } m_destination;
};

struct VertexFormat
{
    std::vector<VertexFormatAttribute> m_attributes;
};

class Display
{
public:
    bool start(uint32_t width, uint32_t height, const char* caption, bool vsync);
    ~Display();

    // keys
    bool get_key_down(ogm_keycode_t key);
    bool get_key_pressed(ogm_keycode_t key);
    bool get_key_released(ogm_keycode_t key);

    // mouse
    coord_t get_mouse_x();
    coord_t get_mouse_y();

    // joysticks
    bool get_joysticks_supported();
    size_t get_joystick_max();
    bool get_joystick_connected(size_t index);
    std::string get_joystick_name(size_t index);
    size_t get_joystick_axis_count(size_t index);
    real_t get_joystick_axis_value(size_t index, size_t axis_index);
    size_t get_joystick_button_count(size_t index);
    bool get_joystick_button_down(size_t index, size_t b);
    bool get_joystick_button_pressed(size_t index, size_t b);
    bool get_joystick_button_released(size_t index, size_t b);

    void begin_render();

    void clear_render();

    void end_render();

    // 3d setup
    void set_depth_test(bool);
    void set_culling(bool);
    void reset_render_state();
    void set_zwrite(bool);
    void set_colour_mask(bool r, bool g, bool b, bool a);

    // pass in nullptr to reset.
    void set_target(TexturePage*);

    // this is disabled when drawing to surfaces.
    void enable_view_projection(bool);

    // sets the current transformation to identity
    void transform_identity();

    // applies the given matrix to the current transformation
    void transform_apply(std::array<float, 16> matrix);

    // applies a rotation to the current transformation
    void transform_apply_rotation(coord_t angle, coord_t x, coord_t y, coord_t z);

    // applies a translation to the current transformation
    void transform_apply_translation(coord_t x, coord_t y, coord_t z);

    // applies a scale to the current transformation
    void transform_apply_scale(coord_t x, coord_t y, coord_t z);

    // sets the current transformation to the given matrix
    void transform_set(std::array<float, 16> matrix);

    // gets the current transformation matrix
    std::array<float, 16> transform_get();

    // stack
    void transform_stack_push();
    void transform_stack_pop();
    void transform_stack_clear();
    bool transform_stack_empty();

    // fog
    void set_fog(bool enabled, uint32_t colour, coord_t start, coord_t end);

    // texture settings
    void set_texture_interpolation(bool);
    bool get_texture_interpolation();
    void set_texture_repeat(bool);
    bool get_texture_repeat();

    // blend modes
    void set_blendmode(uint32_t src, uint32_t dst);
    void set_blendmode_ext(uint32_t src, uint32_t dst, uint32_t src_alpha, uint32_t dst_alpha);

    // vertex formats
    uint32_t make_vertex_format();
    void vertex_format_append_attribute(uint32_t id, VertexFormatAttribute::Destination dest, VertexFormatAttribute::DataType type);
    void vertex_format_finish(uint32_t id);
    void free_vertex_format(uint32_t id);

    uint32_t make_vertex_buffer(size_t size = 0);
    void add_vertex_buffer_data(uint32_t id, unsigned char* data, size_t length);
    void freeze_vertex_buffer(uint32_t id);
    size_t vertex_buffer_get_size(uint32_t id);
    size_t vertex_buffer_get_count(uint32_t id);
    void free_vertex_buffer(uint32_t id);

    model_id_t model_make();
    void model_add_vertex_buffer(model_id_t, uint32_t buffer, PrimitiveType type);
    void model_draw(model_id_t, TexturePage* texture);
    uint32_t model_get_vertex_format(model_id_t);
    void model_free(model_id_t);

    void associate_vertex_buffer_format(uint32_t vb, uint32_t vf);

    void render_buffer(uint32_t vertex_buffer, TexturePage* image, PrimitiveType type);

    // length in floats, not vertices.
    void render_array(size_t length, float* vertex_data, TexturePage* texture, PrimitiveType type);

    // writes Display::get_vertex_size() bytes to the given buffer.
    void write_vertex(float* out, coord_t x, coord_t y, coord_t z=0, uint32_t colour=0xffffffff, coord_t u=0, coord_t v=0) const;
    uint32_t get_vertex_size() const;

    // draws the given image in the given rectangle.
    void draw_image(TextureView*, coord_t x1, coord_t y1, coord_t x2, coord_t y2, coord_t tx1=0, coord_t ty1=0, coord_t tx2=1, coord_t ty2=1);
    void draw_image(TextureView*, coord_t x1, coord_t y1, coord_t x2, coord_t y2, coord_t x3, coord_t y3, coord_t x4, coord_t y4, coord_t tx1, coord_t ty1, coord_t tx2, coord_t ty2, coord_t tx3, coord_t ty3, coord_t tx4, coord_t ty4);
    void draw_image_tiled(TextureView*, bool tiled_x, bool tiled_y, coord_t x1, coord_t y1, coord_t x2, coord_t y2, coord_t tx1=0, coord_t ty1=0, coord_t tx2=1, coord_t ty2=1);

    void draw_filled_rectangle(coord_t x1, coord_t y1, coord_t x2, coord_t y2);
    void draw_filled_circle(coord_t x, coord_t y, coord_t r);
    void draw_outline_rectangle(coord_t x1, coord_t y1, coord_t x2, coord_t y2);
    void draw_outline_circle(coord_t x, coord_t y, coord_t r);

    // TODO: take string_view
    void draw_text(coord_t x, coord_t y, const char* text, real_t halign, real_t valign, bool use_max_width=false, coord_t max_width=0, bool use_sep=false, coord_t sep=0);

    // the number of vertices per circle
    void set_circle_precision(uint32_t prec);
    uint32_t get_circle_precision();

    // fills view with colour
    void draw_fill_colour(uint32_t);

    /////// matrix documentation ///////
    // the following matrices are passed to the vertex shader:
    //
    //   matrix_model (transforms object to be drawn)
    //   matrix_view (rotates and scales world so it is centered on the positive z axis)
    //   matrix_projection (converts view-transformed world into final vertex
    //                      locations.)
    //
    // matrix_world is just matrix_view * matrix_model.
    //
    // when matrix_view is identity, the view is looking straight down the
    // positive z axis from (0, 0, 0) towards (0, 0, 1).
    // matrix_projection usually has a near-plane of 0.
    // coordinate (0, 0) is at the top-left of the screen.
    // z is the depth. Positive z is further into the screen.
    // Negative z is behind the camera.
    ////////////////////////////////////

    // setters
    void set_matrix_view(std::array<float, 16> matrix);
    void set_matrix_projection(std::array<float, 16> matrix);
    void set_matrix_model(std::array<float, 16> matrix);

    // getters
    std::array<float, 16> get_matrix_view();
    std::array<float, 16> get_matrix_projection();
    std::array<float, 16> get_matrix_model();

    // window
    void window_set_size(uint32_t w, uint32_t h);
    void window_set_position(uint32_t x, uint32_t y);
    void window_set_caption(const char* caption);
    void window_set_fullscreen(bool fullscreen);
    bool window_get_fullscreen();
    void window_set_vsync(bool vsync);
    uint32_t window_get_width();
    uint32_t window_get_height();

    // screen
    uint32_t get_display_width();
    uint32_t get_display_height();

#ifndef __3DS__
    GLFWwindow* get_window() { return m_window; }
#endif

private:
#ifndef __3DS__
    GLFWwindow* m_window;
#endif
};

}
}
