#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox; // 使用了一个不同类型的采样器，samplerCube，我们将使用texture函数使用它进行采样，但这次我们将使用一个vec3的方向向量而不是vec2。

// 将输入的位置向量作为输出给片段着色器的纹理坐标。片段着色器会将它作为输入来采样samplerCube：
void main()
{    
    // 片段着色器非常直观。我们将顶点属性的位置向量作为纹理的方向向量，并使用它从立方体贴图中采样纹理值。
    FragColor = texture(skybox, TexCoords);
}