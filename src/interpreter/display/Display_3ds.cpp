#ifdef __3DS__

#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "ogm/interpreter/display/Display.hpp"
#include "ogm/interpreter/Executor.hpp"
#include "ogm/common/types.hpp"

// Embedded Shader Symbols (from bin2s)
extern const u8 default_v_shbin[];
extern const u32 default_v_shbin_size;

namespace ogm::interpreter {
    Display* g_active_display = nullptr;
    volatile bool g_key_down[256];
    volatile bool g_key_pressed[256];
    volatile bool g_key_released[256];

    uint32_t g_window_width = 0;
    uint32_t g_window_height = 0;

    // GML vk_ constants
    const int vk_space = 32;
    const int vk_shift = 16;
    const int vk_enter = 13;
    const int vk_escape = 27;
    const int vk_up = 38;
    const int vk_down = 40;
    const int vk_left = 37;
    const int vk_right = 39;
    const int vk_control = 17;
    const int vk_alt = 18;
    const int vk_x = 88;
    const int vk_y = 89;
    const int vk_zl = 162;
    const int vk_zr = 163;
    const int mb_left = 1;


// Static constants for 3DS rendering
static C2D_TextBuf g_textBuf;
static C3D_RenderTarget* g_topTarget;

// Shader Program Data
static DVLB_s* g_defaultDvlb;
static shaderProgram_s g_defaultShader;
static int8_t g_uLocProjection;

bool Display::start(uint32_t width, uint32_t height, const char* caption, bool vsync) {
    if (g_active_display != nullptr) {
        throw MiscError("Multiple displays not supported");
    }

    // Setup screens
    g_topTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    // Initialize text buffer
    g_textBuf = C2D_TextBufNew(4096);

    // Load Default Shader from embedded buffer
    g_defaultDvlb = DVLB_ParseFile((u32*)default_v_shbin, (u32)&default_v_shbin_size);
    if (g_defaultDvlb) {
        shaderProgramInit(&g_defaultShader);
        shaderProgramSetVsh(&g_defaultShader, &g_defaultDvlb->DVLE[0]);
        C3D_BindProgram(&g_defaultShader);

        // Locate projection uniform (defined in default.v.pica)
        g_uLocProjection = shaderInstanceGetUniformLocation(g_defaultShader.vertexShader, "projection");
    }

    g_active_display = this;
    g_window_width = width;
    g_window_height = height;

    // Clear screen on start
    begin_render();
    clear_render();
    end_render();

    // Default TEV configuration (Stage 0: Modulate Texture * Vertex Colour)
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, (GPU_TEVSRC)0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

    // Initial Blend Mode
    set_blendmode(0, 0); // bm_normal

    return true;
}

Display::~Display() {
    C2D_TextBufDelete(g_textBuf);

    if (g_defaultDvlb) {
        shaderProgramFree(&g_defaultShader);
        DVLB_Free(g_defaultDvlb);
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    g_active_display = nullptr;
}

static GPU_BLENDFACTOR ogm_to_3ds_blend(int32_t ogm_factor) {
    switch (ogm_factor) {
        case 4: return GPU_ZERO;
        case 5: return GPU_ONE;
        case 6: return GPU_SRC_COLOR;
        case 7: return GPU_ONE_MINUS_SRC_COLOR;
        case 8: return GPU_SRC_ALPHA;
        case 9: return GPU_ONE_MINUS_SRC_ALPHA;
        case 10: return GPU_DST_ALPHA;
        case 11: return GPU_ONE_MINUS_DST_ALPHA;
        case 12: return GPU_DST_COLOR;
        case 13: return GPU_ONE_MINUS_DST_COLOR;
        case 14: return GPU_SRC_ALPHA_SATURATE;
        default: return GPU_ONE;
    }
}

void Display::set_blendmode(int32_t src, int32_t dst) {
    if (src == 0 && dst == 0) { // bm_normal
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);
    } else if (src == 1 && dst == 1) { // bm_add
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE, GPU_SRC_ALPHA, GPU_ONE);
    } else if (src == 2 && dst == 2) { // bm_subtract
        C3D_AlphaBlend(GPU_BLEND_REVERSE_SUBTRACT, GPU_BLEND_REVERSE_SUBTRACT, GPU_SRC_ALPHA, GPU_ONE, GPU_SRC_ALPHA, GPU_ONE);
    } else {
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, ogm_to_3ds_blend(src), ogm_to_3ds_blend(dst), ogm_to_3ds_blend(src), ogm_to_3ds_blend(dst));
    }
}

