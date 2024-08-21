#version 320 es
precision mediump float;
out vec4 FragColor;
uniform vec4 ourColor;      //使用uniform全局变量
void main()
{
    FragColor = ourColor;
}