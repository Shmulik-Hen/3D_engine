#include <cmath>
#include <unordered_set>
#include <memory>

// #define DEBUG_PRINTS
#include "validate.h"

namespace config_ns
{

/* values validation */
static void check_range_real(const unit val,
                             const unit min,
                             const unit max,
                             const std::string& path,
                             errors_t& errs)
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

static void check_range_int(const int val,
                            const int min,
                            const int max,
                            const std::string& path,
                            errors_t& errs)
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

static void check_range_uint(const uint32_t val,
                             const uint32_t min,
                             const uint32_t max,
                             const std::string& path,
                             errors_t& errs)
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

static void validate_attrib_values(const attrib& att,
                                   const std::string& path,
                                   errors_t& errs,
                                   const valid_values_def& valid_values [[maybe_unused]])
{
	for (uint32_t i = 0; i < NUM_ATTRIBUTES; i++) {
		unit min_val = MIN_REAL_VAL;
		unit max_val = MAX_REAL_VAL;
		std::string_view name = get_att_name(i);

		if (name == STR_SCL) {
			min_val = MIN_SCALE;
			max_val = MAX_SCALE;
		}

		std::string str = path + DOT + name + PFX;
		unit val = att[i];
		DBG(str + set_precision(val));
		check_range_real(val, min_val, max_val, str, errs);
	}
}

static void validate_vector_values(const vector_3& vec,
                                   const std::string& path,
                                   errors_t& errs,
                                   const valid_values_def& valid_values [[maybe_unused]])
{
	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		unit coord = vec.get(i);
		// clang-format off
		std::string str = path + DOT + STR_CORD +
				  LB + get_coord_name(i) + RB +
				  PFX;
		// clang-format on

		DBG(str + set_precision(coord));
		check_range_real(coord,
		                 MIN_REAL_VAL,
		                 MAX_REAL_VAL,
		                 str,
		                 errs);
	}
}

static void validate_polygon_values(const parse_poly_def& poly,
                                    const std::string& path,
                                    errors_t& errs,
                                    const valid_values_def& valid_values [[maybe_unused]])
{
	std::string name = path + DOT + JSON_STR_NAME;
	DBG(name + PFX + QT + poly.name + QT);

	if (!is_valid_field_value(poly.name)) {
		// clang-format off
		add_error(name + SPC + ERR_INV + PFX +
			  QT + poly.name + QT, errs);
		// clang-format on
	}

	// clang-format off
	DBG(path + DOT + JSON_STR_COLOR + PFX +
	    std::to_string(poly.color_index));
	// clang-format on

	std::string err = path + DOT + JSON_STR_COLOR;
	check_range_uint(poly.color_index,
	                 0,
	                 valid_values.max_color_idx,
	                 err + PFX,
	                 errs);

	DBG(path + DOT + JSON_STR_FORCE + PFX + BOOL(poly.force));

	std::string str = path + PFX + STR_NUM + SPC + JSON_STR_VECS;
	DBG(str + PFX + std::to_string(poly.vectors.size()));

	check_range_uint(poly.vectors.size(),
	                 MIN_VECTORS,
	                 MAX_VECTORS,
	                 str + PFX,
	                 errs);

	int i = 0;
	for (const auto& vector : poly.vectors) {
		// clang-format off
		std::string vec = path + DOT + STR_VEC + LB +
				  std::to_string(++i) + RB;
		// clang-format on

		validate_vector_values(vector, vec, errs, valid_values);
	}
}

static void validate_mesh_values(const parse_mesh_def& mesh,
                                 const std::string& path,
                                 errors_t& errs,
                                 const valid_values_def& valid_values [[maybe_unused]])
{
	std::string msh = path + DOT + JSON_STR_NAME;
	DBG(msh + PFX + QT + mesh.name + QT);

	if (!is_valid_field_value(mesh.name)) {
		// clang-format off
		add_error(msh + SPC + ERR_INV + PFX +
			  QT + mesh.name + QT, errs);
		// clang-format on
	}

	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_POLYS +
	    PFX + std::to_string(mesh.polygons.size()));
	// clang-format on

	if (mesh.polygons.size() < MIN_POLYGONS) {
		// clang-format off
		std::string err = path + PFX + ERR_REQ + SPC +
		                  ERR_ATLST + SPC +
		                  std::to_string(MIN_POLYGONS) +
				  SPC + STR_POLY + SPC + JSON_STR_NAME;
		// clang-format on

		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& poly : mesh.polygons) {
			// clang-format off
			std::string pol = path + DOT + STR_POLY +
			                  LB + std::to_string(++i) + RB;
			DBG(pol + PFX + QT + poly + QT);
			// clang-format on

			if (!is_valid_field_value(poly)) {
				// clang-format off
				add_error(pol + SPC + ERR_INV + PFX +
					  QT + poly + QT, errs);
				// clang-format on
			}
		}
	}
}

static void validate_lib_elem_values(const parse_lib_elem_def& elem,
                                     const std::string& path,
                                     errors_t& errs,
                                     const valid_values_def& valid_values [[maybe_unused]])
{
	std::string name = path + DOT + JSON_STR_NAME;
	DBG(name + PFX + QT + elem.name + QT);

	if (!is_valid_field_value(elem.name)) {
		// clang-format off
		add_error(name + SPC + ERR_INV + PFX +
			  QT + elem.name + QT, errs);
		// clang-format on
	}

	std::string mesh = path + DOT + JSON_STR_MESH;
	DBG(mesh + PFX + QT + elem.mesh + QT);

	if (!is_valid_field_value(elem.mesh)) {
		// clang-format off
		add_error(mesh + SPC + ERR_INV + PFX +
			  QT + elem.mesh + QT, errs);
		// clang-format on
	}
}