void Display::set_blendmode_separate(int32_t src, int32_t dst, int32_t srca, int32_t dsta) {
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, ogm_to_3ds_blend(src), ogm_to_3ds_blend(dst), ogm_to_3ds_blend(srca), ogm_to_3ds_blend(dsta));
}

void Display::shader_set_alpha_test_enabled(bool enabled) {
    C3D_AlphaTest(enabled, GPU_GREATER, 0);
}

void Display::shader_set_alpha_test_threshold(real_t value) {
    // value is 0.0 to 1.0, 3DS expects 0 to 255
    C3D_AlphaTest(true, GPU_GREATER, static_cast<int>(value * 255));
}

static uint32_t g_clear_colour = 0;
static uint32_t g_draw_colour = 0xffffffff;
static float g_draw_alpha = 1.0f;

void Display::begin_render() {
    C2D_TextBufClear(g_textBuf);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
}

void Display::clear_render() {
    uint8_t r = (g_clear_colour & 0x0000FF);
    uint8_t g = (g_clear_colour & 0x00FF00) >> 8;
    uint8_t b = (g_clear_colour & 0xFF0000) >> 16;
    C2D_TargetClear(g_topTarget, C2D_Color32(r, g, b, 255));
    C2D_SceneBegin(g_topTarget);
}

void Display::end_render() {
    C3D_FrameEnd(0);
}

void Display::flip() {
    // Already handled by C3D_FrameEnd(0) in end_render()
}

// Input Handling via libctru
void Display::process_keys() {
    hidScanInput();
    uint32_t held = hidKeysHeld();
    uint32_t pressed = hidKeysDown();
    uint32_t released = hidKeysUp();

    auto map_key = [&](uint32_t k3ds, ogm_keycode_t kogm) {
        g_key_down[kogm] = (held & k3ds);
        g_key_pressed[kogm] = (pressed & k3ds);
        g_key_released[kogm] = (released & k3ds);
    };

    map_key(KEY_A, vk_space);
    map_key(KEY_B, vk_shift);
    map_key(KEY_X, vk_x);
    map_key(KEY_Y, vk_y);
    map_key(KEY_ZL, vk_zl);
    map_key(KEY_ZR, vk_zr);
    map_key(KEY_START, vk_enter);
    map_key(KEY_SELECT, vk_escape);
    map_key(KEY_UP | KEY_CPAD_UP | KEY_CSTICK_UP, vk_up);
    map_key(KEY_DOWN | KEY_CPAD_DOWN | KEY_CSTICK_DOWN, vk_down);
    map_key(KEY_LEFT | KEY_CPAD_LEFT | KEY_CSTICK_LEFT, vk_left);
    map_key(KEY_RIGHT | KEY_CPAD_RIGHT | KEY_CSTICK_RIGHT, vk_right);
    map_key(KEY_L, vk_control);
    map_key(KEY_R, vk_alt);

    // Mouse / Touch
    map_key(KEY_TOUCH, mb_left);

    touchPosition touch;
    hidTouchRead(&touch);
    if (held & KEY_TOUCH) {
        staticExecutor.m_mouse_x = touch.px;
        staticExecutor.m_mouse_y = touch.py;
    }
}

