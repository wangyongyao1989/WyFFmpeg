#version 320 es

in vec3 aPos;
in vec3 aColor;
//in vec2 aTexCoord;
in vec4 aTexCoord;


out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 vMatrix;

void main()
{
    gl_Position = vec4(aPos, 1.0);                      // 注意我们如何把一个vec3作为vec4的构造器的参数
    ourColor = aColor;

   // TexCoord = vec2(aTexCoord.x, aTexCoord.y);          //纹理数据的输入
    TexCoord = (vMatrix * aTexCoord).xy;
}