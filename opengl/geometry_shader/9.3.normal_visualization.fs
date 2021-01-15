#version 330 core
out vec4 FragColor;

void main()
{
    // 因为法线的可视化通常都是用于调试目的，我们可以使用片段着色器，将它们显示为单色的线（如果你愿意也可以是非常好看的线）：
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}

