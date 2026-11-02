#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <string>

#include "utils.h"

namespace graphics_ns
{

constexpr std::uint32_t DEFAULT_SCALE(1U);

#define MAKE_COLOR(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

using val_t = std::int32_t;
using uval_t = std::uint32_t;
using coord_t = uval_t;
using color_t = uval_t;

class graphics
{
public:

	enum color_idx
	{
		__first_color__ = 0,     // Dummy, not an actual color
		black = __first_color__, // 0
		grey,                    // 1
		silver,                  // 2
		white,                   // 3
		red,                     // 4
		maroon,                  // 5
		yellow,                  // 6
		olive,                   // 7
		lime,                    // 8
		green,                   // 9
		aqua,                    // 10
		teal,                    // 11
		blue,                    // 12
		navy,                    // 13
		magenta,                 // 14
		purple,                  // 15
		__last_color__           // Dummy, not an actual color
	};

	enum alpha_idx
	{
		__first_alpha__ = 0, // Dummy, not an actual alpha
		A1 = __first_alpha__,
		A2,
		A3,
		A4,
		A5,
		A6,
		A7,
		A8,
		A9,
		A10,
		A11,
		A12,
		A13,
		A14,
		A15,
		A16,
		__last_alpha__ // Dummy, not an actual alpha
	};

	union ARGB
	{
		color_t c {0};
		struct _channel
		{
			color_t b : 8;
			color_t g : 8;
			color_t r : 8;
			color_t a : 8;
		} channel;

		bool operator==(const ARGB& other) { return c == other.c; }
	};

	struct color_data
	{
		std::string name;
		ARGB color;
	};

	struct frame_buffer
	{
		color_t* pixels {nullptr}; // ARGB8888
		val_t width {0};
		val_t height {0};
		val_t pitch_bytes {0};  // bytes per row (typically width * 4)
		val_t pitch_pixels {0}; // pixels per row (width)
	};

	struct input_state
	{
		bool quit {false};
		bool key_escape {false};
		// Extend later: arrows, mouse, etc.
	};

	struct point
	{
		coord_t x {0};
		coord_t y {0};
	};

	using addr_t = ARGB*;
	typedef std::map<color_idx, color_data> color_map;
	typedef std::map<alpha_idx, uint8_t> alpha_map;

	graphics();
	graphics(const char*);
	graphics(const char*, uval_t, uval_t, uval_t);
	~graphics();

	// class with allocated resource should not be copied
	graphics(const graphics&) = delete;
	graphics& operator=(const graphics&) = delete;

	// event + timing
	void poll_events(input_state&);
	std::uint64_t now_ticks() const;
	std::uint64_t tick_freq() const;

	// framebuffer flow (engine-driven)
	frame_buffer get_backbuffer();
	void fill_buffer(frame_buffer&, const ARGB&);
	void present(); // uploads backbuffer, presents, swaps buffers

	// color management
	ARGB get_color_val(const color_idx) const;
	std::string get_color_name(const color_idx) const;
	uint8_t get_alpha_val(const alpha_idx);
	void set_alpha(ARGB&, const uint8_t);
	val_t get_num_colors() const { return __last_color__ - 1; }
	val_t get_num_alphas() const { return __last_alpha__ - 1; }
	point get_min_position() const { return point {MIN_X_POS, MIN_Y_POS}; }
	point get_max_position() const { return point {(uval_t)_w - 1, (uval_t)_h - 1}; }
	bool is_in_bounds(point p) const { return (p.x < (uval_t)_w && p.y < (uval_t)_h); }

private:

	std::string _title;
	val_t _w {0};
	val_t _h {0};
	val_t _scale {1};
	val_t _scaled_w {0};
	val_t _scaled_h {0};
	val_t _win_size {0};
	val_t _pitch_bytes {0};
	val_t _pitch_pixels {0};
	input_state _last_input {};
	color_map _colors;
	alpha_map _alphas;

	// CPU-side double buffers
	std::vector<color_t> _buf_a;
	std::vector<color_t> _buf_b;
	bool _a_is_front {true};

	// SDL objects (opaque pointers; stored as void* in header to avoid SDL includes here)
	void* _window {nullptr};
	void* _renderer {nullptr};
	void* _texture {nullptr};

	// Timing
	uint64_t _perf_freq {0};

	void init_graphics();
	void swap_buffers();

	// clang-format off
	bool is_valid_color(color_idx i) const { return ((int)i >= (int)__first_color__ && (int)i < (int)__last_color__); }
	bool is_valid_alpha(alpha_idx a) const { return ((int)a >= (int)__first_alpha__ && (int)a < (int)__last_alpha__); }
	// clang-format on
};

} // namespace graphics_ns
