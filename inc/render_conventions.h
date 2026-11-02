#pragma once

#include "vector_3.h"

namespace render_ns
{

// Convention used by this engine right now:
// V = camera -> surface (P - C)
// "Front facing" means dot(N, V) < 0
inline bool front_facing(const vector_3_ns::vector_3& N,
                         const vector_3_ns::vector_3& V_cam_to_surface,
                         bool force)
{
	return force || (vector_3_ns::vector_3::dot(N, V_cam_to_surface) < -EPSILON);
}

// Lighting convention:
// L = surface -> light
inline unit lambert(const vector_3_ns::vector_3& N,
                    const vector_3_ns::vector_3& L_surface_to_light)
{
	return vector_3_ns::vector_3::dot(N, L_surface_to_light);
}

} // namespace render_ns
