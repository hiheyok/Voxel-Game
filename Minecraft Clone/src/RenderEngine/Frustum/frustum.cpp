//***********************************************************************//
//                                                                         //
//        - "Talk to me like I'm a 3 year old!" Programming Lessons -         //
//                                                                       //
//        $Author:        DigiBen        digiben@gametutorials.com //
//                                                                         //
//        $Program:        Frustum Culling //
//                                                                         //
//        $Description:    Demonstrates checking if shapes are in view //
//                                                                         //
//***********************************************************************//
#include "RenderEngine/Frustum/frustum.h"

#include <gl/glew.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "RenderEngine/Camera/camera.h"

// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with
// frustum sides.
enum FrustumSide {
  RIGHT_ = 0,   // The RIGHT side of the frustum
  LEFT_ = 1,    // The LEFT     side of the frustum
  BOTTOM_ = 2,  // The BOTTOM side of the frustum
  TOP_ = 3,     // The TOP side of the frustum
  BACK_ = 4,    // The BACK    side of the frustum
  FRONT_ = 5    // The FRONT side of the frustum
};

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData {
  A = 0,  // The X value of the plane's normal
  B = 1,  // The Y value of the plane's normal
  C = 2,  // The Z value of the plane's normal
  D = 3   // The distance the plane is from the origin
};

///////////////////////////////// NORMALIZE PLANE
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This normalizes a plane (A side) from a given frustum.
/////
///////////////////////////////// NORMALIZE PLANE
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void NormalizePlane(float frustum[6][4], int side) {
  // Here we calculate the magnitude of the normal to the plane (point A B C)
  // Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
  // To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2
  // + z^2)
  float magnitude =
      static_cast<float>(sqrt(frustum[side][A] * frustum[side][A] +
                              frustum[side][B] * frustum[side][B] +
                              frustum[side][C] * frustum[side][C]));

  // Then we divide the plane's values by it's magnitude.
  // This makes it easier to work with.
  frustum[side][A] /= magnitude;
  frustum[side][B] /= magnitude;
  frustum[side][C] /= magnitude;
  frustum[side][D] /= magnitude;
}

