#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <algorithm>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "vector4d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_MATRIX4D_DECLARED)
namespace olc
{

	/*
		A complete 4x4 Matrix structure, with a variety
		of useful utility functions and operator overloads
		specifically targeting 3D graphical transformations

		as per https://en.wikipedia.org/wiki/Transformation_matrix

		Access: column, row
	*/

	/*
	 
	
	Because Matrices can be defined all sort sof ways, I have included this little
	description to clarify how this particular implementation works. For the end
	user's ease of use, the transformations are designed to mimic those found on
	Wikipedias page on transformation matrices, which are in column-major order. 
	
	Memory layout of the 4x4 matrix is as follows idx = R * 4 + C:

		  0x00  0x01  0x02  0x3
	0x00  | 0,0 | 1,0 | 2,0 | 3,0 |
	0x04  | 0,1 | 1,1 | 2,1 | 3,1 |
	0x08  | 0,2 | 1,2 | 2,2 | 3,2 |
	0x0C  | 0,3 | 1,3 | 2,3 | 3,3 |

	This is row-major order (in storage) but we really only access this
	via the idx operator (col, row) so it is effectively column-major order 
	for the user. 

	This is because in graphics we typically want to multiply a vector on 
	the right of the matrix, and we want the translation components to be in 
	the last column.

	Matrix * Vector multiplication is as follows:

	| m11 m12 m13 m14 |   | v1 |   | r1 | (m11*v1 + m12*v2 + m13*v3 + m14*v4)
	| m21 m22 m23 m24 | * | v2 | = | r2 | (m21*v1 + m22*v2 + m23*v3 + m24*v4)
	| m31 m32 m33 m34 |   | v3 |   | r3 | (m31*v1 + m32*v2 + m33*v3 + m34*v4)
	| m41 m42 m43 m44 |   | v4 |   | r4 | (m41*v1 + m42*v2 + m43*v3 + m44*v4)

	Matrix * Matrix multiplication is as follows:

	| a11 a12 a13 a14 |   | b11 b12 b13 b14 |   | r11 r12 r13 r14 | (a11*b11 + a12*b21 + a13*b31 + a14*b41) ...
	| a21 a22 a23 a24 | * | b21 b22 b23 b24 | = | r21 r22 r23 r24 | (a21*b11 + a22*b21 + a23*b31 + a24*b41) ...
	| a31 a32 a33 a34 |   | b31 b32 b33 b34 |   | r31 r32 r33 r34 | (a31*b11 + a32*b21 + a33*b31 + a34*b41) ...
	| a41 a42 a43 a44 |   | b41 b42 b43 b44 |   | r41 r42 r43 r44 | (a41*b11 + a42*b21 + a43*b31 + a44*b41) ...

	Example Translation:

	| 1 0 0 Tx |   | x |   | x' | (1*x + 0*y + 0*z + Tx*1) (x + tx)
	| 0 1 0 Ty | * | y | = | y' | (0*x + 1*y + 0*z + Ty*1) (y + ty)
	| 0 0 1 Tz |   | z |   | z' | (0*x + 0*y + 1*z + Tz*1) (z + tz)
	| 0 0 0 1  |   | 1 |   | x' | (0*x + 0*y + 0*z +  1*1) (1)

	Example Rotation around Y axis:
	| cθ  0 sθ 0 |   | x |   | x' | (cosθ*x + 0*y + sinθ*z + 0*1)  (x*cosθ + z*sinθ)
	| 0   1 0  0 | * | y | = | y' | (0*x + 1*y + 0*z + 0*1)        (y)
	| -sθ 0 cθ 0 |   | z |   | z' | (-sinθ*x + 0*y + cosθ*z + 0*1) (z*cosθ - x*sinθ)
	| 0   0 0  1 |   | 1 |   | x' | (0*x + 0*y + 0*z + 1*1)        (1)

	P' = Projection * View * World * P

	*/



	template<class T>
	struct m_4d
	{
		static_assert(std::is_arithmetic<T>::value, "olc::m_4d<type> must be numeric");

		// The 4x4 elements!
		std::array<T, 16> m{ {0 } };

