#pragma once
#include <stdint.h>

#ifdef N503_DLL_EXPORTS
#define N503_API __declspec(dllexport)
#else
#define N503_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct N503Vector2F_t
    {
        float X;
        float Y;
    } N503Vector2F;

    typedef struct N503Vector3F_t
    {
        float X;
        float Y;
        float Z;
    } N503Vector3F;

    typedef struct N503Color_t
    {
        float Red;
        float Green;
        float Blue;
        float Alpha;
    } N503Color;

    typedef struct N503RectU_t
    {
        uint32_t Left;
        uint32_t Top;
        uint32_t Right;
        uint32_t Bottom;
    } N503RectU;

    typedef struct N503Transform2D_t
    {
        N503Vector2F Position;
        float Rotation;
        N503Vector2F Scale;
    } N503Transform2D;

    typedef struct N503Transform3D_t
    {
        N503Vector3F Position;
        N503Vector3F Rotation;
        N503Vector3F Scale;
    } N503Transform3D;

    typedef struct N503Sprite_t* N503Sprite;

    N503_API N503Sprite N503CreateSprite(const char* path, const N503RectU source);

    N503_API int N503SpriteDestroy(N503Sprite sprite);

    N503_API int N503SetSpriteTransform(N503Sprite sprite, const N503Transform2D transform);

    N503_API int N503SetSpriteColor(N503Sprite sprite, const N503Color color);

    N503_API int N503SetSpriteRenderGroup(N503Sprite sprite, uint32_t group);

    N503_API int N503SetSpriteVisible(N503Sprite sprite, uint32_t visible);

    //N503_API int N503SetSpriteAnchor(N503Sprite sprite, N503Vector2F pivot);


    typedef struct N503SpriteGroup_t* N503SpriteGroup;
    typedef uint32_t (*N503SetSpriteGroupTransformDelegate)(uint64_t index, N503Transform2D* transform, void* pfnDelegate);
    typedef uint32_t (*N503SetSpriteGroupColorDelegate)(uint64_t index, N503Color* color, void* pfnDelegate);
    typedef uint32_t (*N503SetSpriteGroupVisibleDelegate)(uint64_t index, bool visible, void* pfnDelegate);

    N503_API N503SpriteGroup N503CreateSpriteGroup(const char* path, uint16_t count, const N503RectU source);

    N503_API int N503DestroySpriteGroup(N503SpriteGroup spriteGroup);

    //N503_API int N503SetSpriteGroupTransform(N503SpriteGroup spriteGroup, const uint16_t index, const N503Transform2D transform);

    //N503_API int N503SetSpriteGroupColor(N503SpriteGroup spriteGroup, const uint16_t index, const N503Color color);

    //N503_API int N503SetSpriteGroupVisible(N503SpriteGroup spriteGroup, const uint16_t index, const uint32_t visible);

    N503_API int N503SetSpriteGroupTransformBatch(N503SpriteGroup spriteGroup, N503SetSpriteGroupTransformDelegate bridge, void* pfnDelegate);

    N503_API int N503SetSpriteGroupColorBatch(N503SpriteGroup spriteGroup, N503SetSpriteGroupColorDelegate bridge, void* pfnDelegate);

    //N503_API int N503SetSpriteGroupVisibleBatch(N503SpriteGroup spriteGroup, N503SetSpriteGroupVisibleDelegate bridge, void* pfnDelegate);

    N503_API int N503SetSpriteGroupRenderGroupBatch(N503SpriteGroup spriteGroup, uint32_t group);





    typedef struct N503Text_t* N503Text;

    __declspec(dllexport) N503Text N503CreateText(const char* content, const char* font, float size);

    __declspec(dllexport) int N503DestroyText(N503Text instance);

    __declspec(dllexport) int N503SetTextTransform(N503Text instance, const N503Transform2D transform);

    __declspec(dllexport) int N503SetTextColor(N503Text instance, const N503Color color);

    __declspec(dllexport) int N503SetTextVisible(N503Text instance, uint32_t visible);

    __declspec(dllexport) int N503SetTextContent(N503Text instance, const char* content);

#ifdef __cplusplus
}
#endif
