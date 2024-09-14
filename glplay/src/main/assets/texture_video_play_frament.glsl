#version 320 es

precision mediump float;

in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;

// https://stackoverflow.com/questions/26695253/when-switching-to-glsl-300-met-the-following-error
//The predefined variable gl_FragColor does not exist anymore in GLSL ES 3.00.
//out vec4 gl_FragColor;
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
     FragColor = vec4(r, g, b, 1.0f);
//    gl_FragColor = vec4(r, g, b, 1.0);

}
