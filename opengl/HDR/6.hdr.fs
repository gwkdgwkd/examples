#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

// 渲染至浮点帧缓冲和渲染至一个普通的帧缓冲是一样的。新的东西就是这个的hdrShader的片段着色器，用来渲染最终拥有浮点颜色缓冲纹理的2D四边形。
void main()
{             
    // 将Reinhard色调映射应用到之前的片段着色器上，并且为了更好的测量加上一个Gamma校正过滤(包括SRGB纹理的使用)：
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(hdr)
    {
        // reinhard
        // vec3 result = hdrColor / (hdrColor + vec3(1.0)); // Reinhard色调映射
        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure); // 曝光色调映射
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma)); // Gamma校正
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }

    // 直接采样了浮点颜色缓冲并将其作为片段着色器的输出。然而，这个2D四边形的输出是被直接渲染到默认的帧缓冲中，导致所有片段着色器的输出值被约束在0.0到1.0间，
    // 尽管我们已经有了一些存在浮点颜色纹理的值超过了1.0。
    // vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    // color = vec4(hdrColor, 1.0);
}