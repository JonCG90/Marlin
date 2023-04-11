#ifndef MARLIN_DEFS_HPP
#define MARLIN_DEFS_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <glm/glm.hpp>
#pragma clang diagnostic pop

namespace marlin
{

using Vec2f = glm::fvec2;
using Vec3f = glm::fvec3;
using Vec4f = glm::fvec4;
using Mat3f = glm::fmat3;
using Mat4f = glm::fmat4;
using Quatf = glm::fquat;

using Vec2d = glm::dvec2;
using Vec3d = glm::dvec3;
using Vec4d = glm::dvec4;
using Mat3d = glm::dmat3;
using Mat4d = glm::dmat4;
using Quatd = glm::dquat;

using Vec2i = glm::ivec2;
using Vec3i = glm::ivec3;
using Vec4i = glm::ivec4;

using Vec2u = glm::uvec2;
using Vec3u = glm::uvec3;
using Vec4u = glm::uvec4;

template< class T >
using Vec2T = glm::tvec2< T, glm::precision::defaultp >;

template< class T >
using Vec3T = glm::tvec3< T, glm::precision::defaultp >;

template< class T >
using Vec4T = glm::tvec4< T, glm::precision::defaultp >;

template< class T >
using Mat3T = glm::tmat3x3< T, glm::precision::defaultp >;

template< class T >
using Mat4T = glm::tmat4x4< T, glm::precision::defaultp >;

// Use float by default
using Vec2 = Vec2f;
using Vec3 = Vec3f;
using Vec4 = Vec4f;
using Mat3 = Mat3f;
using Mat4 = Mat4f;
using Quat = Quatd;

static constexpr double s_Pi = 3.14159265358979323846264338327950288;
static constexpr double s_2Pi = 2.0 * s_Pi;
static constexpr double s_4Pi = 4.0 * s_Pi;
static constexpr double s_1_Pi = 1.0 / s_Pi;
static constexpr double s_2_Pi = 2.0 / s_Pi;
static constexpr double s_Pi_2 = s_Pi / 2.0;
static constexpr double s_Pi_4 = s_Pi / 4.0;

static constexpr float s_MaxFloat = std::numeric_limits< float >::max();
static constexpr float s_InfinityFloat = std::numeric_limits< float >::infinity();

static constexpr double s_MaxDouble = std::numeric_limits< double >::max();
static constexpr double s_InfinityDouble = std::numeric_limits< double >::infinity();

} // namespace marlin

#endif /* MARLIN_DEFS_HPP */

