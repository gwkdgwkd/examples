#version 330 core
// 由于我们向顶点数组添加了额外的数据，所以我们应该更新光照的顶点着色器：
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 我们可以通过把顶点位置属性乘以模型矩阵（不是观察和投影矩阵）来把它变换到世界空间坐标。
    // 这个在顶点着色器中很容易完成，所以我们声明一个输出变量，并计算它的世界空间坐标：    
    FragPos = vec3(model * vec4(aPos, 1.0));
    // 将法向量由顶点着色器传递到片段着色器:
    Normal = aNormal;  

    gl_Position = projection * view * vec4(FragPos, 1.0);
}