// 3DS Texture Allocation Helper
void* Display_3DS_AllocTexture(uint32_t width, uint32_t height, GPU_TEXCOLOR format, C3D_Tex* out_tex) {
    if (width > 1024 || height > 1024) {
        fprintf(stderr, "[3DS GFX] CRITICAL: Texture size %lux%u exceeds hardware limit (1024x1024)!\n", width, height);
        return nullptr;
    }

    // Allocate from Linear RAM (required for PICA200)
    void* data = linearAlloc(width * height * 4); // Assuming RGBA8
    if (!data) {
        fprintf(stderr, "[3DS GFX] FAILED to allocate %lu bytes from Linear RAM\n", (uint32_t)(width * height * 4));
        return nullptr;
    }

    C3D_TexInit(out_tex, width, height, format);
    return data;
}

// Text Rendering via C2D_TextBuf
void Display::draw_text(coord_t x, coord_t y, const char* text, real_t halign, real_t valign, bool use_max_width, coord_t max_width, bool use_sep, coord_t sep) {
    C2D_Text c2dText;
    C2D_TextParse(&c2dText, g_textBuf, text);
    C2D_TextOptimize(&c2dText);
    C2D_DrawText(&c2dText, C2D_WithColor, x, y, 0, 1.0f, 1.0f, C2D_Color32(255, 255, 255, 255));
}

void Display::draw_image(TextureView* tv, coord_t x1, coord_t y1, coord_t x2, coord_t y2, coord_t tx1, coord_t ty1, coord_t tx2, coord_t ty2) {
    if (!tv || !tv->m_tpage || !tv->m_tpage->m_gl_tex) return;

    // Use C2D_DrawImage for proper sprite rendering
    C3D_Tex* tex = reinterpret_cast<C3D_Tex*>(tv->m_tpage->m_gl_tex);

    // Set up texture environment to blend sprite with vertex color
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

    C2D_ImageTint tint;
    uint8_t r = (g_draw_colour & 0x0000FF);
    uint8_t g = (g_draw_colour & 0x00FF00) >> 8;
    uint8_t b = (g_draw_colour & 0xFF0000) >> 16;
    uint8_t a = static_cast<uint8_t>(g_draw_alpha * 255);
    uint32_t c = C2D_Color32(r, g, b, a);

    C2D_PlainImageTint(&tint, c, 1.0f);

    // tv has already been mapped to absolute atlas texture coords, tx1/ty1/tx2/ty2 are the local bounds within it
    Tex3DS_SubTexture subtex;
    subtex.left = tx1;
    subtex.top = ty1;
    subtex.right = tx2;
    subtex.bottom = ty2;
    subtex.width = std::abs((float)(tx2 - tx1) * tex->width);
    subtex.height = std::abs((float)(ty2 - ty1) * tex->height);

    C2D_Image img;
    img.tex = tex;
    img.subtex = &subtex;

    // Transform coordinates using x1, y1 and size
    float w = x2 - x1;
    float h = y2 - y1;

    C2D_DrawImageAt(img, x1, y1, 0.5f, &tint, w / subtex.width, h / subtex.height);
}

void Display::draw_image(TextureView* tv, coord_t x1, coord_t y1, coord_t x2, coord_t y2, coord_t x3, coord_t y3, coord_t x4, coord_t y4, coord_t tx1, coord_t ty1, coord_t tx2, coord_t ty2, coord_t tx3, coord_t ty3, coord_t tx4, coord_t ty4) {
    // Advanced quad drawing, potentially requires building C3D vertex buffers
    // For now, fallback to basic draw_image using bounding box
    draw_image(tv, x1, y1, x3, y3, tx1, ty1, tx3, ty3);
}

