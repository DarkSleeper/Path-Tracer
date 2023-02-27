#pragma once

float det2x2(float a, float b,
	     float c, float d) {
  return a * d - b * c;
}

float det3x3(float a1,float a2,float a3,
	     float b1,float b2,float b3,
	     float c1,float c2,float c3) {
  return
      a1 * det2x2( b2, b3, c2, c3 )
    - b1 * det2x2( a2, a3, c2, c3 )
    + c1 * det2x2( a2, a3, b2, b3 );
}

float det4x4(float a1, float a2, float a3, float a4, 
	     float b1, float b2, float b3, float b4, 
	     float c1, float c2, float c3, float c4, 
	     float d1, float d2, float d3, float d4) {
  return 
      a1 * det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
    - b1 * det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
    + c1 * det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
    - d1 * det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
}
