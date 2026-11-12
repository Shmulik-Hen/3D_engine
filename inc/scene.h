#pragma once

#include "element.h"
#include "scene_types.h"

// clang-format off
namespace element_ns  { class element; }
namespace graphics_ns { class graphics; }
namespace polygon_ns  { class polygon; }
// clang-format on

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
	void render();
	bool keep_going();
	config_ns::parse_scn_def* find_scene(const std::string&);

private:

	static bool _keep_going;

	void sort();
	void draw();
	void set_defaults();
	void check_overrides(const config_ns::parse_scn_def&);
	void init_graphics();
};

} // namespace scene_ns
