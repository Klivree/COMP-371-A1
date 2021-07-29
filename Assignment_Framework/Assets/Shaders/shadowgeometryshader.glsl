#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
    
uniform mat4 shadowMatrices1;
uniform mat4 shadowMatrices2;
uniform mat4 shadowMatrices3;
uniform mat4 shadowMatrices4;
uniform mat4 shadowMatrices5;
uniform mat4 shadowMatrices6;


out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    gl_Layer = 0; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) { // for each triangle vertex
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices1 * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

    gl_Layer = 1; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) { // for each triangle vertex
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices2 * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

    gl_Layer = 2; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) { // for each triangle vertex
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices3 * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

    gl_Layer = 3; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) { // for each triangle vertex
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices4 * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

    gl_Layer = 4; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) { // for each triangle vertex
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices5 * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

    gl_Layer = 5; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) { // for each triangle vertex
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices6 * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

}  