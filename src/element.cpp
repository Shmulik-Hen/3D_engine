// #define DEBUG_PRINTS
#include "utils.h"
#include "element.h"

namespace element_ns
{

using namespace matrix_ns;
using namespace polygon_ns;
using polygon = polygon_ns::polygon;

void element::init_from_def(const config_ns::parse_scn_elem_def& def)
{
	_name = def.name;
	_active = def.active;

	if (def.origin) {
		_origin = *def.origin;
	}

	if (def.motion) {
		_motion = *def.motion;
	}
}

void element::set_parent(element* parent)
{
	_parent = parent;
	_parent_name = parent ? parent->_name : "";

	if (parent) {
		add_node(parent);
	}
}

void element::add_polygon(polygon* poly)
{
	_polygons.push_back(poly);
}

void element::print() const
{
#ifdef DEBUG_PRINTS
	DBG(STR("element:", 1));
	DBG(STR("  name: ", 1) << _name);
	DBG(STR("  parent_name: ", 1) << _parent_name);
	DBG(STR("  active: ", 1) << _active);
	DBG(STR("   dirty: ", 1) << _dirty);
	_origin.print();

	if (!_polygons.empty()) {
		DBG("num polygons: " << (int)_polygons.size());
		DBG(STR("  polygons:", 1));
		for (const auto& poly : _polygons) {
			poly->print();
		}
	}
#endif // DEBUG_PRINTS
}

#ifdef DEBUG_PRINTS
static void prn(element_ns::element* e, void* data [[maybe_unused]])
{
	if (e) {
		e->print();
	}
}
#endif // DEBUG_PRINTS

void element::print_all()
{
#ifdef DEBUG_PRINTS
	this->print_tree(prn, NULL);
#endif // DEBUG_PRINTS
}

static bool match_element_name(element_ns::element* e, void* data)
{
	auto* key = static_cast<const std::string*>(data);
	return e && key && (e->get_name() == *key);
}

element* element::find(element* root, const std::string& s) const
{
	return root ? root->search_tree(match_element_name, (void*)&s) : nullptr;
}

void element::update(const attrib& att)
{
	_origin += att;
	_dirty = true;
}

void element::update()
{
	if (_motion.has_value()) {
		_origin += _motion.value();
		_dirty = true;
	}
}

void element::update(const matrix& p_trans, const matrix& p_rot, const bool p_dirty, frame_context& frame_ctx)
{
	if (!_active) {
		return;
	}

	_trans_mat.prep_trans_mat(_origin);
	_rot_mat.prep_rot_mat(_origin);

	_world_dirty = _dirty || p_dirty;

	if (_world_dirty) {
		_trans_mat = p_trans * _trans_mat;
		_rot_mat = p_rot * _rot_mat;

		if (_polygons.size()) {
			for (auto poly : _polygons) {
				if (poly) {
					poly->update(_trans_mat, _rot_mat, frame_ctx);
				}
			}
		}

		_dirty = false;
	}
}

void element::update(frame_context& frame_ctx)
{
	matrix m_trans, m_rot;
	bool parent_dirty;

	if (_parent) {
		m_trans = _parent->_trans_mat;
		m_rot = _parent->_rot_mat;
		parent_dirty = _parent->_world_dirty;
	}
	else {
		m_trans = matrix_ns::get_unit_mat();
		m_rot = matrix_ns::get_unit_mat();
		parent_dirty = false;
	}

	DBG("element::update: " << _name << "parent: " << _parent_name);
	DBG("transform:");
	m_trans.print();
	DBG("rotate:");
	m_rot.print();
	update(m_trans, m_rot, parent_dirty, frame_ctx);
}

static void upd(element_ns::element* e, void* data)
{
	if (!e || !data) {
		return;
	}

	auto* frame_ctx = static_cast<scene_ns::frame_context*>(data);
	e->update(*frame_ctx);
}

void element::update_all(frame_context& frame_ctx)
{
	this->update_tree(upd, &frame_ctx);
}

polygon* element::find(const poly_ptr_list_t& poly_list, const std::string& s) const
{
	for (const auto poly : poly_list) {
		if (poly && poly->get_name() == s) {
			return poly;
		}
	}

	return nullptr;
}

} // namespace element_ns
