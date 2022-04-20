#ifndef GFX_RENDERING_API_H
#define GFX_RENDERING_API_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct ShaderProgram;

struct GfxRenderingAPI {
    bool (*z_is_from_0_to_1)(void);
    void (*unload_shader)(struct ShaderProgram *old_prg);
    void (*load_shader)(struct ShaderProgram *new_prg);
    struct ShaderProgram *(*create_and_load_new_shader)(std::uint64_t shader_id0, std::uint32_t shader_id1);
    struct ShaderProgram *(*lookup_shader)(std::uint64_t shader_id0, std::uint32_t shader_id1);
    void (*shader_get_info)(struct ShaderProgram *prg, std::uint8_t *num_inputs, bool used_textures[2]);
    std::uint32_t (*new_texture)(void);
    void (*select_texture)(int tile, std::uint32_t texture_id);
    void (*upload_texture)(const std::uint8_t *rgba32_buf, std::uint32_t width, std::uint32_t height);
    void (*set_sampler_parameters)(int sampler, bool linear_filter, std::uint32_t cms, std::uint32_t cmt);
    void (*set_depth_test_and_mask)(bool depth_test, bool z_upd);
    std::uint16_t (*get_pixel_depth)(float x, float y);
    void (*set_zmode_decal)(bool zmode_decal);
    void (*set_viewport)(int x, int y, int width, int height);
    void (*set_scissor)(int x, int y, int width, int height);
    void (*set_use_alpha)(bool use_alpha);
    void (*draw_triangles)(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris);
    void (*init)(void);
    void (*on_resize)(void);
    void (*start_frame)(void);
    void (*end_frame)(void);
    void (*finish_render)(void);
    int (*create_framebuffer)(std::uint32_t width, std::uint32_t height);
    void (*resize_framebuffer)(int fb, std::uint32_t width, std::uint32_t height);
    void (*set_framebuffer)(int fb);
    void (*reset_framebuffer)();
    void (*select_texture_fb)(int fbID);
    void (*delete_texture)(std::uint32_t texID);
};

#endif
