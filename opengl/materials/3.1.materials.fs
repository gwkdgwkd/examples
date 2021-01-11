#version 330 core
out vec4 FragColor;

// 当描述一个物体的时候，我们可以用这三个分量来定义一个材质颜色(Material Color)：环境光照(Ambient Lighting)、漫反射光照(Diffuse Lighting)
// 和镜面光照(Specular Lighting)。通过为每个分量指定一个颜色，我们就能够对物体的颜色输出有着精细的控制了。现在，我们再添加反光度(Shininess)这个
// 分量到上述的三个颜色中，这就有我们需要的所有材质属性了：
struct Material {
    vec3 ambient; // ambient材质向量定义了在环境光照下这个物体反射得是什么颜色，通常这是和物体颜色相同的颜色。
    vec3 diffuse; // diffuse材质向量定义了在漫反射光照下物体的颜色。（和环境光照一样）漫反射颜色也要设置为我们需要的物体颜色。
    vec3 specular; // specular材质向量设置的是镜面光照对物体的颜色影响（或者甚至可能反射一个物体特定的镜面高光颜色）。   
    float shininess; // shininess影响镜面高光的散射/半径。
}; 

// 为光照属性创建一个与材质结构体类似的结构体：
// 一个光源对它的ambient、diffuse和specular光照有着不同的强度。
struct Light {
    vec3 position; // 将光源的位置添加到了结构体中。

    vec3 ambient; // 环境光照通常会设置为一个比较低的强度，因为我们不希望环境光颜色太过显眼。
    vec3 diffuse; // 光源的漫反射分量通常设置为光所具有的颜色，通常是一个比较明亮的白色。
    vec3 specular; // 镜面光分量通常会保持为vec3(1.0)，以最大强度发光。
};

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material; // 创建一个结构体(Struct)来储存物体的材质属性。我们也可以把它们储存为独立的uniform值，但是作为一个结构体来储存会更有条理一些。
uniform Light light;

void main()
{
    // ambient, 环境光
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse, 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular, 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 