void Display::draw_image_tiled(TextureView* tv, bool tilex, bool tiley, coord_t x, coord_t y, coord_t w, coord_t h, coord_t px, coord_t py, coord_t pw, coord_t ph) {
    if (!tv || !tv->m_tpage || !tv->m_tpage->m_gl_tex) return;

    C3D_Tex* tex = reinterpret_cast<C3D_Tex*>(tv->m_tpage->m_gl_tex);

    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

    C2D_ImageTint tint;
    uint8_t r = (g_draw_colour & 0x0000FF);
    uint8_t g = (g_draw_colour & 0x00FF00) >> 8;
    uint8_t b = (g_draw_colour & 0xFF0000) >> 16;
    uint8_t a = static_cast<uint8_t>(g_draw_alpha * 255);
    uint32_t c = C2D_Color32(r, g, b, a);

    C2D_PlainImageTint(&tint, c, 1.0f);

    // tv has already been mapped to absolute atlas texture coords, tx1/ty1/tx2/ty2 are the local bounds within it
    Tex3DS_SubTexture subtex;
    subtex.left = px;
    subtex.top = py;
    subtex.right = px + pw;
    subtex.bottom = py + ph;
    subtex.width = std::abs((float)(pw) * tex->width);
    subtex.height = std::abs((float)(ph) * tex->height);

    C2D_Image img;
    img.tex = tex;
    img.subtex = &subtex;

    // Transform coordinates using x1, y1 and size
    float scaled_w = w / subtex.width;
    float scaled_h = h / subtex.height;

    // Simplistic tiled drawing
    coord_t cx = x;
    while (cx < x + w) {
        coord_t cy = y;
        while (cy < y + h) {
            C2D_DrawImageAt(img, cx, cy, 0.5f, &tint, scaled_w, scaled_h);
            cy += subtex.height * scaled_h;
            if (!tiley) break;
        }
        cx += subtex.width * scaled_w;
        if (!tilex) break;
    }
}

// Linker Stubs for missing Display methods on 3DS
void Display::set_matrix_view(real_t x, real_t y, real_t z, real_t w, real_t h) {}
void Display::set_matrix_projection() {}
void Display::set_matrix_model(real_t x, real_t y, real_t z, real_t w, real_t h) {}
void Display::set_matrix_pre_model() {}
void Display::check_error(const std::string& ctx) {}
void Display::set_multisample(uint32_t s) {}
void Display::delay(real_t ms) {}
bool Display::window_close_requested() { return false; }
uint32_t Display::make_vertex_format() { return 0; }
void Display::vertex_format_append_attribute(uint32_t vf, VertexFormatAttribute attr) {}
uint32_t Display::make_vertex_buffer(size_t size) { return 0; }
void Display::add_vertex_buffer_data(uint32_t id, unsigned char* data, size_t length) {}
void Display::freeze_vertex_buffer(uint32_t vb) {}
void Display::render_buffer(uint32_t vb, TexturePage* tp, uint32_t glenum) {}
void Display::bind_and_compile_shader(uint32_t s, const std::string& v, const std::string& f) {}
geometry::AABB<coord_t> Display::get_viewable_aabb() { return {}; }

template<bool write>
void Display::serialize(typename state_stream<write>::state_stream_t& s) {}
template void Display::serialize<true>(typename state_stream<true>::state_stream_t& s);
template void Display::serialize<false>(typename state_stream<false>::state_stream_t& s);

geometry::Vector<real_t> Display::get_window_dimensions() { return {static_cast<real_t>(g_window_width), static_cast<real_t>(g_window_height)}; }
void Display::set_target(TexturePage* tp) {}
void Display::enable_view_projection(bool) {}
void Display::associate_vertex_buffer_format(uint32_t vb, uint32_t vf) {}
size_t Display::vertex_buffer_get_count(uint32_t id) { return 0; }
size_t Display::vertex_buffer_get_size(uint32_t id) { return 0; }
void Display::free_vertex_format(uint32_t id) {}
void Display::vertex_format_finish(uint32_t id) {}
void Display::free_vertex_buffer(uint32_t id) {}

