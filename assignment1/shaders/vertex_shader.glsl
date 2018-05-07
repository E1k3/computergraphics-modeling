#version 330 core

layout(location = 0) in vec3 pos;
out vec4 vertex_color;

void main()
{
	gl_Position = vec4(pos / 100.f, 1.f);
	vertex_color = vec4(1.f);
}