static void validate_library_values(const parse_lib_def& lib,
                                    const std::string& path,
                                    errors_t& errs,
                                    const valid_values_def& valid_values [[maybe_unused]])
{
	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_POLYS +
	    PFX + std::to_string(lib.polygons.size()));
	// clang-format on

	if (lib.polygons.size() < MIN_POLYGONS) {
		// clang-format off
		std::string err = path + PFX + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
				  std::to_string(MIN_POLYGONS) +
				  SPC + STR_POLY;
		// clang-format on

		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& polygon : lib.polygons) {
			// clang-format off
			std::string polygons = path + DOT + STR_POLY +
					       LB + std::to_string(++i) + RB;
			// clang-format on

			validate_polygon_values(polygon, polygons, errs, valid_values);
		}
	}

	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_MESHS +
	    PFX + std::to_string(lib.meshes.size()));
	// clang-format on

	if (lib.meshes.size() < MIN_MESHES) {
		// clang-format off
		std::string err = path + PFX + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_MESHES) +
				  SPC + JSON_STR_MESH;
		// clang-format on

		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& mesh : lib.meshes) {
			// clang-format off
			std::string meshes = path + DOT + JSON_STR_MESH +
					     LB + std::to_string(++i) + RB;
			// clang-format on

			validate_mesh_values(mesh, meshes, errs, valid_values);
		}
	}

	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_ELEMS +
	    PFX + std::to_string(lib.lib_elem_defs.size()));
	// clang-format on

	if (lib.lib_elem_defs.size() < MIN_LIB_ELEMS) {
		// clang-format off
		std::string err = path + PFX + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_LIB_ELEMS) +
				  SPC + STR_ELEM;
		// clang-format on

		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& element : lib.lib_elem_defs) {
			// clang-format off
			std::string elems = path + DOT + STR_ELEM +
					    LB + std::to_string(++i) + RB;
			// clang-format on

			validate_lib_elem_values(element, elems, errs, valid_values);
		}
	}
}

static void validate_scene_elem_values(const parse_scn_elem_def& elem,
                                       const std::string& path,
                                       errors_t& errs,
                                       const valid_values_def& valid_values [[maybe_unused]])
{
	std::string name = path + DOT + JSON_STR_NAME;
	DBG(name + PFX + QT + elem.name + QT);
	if (!is_valid_field_value(elem.name)) {
		// clang-format off
		add_error(name + SPC + ERR_INV + PFX +
			  QT + elem.name + QT, errs);
		// clang-format on
	}

	std::string parent = path + DOT + JSON_STR_PAREN;
	if (elem.parent.has_value()) {
		DBG(parent + PFX + QT + elem.parent.value() + QT);

		if (!is_valid_field_value(elem.parent.value())) {
			// clang-format off
			add_error(parent + SPC + ERR_INV + PFX +
				  QT + elem.parent.value() + QT, errs);
			// clang-format on
		}
	}
	else {
		DBG(parent + SPC + ERR_NULL);
	}

	DBG(path + DOT + JSON_STR_ACT + PFX + BOOL(elem.active));

	std::string ref = path + DOT + JSON_STR_REF;
	if (elem.ref.has_value()) {
		DBG(ref + PFX + QT + elem.ref.value() + QT);

		if (!is_valid_field_value(elem.ref.value())) {
			// clang-format off
			add_error(ref + SPC + ERR_INV + PFX +
				  QT + elem.ref.value() + QT, errs);
			// clang-format on
		}
	}
	else {
		DBG(ref + SPC + ERR_NULL);
	}

	std::string origin = path + DOT + JSON_STR_ORIG;
	if (elem.origin.has_value()) {
		validate_attrib_values(elem.origin.value(), origin, errs, valid_values);
	}

	std::string motion = path + DOT + JSON_STR_MOTN;
	if (elem.motion.has_value()) {
		validate_attrib_values(elem.motion.value(), motion, errs, valid_values);
	}
}

static void validate_environment_values(const parse_env_def& env,
                                        const std::string& path,
                                        errors_t& errs,
                                        const valid_values_def& valid_values [[maybe_unused]])
{
	if (env.resolution.has_value()) {
		std::string str = path + JSON_STR_RES + PFX + STR_WID;
		check_range_uint(env.resolution.value().width,
		                 MIN_RES_WIDTH,
		                 MAX_RES_WIDTH,
		                 str + PFX,
		                 errs);

		str = path + JSON_STR_RES + PFX + STR_HGT;
		check_range_uint(env.resolution.value().height,
		                 MIN_RES_HEIGHT,
		                 MAX_RES_HEIGHT,
		                 str + PFX,
		                 errs);
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
		// clang-format on

		std::string str = path + JSON_STR_COLOR;
		check_range_uint((uint32_t)env.color_idx.value(),
		                 0,
		                 (uint32_t)valid_values.max_color_idx,
		                 str + PFX,
		                 errs);
	}

	if (env.light_type.has_value()) {
		// clang-format off
		DBG(path + JSON_STR_LIGHT_TYPE + PFX +
		    std::to_string(env.light_type.value()));
		// clang-format on

		std::string str = path + JSON_STR_LIGHT_TYPE;
		check_range_uint(env.light_type.value(),
		                 __lt_min__,
		                 __lt_max__ - 1,
		                 str + PFX,
		                 errs);
	}

	if (env.light_direction.has_value()) {
		std::string str = path + JSON_STR_LIGHT_DIR;
		validate_vector_values(env.light_direction.value(),
		                       str,
		                       errs,
		                       valid_values);
	}

	if (env.light_position.has_value()) {
		std::string str = path + JSON_STR_LIGHT_POS;
		validate_vector_values(env.light_position.value(),
		                       str,
		                       errs,
		                       valid_values);
	}

	if (env.camera_position.has_value()) {
		std::string str = path + JSON_STR_CAM_POS;
		validate_vector_values(env.camera_position.value(),
		                       str,
		                       errs,
		                       valid_values);
	}

	if (env.focal_len.has_value()) {
		std::string str = path + JSON_STR_FOCAL_LEN;
		check_range_real(env.focal_len.value(),
		                 MIN_FOCAL_LEN,
		                 MAX_FOCAL_LEN,
		                 str + PFX,
		                 errs);
	}

	if (env.near_eps.has_value()) {
		std::string str = path + JSON_STR_NEAR_EPS;
		check_range_real(env.near_eps.value(),
		                 MIN_NEAR_EPS,
		                 MAX_NEAR_EPSN,
		                 str + PFX,
		                 errs);
	}
}

