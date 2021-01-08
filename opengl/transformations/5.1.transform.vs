#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 transform; // GLSL也有mat2和mat3类型从而允许了像向量一样的混合运算。

void main()
{
    // 如何把矩阵传递给着色器？GLSL里也有一个mat4类型。修改顶点着色器让其接收一个mat4的uniform变量，然后再用矩阵uniform乘以位置向量：
    gl_Position = transform * vec4(aPos, 1.0f);
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}