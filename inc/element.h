#pragma once

#include "polygon.h"
#include "treenode.h"

// clang-format off
namespace attrib_ns   { class attrib; }
namespace graphics_ns { class graphics; }
namespace matrix_ns   { class matrix; }
namespace vector_3_ns { class vector_3; }
// clang-format on

namespace element_ns
{

class element : public treenode_ns::treenode<element>
{
public:

	using frame_context = scene_ns::frame_context;

	element() = default;
	~element() = default;

	void init_from_def(const polygon_ns::polygon::polylist_t&, element*, const config_ns::element_def&);
	void print() const;
	void print_all();

	element* find(element*, const std::string&) const;

	void update(const attrib_ns::attrib&);
	void update();
	void update(const matrix_ns::matrix&, const matrix_ns::matrix&, const bool, frame_context&);
	void update(frame_context&);
	void update_all(frame_context&);

	const std::string get_name() const { return _name; }
	bool has_motion() const { return _motion.has_value(); }

	bool _world_dirty {false};

private:

	polygon_ns::polygon::polylist_t _polygons;
	std::string _name;
	std::string _parent_name;
	element* _parent {nullptr}; // raw pointer, non-owning
	matrix_ns::matrix _trans_mat;
	matrix_ns::matrix _rot_mat;
	attrib_ns::attrib _origin;
	bool _active {false};
	bool _dirty {false};
	std::optional<attrib_ns::attrib> _motion;

	polygon_ns::polygon* find(const polygon_ns::polygon::polylist_t&, const std::string&) const;
};

} // namespace element_ns