static void validate_runtime_values(const parse_run_def& run,
                                    const std::string& path,
                                    errors_t& errs,
                                    const valid_values_def& valid_values [[maybe_unused]])
{
	if (run.loops.has_value()) {
		// clang-format off
		DBG(path + JSON_STR_LOOPS + PFX +
		    std::to_string(run.loops.value()));
		// clang-format on

		std::string str = path + JSON_STR_LOOPS;
		check_range_int(run.loops.value(),
		                MIN_LOOPS,
		                MAX_LOOPS,
		                str + PFX,
		                errs);
	}

	if (run.loop_delay.has_value()) {
		// clang-format off
		DBG(path + JSON_STR_LOOP_DELAY + PFX +
		    std::to_string(run.loop_delay.value()));
		// clang-format on

		std::string str = path + JSON_STR_LOOP_DELAY;
		check_range_uint(run.loop_delay.value(),
		                 MIN_LOOP_DELAY,
		                 MAX_LOOP_DELAY,
		                 str + PFX,
		                 errs);
	}
}

static void validate_scene_values(const parse_scn_def& scene,
                                  const std::string& path,
                                  errors_t& errs,
                                  const valid_values_def& valid_values [[maybe_unused]])
{
	std::string name = path + DOT + JSON_STR_NAME;
	DBG(name + PFX + QT + scene.name + QT);

	if (!is_valid_field_value(scene.name)) {
		// clang-format off
		add_error(name + SPC + ERR_INV + PFX +
			  QT + scene.name + QT, errs);
		// clang-format on
	}

	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_ELEMS + PFX +
	    std::to_string(scene.scene_elem_defs.size()));
	// clang-format on

	if (scene.scene_elem_defs.size() < MIN_SCN_ELEMS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_SCN_ELEMS) +
				  SPC + JSON_STR_ELEMS;
		// clang-format on

		add_error(err, errs);
	}

	int i = 0;
	for (const auto& elem : scene.scene_elem_defs) {
		// clang-format off
		std::string elems = path + DOT + STR_ELEM +
				    LB + std::to_string(++i) + RB;
		// clang-format on
		validate_scene_elem_values(elem, elems, errs, valid_values);
	}

	if (scene.env.has_value()) {
		std::string env = path + DOT + JSON_STR_ENV;
		validate_environment_values(scene.env.value(),
		                            env + DOT,
		                            errs,
		                            valid_values);
	}

	if (scene.run.has_value()) {
		std::string run = path + DOT + JSON_STR_RUN;
		validate_runtime_values(scene.run.value(),
		                        run + DOT,
		                        errs,
		                        valid_values);
	}
}

static void validate_values(const AST& ast,
                            const std::string& path,
                            errors_t& errs)
{
	std::string local_path = path + STR_VALS;

	DBG(local_path);

	std::string lib = local_path + PFX + JSON_STR_LIB;
	DBG(lib);

	validate_library_values(ast.library_defs,
	                        lib,
	                        errs,
	                        ast.valid_values);

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
		int i = 0;
		for (const auto& scene : ast.scene_defs) {
			// clang-format off
			std::string scn = scenes + DOT + STR_SCN +
					  LB + std::to_string(++i) + RB;
			// clang-format on

			validate_scene_values(scene, scn, errs, ast.valid_values);
		}
	}
}
/* values validation end*/

/* references validation*/
/* collect */
static void report_duplicates(const std::string& path,
                              const std::string& type,
                              const std::string& name,
                              errors_t& errs)
{
	// clang-format off
	add_error(path + DOT + type + PFX + ERR_DUP +
		  SPC + STR_DET + PFX + QT + name + QT, errs);
	// clang-format on
}

static void validate_collect_lib_poly_refs(const parse_lib_def& lib_defs,
                                           const std::string& path,
                                           valid_poly_refs& vld_poly_refs,
                                           errors_t& errs)
{
	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_POLYS +
	    PFX + std::to_string(lib_defs.polygons.size()));
	// clang-format on

	int i = 0;
	for (const auto& poly : lib_defs.polygons) {
		std::string str = path + LB + std::to_string(++i) + RB;
		DBG(str + DOT + JSON_STR_NAME + PFX + QT + poly.name + QT);

		auto [it, inserted] = vld_poly_refs.poly_keys.insert(poly.name);
		if (!inserted) {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC +
			    QT + poly.name + QT + SPC + ERR_FAIL);
			// clang-format on

			report_duplicates(path, JSON_STR_POLYS, poly.name, errs);
		}
		else {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC +
			    QT + poly.name + QT + SPC + STR_SUCC);
			// clang-format on

			valid_poly_ref& poly_ref = vld_poly_refs.poly_refs.emplace_back();
			poly_ref.poly_def_name = &poly.name;
			poly_ref.poly_def_ptr = &poly;
		}
	}
}

static void validate_collect_lib_mesh_poly_refs(const parse_mesh_def& mesh_def,
                                                const std::string& path,
                                                valid_mesh_ref& vld_mesh_ref,
                                                errors_t& errs)
{
	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_POLYS +
	    PFX + std::to_string(mesh_def.polygons.size()));
	// clang-format on

	std::string local_path = path + DOT + JSON_STR_POLYS;

	int i = 0;
	for (const auto& poly : mesh_def.polygons) {
		// clang-format off
		std::string str = local_path + LB + std::to_string(++i) + RB;
		DBG(str + DOT + JSON_STR_NAME + PFX + QT + poly + QT);
		// clang-format on

		auto [it, inserted] = vld_mesh_ref.mesh_poly_keys.insert(poly);
		if (!inserted) {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC + QT + poly + QT +
			    SPC + ERR_FAIL);
			// clang-format on

			report_duplicates(path, JSON_STR_POLYS, poly, errs);
		}
		else {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC + QT + poly + QT +
			    SPC + STR_SUCC);
			// clang-format on
		}
	}
}