		// Constructor created identity matrix
		inline constexpr m_4d()
		{
			identity();
		}

		// Copy constructor
		inline constexpr m_4d(const m_4d& mat) = default;

		// Assignment operator
		inline constexpr m_4d& operator=(const m_4d& mat) = default;

		// Retrieve a specific element's 1D index
		inline constexpr size_t idx(const size_t c, const size_t r) const
		{
			return r * 4 + c; // Column-major order (for user) but row-major order in storage
		}

		// Retrieve non-const access to specific element
		inline constexpr T& operator()(const size_t col, const size_t row)
		{
			return m[idx(col, row)];
		}

		// Retrieve const access to specific element
		inline constexpr const T& operator()(const size_t col, const size_t row) const
		{
			return m[idx(col, row)];
		}

		// Set all elements to 0
		inline constexpr void clear()
		{
			std::fill(m.begin(), m.end(), T(0));
		}

		// Create identity matrix
		inline constexpr void identity()
		{
			clear();
			auto& me = (*this);
			me(0, 0) = T(1);
			me(1, 1) = T(1);
			me(2, 2) = T(1);
			me(3, 3) = T(1);
		}

		inline constexpr auto transpose() const
		{
			olc::m_4d<T> out;
			auto& me = (*this);
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					out(i, j) = me(j, i);
			return out;
		}

		// Create translation matrix via components
		template<typename Q>
		inline constexpr void translate(const Q x, const Q y, const Q z)
		{
			identity();
			auto& me = (*this);
			me(3, 0) = T(x);
			me(3, 1) = T(y);
			me(3, 2) = T(z);
		}

		// Create translation matrix via vector (x, y, z components)
		template<typename Q>
		inline constexpr void translate(const olc::v_4d<Q>& v)
		{
			translate(v.x, v.y, v.z);
		}

		// Create scaling matrix via components
		template<typename Q>
		inline constexpr void scale(const Q x, const Q y, const Q z)
		{
			identity();
			auto& me = (*this);
			me(0, 0) = T(x);
			me(1, 1) = T(y);
			me(2, 2) = T(z);
		}

		// Create scaling matrix via vector (x, y, z components)
		template<typename Q>
		inline constexpr void scale(const olc::v_4d<Q>& v)
		{
			scale(v.x, v.y, v.z);
		}

		// Create rotation matrix around X axis with radians
		template<typename Q>
		inline constexpr void rotateX(const Q rads)
		{
			identity();
			auto& me = (*this);
			me(1, 1) = std::cos(T(rads));
			me(1, 2) = std::sin(T(rads));
			me(2, 1) = -me(1, 2);
			me(2, 2) = me(1, 1);
		}

		// Create rotation matrix around Y axis with radians
		template<typename Q>
		inline constexpr void rotateY(const Q rads)
		{
			identity();
			auto& me = (*this);
			me(0, 0) = std::cos(T(rads));
			me(0, 2) = -std::sin(T(rads));
			me(2, 0) = -me(0, 2);
			me(2, 2) = me(0, 0);
		}

		// Create rotation matrix around Z axis with radians
		template<typename Q>
		inline constexpr void rotateZ(const Q rads)
		{
			identity();
			auto& me = (*this);
			me(0, 0) = std::cos(T(rads));
			me(0, 1) = std::sin(T(rads));
			me(1, 0) = -me(0, 1);
			me(1, 1) = me(0, 0);
		}

		// Create perspective projection matrix
		template<typename Q>
		inline constexpr void perspective(const Q fov, const Q ratio, const Q nearplane, const Q farplane)
		{
			identity();
			auto& me = (*this);
			T invFOV = T(1) / tan(fov * T(0.5));

			me(0,0) = invFOV / ratio;  // X scale
			me(1,1) = invFOV;         // Y scale
			me(2,2) = (farplane + nearplane) / (nearplane - farplane);      // Z mapping
			me(3,2) = (2.0f * farplane * nearplane) / (nearplane - farplane); // Z offset
			me(2,3) = -1.0f;           // Perspective divide by -Z
			me(3,3) = 0.0f;
		}

