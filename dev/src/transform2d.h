#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <stack>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "matrix3d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_TRANSFORM2D_DECLARED)
#if OLC_HOST == OLC_HOST_MACOS || OLC_HOST == OLC_HOST_IOS
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-qualifiers" // Silence warnings about ignored qualifiers in olc::t_2d
#endif
namespace olc
{
	namespace internal
	{
		// Return sign of number as -1 or +1
		template <typename T>
		inline constexpr int sgn(T val)
		{
			return (T(0) < val) - (val < T(0));
		}
	}



	/*
		A complete 2D transformation structure
	*/
	template<class T>
	class t_2d
	{
		static_assert(std::is_floating_point<T>::value, "olc::t_2d<type> must be floating point type");

	public:
		// Constructor
		inline constexpr t_2d()
		{
			identity();
		}

		// Copy constructor
		inline constexpr t_2d(const t_2d& t) = default;

		// Assignment operator
		inline constexpr t_2d& operator=(const t_2d& t) = default;

		inline constexpr void identity()
		{
			// Clear stacks and reset to identity			
			m_stackForward = std::stack<olc::m_3d<T>>();
			m_stackInverse = std::stack<olc::m_3d<T>>();
			
			m_stackForward.push(olc::m_3d<T>::identity_matrix());
			m_stackInverse.push(olc::m_3d<T>::identity_matrix());
		}

		// Transform a vector by this transform
		template<typename Q>
		inline constexpr auto forward(const olc::v_2d<Q>& v) const
		{
			return m_stackForward.top() * v;
		}

		template<typename Q>
		inline constexpr auto forwardRound(const olc::v_2d<Q>& v) const
		{
			return (m_stackForward.top() * v).round();
		}

