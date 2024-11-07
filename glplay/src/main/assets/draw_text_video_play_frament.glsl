#version 320 es

precision mediump float;

in vec2 v_texcoord;
in vec2 v_picTextureCoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform lowp sampler2D s_texturePic;

out vec4 FragColor;

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

     //FragColor = vec4(r, g, b, 1.0f);
     //FragColor =  picTexture * vec4(r, g, b, 1.0f);
     FragColor =  texture(s_texturePic, v_picTextureCoord) * vec4(r, g, b, 1.0f);
}
