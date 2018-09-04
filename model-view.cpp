#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <math.h>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "InputState.h"
#include "Viewer.h"
#include "Table.h"
#include "Sphere.hpp"
#include "Shader.hpp"
#include "Bouncer.h"

float tableWidth = 4.0f;
float tableLength = 6.0f;

clock_t startT;
clock_t prevT;
clock_t currentT;
double deltaT;

Table *TheTable;

Sphere *sphere;
unsigned int sphereVaoHandle;

Bouncer *bouncer;

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define LIGHT_SPEED 0.2f

int winX = 500;
int winY = 500;

/**
 Cameras
*/
WorldObjectViewer *ObjCam;
Viewer *Camera;

glm::vec3 cameraPos(0.0f, 5.0f, -17.0f);

glm::vec3 dirVec;

// Data structure storing mouse input info
InputState Input;

int vScale = 100;

unsigned int tableID;
unsigned int ballsID;

float lightX = 2.0f;
float lightY = 10.0f;
float lightZ = -3.0f;

int createSphereVAO(){
	// Using the sphere class to generate vertices and element indices
	sphere = new Sphere(0.4f, 16, 16);

	glGenVertexArrays(1, &sphereVaoHandle);
	glBindVertexArray(sphereVaoHandle);

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex position attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->vertCount,
                 sphere->vertices,
                 GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	//Normal attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->normCount,
                 sphere->normals,
                 GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

	// Vertex indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * sphere->indCount,
                 sphere->indices,
                 GL_STATIC_DRAW);


	return 0; // success
}

void initBouncer(){

	glm::vec3 lim = glm::vec3(tableWidth - 0.6, 0.1, tableLength - 0.6);

	bouncer = new Bouncer(3, 0.4, lim, 0.2);

	std::vector<glm::vec3> positions;
	positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(-2.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(2.0f, 0.0f, 0.0f));

	bouncer->initialise(positions);

}

int setupShaders(unsigned int id)
{
	glUseProgram(id);

    // Uniform lighting variables
    int lightambientHandle = glGetUniformLocation(id, "light_ambient");
	int lightdiffuseHandle = glGetUniformLocation(id, "light_diffuse");
	int lightspecularHandle = glGetUniformLocation(id, "light_specular");
	if ( lightambientHandle == -1 ||
         lightdiffuseHandle == -1 ||
         lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables\n");
		return 1;
    }

	float lightambient[3] = { 0.12f, 0.1f, 0.1f };	// ambient light components
    float lightdiffuse[3] = { 0.0f, 1.0f, 0.0f };	// diffuse light components
    float lightspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular light components

	glUniform3fv(lightambientHandle, 1, lightambient);
	glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
	glUniform3fv(lightspecularHandle, 1, lightspecular);

    // Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(id, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(id, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(id, "mtl_specular");
	if ( mtlambientHandle == -1 ||
         mtldiffuseHandle == -1 ||
         mtlspecularHandle == -1) {
        fprintf(stderr, "Error: can't find material uniform variables\n");
		return 1;
    }

	float mtlambient[3] = {0.12f, 0.1f, 0.1f};	// ambient material
    float mtldiffuse[3] = {0.0f, 1.0f, 0.0f};	// diffuse material
    float mtlspecular[3] = {1.0f, 1.0f, 1.0f};	// specular material

	glUniform3fv(mtlambientHandle, 1, mtlambient);
	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
	glUniform3fv(mtlspecularHandle, 1, mtlspecular);

	return 0;	// return success
}

