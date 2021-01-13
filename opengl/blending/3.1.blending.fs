#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{   
    // 保证你在片段着色器中获取了纹理的全部4个颜色分量，而不仅仅是RGB分量： 
    // FragColor = vec4(vec3(texture(texture1, TexCoords)), 1.0);
    // FragColor = texture(texture1, TexCoords); // 没有处理alpha
    vec4 texColor = texture(texture1, TexCoords);
    // GLSL给了我们discard命令，一旦被调用，它就会保证片段不会被进一步处理，所以就不会进入颜色缓冲。有了这个指令，我们就能够在片段着色
    // 器中检测一个片段的alpha值是否低于某个阈值，如果是的话，则丢弃这个片段，就好像它不存在一样：
    // 检测被采样的纹理颜色的alpha值是否低于0.1的阈值，如果是的话，则丢弃这个片段。片段着色器保证了它只会渲染不是（几乎）完全透明的片段。
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}