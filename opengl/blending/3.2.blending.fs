#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{             
    // 由于启用了混合，我们就不需要丢弃片段了，所以我们把片段着色器还原：
    FragColor = texture(texture1, TexCoords);
}