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
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_VECTOR2D_DECLARED)
namespace olc
{
	template <typename T>
	concept numeric = std::integral<T> || std::floating_point<T>;

	/*
		A complete 2D geometric vector structure, with a variety
		of useful utility functions and operator overloads
	*/
	template<class T>
	struct v_2d
	{
		static_assert(std::is_arithmetic<T>::value, "olc::v_2d<type> must be numeric");

		union
		{
#pragma warning(disable:4201) // Top MSVC whinging about anonymous structs
			struct
			{
				// x-axis component
				T x;
				// y-axis component
				T y;
			};
#pragma warning(default:4201)

			std::array<T, 2> xy = { {0,0} };
		};

		// Default constructor
		inline constexpr v_2d() = default;

		// Specific constructor
		inline constexpr v_2d(T _x, T _y) : x(_x), y(_y)
		{}

		// Copy constructor
		inline constexpr v_2d(const v_2d& v) = default;

		// Assignment operator
		inline constexpr v_2d& operator=(const v_2d& v) = default;


		inline constexpr std::array<T, 2> a() const
		{
			return xy;
		}

		// Returns rectangular area of vector
		inline constexpr auto area() const
		{
			return x * y;
		}

		// Returns magnitude of vector
		inline constexpr auto mag() const
		{
			return std::sqrt(x * x + y * y);
		}

		// Returns magnitude squared of vector (useful for fast comparisons)
		inline constexpr T mag2() const
		{
			return x * x + y * y;
		}

		// Returns normalised version of vector
		inline constexpr v_2d norm() const
		{
			auto r = 1 / mag();
			return v_2d(x * r, y * r);
		}

		// Returns vector at 90 degrees to this one
		inline constexpr v_2d perp() const
		{
			return v_2d(-y, x);
		}

		// Rounds both components down
		inline constexpr v_2d floor() const
		{
			return v_2d(std::floor(x), std::floor(y));
		}

		// Rounds both components accurately
		inline constexpr v_2d round() const
		{
			return v_2d(std::round(x), std::round(y));
		}

		// Rounds both components up
		inline constexpr v_2d ceil() const
		{
			return v_2d(std::ceil(x), std::ceil(y));
		}

		// Returns 'element-wise' max of this and another vector
		inline constexpr v_2d max(const v_2d& v) const
		{
			return v_2d(std::max(x, v.x), std::max(y, v.y));
		}

		// Returns 'element-wise' min of this and another vector
		inline constexpr v_2d min(const v_2d& v) const
		{
			return v_2d(std::min(x, v.x), std::min(y, v.y));
		}

		// Returns 'element-wise' abs of this vector
		inline constexpr v_2d abs() const
		{
			return v_2d(std::abs(x), std::abs(y));
		}

		// Calculates scalar dot product between this and another vector
		inline constexpr auto dot(const v_2d& rhs) const
		{
			return this->x * rhs.x + this->y * rhs.y;
		}

		// Calculates 'scalar' cross product between this and another vector (useful for winding orders)
		inline constexpr auto cross(const v_2d& rhs) const
		{
			return this->x * rhs.y - this->y * rhs.x;
		}

		// Treat this as polar coordinate (R, Theta), return cartesian equivalent (X, Y)
		inline constexpr v_2d cart() const
		{
			return v_2d(std::cos(y) * x, std::sin(y) * x);
		}

		// Treat this as cartesian coordinate (X, Y), return polar equivalent (R, Theta)
		inline constexpr v_2d polar() const
		{
			return v_2d(mag(), std::atan2(y, x));
		}

		// Clamp the components of this vector in between the 'element-wise' minimum and maximum of 2 other vectors
		inline constexpr v_2d clamp(const v_2d& v1, const v_2d& v2) const
		{
			return this->max(v1).min(v2);
		}

		// Linearly interpolate between this vector, and another vector, given normalised parameter 't'
		inline constexpr v_2d lerp(const v_2d& v1, const double t) const
		{
			return (*this) * (T(1.0 - t)) + (v1 * T(t));
		}

		// Compare if this vector is numerically equal to another
		inline constexpr bool operator == (const v_2d& rhs) const
		{
			return (this->x == rhs.x && this->y == rhs.y);
		}

