#version 320 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

float weight[5] = float[](0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

void main()
{
    vec2 texOffset = vec2(1.0) / vec2(textureSize(screenTexture, 0)); // 获取纹理的像素大小
    vec3 result = texture(screenTexture, TexCoords).rgb * weight[0]; // 中心像素

    // 水平模糊
    for (int i = 1; i < 5; ++i) {
        float offset = texOffset.x * float(i);
        result += texture(screenTexture, TexCoords + vec2(offset, 0.0)).rgb * weight[i];
        result += texture(screenTexture, TexCoords - vec2(offset, 0.0)).rgb * weight[i];
    }

    FragColor = vec4(result, 1.0);
}