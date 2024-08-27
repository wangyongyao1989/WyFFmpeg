#version 320 es

layout (location = 0) in vec3 aPos;     // 位置变量的属性位置值为0
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);                      // 注意我们如何把一个vec3作为vec4的构造器的参数
    ourColor = aColor;

    TexCoord = vec2(aTexCoord.x, aTexCoord.y);          //纹理数据的输入
}