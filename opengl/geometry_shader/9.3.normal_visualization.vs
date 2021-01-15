#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;

// 这次在几何着色器中，我们会使用模型提供的顶点法线，而不是自己生成
void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));
    gl_Position = view * model * vec4(aPos, 1.0); 
}