static void validate_collect_lib_mesh_refs(const parse_lib_def& lib_defs,
                                           const std::string& path,
                                           valid_mesh_refs& vld_mesh_refs,
                                           errors_t& errs)
{
	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_MESHS +
	    PFX + std::to_string(lib_defs.meshes.size()));
	// clang-format on

	int i = 0;
	for (const auto& mesh : lib_defs.meshes) {
		std::string str = path + LB + std::to_string(++i) + RB;
		// clang-format off
		DBG(str + DOT +  JSON_STR_NAME + PFX +
		    QT + mesh.name + QT);
		// clang-format on

		auto [it, inserted] = vld_mesh_refs.mesh_keys.insert(mesh.name);
		if (!inserted) {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC + QT + mesh.name + QT +
			    SPC + ERR_FAIL);
			// clang-format on

			report_duplicates(path, JSON_STR_MESH, mesh.name, errs);
		}
		else {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC + QT + mesh.name + QT +
			    SPC + STR_SUCC);
			// clang-format on

			valid_mesh_ref& mesh_ref = vld_mesh_refs.mesh_refs.emplace_back();
			mesh_ref.mesh_def_name = &mesh.name;
			mesh_ref.mesh_def_ptr = &mesh;
			validate_collect_lib_mesh_poly_refs(mesh, str, mesh_ref, errs);
		}
	}
}

static void validate_collect_lib_elem_refs(const parse_lib_def& lib_defs,
                                           const std::string& path,
                                           valid_lib_elem_refs& vld_elem_refs,
                                           errors_t& errs)
{
	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_ELEMS +
	    PFX + std::to_string(lib_defs.lib_elem_defs.size()));
	// clang-format on

	int i = 0;
	for (const auto& lib_elem : lib_defs.lib_elem_defs) {
		std::string str = path + LB + std::to_string(++i) + RB;
		// clang-format off
		DBG(str + DOT +  JSON_STR_NAME + PFX +
		    QT + lib_elem.name + QT);
		// clang-format on

		auto [it, inserted] = vld_elem_refs.elem_keys.insert(lib_elem.name);
		if (!inserted) {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC + QT + lib_elem.name + QT +
			    SPC + ERR_FAIL);
			// clang-format on

			report_duplicates(path, JSON_STR_ELEMS, lib_elem.name, errs);
		}
		else {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC +
			    QT + lib_elem.name + QT + SPC + STR_SUCC);
			// clang-format on

			valid_lib_elem_ref& elem_ref = vld_elem_refs.elem_refs.emplace_back();
			elem_ref.elem_def_name = &lib_elem.name;
			elem_ref.elem_def_mesh = &lib_elem.mesh;
			elem_ref.elem_def_ptr = &lib_elem;
		}
	}
}

static void validate_collect_lib_refs(const parse_lib_def& lib_defs,
                                      const std::string& path,
                                      valid_library_refs& lib_refs,
                                      errors_t& errs)
{
	validate_collect_lib_poly_refs(lib_defs,
	                               path + DOT + JSON_STR_POLYS,
	                               lib_refs.lib_poly_refs,
	                               errs);

	validate_collect_lib_mesh_refs(lib_defs,
	                               path + DOT + JSON_STR_MESHS,
	                               lib_refs.lib_mesh_refs,
	                               errs);

	validate_collect_lib_elem_refs(lib_defs,
	                               path + DOT + JSON_STR_ELEMS,
	                               lib_refs.lib_elem_refs,
	                               errs);
}

static void validate_collect_scn_elem_ref(const parse_scn_elem_def& scn_elem_def,
                                          const std::string& path [[maybe_unused]],
                                          valid_scn_elem_ref& vld_scn_elem_ref)
{
	vld_scn_elem_ref.scn_elem_def_ptr = &scn_elem_def;
	vld_scn_elem_ref.scn_elem_def_name = &scn_elem_def.name;

	// clang-format off
	DBG(path + DOT + JSON_STR_NAME + PFX +
		QT + *vld_scn_elem_ref.scn_elem_def_name + QT);
	// clang-format on

	if (scn_elem_def.parent.has_value()) {
		vld_scn_elem_ref.scn_elem_def_parent = &scn_elem_def.parent.value();
		// clang-format off
		DBG(path + DOT + JSON_STR_PAREN + PFX +
		    QT + *vld_scn_elem_ref.scn_elem_def_parent + QT);
		// clang-format on
	}
	else {
		DBG(path + DOT + JSON_STR_PAREN + SPC + ERR_NULL);
	}

	if (scn_elem_def.ref.has_value()) {
		vld_scn_elem_ref.scn_elem_def_ref = &scn_elem_def.ref.value();
		// clang-format off
		DBG(path + DOT + JSON_STR_REF + PFX +
		    QT + *vld_scn_elem_ref.scn_elem_def_ref + QT);
		// clang-format on
	}
	else {
		DBG(path + DOT + JSON_STR_REF + SPC + ERR_NULL);
	}
}

static void validate_collect_scene_ref(const parse_scn_def& scn_def,
                                       const std::string& path,
                                       valid_scene_ref& vld_scn_ref,
                                       errors_t& errs)
{
	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_ELEMS +
	    PFX + std::to_string(scn_def.scene_elem_defs.size()));
	// clang-format on

	vld_scn_ref.scn_def_ptr = &scn_def;
	vld_scn_ref.scn_def_name = &scn_def.name;

	std::string local_path = path + DOT + JSON_STR_ELEMS;

	int i = 0;
	for (const auto& scn_elem : scn_def.scene_elem_defs) {
		// clang-format off
		std::string str = local_path + LB + std::to_string(++i) + RB;
		// clang-format on

		auto [it, inserted] = vld_scn_ref.scn_elem_keys.insert(scn_elem.name);
		if (!inserted) {
			// clang-format off
			DBG(str + PFX + STR_IKEY +
			    SPC + QT + scn_elem.name + QT + SPC + ERR_FAIL);
			// clang-format on

			report_duplicates(path, JSON_STR_ELEMS, scn_elem.name, errs);
		}
		else {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC +
			    QT + scn_elem.name + QT + SPC + STR_SUCC);
			// clang-format on

			valid_scn_elem_ref& elem_ref = vld_scn_ref.scn_elem_refs.emplace_back();
			validate_collect_scn_elem_ref(scn_elem, str, elem_ref);
		}
	}
}

