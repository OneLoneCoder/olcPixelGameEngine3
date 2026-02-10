#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "vector2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_MATRIX3D_DECLARED)
namespace olc
{

	/*
		A complete 3x3 Matrix structure, with a variety
		of useful utility functions and operator overloads
		specifically targeting 2D graphical transformations

		as per https://en.wikipedia.org/wiki/Affine_transformation

		Access: column, row
	*/
	template<class T>
	struct m_3d
	{
		static_assert(std::is_arithmetic<T>::value, "olc::m_2d<type> must be numeric");

		// The 3x3 elements!
		std::array<T, 9> m{ {0 } };

		// Constructor created identity matrix
		inline constexpr m_3d()
		{
			identity();
		}

		// Copy constructor
		inline constexpr m_3d(const m_3d& mat) = default;

		// Assignment operator
		inline constexpr m_3d& operator=(const m_3d& mat) = default;

		// Retrieve a specific element's 1D index
		inline constexpr size_t idx(const size_t c, const size_t r) const
		{
			return r * 3 + c;
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
			me(0, 0) = 1;
			me(1, 1) = 1;
			me(2, 2) = 1;			
		}

		inline constexpr std::array<T, 16> m4x4()
		{
			auto& me = (*this);
			return {
				me(0,0), me(1,0), me(2,0), 0,
				me(0,1), me(1,1), me(2,1), 0,
				me(0,2), me(1,2), me(2,2), 0,
					  0,       0,       0, 1
			};
		}

		// Create translation matrix via components
		template<typename Q>
		inline constexpr void translate(const Q x, const Q y)
		{
			identity();
			auto& me = (*this);
			me(2, 0) = T(x);
			me(2, 1) = T(y);
		}

		// Create translation matrix via vector
		template<typename Q>
		inline constexpr void translate(const olc::v_2d<Q>& v)
		{
			translate(v.x, v.y);
		}

		// Create scaling matrix via components
		template<typename Q>
		inline constexpr void scale(const Q x, const Q y)
		{
			identity();
			auto& me = (*this);
			me(0, 0) = T(x);
			me(1, 1) = T(y);
		}

		// Create scaling matrix via vector
		template<typename Q>
		inline constexpr void scale(const olc::v_2d<Q>& v)
		{
			scale(v.x, v.y);
		}

		// Create rotation matrix with radians
		template<typename Q>
		inline constexpr void rotate(const Q rads)
		{
			identity();
			auto& me = (*this);
			me(0, 0) = std::cos(rads);
			me(0, 1) = std::sin(rads);
			me(1, 0) = -me(0, 1);
			me(1, 1) = me(0, 0);
		}

		// Create shearing matrix via components
		template<typename Q>
		inline constexpr void shear(const Q x, const Q y)
		{
			identity();
			auto& me = (*this);
			me(0, 1) = T(y);
			me(1, 0) = T(x);
		}

		// Create shearing matrix via vector
		template<typename Q>
		inline constexpr void shear(const olc::v_2d<Q>& v)
		{
			shear(v.x, v.y);
		}


		// Return inverted matrix
		inline constexpr auto invert() const
		{
			// https://stackoverflow.com/a/18504573
			olc::m_3d<T> out;
			auto& me = (*this);

			T det = me(0, 0) * (me(1, 1) * me(2, 2) - me(2, 1) * me(1, 2)) -
				me(0, 1) * (me(1, 0) * me(2, 2) - me(1, 2) * me(2, 0)) +
				me(0, 2) * (me(1, 0) * me(2, 1) - me(1, 1) * me(2, 0));

			T invdet = T(1) / det;

			out(0, 0) = (me(1, 1) * me(2, 2) - me(2, 1) * me(1, 2)) * invdet;
			out(0, 1) = (me(0, 2) * me(2, 1) - me(0, 1) * me(2, 2)) * invdet;
			out(0, 2) = (me(0, 1) * me(1, 2) - me(0, 2) * me(1, 1)) * invdet;
			out(1, 0) = (me(1, 2) * me(2, 0) - me(1, 0) * me(2, 2)) * invdet;
			out(1, 1) = (me(0, 0) * me(2, 2) - me(0, 2) * me(2, 0)) * invdet;
			out(1, 2) = (me(1, 0) * me(0, 2) - me(0, 0) * me(1, 2)) * invdet;
			out(2, 0) = (me(1, 0) * me(2, 1) - me(2, 0) * me(1, 1)) * invdet;
			out(2, 1) = (me(2, 0) * me(0, 1) - me(0, 0) * me(2, 1)) * invdet;
			out(2, 2) = (me(0, 0) * me(1, 1) - me(1, 0) * me(0, 1)) * invdet;
			return out;
		}

		// Transform a vector by this matrix
		template<typename Q>
		inline constexpr auto operator * (const olc::v_2d<Q>& v) const
		{
			auto& me = *this;
			olc::v_2d<Q> vOut;
			vOut.x = Q(me(0, 0) * v.x + me(1, 0) * v.y + me(2, 0) * Q(1));
			vOut.y = Q(me(0, 1) * v.x + me(1, 1) * v.y + me(2, 1) * Q(1));
			Q    z = Q(me(0, 2) * v.x + me(1, 2) * v.y + me(2, 2) * Q(1));
			return (vOut / z);
		}

		// Multiply this matrix with another
		template<typename Q>
		inline constexpr auto operator * (const olc::m_3d<Q>& rhs) const
		{
			auto& me = *this;
			olc::m_3d<T> out;
			for (size_t c = 0; c < 3; c++)
				for (size_t r = 0; r < 3; r++)
					out(r, c) = me(r, 0) * rhs(0, c) + me(r, 1) * rhs(1, c) + me(r, 2) * rhs(2, c);
			return out;
		}


		// Transform a vector of v_2d by this matrix
		template<typename Q>
		inline constexpr auto transform(const std::vector<olc::v_2d<Q>>& v)
		{
			std::vector<olc::v_2d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_2d<Q>& i) {return (*this) * i; });
			return o;
		}



		// Return this matrix as a std::string, of the form "[c1r1, c2r1, c3r1]\n[c1r2, c2r2, c3r2]\n[c1r3, c2r3, c3r3]"
		inline std::string str() const
		{
			const auto& me = *this;
			return std::string("[") + std::to_string(me(0, 0)) + "," + std::to_string(me(1, 0)) + "," + std::to_string(me(2, 0)) + "]\n"
				+ "[" + std::to_string(me(0, 1)) + "," + std::to_string(me(1, 1)) + "," + std::to_string(me(2, 1)) + "]\n"
				+ "[" + std::to_string(me(0, 2)) + "," + std::to_string(me(1, 2)) + "," + std::to_string(me(2, 2)) + "]\n";
		}
	};

	// Allow olc::m_3d to play nicely with std::cout
	template<class T>
	inline std::ostream& operator << (std::ostream& os, const m_3d<T>& rhs)
	{
		os << rhs.str();
		return os;
	}

	// Convenient types ready-to-go
	typedef m_3d<float> mf3d;
	typedef m_3d<double> md3d;
}
#define PGE_MATRIX3D_DECLARED 1
#endif
//! END DECLARATION