float Display::get_alpha() { return g_draw_alpha; }
void Display::set_alpha(float a) { g_draw_alpha = a; }
uint32_t Display::get_colour() { return g_draw_colour; }
void Display::set_colour(uint32_t c) { g_draw_colour = c; }

matrix_t Display::get_matrix_view() { return {}; }
matrix_t Display::get_matrix_model() { return {}; }
matrix_t Display::get_matrix_projection() { return {}; }
void Display::set_matrix_view(matrix_t) {}
void Display::set_matrix_model(matrix_t) {}
void Display::set_matrix_projection(matrix_t) {}
void Display::set_matrix_pre_model(coord_t x, coord_t y, coord_t xscale, coord_t yscale, real_t angle) {}

// Transform stack (not natively in Display.hpp? Or maybe added recently)
// Adding stubs just in case they are declared somewhere or used
bool Display::transform_stack_push() { return true; }
bool Display::transform_stack_pop() { return true; }
void Display::transform_apply_translation(real_t x, real_t y, real_t z) {}

model_id_t Display::model_make() { return 0; }
void Display::model_free(model_id_t) {}
void Display::model_draw(model_id_t, TexturePage*) {}
uint32_t Display::model_get_vertex_format(model_id_t) { return 0; }
void Display::model_add_vertex_buffer(model_id_t, uint32_t, uint32_t) {}

void Display::use_shader(uint32_t) {}
int32_t Display::shader_get_uniform_id(uint32_t, const std::string&) { return -1; }
void Display::shader_set_uniform_f(int32_t, int, float*) {}

void Display::clear_keys() {}
bool Display::get_key_down(ogm_keycode_t k) { return g_key_down[k]; }
bool Display::get_key_pressed(ogm_keycode_t k) { return g_key_pressed[k]; }
bool Display::get_key_released(ogm_keycode_t k) { return g_key_released[k]; }
bool Display::get_key_direct(ogm_keycode_t k) { return g_key_down[k]; }
ogm_keycode_t Display::get_current_key() { return 0; }
real_t Display::get_key_last() { return 0; }
void Display::set_key_last(real_t) {}
const std::string& Display::get_char_last() { static std::string s; return s; }
void Display::set_char_last(std::string&&) {}

bool Display::get_joysticks_supported() { return false; }
size_t Display::get_joystick_max() { return 0; }
bool Display::get_joystick_connected(size_t index) { return false; }
std::string Display::get_joystick_name(size_t index) { return ""; }
size_t Display::get_joystick_axis_count(size_t index) { return 0; }
real_t Display::get_joystick_axis_value(size_t index, size_t axis_index) { return 0; }
size_t Display::get_joystick_button_count(size_t index) { return 0; }
bool Display::get_joystick_button_down(size_t index, size_t b) { return false; }
bool Display::get_joystick_button_pressed(size_t index, size_t b) { return false; }
bool Display::get_joystick_button_released(size_t index, size_t b) { return false; }

uint32_t Display::get_colour4() { return 0xffffffff; }
void Display::set_colours4(uint32_t[4]) {}
void Display::get_colours4(uint32_t*) {}

void Display::write_vertex(float*, coord_t, coord_t, coord_t, uint32_t, coord_t, coord_t) const {}
uint32_t Display::get_vertex_size() const { return 0; }
void Display::render_array(size_t, float*, TexturePage*, uint32_t) {}
void Display::draw_outline_rectangle(coord_t x1, coord_t y1, coord_t x2, coord_t y2) {
    uint8_t r = (g_draw_colour & 0x0000FF);
    uint8_t g = (g_draw_colour & 0x00FF00) >> 8;
    uint8_t b = (g_draw_colour & 0xFF0000) >> 16;
    uint8_t a = static_cast<uint8_t>(g_draw_alpha * 255);
    uint32_t c = C2D_Color32(r, g, b, a);

    // Top
    C2D_DrawLine(x1, y1, c, x2, y1, c, 1.0f, 0.5f);
    // Right
    C2D_DrawLine(x2, y1, c, x2, y2, c, 1.0f, 0.5f);
    // Bottom
    C2D_DrawLine(x2, y2, c, x1, y2, c, 1.0f, 0.5f);
    // Left
    C2D_DrawLine(x1, y2, c, x1, y1, c, 1.0f, 0.5f);
}