static void validate_collect_scene_refs(AST& ast,
                                        const std::string& path,
                                        valid_scene_refs& vld_scn_refs,
                                        errors_t& errs)
{
	// clang-format off
	DBG(path + PFX + STR_NUM + SPC + JSON_STR_SCNS +
	    PFX + std::to_string(ast.scene_defs.size()));
	// clang-format on

	int i = 0;
	for (const auto& scn_def : ast.scene_defs) {
		// clang-format off
		std::string str = path + DOT + STR_SCN +
				  LB + std::to_string(++i) + RB;
		DBG(str + DOT +  JSON_STR_NAME + PFX +
		    QT + scn_def.name + QT);
		// clang-format on

		auto [it, inserted] = vld_scn_refs.scene_ref_keys.insert(scn_def.name);
		if (!inserted) {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC +
			    QT + scn_def.name + QT + SPC + ERR_FAIL);
			// clang-format on

			std::string s(STR_SCN);
			report_duplicates(path, s, scn_def.name, errs);
		}
		else {
			// clang-format off
			DBG(str + PFX + STR_IKEY + SPC +
			    QT + scn_def.name + QT + SPC + STR_SUCC);
			// clang-format on

			valid_scene_ref& scn_ref = vld_scn_refs.scene_ref_refs.emplace_back();
			validate_collect_scene_ref(scn_def, str, scn_ref, errs);
		}
	}
}

static void validate_collect_references(AST& ast,
                                        const std::string& path,
                                        errors_t& errs)
{
	std::string local_path = path + PFX + "collect";

	std::string lib_path = local_path + PFX + JSON_STR_LIB;
	DBG(lib_path);
	validate_collect_lib_refs(ast.library_defs,
	                          lib_path,
	                          ast.valid_references.valid_lib_refs,
	                          errs);
	if (errs.size()) {
		return;
	}

	std::string scenes_path = local_path + PFX + JSON_STR_SCNS;
	DBG(scenes_path);
	validate_collect_scene_refs(ast,
	                            scenes_path,
	                            ast.valid_references.valid_scn_refs,
	                            errs);
}
/* collect end */

/* resolve */
static valid_poly_ref* find_lib_poly_ref(const valid_library_refs& lib_refs,
                                         const std::string& name)
{
	for (auto& poly_ref : lib_refs.lib_poly_refs.poly_refs) {
		if (poly_ref.poly_def_name &&
		    *poly_ref.poly_def_name == name) {
			return const_cast<valid_poly_ref*>(&poly_ref);
		}
	}

	return nullptr;
}

static valid_mesh_ref* find_lib_mesh_ref(const valid_library_refs& lib_refs,
                                         const std::string& name)
{
	for (auto& mesh_ref : lib_refs.lib_mesh_refs.mesh_refs) {
		if (mesh_ref.mesh_def_name &&
		    *mesh_ref.mesh_def_name == name) {
			return const_cast<valid_mesh_ref*>(&mesh_ref);
		}
	}

	return nullptr;
}

static valid_lib_elem_ref* find_lib_elem_ref(const valid_library_refs& lib_refs,
                                             const std::string& name)
{
	for (auto& elem_ref : lib_refs.lib_elem_refs.elem_refs) {
		if (elem_ref.elem_def_name &&
		    *elem_ref.elem_def_name == name) {
			return const_cast<valid_lib_elem_ref*>(&elem_ref);
		}
	}

	return nullptr;
}

static valid_scn_elem_ref* find_scn_elem_ref(const valid_scene_ref& scn_ref,
                                             const std::string& name)
{
	for (auto& elem_ref : scn_ref.scn_elem_refs) {
		if (elem_ref.scn_elem_def_name &&
		    *elem_ref.scn_elem_def_name == name) {
			return const_cast<valid_scn_elem_ref*>(&elem_ref);
		}
	}

	return nullptr;
}

static valid_scene_ref* find_scn_ref(const valid_scene_refs& scn_refs,
                                     const std::string& name)
{
	for (auto& scn_ref : scn_refs.scene_ref_refs) {
		if (scn_ref.scn_def_name &&
		    *scn_ref.scn_def_name == name) {
			return const_cast<valid_scene_ref*>(&scn_ref);
		}
	}

	return nullptr;
}

static void check_array_mismatch(const std::string& path,
                                 size_t keys,
                                 size_t refs)
{
	if (!refs || !keys || refs != keys) {
		// clang-format off
		std::string err = ERR_INTR + PFX + path + PFX + STR_ARR +
				  SPC + ERR_MISM +
				  LP + std::to_string(refs) + SEP +
				  std::to_string(keys) + RP;
		// clang-format on

		sys_error(err.c_str());
	}
}

static void validate_resolve_lib_poly_refs(const valid_library_refs& lib_refs,
                                           const std::string& path,
                                           errors_t& errs [[maybe_unused]])
{
	const auto& poly_keys = lib_refs.lib_poly_refs.poly_keys;
	size_t refs_size = lib_refs.lib_poly_refs.poly_refs.size();
	size_t keys_size = poly_keys.size();

	// clang-format off
	DBG(path + PFX + "poly_refs" + SPC + STR_SIZE +
	    PFX + std::to_string(refs_size));
	DBG(path + PFX + "poly_keys" + SPC + STR_SIZE +
	    PFX + std::to_string(keys_size));
	// clang-format on

	// sanity
	check_array_mismatch(path, keys_size, refs_size);
}

static void validate_resolve_lib_mesh_poly_refs(const valid_library_refs& lib_refs,
                                                const std::string& path,
                                                valid_mesh_ref& vld_mesh_ref,
                                                errors_t& errs)
{
	const auto& poly_keys = vld_mesh_ref.mesh_poly_keys;

	int i = 0;
	for (auto& poly_key : poly_keys) {
		std::string str = path + LB + std::to_string(++i) + RB;
		valid_poly_ref* poly_ref =
			find_lib_poly_ref(lib_refs, poly_key);
		if (!poly_ref) {
			std::string err = str + SPC + QT + poly_key + QT + SPC + ERR_MISS;
			DBG(err);
			add_error(err, errs);
		}
		else {
			DBG(str + SPC + QT + poly_key + QT + SPC + STR_FOUND);
			vld_mesh_ref.mesh_poly_refs.push_back(poly_ref);
		}
	}

	size_t refs_size = vld_mesh_ref.mesh_poly_refs.size();
	size_t keys_size = poly_keys.size();

	// clang-format off
	DBG(path + PFX + "mesh_poly_refs" + SPC + STR_SIZE +
	    PFX + std::to_string(refs_size));
	DBG(path + PFX + "mesh_poly_keys" + SPC + STR_SIZE +
	    PFX + std::to_string(keys_size));
	// clang-format on

	// sanity
	check_array_mismatch(path, keys_size, refs_size);
}

