#include "matrix.h"

#include "vector.h"

#define MATRIX_INVERSE_EPSILON		1e-14

#define DEG2RAD (M_PI/180.0)
#  define SQRTF(X)  (float) sqrt((float) (X))

/*
 * Transform a point (column vector) by a matrix:   Q = M * P
 */
#define TRANSFORM_POINT( Q, M, P )					\
   Q[0] = M[0] * P[0] + M[4] * P[1] + M[8] *  P[2] + M[12] * P[3];	\
   Q[1] = M[1] * P[0] + M[5] * P[1] + M[9] *  P[2] + M[13] * P[3];	\
   Q[2] = M[2] * P[0] + M[6] * P[1] + M[10] * P[2] + M[14] * P[3];	\
   Q[3] = M[3] * P[0] + M[7] * P[1] + M[11] * P[2] + M[15] * P[3];


#define TRANSFORM_POINT3( Q, M, P )				\
   Q[0] = M[0] * P[0] + M[4] * P[1] + M[8] *  P[2] + M[12];	\
   Q[1] = M[1] * P[0] + M[5] * P[1] + M[9] *  P[2] + M[13];	\
   Q[2] = M[2] * P[0] + M[6] * P[1] + M[10] * P[2] + M[14];	\
   Q[3] = M[3] * P[0] + M[7] * P[1] + M[11] * P[2] + M[15];


bool idMat3::Inverse(void)
{
	// 18+3+9 = 30 multiplications
	//			 1 division
	idMat3 inverse;
   idMat3 &mat = *this;
	double det, invDet;

	inverse(0)(0) = mat(1)(1) * mat(2)(2) - mat(1)(2) * mat(2)(1);
	inverse(1)(0) = mat(1)(2) * mat(2)(0) - mat(1)(0) * mat(2)(2);
	inverse(2)(0) = mat(1)(0) * mat(2)(1) - mat(1)(1) * mat(2)(0);

	det = mat(0)(0) * inverse(0)(0) + mat(0)(1) * inverse(1)(0) + mat(0)(2) * inverse(2)(0);

	if (fabs(det) < MATRIX_INVERSE_EPSILON) {
		return false;
	}

	invDet = 1.0f / det;

	inverse(0)(1) = mat(0)(2) * mat(2)(1) - mat(0)(1) * mat(2)(2);
	inverse(0)(2) = mat(0)(1) * mat(1)(2) - mat(0)(2) * mat(1)(1);
	inverse(1)(1) = mat(0)(0) * mat(2)(2) - mat(0)(2) * mat(2)(0);
	inverse(1)(2) = mat(0)(2) * mat(1)(0) - mat(0)(0) * mat(1)(2);
	inverse(2)(1) = mat(0)(1) * mat(2)(0) - mat(0)(0) * mat(2)(1);
	inverse(2)(2) = mat(0)(0) * mat(1)(1) - mat(0)(1) * mat(1)(0);

	mat(0)(0) = inverse(0)(0) * invDet;
	mat(0)(1) = inverse(0)(1) * invDet;
	mat(0)(2) = inverse(0)(2) * invDet;

	mat(1)(0) = inverse(1)(0) * invDet;
	mat(1)(1) = inverse(1)(1) * invDet;
	mat(1)(2) = inverse(1)(2) * invDet;

	mat(2)(0) = inverse(2)(0) * invDet;
	mat(2)(1) = inverse(2)(1) * invDet;
	mat(2)(2) = inverse(2)(2) * invDet;

	return true;
}

void idMat4::Translate(const idVec3 &v)
{
	float x = v[0];
	float y = v[1];
	float z = v[2];
   m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
   m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
   m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
   m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];
}