		// Compare if this vector is not numerically equal to another
		inline constexpr bool operator != (const v_2d& rhs) const
		{
			return (this->x != rhs.x || this->y != rhs.y);
		}

		// Return this vector as a std::string, of the form "(x,y)"
		inline std::string str() const
		{
			return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")";
		}

		// Assuming this vector is incident, given a normal, return the reflection
		inline constexpr v_2d reflect(const v_2d& n) const
		{
			return (*this) - 2.0 * (this->dot(n) * n);
		}

		// Allow 'casting' from other v_2d types
		template<class F>
		inline constexpr operator v_2d<F>() const
		{
			return { static_cast<F>(this->x), static_cast<F>(this->y) };
		}
	};

	// Multiplication operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator * (const TL& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs * rhs.x, lhs * rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator * (const v_2d<TL>& lhs, const TR& rhs)
	{
		return v_2d(lhs.x * rhs, lhs.y * rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator * (const v_2d<TL>& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs.x * rhs.x, lhs.y * rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator *= (v_2d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs * rhs;
		return lhs;
	}

	// Division operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator / (const TL& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs / rhs.x, lhs / rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator / (const v_2d<TL>& lhs, const TR& rhs)
	{
		return v_2d(lhs.x / rhs, lhs.y / rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator / (const v_2d<TL>& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs.x / rhs.x, lhs.y / rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator /= (v_2d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs / rhs;
		return lhs;
	}

	// Unary Addition operator (pointless but i like the platinum trophies)
	template<class T>
	inline constexpr auto operator + (const v_2d<T>& lhs)
	{
		return v_2d(+lhs.x, +lhs.y);
	}

	// Addition operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator + (const TL& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs + rhs.x, lhs + rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator + (const v_2d<TL>& lhs, const TR& rhs)
	{
		return v_2d(lhs.x + rhs, lhs.y + rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator + (const v_2d<TL>& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator += (v_2d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs + rhs;
		return lhs;
	}

	template<class TL, class TR>
	inline constexpr auto operator += (v_2d<TL>& lhs, const v_2d<TR>& rhs)
	{
		lhs = lhs + rhs;
		return lhs;
	}

	// Unary negation operator overoad for inverting a vector
	template<class T>
	inline constexpr auto operator - (const v_2d<T>& lhs)
	{
		return v_2d(-lhs.x, -lhs.y);
	}

	// Subtraction operator overloads between vectors and scalars, and vectors and vectors
	template<olc::numeric TL, class TR>
	inline constexpr auto operator - (const TL& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs - rhs.x, lhs - rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator - (const v_2d<TL>& lhs, const TR& rhs)
	{
		return v_2d(lhs.x - rhs, lhs.y - rhs);
	}

	template<class TL, class TR>
	inline constexpr auto operator - (const v_2d<TL>& lhs, const v_2d<TR>& rhs)
	{
		return v_2d(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	template<class TL, olc::numeric TR>
	inline constexpr auto operator -= (v_2d<TL>& lhs, const TR& rhs)
	{
		lhs = lhs - rhs;
		return lhs;
	}

	// Greater/Less-Than Operator overloads - mathematically useless, but handy for "sorted" container storage
	template<class TL, class TR>
	inline constexpr bool operator < (const v_2d<TL>& lhs, const v_2d<TR>& rhs)
	{
		return (lhs.y < rhs.y) || (lhs.y == rhs.y && lhs.x < rhs.x);
	}

	template<class TL, class TR>
	inline constexpr bool operator > (const v_2d<TL>& lhs, const v_2d<TR>& rhs)
	{
		return (lhs.y > rhs.y) || (lhs.y == rhs.y && lhs.x > rhs.x);
	}

	// Allow olc::v_2d to play nicely with std::cout
	template<class T>
	inline std::ostream& operator << (std::ostream& os, const v_2d<T>& rhs)
	{
		os << rhs.str();
		return os;
	}

	// Convenient types ready-to-go
	typedef v_2d<int32_t> vi2d;
	typedef v_2d<uint32_t> vu2d;
	typedef v_2d<float> vf2d;
	typedef v_2d<double> vd2d;
}
#define PGE_VECTOR2D_DECLARED 1
#endif
//! END DECLARATION