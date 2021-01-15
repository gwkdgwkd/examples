#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];

out vec2 TexCoords; 

uniform float time;

// 知道了如何计算法向量就能够创建一个explode函数了，它使用法向量和顶点位置向量作为参数。这个函数会返回一个新的向量，它是位置向量沿着法线向量进行位移之后的结果：
vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    // sin函数接收一个time参数，它根据时间返回一个-1.0到1.0之间的值。因为我们不想让物体向内爆炸(Implode)，我们将sin值变换到了[0, 1]的范围内。
    // 最终的结果会乘以normal向量，并且最终的direction向量会被加到位置向量上。
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

// 使用3个输入顶点坐标来获取法向量：
vec3 GetNormal()
{
    // 使用减法获取了两个平行于三角形表面的向量a和b。因为两个向量相减能够得到这两个向量之间的差值，
    // 并且三个点都位于三角平面上，对任意两个向量相减都能够得到一个平行于平面的向量。
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b)); // 如果我们交换了cross函数中a和b的位置，我们会得到一个指向相反方向的法向量——这里的顺序很重要！
}

void main() {    
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}