///////////////////////////////// CALCULATE FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This extracts our frustum from the projection and modelview matrix.
/////
///////////////////////////////// CALCULATE FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFrustum::CalculateFrustum(Camera* cam) {
  float clip[16];

  glm::mat4 modl = cam->GetViewMatrix();
  glm::mat4 proj = glm::perspective(
      glm::radians(cam->fov_),
      static_cast<float>(cam->screen_res_.x) / cam->screen_res_.y, 0.001f,
      32768.0f);

  // Now that we have our modelview and projection matrix, if we combine these
  // 2 matrices, it will give us our clipping planes.  To combine 2 matrices,
  // we multiply them.

  clip[0] = modl[0].x * proj[0].x + modl[0].y * proj[1].x +
            modl[0].z * proj[2].x + modl[0].w * proj[3].x;
  clip[1] = modl[0].x * proj[0].y + modl[0].y * proj[1].y +
            modl[0].z * proj[2].y + modl[0].w * proj[3].y;
  clip[2] = modl[0].x * proj[0].z + modl[0].y * proj[1].z +
            modl[0].z * proj[2].z + modl[0].w * proj[3].z;
  clip[3] = modl[0].x * proj[0].w + modl[0].y * proj[1].w +
            modl[0].z * proj[2].w + modl[0].w * proj[3].w;

  clip[4] = modl[1].x * proj[0].x + modl[1].y * proj[1].x +
            modl[1].z * proj[2].x + modl[1].w * proj[3].x;
  clip[5] = modl[1].x * proj[0].y + modl[1].y * proj[1].y +
            modl[1].z * proj[2].y + modl[1].w * proj[3].y;
  clip[6] = modl[1].x * proj[0].z + modl[1].y * proj[1].z +
            modl[1].z * proj[2].z + modl[1].w * proj[3].z;
  clip[7] = modl[1].x * proj[0].w + modl[1].y * proj[1].w +
            modl[1].z * proj[2].w + modl[1].w * proj[3].w;

  clip[8] = modl[2].x * proj[0].x + modl[2].y * proj[1].x +
            modl[2].z * proj[2].x + modl[2].w * proj[3].x;
  clip[9] = modl[2].x * proj[0].y + modl[2].y * proj[1].y +
            modl[2].z * proj[2].y + modl[2].w * proj[3].y;
  clip[10] = modl[2].x * proj[0].z + modl[2].y * proj[1].z +
             modl[2].z * proj[2].z + modl[2].w * proj[3].z;
  clip[11] = modl[2].x * proj[0].w + modl[2].y * proj[1].w +
             modl[2].z * proj[2].w + modl[2].w * proj[3].w;

  clip[12] = modl[3].x * proj[0].x + modl[3].y * proj[1].x +
             modl[3].z * proj[2].x + modl[3].w * proj[3].x;
  clip[13] = modl[3].x * proj[0].y + modl[3].y * proj[1].y +
             modl[3].z * proj[2].y + modl[3].w * proj[3].y;
  clip[14] = modl[3].x * proj[0].z + modl[3].y * proj[1].z +
             modl[3].z * proj[2].z + modl[3].w * proj[3].z;
  clip[15] = modl[3].x * proj[0].w + modl[3].y * proj[1].w +
             modl[3].z * proj[2].w + modl[3].w * proj[3].w;

  // Now we actually want to get the sides of the frustum.  To do this we take
  // the clipping planes we received above and extract the sides from them.

  // This will extract the RIGHT side of the frustum
  frustum_[RIGHT_][A] = clip[3] - clip[0];
  frustum_[RIGHT_][B] = clip[7] - clip[4];
  frustum_[RIGHT_][C] = clip[11] - clip[8];
  frustum_[RIGHT_][D] = clip[15] - clip[12];

  // Now that we have a normal (A,B,C) and a distance (D) to the plane,
  // we want to normalize that normal and distance.

  // Normalize the RIGHT side
  NormalizePlane(frustum_, RIGHT_);

  // This will extract the LEFT side of the frustum
  frustum_[LEFT_][A] = clip[3] + clip[0];
  frustum_[LEFT_][B] = clip[7] + clip[4];
  frustum_[LEFT_][C] = clip[11] + clip[8];
  frustum_[LEFT_][D] = clip[15] + clip[12];

  // Normalize the LEFT side
  NormalizePlane(frustum_, LEFT_);

  // This will extract the BOTTOM side of the frustum
  frustum_[BOTTOM_][A] = clip[3] + clip[1];
  frustum_[BOTTOM_][B] = clip[7] + clip[5];
  frustum_[BOTTOM_][C] = clip[11] + clip[9];
  frustum_[BOTTOM_][D] = clip[15] + clip[13];

  // Normalize the BOTTOM side
  NormalizePlane(frustum_, BOTTOM_);

  // This will extract the TOP side of the frustum
  frustum_[TOP_][A] = clip[3] - clip[1];
  frustum_[TOP_][B] = clip[7] - clip[5];
  frustum_[TOP_][C] = clip[11] - clip[9];
  frustum_[TOP_][D] = clip[15] - clip[13];

  // Normalize the TOP side
  NormalizePlane(frustum_, TOP_);

  // This will extract the BACK side of the frustum
  frustum_[BACK_][A] = clip[3] - clip[2];
  frustum_[BACK_][B] = clip[7] - clip[6];
  frustum_[BACK_][C] = clip[11] - clip[10];
  frustum_[BACK_][D] = clip[15] - clip[14];

  // Normalize the BACK side
  NormalizePlane(frustum_, BACK_);

  // This will extract the FRONT side of the frustum
  frustum_[FRONT_][A] = clip[3] + clip[2];
  frustum_[FRONT_][B] = clip[7] + clip[6];
  frustum_[FRONT_][C] = clip[11] + clip[10];
  frustum_[FRONT_][D] = clip[15] + clip[14];

  // Normalize the FRONT side
  NormalizePlane(frustum_, FRONT_);
}

// The code below will allow us to make checks within the frustum.  For example,
// if we want to see if a point, a sphere, or a cube lies inside of the frustum.
// Because all of our planes point INWARDS (The normals are all pointing inside
// the frustum) we then can assume that if a point is in FRONT of all of the
// planes, it's inside.

