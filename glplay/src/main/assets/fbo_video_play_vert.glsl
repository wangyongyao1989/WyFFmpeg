#version 320 es

out vec2 v_texcoord;
in vec3 position;
in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    v_texcoord = texcoord;      //YUV纹理数据的输入
   // gl_Position =  position;
    gl_Position = projection * view * model * vec4(position, 1.0);
}

