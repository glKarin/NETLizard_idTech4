#ifndef _KARIN_MATRIX_H
#define _KARIN_MATRIX_H

// column-major

#include <string.h>

class idVec3;

class idMat3
{

	public:
		class idVec3Ref
		{
			public:
			float operator()(int i) const {
				return operator[](i);
			}
			float & operator()(int i) {
				return operator[](i);
			}
			float operator[](int i) const {
				switch(i)
				{
					case 0: return *x;
					case 1: return *y;
					case 2: return *z;
					default: throw "idVec3Ref::operator[](int)const";
				}
			}
			float & operator[](int i) {
				switch(i)
				{
					case 0: return *x;
					case 1: return *y;
					case 2: return *z;
					default: throw "idVec3Ref::operator[](int)";
				}
			}
			operator idVec3() const;
			
			private:
				float *x;
				float *y;
				float *z;

				idVec3Ref(float *x, float *y, float *z)
				: x(x), y(y), z(z) {}

				friend class idMat3;
		};
	public:
		float m[9] = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1,
		};
		idMat3() = default;
		idMat3(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22);
		void Identity(void);
      bool Inverse(void);
		friend idMat3 operator*(const idMat3 &a, const idMat3 &b);
		operator const float *() const {
			return m;
		}
		operator float *() {
			return m;
		}
		float operator[](int i) const {
			return m[i];
		}
		float & operator[](int i) {
			return m[i];
		}
		float operator()(int col, int row) const {
			return m[row * 3 + col];
		}
		float & operator()(int col, int row) {
			return m[row * 3 + col];
		}
		const idVec3Ref & operator()(int col) const {
			return cols[col];
		}
		idVec3Ref & operator()(int col) {
			return cols[col];
		}

		private:
		idVec3Ref cols[3] = {
			{&m[0], &m[3], &m[6]},
			{&m[1], &m[4], &m[7]},
			{&m[2], &m[5], &m[8]},
		};
};

class idMat4
{
	public:
		float m[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};
		
		void Translate(const idVec3 &v);
		void Rotate(float angle, const idVec3 &normal);
		void Identity(void);
      void Transposed(void);
		friend idMat4 operator*(const idMat4 &a, const idMat4 &b);
		friend idVec3 operator*(const idVec3 &v, const idMat4 &m); // u = v * m
		friend idVec3 operator*(const idMat4 &m, const idVec3 &v); // Q = M * P
      void SetCol3(int col, const float v[]);
		operator idMat3() const;
		operator const float *() const {
			return m;
		}
		operator float *() {
			return m;
		}
		float operator[](int i) const {
			return m[i];
		}
		float & operator[](int i) {
			return m[i];
		}
		float operator()(int col, int row) const {
			return m[row * 4 + col];
		}
		float & operator()(int col, int row) {
			return m[row * 4 + col];
		}
};

inline idMat3::idMat3(float f00, float f01, float f02,
 float f10, float f11, float f12,
  float f20, float f21, float f22)
{
   m[0] = f00;
   m[3] = f01;
   m[6] = f02;
   m[1] = f10;
   m[4] = f11;
   m[7] = f12;
   m[2] = f20;
   m[5] = f21;
   m[8] = f22;
}

inline void idMat3::Identity(void)
{
	m[0] = m[4] = m[8] = 1.0;
	m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0.0;
}

inline idMat3 operator*(const idMat3 &a, const idMat3 &b)
{
	int i, j;
	const float *m1Ptr, *m2Ptr;
	float *dstPtr;
	idMat3 dst;

	m1Ptr = a.m;
	m2Ptr = b.m;
	dstPtr = dst.m;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
			          + m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
			          + m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
			dstPtr++;
		}

		m1Ptr += 3;
	}

	return dst;
}



inline void idMat4::Identity(void)
{
	m[0] = m[5] = m[10] = m[15] = 1.0;
	m[1] = m[2] = m[3]
    = m[4] = m[6] = m[7]
     = m[8] = m[9] = m[11]
      = m[12] = m[13] = m[14] = 0.0;
}

inline idMat4 operator*(const idMat4 &a, const idMat4 &b)
{
#define A(row,col)  a.m[(col<<2)+row]
#define B(row,col)  b.m[(col<<2)+row]
#define P(row,col)  product.m[(col<<2)+row]
	idMat4 product;
	for (int i = 0; i < 4; i++) {
		float ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
		P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}
	return product;
#undef A
#undef B
#undef P
}

inline void idMat4::SetCol3(int col, const float v[])
{
   m[col] = v[0];
   m[4 + col] = v[1];
   m[8 + col] = v[2];
}

inline void idMat4::Transposed(void)
{
   float to[16];
   float *from = m;

   to[0] = from[0];
   to[1] = from[4];
   to[2] = from[8];
   to[3] = from[12];
   to[4] = from[1];
   to[5] = from[5];
   to[6] = from[9];
   to[7] = from[13];
   to[8] = from[2];
   to[9] = from[6];
   to[10] = from[10];
   to[11] = from[14];
   to[12] = from[3];
   to[13] = from[7];
   to[14] = from[11];
   to[15] = from[15];

   memcpy(m, to, sizeof(to));
}

inline idMat4::operator idMat3() const
{
	return {
		m[0], m[1], m[2],
		m[4], m[5], m[6],
		m[8], m[9], m[10]
	};
}
#endif