///////////////////////////////// POINT IN FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This determines if a point is inside of the frustum
/////
///////////////////////////////// POINT IN FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::PointInFrustum(float x, float y, float z) const {
  // If you remember the plane equation (A*x + B*y + C*z + D = 0), then the
  // rest of this code should be quite obvious and easy to figure out
  // yourself. In case don't know the plane equation, it might be a good idea
  // to look at our Plane Collision tutorial at www.GameTutorials.com in
  // OpenGL Tutorials. I will briefly go over it here.  (A,B,C) is the (X,Y,Z)
  // of the normal to the plane. They are the same thing... but just called
  // ABC because you don't want to say: (x*x + y*y + z*z + d = 0).  That would
  // be wrong, so they substitute them. the (x, y, z) in the equation is the
  // point that you are testing.  The D is The distance the plane is from the
  // origin.  The equation ends with "= 0" because that is true when the point
  // (x, y, z) is ON the plane.  When the point is NOT on the plane, it is
  // either a negative number (the point is behind the plane) or a positive
  // number (the point is in front of the plane).  We want to check if the
  // point is in front of the plane, so all we have to do is go through each
  // point and make sure the plane equation goes out to a positive number on
  // each side of the frustum. The result (be it positive or negative) is the
  // distance the point is front the plane.

  // Go through all the sides of the frustum
  for (int i = 0; i < 6; i++) {
    // Calculate the plane equation and check if the point is behind a side
    // of the frustum
    if (frustum_[i][A] * x + frustum_[i][B] * y + frustum_[i][C] * z +
            frustum_[i][D] <=
        0) {
      // The point was behind a side, so it ISN'T in the frustum
      return false;
    }
  }

  // The point was inside of the frustum (In front of ALL the sides of the
  // frustum)
  return true;
}

///////////////////////////////// SPHERE IN FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This determines if a sphere is inside of our frustum by it's center and
/// radius.
/////
///////////////////////////////// SPHERE IN FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::SphereInFrustum(float x, float y, float z, float radius) const {
  // Now this function is almost identical to the PointInFrustum(), except we
  // now have to deal with a radius around the point.  The point is the center
  // of the radius.  So, the point might be outside of the frustum, but it
  // doesn't mean that the rest of the sphere is.  It could be half and half.
  // So instead of checking if it's less than 0, we need to add on the radius
  // to that.  Say the equation produced -2, which means the center of the
  // sphere is the distance of 2 behind the plane.  Well, what if the radius
  // was 5?  The sphere is still inside, so we would say, if(-2 < -5) then we
  // are outside.  In that case it's false, so we are inside of the frustum,
  // but a distance of 3.  This is reflected below.

  // Go through all the sides of the frustum
  for (int i = 0; i < 6; i++) {
    // If the center of the sphere is farther away from the plane than the
    // radius
    if (frustum_[i][A] * x + frustum_[i][B] * y + frustum_[i][C] * z +
            frustum_[i][D] <=
        -radius) {
      // The distance was greater than the radius so the sphere is outside
      // of the frustum
      return false;
    }
  }

  // The sphere was inside of the frustum!
  return true;
}

