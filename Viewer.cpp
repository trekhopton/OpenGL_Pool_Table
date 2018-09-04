
#include <stdio.h>
#include <math.h>

#include "Viewer.h"
#include "glm/gtc/matrix_transform.hpp"

#define DEG2RAD(x) ((x)*M_PI/180.0)
#define RAD2DEG(x) ((x)*180.0/M_PI)

Viewer::Viewer( glm::vec3 eye )
{
    initEye = eye;
    reset();
}

Viewer::~Viewer(){}

/**
 Assumes the current matrix has been calculated (usually in
 update() or reset())
*/
const glm::mat4 Viewer::getViewMtx() const
{
    return viewMtx;
}

/**
 Resets the view matrix to the value the camera was
 initialised with. Assumes looking at the origin.
*/
void Viewer::reset()
{
    glm::vec3 at(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    viewMtx = glm::lookAt(initEye, at, up);
}

// ObjectViewer::ObjectViewer(glm::vec3 eye)
//     : Viewer(eye)
// {}

// void ObjectViewer::update( InputState &input )
// {
//     float xRot, yRot;
//     input.readDeltaAndReset( &yRot, &xRot );
//
//     if ( input.lMousePressed )
//     {
//         // The first 3 rows of the view matrix are the camera x, y, z axes
//         // in world coordinate space. (see lecture 6)
//         glm::vec3 eyeX( viewMtx[0][0], viewMtx[1][0], viewMtx[2][0] );
//         glm::vec3 eyeY( viewMtx[0][1], viewMtx[1][1], viewMtx[2][1] );
//         glm::vec3 eyeZ( viewMtx[0][2], viewMtx[1][2], viewMtx[2][2] );
//
//         // Rotate about the eye's y axis.
//         if ( yRot != 0 )
//         {
//             float sinY = sin(DEG2RAD(yRot));
//             float cosY = cos(DEG2RAD(yRot));
//
//             glm::vec3 tmpX = eyeX;
//             eyeX = cosY*tmpX + sinY*eyeZ;
//             eyeZ = -sinY*tmpX + cosY*eyeZ;
//         }
//         // Rotate about the eye's x axis.
//         if ( xRot != 0 )
//         {
//             float sinX = sin(DEG2RAD(xRot));
//             float cosX = cos(DEG2RAD(xRot));
//
//             glm::vec3 tmpY = eyeY;
//             eyeY = cosX*tmpY - sinX*eyeZ;
//             eyeZ = sinX*tmpY + cosX*eyeZ;
//         }
//
//         // Update the view matrix with new eye axes.
//         viewMtx[0][0] = eyeX[0];
//         viewMtx[1][0] = eyeX[1];
//         viewMtx[2][0] = eyeX[2];
//
//         viewMtx[0][1] = eyeY[0];
//         viewMtx[1][1] = eyeY[1];
//         viewMtx[2][1] = eyeY[2];
//
//         viewMtx[0][2] = eyeZ[0];
//         viewMtx[1][2] = eyeZ[1];
//         viewMtx[2][2] = eyeZ[2];
//     }
// }

/**
   Again, you almost certainly DON'T want to use this camera!
 */
WorldObjectViewer::WorldObjectViewer(glm::vec3 eye)
    : Viewer(eye)
{
    xRot = 0.0f;
    yRot = 0.0f;
}

void WorldObjectViewer::update( InputState &input )
{
    float x, y;
    input.readDeltaAndReset(&x, &y);

    if ( input.lMousePressed )
    {
        // Update rotation about world x and y axes
        yRot += 0.01f * x;
        xRot += 0.01f * y;

        // Rotate from our original camera position based on mouse motion
        reset();
        viewMtx = glm::rotate( viewMtx, yRot, glm::vec3(0.0f, 1.0f, 0.0f) );
        viewMtx = glm::rotate( viewMtx, xRot, glm::vec3(1.0f, 0.0f, 0.0f) );
    }
}
