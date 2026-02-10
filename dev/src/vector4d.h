#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <concepts>
#include <cstdint>
#include <string>
#include <array>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "vector2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_VECTOR4D_DECLARED)
namespace olc
{
	/*
		A complete 4D geometric vector structure, with a variety
		of useful utility functions and operator overloads.
	*/
	template<class T>
	struct v_4d
	{
		static_assert(std::is_arithmetic<T>::value, "olc::v_4d<type> must be numeric");

		union
		{
#pragma warning(disable:4201) // Top MSVC whinging about anonymous structs
			struct
			{
				// x-axis component
				T x;
				// y-axis component
				T y;
				// z-axis component
				T z;
				// w-axis component
				T w;
			};
#pragma warning(default:4201)

			std::array<T, 4> xyzw = { {0,0,0,1} };
		};

		// Default constructor
		inline constexpr v_4d() = default;

		// Specific constructor
		inline constexpr v_4d(T _x, T _y, T _z, T _w = 1) : x(_x), y(_y), z(_z), w(_w)
		{}

		inline constexpr v_4d(const v_2d<T>& v, T _z, T _w) : x(v.x), y(v.y), z(_z), w(_w)
		{}

		inline constexpr v_4d(const v_2d<T>& v1, const v_2d<T>& v2) : x(v1.x), y(v1.y), z(v2.x), w(v2.y)
		{}

		// Copy constructor
		inline constexpr v_4d(const v_4d& v) = default;

		// Assignment operator
		inline constexpr v_4d& operator=(const v_4d& v) = default;


		inline constexpr std::array<T, 4> a() const
		{
			return xyzw;
		}

		inline constexpr v_2d<T> xy() const
		{
			return v_2d<T>(x, y);
		}

		inline constexpr v_2d<T> zw() const
		{
			return v_2d<T>(z, w);
		}

		// Returns magnitude of vector
		inline constexpr auto mag() const
		{
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		// Returns magnitude squared of vector (useful for fast comparisons)
		inline constexpr T mag2() const
		{
			return x * x + y * y + z * z + w * w;
		}

		// Returns normalised version of vector
		inline constexpr v_4d norm() const
		{
			auto r = 1 / mag();
			return v_4d(x * r, y * r, z * r, w * r);
		}

		// Rounds all components down
		inline constexpr v_4d floor() const
		{
			return v_4d(std::floor(x), std::floor(y), std::floor(z), std::floor(w));
		}

		// Rounds all components accurately
		inline constexpr v_4d round() const
		{
			return v_4d(std::round(x), std::round(y), std::round(z), std::round(w));
		}

		// Rounds all components up
		inline constexpr v_4d ceil() const
		{
			return v_4d(std::ceil(x), std::ceil(y), std::ceil(z), std::ceil(w));
		}

		// Returns 'element-wise' max of this and another vector
		inline constexpr v_4d max(const v_4d& v) const
		{
			return v_4d(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z), std::max(w, v.w));
		}

		// Returns 'element-wise' min of this and another vector
		inline constexpr v_4d min(const v_4d& v) const
		{
			return v_4d(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z), std::min(w, v.w));
		}

		// Returns 'element-wise' abs of this vector
		inline constexpr v_4d abs() const
		{
			return v_4d(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
		}

		// Calculates scalar dot product between this and another vector
		inline constexpr auto dot(const v_4d& rhs) const
		{
			return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z + this->w * rhs.w;
		}

		// Calculates cross product between this and another vector
		inline constexpr v_4d cross(const v_4d& rhs) const
		{
			return v_4d(this->y * rhs.z - this->z * rhs.y, this->z * rhs.x - this->x * rhs.z, this->x * rhs.y - this->y * rhs.x, 0);
		}

		// Clamp the components of this vector in between the 'element-wise' minimum and maximum of 2 other vectors
		inline constexpr v_4d clamp(const v_4d& v1, const v_4d& v2) const
		{
			return this->max(v1).min(v2);
		}

		// Linearly interpolate between this vector, and another vector, given normalised parameter 't'
		inline constexpr v_4d lerp(const v_4d& v1, const double t) const
		{
			return (*this) * (T(1.0 - t)) + (v1 * T(t));
		}

		// Compare if this vector is numerically equal to another
		inline constexpr bool operator == (const v_4d& rhs) const
		{
			return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w);
		}

