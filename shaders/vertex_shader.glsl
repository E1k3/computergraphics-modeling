#version 330 core

layout(location = 0) in vec3 pos;
out vec4 vertex_color;

uniform mat4 mvp;

void main()
{
	vec4 position = mvp * vec4(pos / 100.f, 1.f);
	gl_Position = position;
	vertex_color = vec4(position.zzz * -0.5f + 0.5f, 1.f);
}
