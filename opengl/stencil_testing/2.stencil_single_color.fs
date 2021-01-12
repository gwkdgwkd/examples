#version 330 core
out vec4 FragColor;

void main()
{
    // 创建一个很简单的片段着色器，它会输出一个边框颜色。我们简单地给它设置一个硬编码的颜色值
    FragColor = vec4(0.04, 0.28, 0.26, 1.0);
}