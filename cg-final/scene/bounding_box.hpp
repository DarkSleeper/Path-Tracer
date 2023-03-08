#pragma once

#include <glm/glm.hpp>
#include <assert.h>

#define min2(a,b) (((a)<(b))?(a):(b))
#define max2(a,b) (((a)>(b))?(a):(b))
#define min3(a,b,c) min2(min2(a,b),c)
#define max3(a,b,c) max2(max2(a,b),c)

// ====================================================================
// ====================================================================

class Bounding_Box {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Bounding_Box(glm::vec3 _min, glm::vec3 _max) {
    set(_min,_max); 
  }

  // ACCESSORS
  void get(glm::vec3 &_min, glm::vec3 &_max) const {
    _min = min;
    _max = max; 
  }
  glm::vec3 get_min() const { return min; }
  glm::vec3 get_max() const { return max; }

  // MODIFIERS
  void set(Bounding_Box *bb) {
    assert(bb != NULL);
    min = bb->min;
    max = bb->max; 
  }
  void set(glm::vec3 _min, glm::vec3 _max) {
    assert (_min.x <= _max.x &&
	    _min.y <= _max.y &&
	    _min.z <= _max.z);
    min = _min;
    max = _max; 
  }
  void extend(const glm::vec3 v) {
    min = glm::vec3(min2(min.x,v.x),
		min2(min.y,v.y),
		min2(min.z,v.z));
    max = glm::vec3(max2(max.x,v.x),
		max2(max.y,v.y),
		max2(max.z,v.z)); 
  }
  void extend(Bounding_Box const& bb) {
    extend(bb.min);
    extend(bb.max); 
  }

  // DEBUGGING 
  void print() const {
    printf ("%f %f %f  -> %f %f %f\n", min.x,min.y,min.z,
	    max.x,max.y,max.z); 
  }

private:

  Bounding_Box() { assert(0); } // don't use this constructor

  // REPRESENTATION
  glm::vec3 min;
  glm::vec3 max;

};