void idMat4::Rotate(float angle, const idVec3 &normal)
{
   float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
   idMat4 m;
   bool optimized;

   float x = normal[0];
   float y = normal[1];
   float z = normal[2];

   s = (float) sin( angle * DEG2RAD );
   c = (float) cos( angle * DEG2RAD );

   optimized = false;

#define M(row,col)  m.m[col*4+row]

   if (x == 0.0F) {
      if (y == 0.0F) {
         if (z != 0.0F) {
            optimized = true;
            /* rotate only around z-axis */
            M(0,0) = c;
            M(1,1) = c;
            if (z < 0.0F) {
               M(0,1) = s;
               M(1,0) = -s;
            }
            else {
               M(0,1) = -s;
               M(1,0) = s;
            }
         }
      }
      else if (z == 0.0F) {
         optimized = true;
         /* rotate only around y-axis */
         M(0,0) = c;
         M(2,2) = c;
         if (y < 0.0F) {
            M(0,2) = -s;
            M(2,0) = s;
         }
         else {
            M(0,2) = s;
            M(2,0) = -s;
         }
      }
   }
   else if (y == 0.0F) {
      if (z == 0.0F) {
         optimized = true;
         /* rotate only around x-axis */
         M(1,1) = c;
         M(2,2) = c;
         if (x < 0.0F) {
            M(1,2) = s;
            M(2,1) = -s;
         }
         else {
            M(1,2) = -s;
            M(2,1) = s;
         }
      }
   }

   if (!optimized) {
      const float mag = SQRTF(x * x + y * y + z * z);

      if (mag <= 1.0e-4) {
         /* no rotation, leave mat as-is */
         return;
      }

      x /= mag;
      y /= mag;
      z /= mag;


      /*
       *     Arbitrary axis rotation matrix.
       *
       *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
       *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
       *  (which is about the X-axis), and the two composite transforms
       *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
       *  from the arbitrary axis to the X-axis then back.  They are
       *  all elementary rotations.
       *
       *  Rz' is a rotation about the Z-axis, to bring the axis vector
       *  into the x-z plane.  Then Ry' is applied, rotating about the
       *  Y-axis to bring the axis vector parallel with the X-axis.  The
       *  rotation about the X-axis is then performed.  Ry and Rz are
       *  simply the respective inverse transforms to bring the arbitrary
       *  axis back to its original orientation.  The first transforms
       *  Rz' and Ry' are considered inverses, since the data from the
       *  arbitrary axis gives you info on how to get to it, not how
       *  to get away from it, and an inverse must be applied.
       *
       *  The basic calculation used is to recognize that the arbitrary
       *  axis vector (x, y, z), since it is of unit length, actually
       *  represents the sines and cosines of the angles to rotate the
       *  X-axis to the same orientation, with theta being the angle about
       *  Z and phi the angle about Y (in the order described above)
       *  as follows:
       *
       *  cos ( theta ) = x / sqrt ( 1 - z^2 )
       *  sin ( theta ) = y / sqrt ( 1 - z^2 )
       *
       *  cos ( phi ) = sqrt ( 1 - z^2 )
       *  sin ( phi ) = z
       *
       *  Note that cos ( phi ) can further be inserted to the above
       *  formulas:
       *
       *  cos ( theta ) = x / cos ( phi )
       *  sin ( theta ) = y / sin ( phi )
       *
       *  ...etc.  Because of those relations and the standard trigonometric
       *  relations, it is pssible to reduce the transforms down to what
       *  is used below.  It may be that any primary axis chosen will give the
       *  same results (modulo a sign convention) using thie method.
       *
       *  Particularly nice is to notice that all divisions that might
       *  have caused trouble when parallel to certain planes or
       *  axis go away with care paid to reducing the expressions.
       *  After checking, it does perform correctly under all cases, since
       *  in all the cases of division where the denominator would have
       *  been zero, the numerator would have been zero as well, giving
       *  the expected result.
       */

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * s;
      ys = y * s;
      zs = z * s;
      one_c = 1.0F - c;

      /* We already hold the identity-matrix so we can skip some statements */
      M(0,0) = (one_c * xx) + c;
      M(0,1) = (one_c * xy) - zs;
      M(0,2) = (one_c * zx) + ys;
/*    M(0,3) = 0.0F; */

      M(1,0) = (one_c * xy) + zs;
      M(1,1) = (one_c * yy) + c;
      M(1,2) = (one_c * yz) - xs;
/*    M(1,3) = 0.0F; */

      M(2,0) = (one_c * zx) - ys;
      M(2,1) = (one_c * yz) + xs;
      M(2,2) = (one_c * zz) + c;
/*    M(2,3) = 0.0F; */

/*
      M(3,0) = 0.0F;
      M(3,1) = 0.0F;
      M(3,2) = 0.0F;
      M(3,3) = 1.0F;
*/
   }
#undef M

   *this = *this * m;
}

idVec3 operator*(const idVec3 &v, const idMat4 &m)
{
   const float v0 = v[0], v1 = v[1], v2 = v[2], v3 = 1;
   float u[4];
#define M(row,col)  m.m[row + col*4]
   u[0] = v0 * M(0,0) + v1 * M(1,0) + v2 * M(2,0) + v3 * M(3,0);
   u[1] = v0 * M(0,1) + v1 * M(1,1) + v2 * M(2,1) + v3 * M(3,1);
   u[2] = v0 * M(0,2) + v1 * M(1,2) + v2 * M(2,2) + v3 * M(3,2);
   u[3] = v0 * M(0,3) + v1 * M(1,3) + v2 * M(2,3) + v3 * M(3,3);
#undef M
   return {
    u[0] / u[3],
    u[1] / u[3],
    u[2] / u[3]
   };
}

idVec3 operator*(const idMat4 &m, const idVec3 &v)
{
   const float *p = v;
   float u[4];
   TRANSFORM_POINT3(u, m.m, p);
   return {
    u[0] / u[3],
    u[1] / u[3],
    u[2] / u[3]
   };
}