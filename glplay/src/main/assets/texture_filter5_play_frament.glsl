#version 320 es

precision mediump float;
const float PI = 3.1415926535;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;

out vec4 FragColor;



//利希滕斯坦式过滤器
void main() {

        float size = texSize.x / 75.0;
        float radius = size * 0.5;
        vec2 fragCoord = v_texcoord * texSize.xy;
        vec2 quadPos = floor(fragCoord.xy / size) * size;
        vec2 quad = quadPos/texSize.xy;
        vec2 quadCenter = (quadPos + size/2.0);
        float dist = length(quadCenter - fragCoord.xy);
        float y, u, v, r, g, b;
        y = texture(s_textureY, quad).r;
        u = texture(s_textureU, quad).r;
        v = texture(s_textureV, quad).r;
        u = u - 0.5;
        v = v - 0.5;
        r = y + 1.403 * v;
        g = y - 0.344 * u - 0.714 * v;
        b = y + 1.770 * u;
        if (dist > radius) {
          FragColor = vec4(0.25);
        } else {
          FragColor = vec4(r, g, b, 1.0);
        }

}
