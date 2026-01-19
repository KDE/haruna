/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    vec4 colorSource;
    float qt_Opacity;
};
layout(binding = 1) uniform sampler2D maskSource;
void main() {
    fragColor = colorSource * texture(maskSource, qt_TexCoord0).a * qt_Opacity;
}
