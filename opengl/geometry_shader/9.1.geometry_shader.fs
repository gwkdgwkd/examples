#version 330 core
out vec4 FragColor;

in vec3 fColor;

void main()
{ 
    // FragColor = vec4(0.0, 1.0, 0.0, 1.0); // 硬编码绿色
    FragColor = vec4(fColor, 1.0);   
}