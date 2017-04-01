#version 410 core

in vec3 vv3color;

out vec4 fragColor;

void main() 
{
	fragColor = vec4 ( vv3color , 1.0 );
}



