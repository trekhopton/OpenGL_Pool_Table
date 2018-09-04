
#version 330


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_matrix;

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle

// Position variables. 1 per vertex.
layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

//out
out vec4 pos;
out vec3 normal; // the eye space normal

void main(void)
{

	pos =  model * vec4(a_vertex, 1.0);
	normal = normalize(normal_matrix * a_normal);

	gl_Position = projection * view * pos;

}