		// Transform a vector of v_2d by this transform
		template<typename Q>
		inline constexpr auto forward(const std::vector<olc::v_2d<Q>>& v) const
		{
			std::vector<olc::v_2d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_2d<Q>& i) {return m_stackForward.top() * i; });
			return o;
		}

		template<typename Q>
		inline constexpr auto forwardRound(const std::vector<olc::v_2d<Q>>& v) const
		{
			std::vector<olc::v_2d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_2d<Q>& i) {return (m_stackForward.top() * i).round(); });
			return o;
		}

		// Transform a vector by this transform in place
		template<typename Q>
		inline constexpr auto forwardX(std::vector<olc::v_2d<Q>>&& v) const
		{
			std::transform(v.begin(), v.end(), v.begin(), [this](const olc::v_2d<Q>& i) {return (m_stackForward.top() * i); });
			return v;
		}

		template<typename Q>
		inline constexpr auto forwardRoundX(std::vector<olc::v_2d<Q>>&& v) const
		{
			std::transform(v.begin(), v.end(), v.begin(), [this](const olc::v_2d<Q>& i) {return (m_stackForward.top() * i).round(); });
			return v;
		}

		// Transform a vector by the inverse of this transform
		template<typename Q>
		inline constexpr auto inverse(const olc::v_2d<Q>& v) const
		{
			return m_stackInverse.top() * v;
		}

		// Transform a vector by this transform
		template<typename Q>
		inline constexpr auto inverse(const std::vector<olc::v_2d<Q>>& v) const
		{
			std::vector<olc::v_2d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_2d<Q>& i) {return m_stackInverse.top() * i; });
			return o;
		}

		// Transform a vector by this transform in place
		template<typename Q>
		inline constexpr auto inverseX(std::vector<olc::v_2d<Q>>&& v) const
		{
			std::transform(v.begin(), v.end(), v.begin(), [this](const olc::v_2d<Q>& i) {return m_stackInverse.top() * i; });
			return v;
		}


		// Set scaling component of this transformation
		template<typename Q>
		inline constexpr void scale(const olc::v_2d<Q>& v)
		{
			m_stackForward.push(m_stackForward.top() * olc::m_3d<T>::scaling(v));
			m_stackInverse.push(m_stackForward.top().invert());
		}

		// Get scaling component of this transformation
		inline constexpr const auto scale() const
		{
			const auto& mat = m_stackForward.top();
			return olc::v_2d<T>
			{
				T(olc::internal::sgn(mat(0, 0)) * std::hypot(mat(0, 0), mat(1, 0))),
				T(olc::internal::sgn(mat(1, 1)) * std::hypot(mat(0, 1), mat(1, 1)))
			};
		}

		// Set translation component of this transformation
		template<typename Q>
		inline constexpr void translate(const olc::v_2d<Q>& v)
		{
			m_stackForward.push(m_stackForward.top() * olc::m_3d<T>::translation(v));
			m_stackInverse.push(m_stackForward.top().invert());
		}

		// Get translation component of this transformation
		inline constexpr const auto translation() const
		{
			const auto& mat = m_stackForward.top();
			return olc::v_2d<T>
			{
				T(mat(2, 0)),
				T(mat(2, 1))
			};
		}

		// Set translation component of this transformation
		template<typename Q>
		inline constexpr void rotate(const Q& v, const olc::v_2d<T>& p = { 0,0 })
		{
			m_stackForward.push(m_stackForward.top() * (olc::m_3d<T>::translation(-p) * olc::m_3d<T>::rotation(v) * olc::m_3d<T>::translation(p)));
			m_stackInverse.push(m_stackForward.top().invert());
		}

		// Get translation component of this transformation
		inline constexpr const auto rotation() const
		{
			const auto& mat = m_stackForward.top();
			return T(std::atan2(mat(0, 1), mat(1, 1)));
		}

		// Set shear component of this transformation
		template<typename Q>
		inline constexpr void shear(const olc::v_2d<Q>& v)
		{
			m_stackForward.push(m_stackForward.top() * olc::m_3d<T>::shearing(v));
			m_stackInverse.push(m_stackForward.top().invert());
		}

		// Get forward transformation matrix
		inline constexpr const auto& forward_matrix() const
		{
			return m_stackForward.top();
		}

		// Get inverse transformation matrix
		inline constexpr const auto& inverse_matrix() const
		{
			return m_stackInverse.top();
		}

		template<typename Q>
		inline constexpr auto operator * (const olc::t_2d<Q>& rhs) const
		{
			auto& me = *this;		
			return me.forward_matrix() * rhs.forward_matrix();			
		}

		// Push a new transform onto the stack, multiplying it with the 
		// current transform (arbitrary affine matrix)
		template<typename Q>
		inline constexpr auto push(const olc::m_3d<Q>& m)
		{
			m_stackForward.push(m_stackForward.top() * m);
			m_stackInverse.push(m_stackForward.top().invert());

		}

		// Pop the last transform off the stack, reverting to the 
		// previous transform
		inline constexpr auto pop()
		{
			if (!m_stackForward.empty())
			{
				m_stackForward.pop();
				m_stackInverse.pop();

				if (m_stackForward.empty())
					identity();
			}
		}

		// Condense the transform stack into a single matrix, leaving
		// the current transformation intact but removing all history
		inline constexpr void squash()
		{
			if (m_stackForward.size() > 1)
			{
				auto top = m_stackForward.top();
				m_stackForward = std::stack<olc::m_3d<T>>();
				m_stackInverse = std::stack<olc::m_3d<T>>();
				m_stackForward.push(top);
				m_stackInverse.push(top.invert());
			}
		}

	protected:
		std::stack<olc::m_3d<T>> m_stackForward;
		std::stack<olc::m_3d<T>> m_stackInverse;
	};


	typedef t_2d<float> tf2d;
	typedef t_2d<double> td2d;
}
#if OLC_HOST == OLC_HOST_MACOS || OLC_HOST == OLC_HOST_IOS
#pragma clang diagnostic pop
#endif
#define PGE_TRANSFORM2D_DECLARED 1
#endif
//! END DECLARATION
