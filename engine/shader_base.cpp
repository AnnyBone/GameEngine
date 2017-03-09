/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "engine_base.h"

#include "video.h"
#include "video_shader.h"

#include "client/shader_base.h"

BaseShader::BaseShader() : ShaderProgram("base"),
                           alpha_test(false) {
}

void BaseShader::Initialize() {
    RegisterShader("base", PL_SHADER_VERTEX);
    RegisterShader("base", PL_SHADER_FRAGMENT);

    Link();

    RegisterAttributes();

    // Textures
    SHADER_REGISTER_UNIFORM(u_diffusemap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_DIFFUSE);
    SHADER_REGISTER_UNIFORM(u_detailmap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_DETAIL);
    SHADER_REGISTER_UNIFORM(u_fullbrightmap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_FULLBRIGHT);
    SHADER_REGISTER_UNIFORM(u_spheremap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_SPHERE);

    // Alpha
    SHADER_REGISTER_UNIFORM(u_alphaclamp, VL_UNIFORM_FLOAT, 0.5f);
    SHADER_REGISTER_UNIFORM(u_alphatest, VL_UNIFORM_BOOL, alpha_test);

    // Lighting
    SHADER_REGISTER_UNIFORM(u_lightcolour, VL_UNIFORM_VEC4, pl_white);
    SHADER_REGISTER_UNIFORM(u_lightposition, VL_UNIFORM_VEC3, pl_origin3f);

    // Vertex scaling
    SHADER_REGISTER_UNIFORM(u_vertexscale, VL_UNIFORM_FLOAT, 1.0f);
}

void BaseShader::EnableAlphaTest() {
    if (alpha_test)
        return;

    alpha_test = true;
    SetUniformVariable(u_alphatest, alpha_test);
}

void BaseShader::DisableAlphaTest() {
    if (!alpha_test)
        return;

    alpha_test = false;
    SetUniformVariable(u_alphatest, alpha_test);
}