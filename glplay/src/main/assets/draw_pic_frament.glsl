#version 320 es

precision mediump float;

in vec2 v_texcoord;
uniform lowp sampler2D s_texturePic;


out vec4 FragColor;

void main() {
     FragColor = texture(s_texturePic, v_texcoord);
}
