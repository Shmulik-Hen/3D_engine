#pragma once
#include <filesystem>

#include "attrib.h"
#include "element.h"
#include "scene_types.h"

// clang-format off
namespace attrib_ns  { class attrib; }
namespace element_ns  { class element; }
namespace graphics_ns { class graphics; }
namespace polygon_ns  { class polygon; }
// clang-format on

namespace fs = std::filesystem;

namespace scene_ns
{

class my_scene
{
public:

	// Own everything allocated during parsing/runtime
	std::list<std::unique_ptr<polygon_ns::polygon>> polygons_owned;
	std::list<std::unique_ptr<element_ns::element>> elements_owned;

	// Non-owning views
	element_ns::element::poly_ptr_list_t poly_list;

	// Root of element tree (non-owning)
	element_ns::element* root {nullptr};

	frame_context frame_ctx;

	config_ns::AST ast;

	graphics_ns::graphics* gfx {nullptr};

	my_scene();
	my_scene(const my_scene&) = delete;
	my_scene& operator=(const my_scene&) = delete;
	my_scene(my_scene&&) = delete;
	my_scene& operator=(my_scene&&) = delete;
	~my_scene() = default;

	polygon_ns::polygon* add_polygon();
	element_ns::element* add_element();
	bool parse(std::ifstream&);
	bool validate();
	void build(const config_ns::parse_scn_def&);
	void update();
	void refresh_elements();
	void poll_events();
	void render();
	inline bool keep_going() { return _keep_going; };
	config_ns::parse_scn_def* find_scene(const std::string&);

private:

	bool _keep_going {true};
	bool _pause {false};
	bool _step {false};
	bool _capture {false};
	bool _record {false};
	bool _captures_dir_created {false};
	bool _session_dir_created {false};
	int _loops {0};
	int _orig_loops {0};
	uint32_t _orig_loop_delay {0};
	uint32_t _frames_remaining {0};
	uint64_t _frames_counter {0};
	attrib_ns::attrib _unit_att;
	fs::path _captures_dir;

	void sort();
	void draw();
	void set_defaults();
	void check_overrides(const config_ns::parse_scn_def&, const config_ns::valid_scene_ref&);
	void init_graphics();
	bool matches(const config_ns::key_binding&, SDL_Keycode, SDL_Keymod);
	void handle_kbd_event(SDL_KeyboardEvent&);
	void create_directories();
	void create_capture_filename(std::ostringstream&);
	void capture_frame(graphics_ns::graphics::frame_buffer&);
	SDL_Keymod normalize_modifiers(SDL_Keymod&);
};

} // namespace scene_ns
