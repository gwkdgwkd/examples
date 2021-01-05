#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

// 这个片段着色器从uniform变量中接受物体的颜色和光源的颜色。

void main()
{
    FragColor = vec4(lightColor * objectColor, 1.0);
}