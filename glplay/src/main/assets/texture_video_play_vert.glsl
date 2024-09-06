#version 320 es

out vec2 v_texcoord;

in vec4 position;
in vec2 texcoord;

void main() {
    v_texcoord = texcoord;
    gl_Position =  position;
}
