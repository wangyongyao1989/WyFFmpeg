#version 320 es

out vec2 v_texcoord;
//out vec2 v_picTextureCoord;

in vec4 position;
in vec2 texcoord;
//in vec2 picTextureCoord;    //纹理图片坐标


void main() {
    v_texcoord = texcoord;      //YUV纹理数据的输入
   // v_picTextureCoord = picTextureCoord; //纹理数据的输入
    gl_Position =  position;
}

