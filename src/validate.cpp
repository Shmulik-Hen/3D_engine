#include <cmath>

// #define DEBUG_PRINTS
#include "validate.h"

namespace config_ns
{

static void check_range_real(const unit val, const unit min, const unit max, const std::string path, errors_t& errs)
{
	if (!std::isfinite(val) || val < min || val > max) {
		// clang-format off
		std::string err = path + set_precision(val) + SPC +
				  ERR_ILG + SPC +
				  LB + set_precision(min) + DASH +
				  set_precision(max) + RB;
		// clang-format on
		add_error(err, errs);
	}
}

static void check_range_int(const int val, const int min, const int max, const std::string path, errors_t& errs)
{
	if (val < min || val > max) {
		// clang-format off
		std::string err = path + std::to_string(val) + SPC +
				  ERR_ILG+ SPC +
				  LB + std::to_string(min) + DASH +
				  std::to_string(max) + RB;
		// clang-format on
		add_error(err, errs);
	}
}

static void check_range_uint(const uint32_t val, const uint32_t min, const uint32_t max, const std::string path, errors_t& errs)
{
	if (val < min || val > max) {
		// clang-format off
		std::string err = path + std::to_string(val) + SPC +
				  ERR_ILG+ SPC +
				  LB + std::to_string(min) + DASH +
				  std::to_string(max) + RB;
		// clang-format on
		add_error(err, errs);
	}
}

static void validate_attrib_values(const attrib& att, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	for (uint32_t i = 0; i < NUM_ATTRIBUTES; i++) {
		unit min_val = MIN_REAL_VAL;
		unit max_val = MAX_REAL_VAL;
		std::string_view name = get_att_name(i);

		if (name == STR_SCL) {
			min_val = MIN_SCALE;
			max_val = MAX_SCALE;
		}

		std::string str = path + name + PFX;
		unit val = att[i];
		DBG(str + set_precision(val));
		check_range_real(val, min_val, max_val, str, errs);
	}
}

static void validate_vector_values(const vector_3& vec, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		unit coord = vec.get(i);
		// clang-format off
		std::string str = path + STR_CORD +
				  LB + get_coord_name(i) + RB +
				  PFX;
		// clang-format on

		DBG(str + set_precision(coord));
		check_range_real(coord, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	}
}

static void validate_polygon_values(const polygon_def& poly, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + poly.name + QT);

	if (!is_valid_field_value(poly.name)) {
		add_error(name + SPC + ERR_INV + PFX + QT + poly.name + QT, errs);
	}

	DBG(path + JSON_STR_COLOR + PFX + std::to_string(poly.color_index));

	std::string err = path + JSON_STR_COLOR;
	check_range_uint(poly.color_index, 0, valids.max_color_idx, err + PFX, errs);

	DBG(path + JSON_STR_FORCE + PFX + BOOL(poly.force));

	std::string str = path + STR_NUM + SPC + JSON_STR_VECS;
	DBG(str + PFX + std::to_string(poly.vectors.size()));
	check_range_uint(poly.vectors.size(), MIN_VECTORS, MAX_VECTORS, str + PFX, errs);
	int i = 1;
	for (const auto& vector : poly.vectors) {
		// clang-format off
		std::string vec = path + STR_VEC + LB +
				  std::to_string(i) + RB;
		// clang-format on
		// DBG(vec);
		validate_vector_values(vector, vec + DOT, errs, valids);
		i++;
	}
}

static void validate_mesh_values(const mesh_def& mesh, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string msh = path + JSON_STR_NAME;
	DBG(msh + PFX + QT + mesh.name + QT);

	if (!is_valid_field_value(mesh.name)) {
		add_error(msh + SPC + ERR_INV + PFX + QT + mesh.name + QT, errs);
	}

	DBG(path + STR_NUM + SPC + JSON_STR_POLYS + PFX + std::to_string(mesh.polygons.size()));
	if (mesh.polygons.size() < MIN_POLYGONS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
		                  ERR_ATLST + SPC +
		                  std::to_string(MIN_POLYGONS) +
				  SPC + STR_POLY + SPC + JSON_STR_NAME;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 1;
		for (const auto& poly : mesh.polygons) {
			// clang-format off
			std::string pol = path + STR_POLY +
			                  LB + std::to_string(i) + RB;
			// clang-format on
			DBG(pol + PFX + QT + poly + QT);

			if (!is_valid_field_value(poly)) {
				add_error(pol + SPC + ERR_INV + PFX + QT + poly + QT, errs);
			}
			i++;
		}
	}
}

static void validate_lib_elem_values(const lib_elem_def& elem, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + elem.name + QT);

	if (!is_valid_field_value(elem.name)) {
		add_error(name + SPC + ERR_INV + PFX + QT + elem.name + QT, errs);
	}

	std::string mesh = path + JSON_STR_MESH;
	DBG(mesh + PFX + QT + elem.mesh + QT);

	if (!is_valid_field_value(elem.mesh)) {
		add_error(mesh + SPC + ERR_INV + PFX + QT + elem.mesh + QT, errs);
	}
}

