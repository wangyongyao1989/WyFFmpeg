#version 320 es

precision mediump float;
const float PI = 3.1415926535;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;

out vec4 FragColor;

 vec4 YuvToRgb(vec2 uv) {
        float y, u, v, r, g, b;
        y = texture(s_textureY, uv).r;
        u = texture(s_textureU, uv).r;
        v = texture(s_textureV, uv).r;
        u = u - 0.5;
        v = v - 0.5;
        r = y + 1.403 * v;
        g = y - 0.344 * u - 0.714 * v;
        b = y + 1.770 * u;
        return vec4(r, g, b, 1.0);
    }

//边缘检测滤波器
void main() {
               vec2 pos = v_texcoord.xy;
               vec2 onePixel = vec2(1, 1) / texSize;
               vec4 color = vec4(0);
               mat3 edgeDetectionKernel = mat3(
                    -1, -1, -1,
                    -1, 8, -1,
                     -1, -1, -1
               );
               for(int i = 0; i < 3; i++) {
                  for(int j = 0; j < 3; j++) {
                    vec2 samplePos = pos + vec2(i - 1 , j - 1) * onePixel;
                    vec4 sampleColor = YuvToRgb(samplePos);
                    sampleColor *= edgeDetectionKernel[i][j];
                    color += sampleColor;
                  }
               }
               FragColor = vec4(color.rgb, 1.0);

}
