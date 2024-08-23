#version 320 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture;
//uniform vec4 ourColor;      //使用uniform全局变量

void main()
{
    //FragColor = ourColor;
    FragColor = texture(texture, TexCoord);
}