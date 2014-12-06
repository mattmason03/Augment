#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 color;

layout (location = 0) uniform mat4 projection;

out VS_OUT{
	vec2 texCoord;
	vec4 color;
} vs_out;

void main(void){

	vs_out.texCoord = texCoord;
	vs_out.color = color;
	gl_Position = projection * vec4(position, 0.0, 1.0);

}