static void validate_resolve_lib_mesh_refs(const valid_library_refs& lib_refs,
                                           const std::string& path,
                                           errors_t& errs)
{
	const auto& lib_mesh_refs = lib_refs.lib_mesh_refs;
	const auto& lib_mesh_keys = lib_mesh_refs.mesh_keys;
	size_t refs_size = lib_mesh_refs.mesh_refs.size();
	size_t keys_size = lib_mesh_keys.size();

	// clang-format off
	DBG(path + PFX + "mesh_refs" + SPC + STR_SIZE +
	    PFX + std::to_string(refs_size));
	DBG(path + PFX + "mesh_keys" + SPC + STR_SIZE +
	    PFX + std::to_string(keys_size));
	// clang-format on

	// sanity
	check_array_mismatch(path, keys_size, refs_size);

	int i = 0;
	for (auto& mesh_key : lib_mesh_keys) {
		std::string str = path + LB + std::to_string(++i) + RB;

		valid_mesh_ref* mesh_ref = find_lib_mesh_ref(lib_refs, mesh_key);
		if (!mesh_ref) {
			std::string err = str + SPC + QT + mesh_key + QT + SPC + ERR_MISS;
			DBG(err);
			add_error(err, errs);
		}
		else {
			DBG(str + SPC + QT + mesh_key + QT + SPC + STR_FOUND);
			validate_resolve_lib_mesh_poly_refs(lib_refs,
			                                    str + DOT + JSON_STR_POLYS,
			                                    *mesh_ref,
			                                    errs);
		}
	}
}

static void validate_resolve_lib_elem_mesh_ref(const valid_library_refs& lib_refs,
                                               const std::string& path,
                                               valid_lib_elem_ref& vld_elem_ref,
                                               errors_t& errs)
{
	const std::string mesh_key = *vld_elem_ref.elem_def_mesh;
	valid_mesh_ref* mesh_ref = find_lib_mesh_ref(lib_refs, mesh_key);
	if (!mesh_ref) {
		// clang-format off
		std::string err = path + DOT + JSON_STR_MESH +
				  SPC + QT + mesh_key + QT + SPC + ERR_MISS;
		// clang-format on

		DBG(err);
		add_error(err, errs);
	}
	else {
		// clang-format off
		DBG(path + DOT + JSON_STR_MESH + SPC +
		    QT + mesh_key + QT + SPC + STR_FOUND);
		// clang-format on

		vld_elem_ref.elem_mesh_ref = mesh_ref;
	}
}

static void validate_resolve_lib_elem_refs(const valid_library_refs& lib_refs,
                                           const std::string& path,
                                           errors_t& errs)
{
	const auto& lib_elem_refs = lib_refs.lib_elem_refs;
	const auto& lib_elem_keys = lib_elem_refs.elem_keys;
	size_t refs_size = lib_elem_refs.elem_refs.size();
	size_t keys_size = lib_elem_keys.size();

	// clang-format off
	DBG(path + PFX + "lib_elem_refs" + SPC + STR_SIZE +
	    PFX + std::to_string(refs_size));
	DBG(path + PFX + "lib_elem_keys" + SPC + STR_SIZE +
	    PFX + std::to_string(keys_size));
	// clang-format on

	// sanity
	check_array_mismatch(path, keys_size, refs_size);

	int i = 0;
	for (auto& lib_elem_key : lib_elem_keys) {
		std::string str = path + LB + std::to_string(++i) + RB;

		valid_lib_elem_ref* elem_ref = find_lib_elem_ref(lib_refs, lib_elem_key);
		if (!elem_ref) {
			// clang-format off
			std::string err = str + DOT + JSON_STR_NAME + SPC +
					  QT + lib_elem_key + QT + SPC + ERR_MISS;
			// clang-format on

			DBG(err);
			add_error(err, errs);
		}
		else {
			// clang-format off
			DBG(str + DOT + JSON_STR_NAME + SPC +
			    QT + lib_elem_key + QT + SPC + STR_FOUND);
			// clang-format on

			validate_resolve_lib_elem_mesh_ref(lib_refs, str, *elem_ref, errs);
		}
	}
}

static void validate_resolve_lib_refs(const std::string& path,
                                      valid_library_refs& lib_refs,
                                      errors_t& errs)
{
	validate_resolve_lib_poly_refs(lib_refs,
	                               path + DOT + JSON_STR_POLYS,
	                               errs);

	validate_resolve_lib_mesh_refs(lib_refs,
	                               path + DOT + JSON_STR_MESHS,
	                               errs);

	validate_resolve_lib_elem_refs(lib_refs,
	                               path + DOT + JSON_STR_ELEMS,
	                               errs);
}

static void validate_resolve_scene_elem_ref(const valid_library_refs& lib_refs,
                                            const std::string& path,
                                            valid_scn_elem_ref& scn_elem_ref,
                                            errors_t& errs)
{
	if (scn_elem_ref.scn_elem_def_ref) {
		std::string lib_elem_name = *scn_elem_ref.scn_elem_def_ref;
		valid_lib_elem_ref* lib_elem_ref = find_lib_elem_ref(lib_refs, lib_elem_name);
		if (!lib_elem_ref) {
			// clang-format off
			std::string err = path + DOT + JSON_STR_REF + SPC +
					  QT + lib_elem_name + QT + SPC + ERR_MISS;
			// clang-format on

			DBG(err);
			add_error(err, errs);
		}
		else {
			// clang-format off
			DBG(path + DOT + JSON_STR_REF + SPC +
			    QT + lib_elem_name + QT + SPC + STR_FOUND);
			// clang-format on

			scn_elem_ref.lib_elem_ref = lib_elem_ref;
		}
	}
}

