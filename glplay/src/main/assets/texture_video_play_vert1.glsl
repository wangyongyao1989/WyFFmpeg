#version 320 es

out vec2 v_texcoord;

in vec4 position;
in vec2 texcoord;

uniform mat4 projection;
uniform mat4 rotation;
uniform mat4 scale;

void main() {
      v_texcoord = texcoord;
      //gl_Position = projection * rotation * scale * position;
      gl_Position = projection * rotation * scale * position;
}
