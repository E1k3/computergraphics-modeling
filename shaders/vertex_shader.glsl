#version 330 core

layout(location = 0) in vec3 pos;
out vec4 vertex_color;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(pos / 6.f, 1.f);
	vertex_color = vec4(1.f);
}