static void validate_resolve_scene_ref(const valid_library_refs& lib_refs,
                                       const std::string& path,
                                       valid_scene_ref& vld_scn_ref,
                                       errors_t& errs)
{
	const auto& scn_elem_keys = vld_scn_ref.scn_elem_keys;

	size_t refs_size = vld_scn_ref.scn_elem_refs.size();
	size_t keys_size = scn_elem_keys.size();

	// clang-format off
	DBG(path + DOT + JSON_STR_NAME + PFX +
	    QT + *vld_scn_ref.scn_def_name + QT);
	DBG(path + PFX + "scn_elem_refs" + SPC +
	    STR_SIZE + PFX + std::to_string(refs_size));
	DBG(path + PFX + "scn_elem_keys" + SPC +
	    STR_SIZE + PFX + std::to_string(keys_size));
	// clang-format on

	std::string local_path = path + DOT + JSON_STR_ELEMS;
	// sanity
	check_array_mismatch(local_path, keys_size, refs_size);

	int i = 0;
	for (auto& scn_elem_key : scn_elem_keys) {
		std::string str = local_path + LB + std::to_string(++i) + RB;

		valid_scn_elem_ref* scn_elem_ref =
			find_scn_elem_ref(vld_scn_ref, scn_elem_key);
		if (!scn_elem_ref) {
			// clang-format off
			std::string err = str + DOT + JSON_STR_NAME +
					  SPC + QT + scn_elem_key + QT + SPC + ERR_MISS;
			// clang-format on

			DBG(err);
			add_error(err, errs);
		}
		else {
			// clang-format off
			DBG(str + DOT + JSON_STR_NAME + SPC +
			    QT + scn_elem_key + QT + SPC + STR_FOUND);
			// clang-format on

			if (scn_elem_ref->scn_elem_def_parent) {
				std::string parent_name = *scn_elem_ref->scn_elem_def_parent;
				valid_scn_elem_ref* scn_elem =
					find_scn_elem_ref(vld_scn_ref, parent_name);
				if (!scn_elem) {
					// clang-format off
					std::string err = str + DOT + JSON_STR_PAREN + SPC +
							  QT + parent_name + QT + SPC + ERR_MISS;
					// clang-format on

					DBG(err);
					add_error(err, errs);
				}
				else {
					// clang-format off
					DBG(str + DOT + JSON_STR_PAREN + SPC +
					    QT + parent_name + QT + SPC + STR_FOUND);
					// clang-format on

					scn_elem_ref->scn_parent_ref = scn_elem;
				}
			}

			validate_resolve_scene_elem_ref(lib_refs, str, *scn_elem_ref, errs);
		}
	}
}

static void validate_resolve_scene_refs(const valid_library_refs& lib_refs,
                                        const std::string& path,
                                        valid_scene_refs& scn_refs,
                                        errors_t& errs)
{
	const auto& scn_keys = scn_refs.scene_ref_keys;

	size_t refs_size = scn_refs.scene_ref_refs.size();
	size_t keys_size = scn_keys.size();

	// clang-format off
	DBG(path + PFX + "scenes_refs" + SPC +
	    STR_SIZE + PFX + std::to_string(refs_size));
	DBG(path + PFX + "scenes_keys" + SPC +
	    STR_SIZE + PFX + std::to_string(keys_size));
	// clang-format on

	// sanity
	check_array_mismatch(path, keys_size, refs_size);

	int i = 0;
	for (auto& scn_key : scn_keys) {
		// clang-format off
		std::string str = path + DOT + STR_SCN +
				  LB + std::to_string(++i) + RB;
		// clang-format on

		valid_scene_ref* scene_ref = find_scn_ref(scn_refs, scn_key);
		if (!scene_ref) {
			// clang-format off
			std::string err = str + SPC + QT +
					  scn_key + QT + SPC + ERR_MISS;
			// clang-format on

			DBG(err);
			add_error(err, errs);
		}
		else {
			// clang-format off
			DBG(str + SPC + QT + scn_key + QT +
			    SPC + STR_FOUND);
			// clang-format on

			validate_resolve_scene_ref(lib_refs, str, *scene_ref, errs);
		}
	}
}

static void validate_resolve_references(AST& ast,
                                        const std::string& path,
                                        errors_t& errs)
{
	std::string lib_path = path + PFX + "resolve" + PFX + JSON_STR_LIB;
	DBG(lib_path);

	validate_resolve_lib_refs(lib_path,
	                          ast.valid_references.valid_lib_refs,
	                          errs);
	if (errs.size()) {
		return;
	}

	std::string scenes_path = path + PFX + "resolve" + PFX + JSON_STR_SCNS;
	DBG(scenes_path);
	validate_resolve_scene_refs(ast.valid_references.valid_lib_refs,
	                            scenes_path,
	                            ast.valid_references.valid_scn_refs,
	                            errs);
}

/* resolve end */

static void validate_references(AST& ast,
                                const std::string& path,
                                errors_t& errs)
{
	std::string local_path = path + STR_REFS;
	DBG(local_path);

	validate_collect_references(ast, local_path, errs);
	if (errs.size()) {
		return;
	}

	validate_resolve_references(ast, local_path, errs);
}

/* references validation end */

