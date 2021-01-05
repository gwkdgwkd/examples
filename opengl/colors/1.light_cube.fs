#version 330 core
out vec4 FragColor;

void main()
{
    // 灯的片段着色器给灯定义了一个不变的常量白色，保证了灯的颜色一直是亮的：
    FragColor = vec4(1.0); // set alle 4 vector values to 1.0
}