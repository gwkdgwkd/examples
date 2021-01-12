#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far = 100.0; 
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC ,将深度值变换为NDC
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{     
    // 使用获取到的z值，应用逆变换来获取线性的深度值： 
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far to get depth in range [0,1] for visualization purposes
    FragColor = vec4(vec3(depth), 1.0);
}