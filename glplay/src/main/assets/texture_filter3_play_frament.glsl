#version 320 es

precision mediump float;
const float PI = 3.1415926535;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;

out vec4 FragColor;

//旋流过滤器
void main() {

        float radius = 200.0;
        float angle = 0.8;
        vec2 center = vec2(texSize.x / 2.0, texSize.y / 2.0);
        vec2 tc = v_texcoord * texSize;
        tc -= center;
        float dist = length(tc);
        if (dist < radius) {
              float percent = (radius - dist) / radius;
              float theta = percent * percent * angle * 8.0;
              float s = sin(theta);
              float c = cos(theta);
              tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
        }
        tc += center;
        float y, u, v, r, g, b;
        y = texture(s_textureY, tc / texSize).r;
        u = texture(s_textureU, tc / texSize).r;
        v = texture(s_textureV, tc / texSize).r;
        u = u - 0.5;
        v = v - 0.5;
        r = y + 1.403 * v;
        g = y - 0.344 * u - 0.714 * v;
        b = y + 1.770 * u;
        FragColor = vec4(r, g, b, 1.0);

}