static void validate_scene_elem_values(const scene_elem_def& elem, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + elem.name + QT);
	if (!is_valid_field_value(elem.name)) {
		add_error(name + SPC + ERR_INV + PFX + QT + elem.name + QT, errs);
	}

	std::string parent = path + JSON_STR_PAREN;
	if (elem.parent.has_value()) {
		DBG(parent + PFX + QT + elem.parent.value() + QT);

		if (!is_valid_field_value(elem.parent.value())) {
			add_error(parent + SPC + ERR_INV + PFX + QT + elem.parent.value() + QT, errs);
		}
	}
#ifdef DEBUG_PRINTS
	else {
		DBG(parent + SPC + ERR_NULL);
	}
#endif

	DBG(path + JSON_STR_ACT + PFX + BOOL(elem.active));

	std::string ref = path + JSON_STR_REF;
	if (elem.ref.has_value()) {
		DBG(ref + PFX + QT + elem.ref.value() + QT);

		if (!is_valid_field_value(elem.ref.value())) {
			add_error(ref + SPC + ERR_INV + PFX + QT + elem.ref.value() + QT, errs);
		}
	}

	std::string origin = path + JSON_STR_ORIG;
	if (elem.origin.has_value()) {
		validate_attrib_values(elem.origin.value(), origin + DOT, errs, valids);
	}

	std::string motion = path + JSON_STR_MOTN;
	if (elem.motion.has_value()) {
		validate_attrib_values(elem.motion.value(), motion + DOT, errs, valids);
	}
}

static void validate_environment(const environ_def& env, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	if (env.resolution.has_value()) {
		std::string str = path + JSON_STR_RES + PFX + STR_WID;
		check_range_uint(env.resolution.value().width, MIN_RES_WIDTH, MAX_RES_WIDTH, str + PFX, errs);
		str = path + JSON_STR_RES + PFX + STR_HGT;
		check_range_uint(env.resolution.value().height, MIN_RES_HEIGHT, MAX_RES_HEIGHT, str + PFX, errs);
		// clang-format off
		DBG(path + JSON_STR_RES + PFX +
		    std::to_string(env.resolution.value().width) + 'x' +
		    std::to_string(env.resolution.value().height));
		// clang-format on
	}

	if (env.color_idx.has_value()) {
		// clang-format off
		DBG(path + JSON_STR_COLOR + PFX +
		    std::to_string((uint32_t)env.color_idx.value()));

		std::string str = path + JSON_STR_COLOR;
		check_range_uint((uint32_t)env.color_idx.value(), 0,
				 (uint32_t)valids.max_color_idx, str + PFX, errs);
		// clang-format on
	}

	if (env.light_type.has_value()) {
		// clang-format off
		DBG(path + JSON_STR_LIGHT_TYPE + PFX +
		    std::to_string(env.light_type.value()));
		// clang-format on

		std::string str = path + JSON_STR_LIGHT_TYPE;
		check_range_uint(env.light_type.value(), __lt_min__, __lt_max__ - 1, str + PFX, errs);
	}

	if (env.light_direction.has_value()) {
		std::string str = path + JSON_STR_LIGHT_DIR;
		validate_vector_values(env.light_direction.value(), str + DOT, errs, valids);
	}

	if (env.light_position.has_value()) {
		std::string str = path + JSON_STR_LIGHT_POS;
		validate_vector_values(env.light_position.value(), str + DOT, errs, valids);
	}

	if (env.camera_position.has_value()) {
		std::string str = path + JSON_STR_CAM_POS;
		validate_vector_values(env.camera_position.value(), str + DOT, errs, valids);
	}

	if (env.focal_len.has_value()) {
		std::string str = path + JSON_STR_FOCAL_LEN;
		check_range_real(env.focal_len.value(), MIN_FOCAL_LEN, MAX_FOCAL_LEN, str + PFX, errs);
	}

	if (env.near_eps.has_value()) {
		std::string str = path + JSON_STR_NEAR_EPS;
		check_range_real(env.near_eps.value(), MIN_NEAR_EPS, MAX_NEAR_EPSN, str + PFX, errs);
	}
}

static void validate_runtime(const runtime_def& run, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	if (run.loops.has_value()) {
		// clang-format off
		DBG(path + JSON_STR_LOOPS + PFX +
		    std::to_string(run.loops.value()));
		// clang-format on

		std::string str = path + JSON_STR_LOOPS;
		check_range_int(run.loops.value(), MIN_LOOPS, MAX_LOOPS, str + PFX, errs);
	}

	if (run.loop_delay.has_value()) {
		// clang-format off
		DBG(path + JSON_STR_LOOP_DELAY + PFX +
		    std::to_string(run.loop_delay.value()));
		// clang-format on

		std::string str = path + JSON_STR_LOOP_DELAY;
		check_range_uint(run.loop_delay.value(), MIN_LOOP_DELAY, MAX_LOOP_DELAY, str + PFX, errs);
	}
}