///////////////////////////////// CUBE IN FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This determines if a cube is in or around our frustum by it's center
/// and 1/2 it's length
/////
///////////////////////////////// CUBE IN FRUSTUM
///\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::CubeInFrustum(float x, float y, float z, float size) const {
  // This test is a bit more work, but not too much more complicated.
  // Basically, what is going on is, that we are given the center of the cube,
  // and half the length.  Think of it like a radius.  Then we checking each
  // point in the cube and seeing if it is inside the frustum.  If a point is
  // found in front of a side, then we skip to the next side.  If we get to a
  // plane that does NOT have a point in front of it, then it will return
  // false.

  // *Note* - This will sometimes say that a cube is inside the frustum when
  // it isn't. This happens when all the corners of the bounding box are not
  // behind any one plane. This is rare and shouldn't effect the overall
  // rendering speed.

  for (int i = 0; i < 6; i++) {
    if (frustum_[i][A] * (x - size) + frustum_[i][B] * (y - size) +
            frustum_[i][C] * (z - size) + frustum_[i][D] >
        0)
      continue;
    if (frustum_[i][A] * (x + size) + frustum_[i][B] * (y - size) +
            frustum_[i][C] * (z - size) + frustum_[i][D] >
        0)
      continue;
    if (frustum_[i][A] * (x - size) + frustum_[i][B] * (y + size) +
            frustum_[i][C] * (z - size) + frustum_[i][D] >
        0)
      continue;
    if (frustum_[i][A] * (x + size) + frustum_[i][B] * (y + size) +
            frustum_[i][C] * (z - size) + frustum_[i][D] >
        0)
      continue;
    if (frustum_[i][A] * (x - size) + frustum_[i][B] * (y - size) +
            frustum_[i][C] * (z + size) + frustum_[i][D] >
        0)
      continue;
    if (frustum_[i][A] * (x + size) + frustum_[i][B] * (y - size) +
            frustum_[i][C] * (z + size) + frustum_[i][D] >
        0)
      continue;
    if (frustum_[i][A] * (x - size) + frustum_[i][B] * (y + size) +
            frustum_[i][C] * (z + size) + frustum_[i][D] >
        0)
      continue;
    if (frustum_[i][A] * (x + size) + frustum_[i][B] * (y + size) +
            frustum_[i][C] * (z + size) + frustum_[i][D] >
        0)
      continue;

    // If we get here, it isn't in the frustum
    return false;
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// WOZZERS!  That seemed like an incredible amount to look at, but if you break
// it down, it's not.  Frustum culling is a VERY useful thing when it comes to
// 3D. If you want a large world, there is no way you are going to send it down
// the 3D pipeline every frame and let OpenGL take care of it for you.  That
// would give you a 0.001 frame rate.  If you hit '+' and bring the sphere count
// up to 1000, then take off culling, you will see the HUGE difference it makes.
// Also, you wouldn't really be rendering 1000 spheres.  You would most likely
// use the sphere code for larger objects.  Let me explain.  Say you have a
// bunch of objects, well... all you need to do is give the objects a radius,
// and then test that radius against the frustum.  If that sphere is in the
// frustum, then you render that object.  Also, you won't be rendering a high
// poly sphere so it won't be so slow.  This goes for bounding box's too
// (Cubes).  If you don't want to do a cube, it is really easy to convert the
// code for rectangles.  Just pass in a width and height, instead of just a
// length.  Remember, it's HALF the length of the cube, not the full length.  So
// it would be half the width and height for a rect.
//
// This is a perfect starter for an octree tutorial.  Wrap you head around the
// concepts here and then see if you can apply this to making an octree.
// Hopefully we will have a tutorial up and running for this subject soon.  Once
// you have frustum culling, the next step is getting space partitioning. Either
// it being a BSP tree of an Octree.
//
// Let's go over a brief overview of the things we learned here:
//
// 1) First we need to abstract the frustum from OpenGL.  To do that we need the
//    projection and modelview matrix.  To get the projection matrix we use:
//
//            glGetFloatv( GL_PROJECTION_MATRIX, /* An Array of 16 floats */ );
//    Then, to get the modelview matrix we use:
//
//            glGetFloatv( GL_MODELVIEW_MATRIX, /* An Array of 16 floats */ );
//
//      These 2 functions gives us an array of 16 floats (The matrix).
//
// 2) Next, we need to combine these 2 matrices.  We do that by matrix
// multiplication.
//
// 3) Now that we have the 2 matrixes combined, we can abstract the sides of the
// frustum.
//    This will give us the normal and the distance from the plane to the origin
//    (ABC and D).
//
// 4) After abstracting a side, we want to normalize the plane data.  (A B C and
// D).
//
// 5) Now we have our frustum, and we can check points against it using the
// plane equation.
//    Once again, the plane equation (A*x + B*y + C*z + D = 0) says that if,
//    point (X,Y,Z) times the normal of the plane (A,B,C), plus the distance of
//    the plane from origin, will equal 0 if the point (X, Y, Z) lies on that
//    plane.  If it is behind the plane it will be a negative distance, if it's
//    in front of the plane (the way the normal is facing) it will be a positive
//    number.
//
//
// If you need more help on the plane equation and why this works, download our
// Ray Plane Intersection Tutorial at www.GameTutorials.com.
//
// That's pretty much it with frustums.  There is a lot more we could talk
// about, but I don't want to complicate this tutorial more than I already have.
//
// I want to thank Mark Morley for his tutorial on frustum culling.  Most of
// everything I got here comes from his teaching.  If you want more in-depth,
// visit his tutorial at:
//
// http://www.markmorley.com/opengl/frustumculling.html
//
// Good luck!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
//
//
