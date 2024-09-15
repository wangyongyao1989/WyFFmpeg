#version 320 es

precision mediump float;
const float PI = 3.1415926535;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;

out vec4 FragColor;

//鱼眼滤镜
void main() {

        float aperture = 158.0;
        float apertureHalf = 0.5 * aperture * (PI / 180.0);
        float maxFactor = sin(apertureHalf);
        vec2 uv;
        vec2 xy = 2.0 * v_texcoord.xy - 1.0;
        float d = length(xy);
        if (d < (2.0 - maxFactor)) {
            d = length(xy * maxFactor);
            float z = sqrt(1.0 - d * d);
            float r = atan(d, z) / PI;
            float phi = atan(xy.y, xy.x);
            uv.x = r * cos(phi) + 0.5;
            uv.y = r * sin(phi) + 0.5;
        } else {
            uv = v_texcoord.xy;
        }
        float y, u, v, r, g, b;
        y = texture(s_textureY, uv).r;
        u = texture(s_textureU, uv).r;
        v = texture(s_textureV, uv).r;
        u = u - 0.5;
        v = v - 0.5;
        r = y + 1.403 * v;
        g = y - 0.344 * u - 0.714 * v;
        b = y + 1.770 * u;
        FragColor = vec4(r, g, b, 1.0);

}
