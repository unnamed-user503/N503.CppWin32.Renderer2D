#pragma once
#include <stdint.h>

#ifdef N503_DLL_EXPORTS
#define N503_API __declspec(dllexport)
#else
#define N503_API __declspec(dllimport)
#endif

typedef int32_t n503_result_t;

static const int32_t N503_OK    = 0;
static const int32_t N503_ERROR = -1;

typedef struct n503_vector2_t
{
    float x;
    float y;
} n503_vector2_t;

typedef struct n503_vector3_t
{
    float x;
    float y;
    float z;
} n503_vector3_t;

typedef struct n503_color_t
{
    float red;
    float green;
    float blue;
    float alpha;
} n503_color_t;

typedef struct n503_rect_t
{
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
} n503_rect_t;

typedef struct n503_transform_t
{
    n503_vector3_t position;
    float rotation;
    n503_vector2_t scale;
} n503_transform_t;

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct n503_renderer2d_sprite_t* n503_renderer2d_sprite_h;

    N503_API n503_renderer2d_sprite_h n503_renderer2d_sprite_create(const char* path, const n503_rect_t source);

    N503_API n503_result_t n503_renderer2d_sprite_destroy(n503_renderer2d_sprite_h instance);

    N503_API n503_result_t n503_renderer2d_sprite_set_transform(n503_renderer2d_sprite_h instance, const n503_transform_t transform);

    N503_API n503_result_t n503_renderer2d_sprite_set_color(n503_renderer2d_sprite_h instance, const n503_color_t color);

    N503_API n503_result_t n503_renderer2d_sprite_set_render_group(n503_renderer2d_sprite_h instance, uint32_t group);

    N503_API n503_result_t n503_renderer2d_sprite_set_visible(n503_renderer2d_sprite_h instance, uint32_t visible);

    typedef struct n503_renderer2d_sprite_group_t* n503_renderer2d_sprite_group_h;
    typedef uint32_t (*n503_renderer2d_sprite_group_set_transform_delegate_t)(uint64_t index, n503_transform_t* out_transform, void* user_data);
    typedef uint32_t (*n503_renderer2d_sprite_group_set_color_delegate_t)(uint64_t index, n503_color_t* out_color, void* user_data);

    __declspec(dllexport) n503_renderer2d_sprite_group_h n503_renderer2d_sprite_group_create(const char* path, uint32_t count, const n503_rect_t source);

    __declspec(dllexport) int n503_renderer2d_sprite_group_destroy(n503_renderer2d_sprite_group_h instance);

    __declspec(dllexport) int n503_renderer2d_sprite_group_set_transform(n503_renderer2d_sprite_group_h instance, n503_renderer2d_sprite_group_set_transform_delegate_t delegate, void* user_data);

    __declspec(dllexport) int n503_renderer2d_sprite_group_set_color(n503_renderer2d_sprite_group_h instance, n503_renderer2d_sprite_group_set_color_delegate_t delegate, void* user_data);

    __declspec(dllexport) int n503_renderer2d_sprite_group_set_render_group(n503_renderer2d_sprite_group_h instance, uint32_t group);

    __declspec(dllexport) int n503_renderer2d_sprite_group_set_visible(n503_renderer2d_sprite_group_h instance, uint32_t visible);

    typedef struct n503_renderer2d_text_t* n503_renderer2d_text_h;

    __declspec(dllexport) n503_renderer2d_text_h n503_renderer2d_text_create(const char* text, const char* font, float size, float r, float g, float b, float a);

    __declspec(dllexport) int n503_renderer2d_text_destroy(n503_renderer2d_text_h instance);

    __declspec(dllexport) int n503_renderer2d_text_set_transform(n503_renderer2d_text_h instance, float x, float y, float scaleX, float scaleY, float rotation);

    __declspec(dllexport) int n503_renderer2d_text_set_color(n503_renderer2d_text_h instance, float r, float g, float b, float a);

    __declspec(dllexport) int n503_renderer2d_text_set_visible(n503_renderer2d_text_h instance, uint32_t visible);

    __declspec(dllexport) int n503_renderer2d_text_set_content(n503_renderer2d_text_h instance, const char* content);

#ifdef __cplusplus
}
#endif
