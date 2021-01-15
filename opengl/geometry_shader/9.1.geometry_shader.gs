#version 330 core
layout (points) in;
// layout (points, max_vertices = 1) out;
// layout (line_strip, max_vertices = 2) out;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{    
    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // 1:bottom-left   
    EmitVertex();   
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0); // 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0); // 3:top-left
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0); // 4:top-right
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0); // 5:top
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_house(gl_in[0].gl_Position);

    // 创建一个传递(Pass-through)几何着色器，它会接收一个点图元，并直接将它传递(Pass)到下一个着色器：
    // fColor = gs_in[0].color;
    // gl_Position = gl_in[0].gl_Position; 
    // EmitVertex();
    // EndPrimitive();

    // 画四条线
    // fColor = gs_in[0].color;
    // gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    // EmitVertex();
    // gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    // EmitVertex();
    // EndPrimitive();
}