#version 330 core
out vec4 FragColor;

// 在片段着色器中定义相应的输入变量：
in vec3 Normal; // 法向量 
in vec3 FragPos; // 片段的位置 

// 光源的位置是一个静态变量，我们可以简单地在片段着色器中把它声明为uniform：
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    // 把环境光照添加到场景里非常简单。我们用光的颜色乘以一个很小的常量环境因子，
    // 再乘以物体的颜色，然后将最终结果作为片段的颜色：
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    // 第一件事是计算光源和片段位置之间的方向向量。光的方向向量是光源位置向量与片段位置向量之间的向量差。
    // 我们能够简单地通过让两个向量相减的方式计算向量差。我们同样希望确保所有相关向量最后都转换为单位向量，
    // 所以我们把法线和最终的方向向量都进行标准化：
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    // 当计算光照时我们通常不关心一个向量的模长或它的位置，我们只关心它们的方向。所以，几乎所有的计算都使用
    // 单位向量完成，因为这简化了大部分的计算（比如点乘）。所以当进行光照计算时，确保你总是对相关向量进行标
    // 准化，来保证它们是真正地单位向量。忘记对向量进行标准化是一个十分常见的错误。

    // 下一步，我们对norm和lightDir向量进行点乘，计算光源对当前片段实际的漫发射影响。结果值再乘以光的颜色，
    // 得到漫反射分量。两个向量之间的角度越大，漫反射分量就会越小：
    // 如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数。为此，我们使用
    // max函数返回两个参数之间较大的参数，从而保证漫反射分量不会变成负数。负数颜色的光照是没有定义的，所以最
    // 好避免它，除非你是那种古怪的艺术家。
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 现在我们有了环境光分量和漫反射分量，我们把它们相加，然后把结果乘以物体的颜色，来获得片段最后的输出颜色。
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
} 