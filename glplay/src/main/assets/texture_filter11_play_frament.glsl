#version 320 es

precision mediump float;
const float PI = 3.1415926535;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;

out vec4 FragColor;

 vec4 YuvToRgb(vec2 uv) {
        float y, u, v, r, g, b;
        y = texture(s_textureY, uv).r;
        u = texture(s_textureU, uv).r;
        v = texture(s_textureV, uv).r;
        u = u - 0.5;
        v = v - 0.5;
        r = y + 1.403 * v;
        g = y - 0.344 * u - 0.714 * v;
        b = y + 1.770 * u;
        return vec4(r, g, b, 1.0);
    }

//压花过滤器
void main() {
               vec4 color;
               color.rgb = vec3(0.5);
               vec2 onePixel = vec2(1.0 / texSize.x, 1.0 / texSize.y);
               color -= YuvToRgb(v_texcoord - onePixel) * 5.0;
               color += YuvToRgb(v_texcoord + onePixel) * 5.0;
               color.rgb = vec3((color.r + color.g + color.b) / 3.0);
               FragColor = vec4(color.rgb, 1.0);

}
