#version 320 es

precision mediump float;
const float PI = 3.1415926535;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;

out vec4 FragColor;

//捕食者热视觉滤镜
void main() {
               float y, u, v, r, g, b;
               y = texture(s_textureY, v_texcoord).r;
               u = texture(s_textureU, v_texcoord).r;
               v = texture(s_textureV, v_texcoord).r;
               u = u - 0.5;
               v = v - 0.5;
               r = y + 1.403 * v;
               g = y - 0.344 * u - 0.714 * v;
               b = y + 1.770 * u;
               vec3 color = vec3(r, g, b);
               vec2 uv = v_texcoord.xy;
               vec3 colors[3];
               colors[0] = vec3(0.,0.,1.);
               colors[1] = vec3(1.,1.,0.);
               colors[2] = vec3(1.,0.,0.);
               float lum = (color.r + color.g + color.b)/3.;
               int idx = (lum < 0.5) ? 0 : 1;
               vec3 rgb = mix(colors[idx],colors[idx+1],(lum-float(idx)*0.5)/0.5);
               FragColor = vec4(rgb, 1.0);

}
