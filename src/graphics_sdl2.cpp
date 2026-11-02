#include <SDL2/SDL.h>

// #define DEBUG_PRINTS
#include "graphics.h"
#include "constants.h"

namespace graphics_ns
{

using ARGB = graphics::ARGB;           // bring nested type into namespace scope
using color_idx = graphics::color_idx; // same for color_idx
using color_map = graphics::color_map; // same for color_map
using input_state = graphics::input_state;

constexpr std::int32_t DEFAULT_WIDTH = DEF_RES_WIDTH;
constexpr std::int32_t DEFAULT_HEIGHT = DEF_RES_HEIGHT;
constexpr const char* DEFAULT_NAME = "Graphics_SDL2";

graphics::graphics() :
	_title {DEFAULT_NAME},
	_w {DEFAULT_WIDTH},
	_h {DEFAULT_HEIGHT},
	_scale {DEFAULT_SCALE},
	_scaled_w {_w * _scale},
	_scaled_h {_h * _scale},
	_win_size {_w * _h},
	_pitch_bytes {_w * (val_t)sizeof(color_t)},
	_pitch_pixels {_w}
{
	init_graphics();
}

graphics::graphics(const char* title) :
	_title {title},
	_w {DEFAULT_WIDTH},
	_h {DEFAULT_HEIGHT},
	_scale {DEFAULT_SCALE},
	_scaled_w {_w * _scale},
	_scaled_h {_h * _scale},
	_win_size {_w * _h},
	_pitch_bytes {_w * (val_t)sizeof(color_t)},
	_pitch_pixels {_w}
{
	init_graphics();
}

graphics::graphics(const char* title, uval_t width, uval_t height, uval_t scale) :
	_title {title},
	_w {std::min((int)width, (int)MAX_RES_WIDTH)},
	_h {std::min((int)height, (int)MAX_RES_HEIGHT)},
	_scale {(int)scale},
	_scaled_w {_w * _scale},
	_scaled_h {_h * _scale},
	_win_size {_w * _h},
	_pitch_bytes {_w * (val_t)sizeof(color_t)},
	_pitch_pixels {_w}
{
	if (!_w || !_h || !_scale || _title.empty()) {
		sys_error("Invalid width/height/scale/title");
	}

	init_graphics();
}

graphics::~graphics()
{
	// Destroy in reverse creation order
	if (_texture) {
		SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(_texture));
		_texture = nullptr;
	}

	if (_renderer) {
		SDL_DestroyRenderer(reinterpret_cast<SDL_Renderer*>(_renderer));
		_renderer = nullptr;
	}

	if (_window) {
		SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(_window));
		_window = nullptr;
	}

	SDL_Quit();
}

void graphics::poll_events(input_state& io)
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			DBG("SDL_QUIT");
			io.quit = true;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				DBG("SDLK_ESCAPE");
				io.key_escape = true;
				io.quit = true;
			}
			break;
		default:
			DBG("default: event type: " << HEX(e.type, 8));
			break;
		}
	}

	_last_input = io;
}

std::uint64_t graphics::now_ticks() const
{
	return static_cast<std::uint64_t>(SDL_GetPerformanceCounter());
}

std::uint64_t graphics::tick_freq() const
{
	return _perf_freq;
}

graphics::frame_buffer graphics::get_backbuffer()
{
	auto& back = _a_is_front ? _buf_b : _buf_a;
	frame_buffer fb {back.data(), _w, _h, _pitch_bytes, _pitch_pixels};

	return fb;
}

void graphics::fill_buffer(frame_buffer& fb, const ARGB& color)
{
	std::fill_n(fb.pixels, _win_size, color.c);
}

void graphics::present()
{
	frame_buffer fb = get_backbuffer();
	auto* tex = reinterpret_cast<SDL_Texture*>(_texture);
	auto* ren = reinterpret_cast<SDL_Renderer*>(_renderer);
	int rc;

	// upload pixels to GPU-managed texture
	rc = SDL_UpdateTexture(tex, nullptr, fb.pixels, fb.pitch_bytes);
	if (rc) {
		sys_error("SDL_UpdateTexture failed");
	}

	// Render texture scaled to window
	SDL_RenderClear(ren);

	SDL_Rect dst {0, 0, _scaled_w, _scaled_h};

	rc = SDL_RenderCopy(ren, tex, nullptr, &dst);
	if (rc) {
		sys_error("SDL_RenderCopy failed");
	}

	SDL_RenderPresent(ren);

	// only swap AFTER we showed the completed backbuffer
	swap_buffers();
}

ARGB graphics::get_color_val(const color_idx idx) const
{
	if (is_valid_color(idx)) {
		auto it = _colors.find(idx);
		if (it != _colors.end()) {
			color_data d = it->second;
			return d.color;
		}
	}

	DBG("get_color_val: return default");
	return ARGB {};
}

std::string graphics::get_color_name(const color_idx idx) const
{
	if (is_valid_color(idx)) {
		auto it = _colors.find(idx);
		if (it != _colors.end()) {
			color_data d = it->second;
			return d.name;
		}
	}

	DBG("get_color_name: returning default");
	return "";
}

uint8_t graphics::get_alpha_val(const alpha_idx idx)
{
	if (is_valid_alpha(idx)) {
		auto it = _alphas.find(idx);
		if (it != _alphas.end()) {
			DBG("get_alpha_val: " << HEX((int)it->second, 2));
			return it->second;
		}
	}

	DBG("get_alpha_val: returning default");
	return 0;
}

