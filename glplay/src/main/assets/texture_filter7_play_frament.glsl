#version 320 es

precision mediump float;
const float PI = 3.1415926535;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;

out vec4 FragColor;


//像素过滤器
void main() {

      vec2 pixelSize = vec2(texSize.x/100.0, texSize.y/100.0);
      vec2 uv = v_texcoord.xy;
      float dx = pixelSize.x*(1./texSize.x);
      float dy = pixelSize.y*(1./texSize.y);
      vec2 coord = vec2(dx*floor(uv.x/dx),
      dy*floor(uv.y/dy));
      float y, u, v, r, g, b;
      y = texture(s_textureY, coord).r;
      u = texture(s_textureU, coord).r;
      v = texture(s_textureV, coord).r;
      u = u - 0.5;
      v = v - 0.5;
      r = y + 1.403 * v;
      g = y - 0.344 * u - 0.714 * v;
      b = y + 1.770 * u;

     FragColor = vec4(r, g, b, 1.0);

}
