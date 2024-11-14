#version 320 es
precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);

    FragColor = vec4(textColor, 1.0) * sampled;
}