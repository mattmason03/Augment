#version 430 core

in VS_OUT{
	vec2 texCoord;
	vec4 color;
} fs_in;

layout (binding = 0) uniform sampler2D img;

out vec4 color;

void main(void){
	vec4 imgColor = texture(img, fs_in.texCoord);
	//color = imgColor;
	//color = vec4(fs_in.texCoord, 0, 1);

	color = vec4(fs_in.color.r * imgColor.r, 
				fs_in.color.g * imgColor.g, 
				fs_in.color.b * imgColor.b, 
				fs_in.color.a * imgColor.a);

}