#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// 由于我们已经有了逐片段位置和法线数据(G缓冲中)，我们只需要更新一下几何着色器，让它包含片段的线性深度就行了。
void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos; // 储存片段的位置矢量到第一个G缓冲纹理
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal); // 储存法线信息到G缓冲
    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95); // 和漫反射颜色
}