void Display::draw_filled_rectangle(coord_t x1, coord_t y1, coord_t x2, coord_t y2) {
    uint8_t r = (g_draw_colour & 0x0000FF);
    uint8_t g = (g_draw_colour & 0x00FF00) >> 8;
    uint8_t b = (g_draw_colour & 0xFF0000) >> 16;
    uint8_t a = static_cast<uint8_t>(g_draw_alpha * 255);
    C2D_DrawRectSolid(x1, y1, 0.5f, x2 - x1, y2 - y1, C2D_Color32(r, g, b, a));
}

void Display::draw_filled_circle(coord_t x, coord_t y, coord_t radius) {
    uint8_t r = (g_draw_colour & 0x0000FF);
    uint8_t g = (g_draw_colour & 0x00FF00) >> 8;
    uint8_t b = (g_draw_colour & 0xFF0000) >> 16;
    uint8_t a = static_cast<uint8_t>(g_draw_alpha * 255);
    C2D_DrawCircleSolid(x, y, 0.5f, radius, C2D_Color32(r, g, b, a));
}

void Display::draw_outline_circle(coord_t, coord_t, coord_t) {}
void Display::draw_fill_colour(uint32_t) {}
void Display::set_circle_precision(uint32_t) {}
void Display::set_colour_mask(bool, bool, bool, bool) {}
void Display::set_depth_test(bool) {}
void Display::set_culling(bool) {}
void Display::set_zwrite(bool) {}
void Display::transform_identity() {}
void Display::transform_apply(std::array<float, 16>) {}
void Display::transform_apply_rotation(real_t, real_t, real_t, real_t) {}
void Display::transform_stack_clear() {}
bool Display::transform_stack_empty() { return true; }
bool Display::transform_stack_top() { return true; }
bool Display::transform_stack_discard() { return true; }
void Display::transform_vertex(std::array<double, 3>&) {}
void Display::transform_vertex_mv(std::array<double, 3>&) {}
void Display::transform_vertex_mvp(std::array<double, 3>&) {}
void Display::set_camera(coord_t, coord_t, coord_t, coord_t, coord_t, coord_t, coord_t, coord_t, coord_t, real_t, real_t, coord_t, coord_t) {}
void Display::set_camera_ortho(real_t, real_t, real_t, real_t, real_t) {}
void Display::set_fog(bool, real_t, real_t, uint32_t) {}
void Display::set_window_position(real_t, real_t) {}
void Display::set_window_size(real_t w, real_t h) {
    g_window_width = static_cast<uint32_t>(w);
    g_window_height = static_cast<uint32_t>(h);
}
geometry::Vector<real_t> Display::get_display_dimensions() { return {0, 0}; }
uint32_t Display::get_clear_colour() { return g_clear_colour; }
void Display::set_clear_colour(uint32_t c) { g_clear_colour = c; }
geometry::Vector<real_t> Display::get_mouse_coord_invm() { return {staticExecutor.m_mouse_x, staticExecutor.m_mouse_y}; }
geometry::Vector<real_t> Display::get_mouse_coord() { return {staticExecutor.m_mouse_x, staticExecutor.m_mouse_y}; }
void Display::set_vsync(bool) {}
void Display::set_font(asset::AssetFont*, TextureView*) {}
void Display::disable_scissor() {}
void Display::enable_scissor(real_t, real_t, real_t, real_t) {}
void Display::set_blending_enabled(bool) {}
void Display::set_interpolation_linear(bool) {}

} // namespace ogm::interpreter



#endif
