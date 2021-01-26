#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex> 将位置和纹理纹理坐标的数据合起来存在一个vec4中
out vec2 TexCoords;

uniform mat4 projection;

// 将位置坐标与一个投影矩阵相乘，并将纹理坐标传递给片段着色器：
void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}