/* hierarchy validation */
static void validate_hierarchy_check_roots(const valid_scene_ref& scn_ref,
                                           const std::string& path,
                                           errors_t& errs)
{
	DBG(path + PFX + QT + *scn_ref.scn_def_name + QT);

	std::vector<std::string> parent_names;
	parent_names.clear();

	size_t prev_errs_size = errs.size();
	int roots = 0, i = 0;
	for (const auto& elem_ref : scn_ref.scn_elem_refs) {
		// clang-format off
		std::string elem_path = path + DOT + JSON_STR_ELEMS +
					LB + std::to_string(++i) + RB;
		// clang-format on

		validate_key_t elem_keys;
		const std::string& name = *elem_ref.scn_elem_def_name;
		const std::string* parent = elem_ref.scn_elem_def_parent;
		std::string parent_name = parent ? *parent : "\"\"";

		// clang-format off
		DBG(elem_path + PFX +
		    "name: " + QT + name + QT + SEP +
		    "parent: " + QT + parent_name + QT);
		// clang-format on

		if (!elem_ref.scn_elem_def_parent) {
			roots++;
			parent_names.push_back(std::move(name));

			// clang-format off
			DBG(elem_path + PFX + "parent: " + "is null");
			DBG(elem_path + PFX + "num roots" +
			    PFX + std::to_string(roots));
			// clang-format on
		}
		else {
			DBG(elem_path + PFX + "parent: " + QT + parent_name + QT);
			// clang-format off
			if (parent && name == *parent) {
				std::string err = elem_path + PFX + ERR_SELF + SPC +
						  LP + QT + name + QT + SEP +
						  QT + parent_name + QT + RP;
				// clang-format on

				DBG(err);
				add_error(err, errs);
			}
		}

		if (prev_errs_size != errs.size()) {
			break;
		}
	}

	// clang-format off
	DBG(path + PFX + QT + *scn_ref.scn_def_name + QT + PFX +
	    "final num roots" + PFX + std::to_string(roots));
	// clang-format on

	if (roots < 1) {
		DBG(path + PFX + ERR_NROOT);
		add_error(path + PFX + ERR_NROOT, errs);
	}
	else if (roots > 1) {
		// clang-format off
		DBG(path + PFX + ERR_ROOTS);
		// clang-format on

		std::string tmp;
		for (const auto& name : parent_names) {
			if (!tmp.empty()) {
				tmp += SEP;
			}

			tmp += QT + name + QT;
		}

		add_error(path + PFX + ERR_ROOTS + PFX + tmp, errs);
	}
}

static void validate_hierarchy_check_cycles(const valid_scene_ref& scn_ref,
                                            const std::string& path,
                                            errors_t& errs)
{
	DBG(path + PFX + QT + *scn_ref.scn_def_name + QT);

	std::deque<std::string> names;
	names.clear();

	int skips = 0;
	int i = 0;
	for (const auto& elem_ref : scn_ref.scn_elem_refs) {
		// clang-format off
		std::string elem_path = path + DOT + JSON_STR_ELEMS +
					LB + std::to_string(++i) + RB;
		// clang-format on

		DBG(elem_path + PFX + "name" + PFX + QT + *elem_ref.scn_elem_def_name + QT);
		DBG(elem_path + PFX + "skips" + PFX + std::to_string(skips));
		if (skips && --skips > 1) {
			DBG(elem_path + PFX + "skips" + PFX + std::to_string(skips));
			continue;
		}

		names.clear();

		const valid_scn_elem_ref* current = &elem_ref;
		validate_key_t visited;
		visited.clear();
		DBG(elem_path + PFX + "names.size" + PFX + std::to_string(names.size()));

		while (current) {
			const std::string& name = *current->scn_elem_def_name;
			names.push_back(std::move(name));
			std::string tmp;

			auto [it, inserted] = visited.insert(name);
			if (!inserted) {
				DBG(elem_path + PFX + QT + name + QT + " was not inserted");
				DBG(elem_path + PFX + "names.size" + PFX + std::to_string(names.size()));

				skips = 0;
				while (names.size()) {
					DBG(elem_path + PFX + "names.size" + PFX + std::to_string(names.size()));
					for (const auto& name : names) {
						if (!tmp.empty()) {
							tmp += "->";
						}

						tmp += QT + name + QT;
					}
					DBG(elem_path + PFX + "tmp" + PFX + tmp);

					skips++;
					DBG(elem_path + PFX + "skips" + PFX + std::to_string(skips));
					names.pop_front();
				}

				DBG(elem_path + PFX + ERR_CYCLE + SPC + ERR_DET + PFX + tmp);
				add_error(elem_path + PFX + ERR_CYCLE + SPC + ERR_DET + PFX + tmp, errs);
				break;
			}
			else {
				DBG(elem_path + PFX + QT + name + QT + " was inserted");
				DBG(elem_path + PFX + "names.size" + PFX + std::to_string(names.size()));
			}

			current = current->scn_parent_ref;
		}
	}
}

static void validate_hierarchy(const AST& ast, const std::string& path, errors_t& errs)
{
	std::string local_path = path + STR_HIER;
	DBG(local_path);
	int i = 0;
	for (const auto& scene_ref : ast.valid_references.valid_scn_refs.scene_ref_refs) {
		// clang-format off
		std::string scn_path = JSON_STR_SCNS + DOT + STR_SCN +
					LB + std::to_string(++i) + RB;
		// clang-format on
		DBG(scn_path);

		std::string roots_path = scn_path + PFX + "roots";
		DBG(roots_path);
		validate_hierarchy_check_roots(scene_ref,
		                               roots_path + PFX + scn_path,
		                               errs);

		std::string cycles_path = scn_path + PFX + "cycles";
		DBG(cycles_path);
		validate_hierarchy_check_cycles(scene_ref,
		                                cycles_path + PFX + scn_path,
		                                errs);
	}
}
/* hierarchy validation end */

bool validate_ast(AST& ast)
{
	DBG(STR_VAL);

	errors_t errs;
	std::string path = STR_VAL + PFX;

	validate_values(ast, path, errs);
	if (errs.size()) {
		// clang-format off
		PRN(ERR_PFX + STR_VAL + SPC + STR_VALS +
		    PFX + ERR_DET + NL);
		// clang-format on

		print_errors(errs);
		return false;
	}

	validate_references(ast, path, errs);
	if (errs.size()) {
		// clang-format off
		PRN(ERR_PFX + STR_VAL + SPC + STR_REFS +
		    PFX + ERR_DET + NL);
		// clang-format on
		print_errors(errs);
		return false;
	}

	validate_hierarchy(ast, path, errs);
	if (errs.size()) {
		// clang-format off
		PRN(ERR_PFX + STR_VAL + SPC + STR_HIER +
		    PFX + ERR_DET + NL);
		// clang-format on
		print_errors(errs);
		return false;
	}

	return true;
}

} // namespace config_ns