		// Create orthographic projection matrix
		template<typename Q>
		inline constexpr void orthographic(const Q left, const Q right, const Q bottom, const Q top, const Q near1, const Q far1)
		{
			identity();
			auto& me = (*this);
			me(0, 0) = T(2) / (right - left);
			me(1, 1) = T(2) / (top - bottom);
			me(2, 2) = T(-2) / (far1 - near1);
			me(3, 0) = -(right + left) / (right - left);
			me(3, 1) = -(top + bottom) / (top - bottom);
			me(3, 2) = -(far1 + near1) / (far1 - near1);
		}

		// Return inverted matrix
		inline constexpr auto invert() const
		{
			// Using Gauss-Jordan elimination - AI special this :P
			olc::m_4d<T> out;
			auto& me = (*this);

			T A2323 = me(2, 2) * me(3, 3) - me(2, 3) * me(3, 2);
			T A1323 = me(2, 1) * me(3, 3) - me(2, 3) * me(3, 1);
			T A1223 = me(2, 1) * me(3, 2) - me(2, 2) * me(3, 1);
			T A0323 = me(2, 0) * me(3, 3) - me(2, 3) * me(3, 0);
			T A0223 = me(2, 0) * me(3, 2) - me(2, 2) * me(3, 0);
			T A0123 = me(2, 0) * me(3, 1) - me(2, 1) * me(3, 0);
			T A2313 = me(1, 2) * me(3, 3) - me(1, 3) * me(3, 2);
			T A1313 = me(1, 1) * me(3, 3) - me(1, 3) * me(3, 1);
			T A1213 = me(1, 1) * me(3, 2) - me(1, 2) * me(3, 1);
			T A2312 = me(1, 2) * me(2, 3) - me(1, 3) * me(2, 2);
			T A1312 = me(1, 1) * me(2, 3) - me(1, 3) * me(2, 1);
			T A1212 = me(1, 1) * me(2, 2) - me(1, 2) * me(2, 1);
			T A0313 = me(1, 0) * me(3, 3) - me(1, 3) * me(3, 0);
			T A0213 = me(1, 0) * me(3, 2) - me(1, 2) * me(3, 0);
			T A0312 = me(1, 0) * me(2, 3) - me(1, 3) * me(2, 0);
			T A0212 = me(1, 0) * me(2, 2) - me(1, 2) * me(2, 0);
			T A0113 = me(1, 0) * me(3, 1) - me(1, 1) * me(3, 0);
			T A0112 = me(1, 0) * me(2, 1) - me(1, 1) * me(2, 0);

			T det = me(0, 0) * (me(1, 1) * A2323 - me(1, 2) * A1323 + me(1, 3) * A1223)
				- me(0, 1) * (me(1, 0) * A2323 - me(1, 2) * A0323 + me(1, 3) * A0223)
				+ me(0, 2) * (me(1, 0) * A1323 - me(1, 1) * A0323 + me(1, 3) * A0123)
				- me(0, 3) * (me(1, 0) * A1223 - me(1, 1) * A0223 + me(1, 2) * A0123);

			T invdet = T(1) / det;

			out(0, 0) = invdet * (me(1, 1) * A2323 - me(1, 2) * A1323 + me(1, 3) * A1223);
			out(0, 1) = invdet * -(me(0, 1) * A2323 - me(0, 2) * A1323 + me(0, 3) * A1223);
			out(0, 2) = invdet * (me(0, 1) * A2313 - me(0, 2) * A1313 + me(0, 3) * A1213);
			out(0, 3) = invdet * -(me(0, 1) * A2312 - me(0, 2) * A1312 + me(0, 3) * A1212);
			out(1, 0) = invdet * -(me(1, 0) * A2323 - me(1, 2) * A0323 + me(1, 3) * A0223);
			out(1, 1) = invdet * (me(0, 0) * A2323 - me(0, 2) * A0323 + me(0, 3) * A0223);
			out(1, 2) = invdet * -(me(0, 0) * A2313 - me(0, 2) * A0313 + me(0, 3) * A0213);
			out(1, 3) = invdet * (me(0, 0) * A2312 - me(0, 2) * A0312 + me(0, 3) * A0212);
			out(2, 0) = invdet * (me(1, 0) * A1323 - me(1, 1) * A0323 + me(1, 3) * A0123);
			out(2, 1) = invdet * -(me(0, 0) * A1323 - me(0, 1) * A0323 + me(0, 3) * A0123);
			out(2, 2) = invdet * (me(0, 0) * A1313 - me(0, 1) * A0313 + me(0, 3) * A0113);
			out(2, 3) = invdet * -(me(0, 0) * A1312 - me(0, 1) * A0312 + me(0, 3) * A0112);
			out(3, 0) = invdet * -(me(1, 0) * A1223 - me(1, 1) * A0223 + me(1, 2) * A0123);
			out(3, 1) = invdet * (me(0, 0) * A1223 - me(0, 1) * A0223 + me(0, 2) * A0123);
			out(3, 2) = invdet * -(me(0, 0) * A1213 - me(0, 1) * A0213 + me(0, 2) * A0113);
			out(3, 3) = invdet * (me(0, 0) * A1212 - me(0, 1) * A0212 + me(0, 2) * A0112);

			return out;
		}