void graphics::set_alpha(ARGB& color, const uint8_t alpha)
{
	color.channel.a = alpha;
}

void graphics::init_graphics()
{
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture* tex;
	color_t black_ {0};
	int rc;

	// clang-format off
	const char* tmp_col[__last_color__] = {
		"black",	// 0
		"grey",		// 1
		"silver",	// 2
		"white",	// 3
		"red",		// 4
		"maroon",	// 5
		"yellow",	// 6
		"olive",	// 7
		"lime",		// 8
		"green",	// 9
		"aqua",		// 10
		"teal",		// 11
		"blue",		// 12
		"navy",		// 13
		"magenta",	// 14
		"purple"	// 15
	};

	const uint8_t channels[__last_color__][4] = {
		{ 255,   0,   0,   0 }, // 0  black
		{ 255, 128, 128, 128 }, // 1  grey
		{ 255, 192, 192, 192 }, // 2  silver
		{ 255, 255, 255, 255 }, // 3  white
		{ 255, 255,   0,   0 }, // 4  red
		{ 255, 128,   0,   0 }, // 5  maroon
		{ 255, 255, 255,   0 }, // 6  yellow
		{ 255, 128, 128,   0 }, // 7  olive
		{ 255,   0, 255,   0 }, // 8  lime
		{ 255,   0, 128,   0 }, // 9  green
		{ 255,   0, 255, 255 }, // 10 aqua
		{ 255,   0, 128, 128 }, // 11 teal
		{ 255,   0,   0, 255 }, // 12 blue
		{ 255,   0,   0, 128 }, // 13 navy
		{ 255, 255,   0, 255 }, // 14 magenta
		{ 255, 128,   0, 128 }  // 15 purple
	};

	uint8_t alpha_vals[__last_alpha__] = {
		0x00,
		0x11,
		0x22,
		0x33,
		0x44,
		0x55,
		0x66,
		0x77,
		0x88,
		0x99,
		0xaa,
		0xbb,
		0xcc,
		0xdd,
		0xee,
		0xff
	};
	// clang-format on

	_colors.clear();
	for (color_t i = __first_color__; i < __last_color__; i++) {
		color_idx idx = static_cast<color_idx>(i);
		const char* s = tmp_col[i];
		ARGB color;
		color.c = MAKE_COLOR(channels[idx][0], channels[idx][1], channels[idx][2], channels[idx][3]);

		if (idx == black) {
			black_ = color.c;
		}

		color_data d;
		d.name = std::string {s};
		d.color.c = color.c;

		std::pair p = {idx, d};
		_colors.insert(p);
	}

	if (_colors.empty()) {
		sys_error("colors init failed");
	}

	_alphas.clear();
	for (uval_t i = __first_alpha__; i < __last_alpha__; i++) {
		alpha_idx idx = static_cast<alpha_idx>(i);
		std::pair p = {idx, alpha_vals[idx]};
		_alphas.insert(p);
	}

	if (_alphas.empty()) {
		sys_error("alpha init failed");
	}

	rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	if (rc) {
		sys_error("SDL_Init failed");
	}

	// crisp integer scaling
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	win = SDL_CreateWindow(_title.c_str(),
	                       SDL_WINDOWPOS_CENTERED,
	                       SDL_WINDOWPOS_CENTERED,
	                       _scaled_w,
	                       _scaled_h,
	                       SDL_WINDOW_SHOWN);
	if (!win) {
		sys_error("SDL_CreateWindow failed");
	}
	_window = win;

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (!ren) {
		sys_error("SDL_CreateRenderer failed");
	}
	_renderer = ren;

	// Streaming texture so we can upload ARGB pixels each frame
	tex = SDL_CreateTexture(ren,
	                        SDL_PIXELFORMAT_ARGB8888,
	                        SDL_TEXTUREACCESS_STREAMING,
	                        _w,
	                        _h);
	if (!tex) {
		sys_error("SDL_CreateTexture failed");
	}
	_texture = tex;

	// CPU buffers
	_buf_a.resize(static_cast<size_t>(_win_size));
	_buf_b.resize(static_cast<size_t>(_win_size));
	// avoid uninitialized visuals
	std::fill_n(_buf_a.begin(), _win_size, black_);
	std::fill_n(_buf_b.begin(), _win_size, black_);

	// misc
	_perf_freq = static_cast<std::uint64_t>(SDL_GetPerformanceFrequency());

	DBG("graphics:");
	DBG("  title:         " << _title);
	DBG("  width:         " << _w);
	DBG("  height:        " << _h);
	DBG("  scale:         " << _scale);
	DBG("  scaled width:  " << _scaled_w);
	DBG("  scaled height: " << _scaled_h);
	DBG("  win size:      " << _win_size);
	DBG("  pitch bytes:   " << _pitch_bytes);
	DBG("  pitch pixels:  " << _pitch_pixels);
	DBG("  buff size:     " << DEC(_buf_a.size(), 4));
	DBG("  freq:          " << DEC(_perf_freq, 8) << ENDL);
}

void graphics::swap_buffers()
{
	_a_is_front = !_a_is_front;
}

} // namespace graphics_ns
