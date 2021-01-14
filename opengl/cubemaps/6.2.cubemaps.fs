#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox; // 使用了一个不同类型的采样器，samplerCube，我们将使用texture函数使用它进行采样，但这次我们将使用一个vec3的方向向量而不是vec2。

void main()
{    
    // 创建反射效果并不会很难。我们将会改变箱子的片段着色器，让箱子有反射性：
    // vec3 I = normalize(Position - cameraPos); // 先计算了观察/摄像机方向向量I
    // vec3 R = reflect(I, normalize(Normal)); // 并使用它来计算反射向量R
    // FragColor = vec4(texture(skybox, R).rgb, 1.0); // 之后我们将使用R来从天空盒立方体贴图中采样。

    // 折射，我们已经绑定了立方体贴图，提供了顶点数据和法线，并设置了摄像机位置的uniform。唯一要修改的就是片段着色器：
    float ratio = 1.00 / 1.52; // 光线/视线从空气进入玻璃（如果我们假设箱子是玻璃制的）
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}