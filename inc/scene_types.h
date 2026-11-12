#pragma once

#include <memory>

#include "config_document.h"

// clang-format off
namespace polygon_ns { class polygon; }
namespace graphics_ns { class graphics; }
// clang-format on

namespace scene_ns
{

using drawvec_t = std::vector<polygon_ns::polygon*>;

struct graphics_state
{
	std::unique_ptr<graphics_ns::graphics> gfx {nullptr};
	graphics_ns::graphics::frame_buffer fb;
	graphics_ns::graphics::ARGB bkg_color {};
	uint32_t width {DEF_RES_WIDTH};
	uint32_t height {DEF_RES_HEIGHT};
};

struct light_state
{
	config_ns::light_type type;

	// Convention: direction is "surface -> light", should be normalized
	vector_3_ns::vector_3 direction;

	// Used only if type == point
	vector_3_ns::vector_3 position;
};

struct camera_state
{
	vector_3_ns::vector_3 position;
};

struct projection_state
{
	// distance from camera to projection plane (in world units)
	unit focal_len; // default preserves old look

	// optional safety clamps (can be ignored initially)
	unit near_eps; // avoid divide by ~0
};

struct window_state
{
	// screen min/max points
	graphics_ns::graphics::point min_pos;
	graphics_ns::graphics::point max_pos;
	graphics_ns::graphics::point mid_pos;
	graphics_ns::graphics::color_idx bkg_color_idx;
};

struct runtime_state
{
	int loops;
	uint32_t loop_delay;
};

struct scene_state
{
	graphics_state grfx;
	light_state light;
	camera_state camera;
	projection_state proj;
	window_state win;
	runtime_state run;
};

struct frame_context
{
	std::unique_ptr<drawvec_t> draw_vec {nullptr};
	std::unique_ptr<scene_state> state {nullptr};
};

} // namespace scene_ns
