



#version 330

// The final colour we will see at this location on screen
in vec4 pos; // Interpolated vertex position, in object coords
in vec3 normal;

out vec4 fragColour;

uniform vec4 light_pos;

uniform vec3 light_ambient;     // Light ambient RGBA values
uniform vec3 light_diffuse;     // Light diffuse RGBA values
uniform vec3 light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

const float shininess = 32;

// This is the same function that appeared in the vertex shader for
// per-vertex lighting. Now it is being calculated for each fragment
// individually.
vec3 phongPointLight(in vec4 position, in vec3 norm)
{
    vec3 s = normalize(vec3(light_pos - position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect( -s, norm );

    vec3 ambient = light_ambient * mtl_ambient;

    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = light_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    return ambient + diffuse + spec;
}

void main(void)
{
	fragColour = vec4(phongPointLight(pos, normalize(normal)), 1.0);
}
