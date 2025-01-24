#version 320 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{   //反相
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}