		// Transform a vector by this matrix
		template<typename Q>
		inline constexpr auto operator * (const olc::v_4d<Q>& v) const
		{
			auto& me = *this;
			olc::v_4d<Q> vOut;
			vOut.x = Q(me(0, 0) * v.x + me(1, 0) * v.y + me(2, 0) * v.z + me(3, 0) * v.w);
			vOut.y = Q(me(0, 1) * v.x + me(1, 1) * v.y + me(2, 1) * v.z + me(3, 1) * v.w);
			vOut.z = Q(me(0, 2) * v.x + me(1, 2) * v.y + me(2, 2) * v.z + me(3, 2) * v.w);
			vOut.w = Q(me(0, 3) * v.x + me(1, 3) * v.y + me(2, 3) * v.z + me(3, 3) * v.w);
			return vOut;
		}

		// Multiply this matrix with another
		template<typename Q>
		inline constexpr auto operator * (const olc::m_4d<Q>& rhs) const
		{
			auto& me = *this;
			olc::m_4d<T> out;
			for (size_t c = 0; c < 4; c++)
				for (size_t r = 0; r < 4; r++)
					out(c, r) = me(0, r) * rhs(c, 0) + me(1, r) * rhs(c, 1) + me(2, r) * rhs(c, 2) + me(3, r) * rhs(c, 3);
			return out;
		}

		// Transform a vector of v_4d by this matrix
		template<typename Q>
		inline constexpr auto transform(const std::vector<olc::v_4d<Q>>& v)
		{
			std::vector<olc::v_4d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_4d<Q>& i) {return (*this) * i; });
			return o;
		}

		// Return this matrix as a std::string
		inline std::string str() const
		{
			const auto& me = *this;
			return std::string("[") + std::to_string(me(0, 0)) + "," + std::to_string(me(1, 0)) + "," + std::to_string(me(2, 0)) + "," + std::to_string(me(3, 0)) + "]\n"
				+ "[" + std::to_string(me(0, 1)) + "," + std::to_string(me(1, 1)) + "," + std::to_string(me(2, 1)) + "," + std::to_string(me(3, 1)) + "]\n"
				+ "[" + std::to_string(me(0, 2)) + "," + std::to_string(me(1, 2)) + "," + std::to_string(me(2, 2)) + "," + std::to_string(me(3, 2)) + "]\n"
				+ "[" + std::to_string(me(0, 3)) + "," + std::to_string(me(1, 3)) + "," + std::to_string(me(2, 3)) + "," + std::to_string(me(3, 3)) + "]\n";
		}
	};

	// Allow olc::m_4d to play nicely with std::cout
	template<class T>
	inline std::ostream& operator << (std::ostream& os, const m_4d<T>& rhs)
	{
		os << rhs.str();
		return os;
	}

	// Convenient types ready-to-go
	typedef m_4d<float> mf4d;
	typedef m_4d<double> md4d;
}
#define PGE_MATRIX4D_DECLARED 1
#endif
//! END DECLARATION