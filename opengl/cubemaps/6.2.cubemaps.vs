#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal; // 我们现在使用了一个法向量，所以我们将再次使用法线矩阵(Normal Matrix)来变换它们。
    // 顶点着色器的这个Position输出将用来在片段着色器内计算观察方向向量。
    Position = vec3(model * vec4(aPos, 1.0)); // Position输出向量是一个世界空间的位置向量。
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}