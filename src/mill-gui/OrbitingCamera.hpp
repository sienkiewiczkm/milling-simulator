#ifndef __ORBITING_CAMERA_HPP__
#define __ORBITING_CAMERA_HPP__

#include <glm/mat4x4.hpp>

class OrbitingCamera {
public:
  OrbitingCamera();

  inline void setMinXRotation(float min) { _xMin = min; }
  inline void setMaxXRotation(float max) { _xMax = max; }

  inline float getDist() { return _dist; }
  inline void setDist(float dist) { _dist = dist; }

  void rotate(float dx, float dy);

  glm::mat4 getViewMatrix();
  glm::vec3 getPosition();

private:
  float _xMin, _xMax;
  float _dist;
  float _rotationX, _rotationY;
};

#endif
