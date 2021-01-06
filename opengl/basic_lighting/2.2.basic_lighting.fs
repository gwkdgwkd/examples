#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; // 观察者位置（摄像机位置）
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 计算高光强度
    // specular
    // 首先，我们定义一个镜面强度(Specular Intensity)变量，给镜面高光一个中等亮度颜色，让它不要产生过度的影响。
    // 如果我们把它设置为1.0f，我们会得到一个非常亮的镜面光分量，这对于一个珊瑚色的立方体来说有点太多了。
    float specularStrength = 0.5;

    // 下一步，我们计算视线方向向量，和对应的沿着法线轴的反射向量：
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    // 对lightDir向量进行了取反。reflect函数要求第一个向量是从光源指向片段位置的向量，但是lightDir当前正好相反，
    // 是从片段指向光源（由先前我们计算lightDir向量时，减法的顺序决定）。为了保证我们得到正确的reflect向量，我们
    // 通过对lightDir向量取反来获得相反的方向。第二个参数要求是一个法向量，所以我们提供的是已标准化的norm向量。

    // 计算镜面分量:
    // 先计算视线方向与反射方向的点乘（并确保它不是负值），然后取它的32次幂。这个32是高光的反光度(Shininess)。一个
    // 物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小。
    // 不希望镜面成分过于显眼，所以我们把指数保持为32。
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    // 最后一件事情是把它加到环境光分量和漫反射分量里，再用结果乘以物体的颜色：
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 