void render(double deltaT){

    // Update the camera, and draw the scene.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glm::mat4 viewMtx = Camera->getViewMtx();
	dirVec = glm::vec3(viewMtx[2][0], viewMtx[2][1], viewMtx[2][2]);
	dirVec = glm::normalize(dirVec);

    Camera->update( Input );

	//------view---------

    // First load the viewing matrix from the current camera
    glm::mat4 viewMatrix;
    viewMatrix = Camera->getViewMtx();

	glUseProgram(ballsID);

	// Load it to the shader program
	int viewHandle = glGetUniformLocation(ballsID, "view");
	if (viewHandle == -1) {
		std::cout << "Uniform: view is not an active uniform label\n";
	}
	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );

	//-------spheres-----

	bouncer->update(deltaT);

	std::vector<glm::vec3> posVec = bouncer->getCurrentPos();

	int mvHandle = glGetUniformLocation(ballsID, "model");
	int normHandle = glGetUniformLocation(ballsID, "normal_matrix");
	int lightposHandle = glGetUniformLocation(ballsID, "light_pos");
	if (mvHandle == -1 || normHandle==-1 || lightposHandle == -1) {
		fprintf(stderr, "Error: can't find matrix uniforms\n");
		exit(1);
	}

	// Update the light position
	float lightPos[4] = { lightX, lightY, lightZ, 1.0f };
	glUniform4fv(lightposHandle, 1, lightPos);

    glm::mat4 mvMatrix;
    glm::mat3 normMatrix;

	// Set VAO to the sphere mesh
	glBindVertexArray(sphereVaoHandle);
	// We compute the normal matrix from the current modelview matrix
    // and give it to our program
    mvMatrix = glm::translate(mvMatrix, posVec[0]);
    normMatrix = glm::mat3(mvMatrix);
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// Middle
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
	glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);

	int mtldiffuseHandle = glGetUniformLocation(ballsID, "mtl_diffuse");

    float mtldiffuse[3] = { 1.0f, 0.0f, 0.0f };	// diffuse material

	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);

	mvMatrix = glm::mat4(1.0);
    mvMatrix = glm::translate(mvMatrix, posVec[1]);
    normMatrix = glm::mat3(mvMatrix);
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// Left
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
	glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);

    float mtldiffuse2[3] = { 0.0f, 0.0f, 1.0f };	// diffuse material

	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse2);

	mvMatrix = glm::mat4(1.0);
    mvMatrix = glm::translate(mvMatrix, posVec[2]);
    normMatrix = glm::mat3(mvMatrix);
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// right
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
	glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glFlush();

	// -------table------

	glUseProgram(tableID);

	viewHandle = glGetUniformLocation(tableID, "view");
	if (viewHandle == -1) {
		std::cout << "Uniform: view is not an active uniform label\n";
	}
	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );

	 TheTable->render(tableID);

}


//
// Callbacks
//
void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch(key) {
			case GLFW_KEY_SPACE:
				bouncer->strike(0, dirVec * glm::vec3(5.0f, 0.0f, -5.0f));
				break;
            case GLFW_KEY_ESCAPE: // escape key pressed
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            default:
                break;
            }
    }
}

// Set the projection matrix. Takes into account window aspect ratio, so called
// when the window is resized.
void setProjection()
{
    glm::mat4 projection;
    projection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 1.0f, 30.0f );

	glUseProgram(tableID);

	// Load it to the shader program
	int projHandle = glGetUniformLocation(tableID, "projection");
	if (projHandle == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );

	glUseProgram(ballsID);

	// Load it to the shader program
	projHandle = glGetUniformLocation(ballsID, "projection");
	if (projHandle == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
}

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y )
{
    winX = x;
    winY = y;
    setProjection();
    glViewport( 0, 0, x, y );
}

void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    // Use a global data structure to store mouse info
    // We can then use it however we want
    Input.update((float)x, 0.0f);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        Input.rMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        Input.rMousePressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        Input.lMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Input.lMousePressed = false;
    }
}



/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int main (int argc, char **argv)
{
	GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(1);
    }

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and OpenGL context
    window = glfwCreateWindow(winX, winY, "Modelling and viewing", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	// Set up the shaders we are to use. 0 indicates error.
	tableID = LoadShaders("table.vert", "table.frag");
	ballsID = LoadShaders("balls.vert" , "balls.frag");
	if (tableID == 0 || ballsID == 0) {
		exit(1);
    }

	setupShaders(ballsID);

    // Set OpenGL state we need for this application.
    glClearColor(0.2F, 0.14F, 0.2F, 0.0F);
	glEnable(GL_DEPTH_TEST);

    // Set up the scene and the cameras
    setProjection();

    TheTable = new Table( tableWidth, tableLength, tableID );

	// Create vertex data for sphere and load onto GPU
	createSphereVAO();

	initBouncer();

    ObjCam = new WorldObjectViewer( cameraPos );
    Camera = ObjCam;

    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);

	// GLFW records the time since the program started (in seconds)
    double start = glfwGetTime();
    double now;

    while (!glfwWindowShouldClose(window))
    {
		now = glfwGetTime();

        render( now - start );
		start = now;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    delete TheTable;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);

    return 0;
}
