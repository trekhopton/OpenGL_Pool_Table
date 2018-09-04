
#version 330

in vec4 pos; // Interpolated vertex position, in object coords

// The final colour we will see at this location on screen
out vec4 fragColour;

uniform vec4 colour;

void main(void)
{

	fragColour = colour;

}
