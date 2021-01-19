#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

// 当我们以光的透视图进行场景渲染的时候，我们会用一个比较简单的着色器，这个着色器除了把顶点变换到光空间以外，不会做得更多了。
void main()
{
    // 将一个单独模型的一个顶点，使用lightSpaceMatrix变换到光空间中。
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}