static void validate_library(const library_def& lib, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	DBG(path + STR_NUM + SPC + JSON_STR_POLYS + PFX + std::to_string(lib.polygons.size()));
	if (lib.polygons.size() < MIN_POLYGONS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
				  std::to_string(MIN_POLYGONS) +
				  SPC + STR_POLY;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 1;
		for (const auto& polygon : lib.polygons) {
			// clang-format off
			std::string polygons = path + STR_POLY +
					       LB + std::to_string(i) + RB;
			// clang-format on
			validate_polygon_values(polygon, polygons + DOT, errs, valids);
			i++;
		}
	}

	DBG(path + STR_NUM + SPC + JSON_STR_MESHS + PFX + std::to_string(lib.meshes.size()));
	if (lib.meshes.size() < MIN_MESHES) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_MESHES) +
				  SPC + JSON_STR_MESH;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 1;
		for (const auto& mesh : lib.meshes) {
			// clang-format off
			std::string meshes = path + JSON_STR_MESH +
					     LB + std::to_string(i) + RB;
			// clang-format on
			validate_mesh_values(mesh, meshes + DOT, errs, valids);
			i++;
		}
	}

	DBG(path + STR_NUM + SPC + JSON_STR_ELEMS + PFX + std::to_string(lib.lib_elem_defs.size()));
	if (lib.lib_elem_defs.size() < MIN_LIB_ELEMS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_LIB_ELEMS) +
				  SPC + STR_ELEM;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 1;
		for (const auto& element : lib.lib_elem_defs) {
			// clang-format off
			std::string elems = path + STR_ELEM +
					    LB + std::to_string(i) + RB;
			// clang-format on
			validate_lib_elem_values(element, elems + DOT, errs, valids);
			i++;
		}
	}
}

static void validate_scene(const scene_def& scene, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + scene.name + QT);

	if (!is_valid_field_value(scene.name)) {
		add_error(name + SPC + ERR_INV + PFX + QT + scene.name + QT, errs);
	}

	DBG(path + STR_NUM + SPC + JSON_STR_ELEMS + PFX + std::to_string(scene.scene_elem_defs.size()));
	if (scene.scene_elem_defs.size() < MIN_SCN_ELEMS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_SCN_ELEMS) +
				  SPC + JSON_STR_ELEMS;
		// clang-format on
		add_error(err, errs);
	}

	int i = 1;
	for (const auto& elem : scene.scene_elem_defs) {
		// clang-format off
		std::string elems = path + STR_ELEM +
				    LB + std::to_string(i) + RB;
		// clang-format on
		validate_scene_elem_values(elem, elems + DOT, errs, valids);
		i++;
	}

	if (scene.env.has_value()) {
		std::string env = path + JSON_STR_ENV;
		validate_environment(scene.env.value(), env + DOT, errs, valids);
	}

	if (scene.run.has_value()) {
		std::string run = path + JSON_STR_RUN;
		validate_runtime(scene.run.value(), run + DOT, errs, valids);
	}
}

static void validate_values(const config_ns::AST& ast, const std::string path, errors_t& errs)
{
	std::string local_path = path + STR_VALS;

	DBG(local_path);

	std::string lib = local_path + PFX + JSON_STR_LIB;
	DBG(lib);

	validate_library(ast.library_defs, lib + DOT, errs, ast.valids);

	std::string scenes = local_path + PFX + JSON_STR_SCNS;
	DBG(scenes);

	// clang-format off
	DBG(scenes + PFX + STR_NUM + SPC + JSON_STR_SCNS + PFX +
	    std::to_string(ast.scene_defs.size()));
	// clang-format on

	if (ast.scene_defs.size() < MIN_SCENES) {
		// clang-format off
		std::string err = scenes + SPC + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
				  std::to_string(MIN_SCENES) +
				  SPC + STR_SCN;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 1;
		for (const auto& scene : ast.scene_defs) {
			// clang-format off
			std::string scn = scenes + DOT + STR_SCN +
					  LB + std::to_string(i) + RB;
			// clang-format on
			validate_scene(scene, scn + DOT, errs, ast.valids);
			i++;
		}
	}
}

static void validate_references(const config_ns::AST& ast, const std::string path, errors_t& errs)
{
	DBG(path + STR_REFS);

	std::string local_path = path + STR_REFS + PFX;
}

static void validate_hierarchy(const config_ns::AST& ast, const std::string path, errors_t& errs)
{
	DBG(path + STR_HIER);

	std::string local_path = path + STR_HIER;
}

bool validate_ast(const config_ns::AST& ast)
{
	DBG(STR_VAL);

	config_ns::errors_t errs;
	std::string path = STR_VAL + PFX;

	validate_values(ast, path, errs);
	if (errs.size()) {
		PRN(ERR_PFX + STR_VAL + SPC + ERR_DET + NL);
		print_errors(errs);
		return false;
	}

	validate_references(ast, path, errs);
	if (errs.size()) {
		print_errors(errs);
		return false;
	}

	validate_hierarchy(ast, path, errs);
	if (errs.size()) {
		print_errors(errs);
		return false;
	}

	return true;
}

} // namespace config_ns