		// Compare if this vector is not numerically equal to another
		inline constexpr bool operator != (const v_4d& rhs) const
		{
			return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z || this->w != rhs.w);
		}

		// Return this vector as a std::string, of the form "(x,y,z,w)"
		inline std::string str() const
		{
			return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + "," + std::to_string(this->w) + ")";
		}

		// Allow 'casting' from other v_4d types
		template<class F>
		inline constexpr operator v_4d<F>() const
		{
			return { static_cast<F>(this->x), static_cast<F>(this->y), static_cast<F>(this->z), static_cast<F>(this->w) };
		}
	};

	// Multiplication operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator * (const TL& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator * (const v_4d<TL>& lhs, const TR& rhs)
	{
		return v_4d(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator * (const v_4d<TL>& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator *= (v_4d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs * rhs;
		return lhs;
	}

	// Division operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator / (const TL& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator / (const v_4d<TL>& lhs, const TR& rhs)
	{
		return v_4d(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator / (const v_4d<TL>& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator /= (v_4d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs / rhs;
		return lhs;
	}

	// Unary Addition operator (pointless but i like the platinum trophies)
	template<class T>
	inline constexpr auto operator + (const v_4d<T>& lhs)
	{
		return v_4d(+lhs.x, +lhs.y, +lhs.z, +lhs.w);
	}

	// Addition operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator + (const TL& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator + (const v_4d<TL>& lhs, const TR& rhs)
	{
		return v_4d(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator + (const v_4d<TL>& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator += (v_4d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs + rhs;
		return lhs;
	}

	template<class TL, class TR>
	inline constexpr auto operator += (v_4d<TL>& lhs, const v_4d<TR>& rhs)
	{
		lhs = lhs + rhs;
		return lhs;
	}

	// Unary negation operator overoad for inverting a vector
	template<class T>
	inline constexpr auto operator - (const v_4d<T>& lhs)
	{
		return v_4d(-lhs.x, -lhs.y, -lhs.z, -lhs.w);
	}

	// Subtraction operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator - (const TL& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator - (const v_4d<TL>& lhs, const TR& rhs)
	{
		return v_4d(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator - (const v_4d<TL>& lhs, const v_4d<TR>& rhs)
	{
		return v_4d(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator -= (v_4d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs - rhs;
		return lhs;
	}

	// Greater/Less-Than Operator overloads - mathematically useless, but handy for "sorted" container storage
	template<class TL, class TR>
	inline constexpr bool operator < (const v_4d<TL>& lhs, const v_4d<TR>& rhs)
	{
		return (lhs.w < rhs.w) 
			|| (lhs.w == rhs.w && lhs.z < rhs.z) 
			|| (lhs.w == rhs.w && lhs.z == rhs.z && lhs.y < rhs.y) 
			|| (lhs.w == rhs.w && lhs.z == rhs.z && lhs.y == rhs.y && lhs.x < rhs.x);
	}

	template<class TL, class TR>
	inline constexpr bool operator > (const v_4d<TL>& lhs, const v_4d<TR>& rhs)
	{
		return (lhs.w > rhs.w) 
			|| (lhs.w == rhs.w && lhs.z > rhs.z) 
			|| (lhs.w == rhs.w && lhs.z == rhs.z && lhs.y > rhs.y) 
			|| (lhs.w == rhs.w && lhs.z == rhs.z && lhs.y == rhs.y && lhs.x > rhs.x);
	}

	// Allow olc::v_4d to play nicely with std::cout
	template<class T>
	inline std::ostream& operator << (std::ostream& os, const v_4d<T>& rhs)
	{
		os << rhs.str();
		return os;
	}

	// Convenient types ready-to-go
	typedef v_4d<int32_t> vi4d;
	typedef v_4d<uint32_t> vu4d;
	typedef v_4d<float> vf4d;
	typedef v_4d<double> vd4d;
}
#define PGE_VECTOR4D_DECLARED 1
#endif
//! END DECLARATION