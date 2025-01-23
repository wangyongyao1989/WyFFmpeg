#version 320 es

out vec2 v_texcoord;
in vec4 position;
in vec2 texcoord;

void main() {
    v_texcoord = texcoord;      //YUV纹理数据的输入
    gl_Position =  position;
}
