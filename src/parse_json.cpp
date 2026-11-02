#include <algorithm>
#include <numeric>

// #define DEBUG_PRINTS
#include "parse_json.h"
#include "utils.h"

namespace config_ns
{

static size_t levenshtein_distance(const std::string& source, const std::string& target)
{
	std::vector<size_t> previous_row(target.size() + 1);
	std::vector<size_t> current_row(target.size() + 1);

	std::iota(previous_row.begin(), previous_row.end(), 0);

	for (size_t source_idx = 1; source_idx <= source.size(); ++source_idx) {
		current_row[0] = source_idx;

		for (size_t target_idx = 1; target_idx <= target.size(); ++target_idx) {
			size_t substitution_cost =
				(source[source_idx - 1] == target[target_idx - 1]) ? 0 : 1;

			current_row[target_idx] =
				std::min({previous_row[target_idx] + 1,    // deletion
			                  current_row[target_idx - 1] + 1, // insertion
			                  previous_row[target_idx - 1] + substitution_cost});
		}

		std::swap(previous_row, current_row);
	}

	return previous_row[target.size()];
}

static std::string suggest_field(const std::string& invalid)
{
	size_t best_distance = 3;
	std::string best_match;

	for (const auto& field : hints_field_names) {
		size_t distance = levenshtein_distance(invalid, field);

		if (distance < best_distance) {
			best_distance = distance;
			best_match = field;
		}
	}

	return best_match;
}

static std::string get_hint(const std::string invalid)
{
	std::string hint = suggest_field(invalid);
	return SPC + LP + "did you mean" + SPC + TAG + hint + TAG + SPC + '?' + RP;
}

void check_valid_field_name(const Json::Value& root, errors_t& errs, const std::string& path = "", const std::string& member = "")
{
	if (!root.isNull() && member.size()) {
		DBG("checking" + SPC + QT + path + QT + PFX + QT + escape_control_chars(member) + QT);
		bool rc = is_valid_field_name(member);
		if (!rc) {
			DBG("is_valid_field_name" + QT + escape_control_chars(member) + QT + PFX + "returned false");
			std::string err = path + PFX + QT + escape_control_chars(member) + QT + SPC + ERR_ILG + DOT;
			DBG("err: " + QT + err + QT /*  + NL */);
			std::string hint = get_hint(member);
			DBG("hint" + PFX + QT + hint + QT /*  + NL */);
			if (!hint.empty()) {
				err += hint;
				DBG("err: " + QT + err + QT /*  + NL */);
			}

			DBG("logging an error for" + PFX + QT + escape_control_chars(member) + QT /*  + NL */);
			add_error(err, errs);
			return;
		}
		else if (valid_field_names.find(member) == valid_field_names.end()) {
			DBG("couldn't find " + QT + escape_control_chars(member) + QT + SPC + "in valid_field_names" /*  + NL */);
			if (ignored_field_names.find(member) == ignored_field_names.end()) {
				DBG("couldn't find " + QT + escape_control_chars(member) + QT + SPC + "in ignored_field_names either" /*  + NL */);
				std::string err = path + PFX + ERR_UNS + PFX + QT + escape_control_chars(member) + QT + DOT;
				DBG(QT + err + QT /*  + NL */);
				std::string hint = get_hint(member);
				DBG(hint /*  + NL */);
				if (!hint.empty()) {
					err += hint;
					DBG(QT + err + QT /*  + NL */);
				}

				DBG("logging an error for" + PFX + QT + escape_control_chars(member) + QT /*  + NL */);
				add_error(err, errs);
				return;
			}
			else {
				DBG("but found" + SPC + QT + escape_control_chars(member) + QT + SPC + "in ignored_field_names" /*  + NL */);
			}
		}
	}

	switch (root.type()) {
	case Json::nullValue:
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "value" + PFX + "null");
		break;
	case Json::intValue:
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "type" + PFX + "Int" + PFX + "value" + PFX + std::to_string(root.asInt()));
		break;
	case Json::uintValue:
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "type" + PFX + "UInt" + PFX + "value" + PFX + std::to_string(root.asUInt()));
		break;
	case Json::realValue:
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "type" + PFX + "Float" + PFX + "value" + PFX + set_precision(root.asFloat()));
		break;
	case Json::booleanValue:
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "type" + PFX + "Bool" + PFX + "value" + PFX + BOOL(root.asBool()));
		break;
	case Json::stringValue:
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "type" + PFX + "String" + SEP + STR_LEN + PFX + std::to_string(root[member].asString().length()) + PFX + "value" + PFX + QT + escape_control_chars(root.asString()) + QT);
		break;
	case Json::arrayValue:
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "type" + PFX + "Array" + SEP + STR_SIZE + PFX + std::to_string(root.size()));
		for (Json::Value::ArrayIndex i = 0; i < root.size(); ++i) {
			// Construct the path for array elements (e.g., .users[0])
			std::string arrayPath = path + LB + std::to_string(i + 1) + RB;
			DBG("arrayPath" + PFX + QT + arrayPath + QT);
			check_valid_field_name(root[i], errs, arrayPath);
		}
		break;
	case Json::objectValue: {
		DBG(STR_PATH + PFX + QT + path + QT + PFX + "type" + PFX + "Object" + SEP + STR_SIZE + PFX + std::to_string(root.size()));
		// Get all member names (keys) in the object
		Json::Value::Members members = root.getMemberNames();
		for (const auto& member : members) {
			DBG("member" + PFX + QT + escape_control_chars(member) + QT);
			// Construct the path for object fields (e.g., .user.name)
			std::string objectPath = path.empty() ? member : path + DOT + member;
			DBG("objectPath" + PFX + QT + objectPath + QT + SEP + "member" + PFX + QT + escape_control_chars(member) + QT);
			check_valid_field_name(root[member], errs, objectPath, member);
		}
		break;
	}
	}
}

static bool check_json_version(const Json::Value& ver, const std::string& path, const uint32_t array[JSON_VER_FIELDS])
{
	if (ver.size() != JSON_VER_FIELDS) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX + ERR_REQ + PFX +
		    std::to_string(JSON_VER_FIELDS) + SEP +
		    ERR_CURR + PFX + std::to_string(ver.size()));
		// clang-format on
		return false;
	}

	bool match = true;
	for (uint32_t i = 0; i < JSON_VER_FIELDS; i++) {
		if (!ver[i].isUInt()) {
			// clang-format off
			ERR(path + JSON_STR_VER +
			    LB + std::to_string(i + 1) + RB +
			    PFX + ERR_UINT);
			// clang-format on
			return false;
		}

		if (ver[i].asUInt() != array[i]) {
			match = false;
			break;
		}
	}

	if (!match) {
		// clang-format off
		ERR(path + ERR_VER + PFX +
		    ERR_REQ + PFX +
		    std::to_string(array[0]) + DOT +
		    std::to_string(array[1]) + DOT +
		    std::to_string(array[2]) + SEP +
		    ERR_CURR + PFX +
		    ver[0].asString() + DOT +
		    ver[1].asString() + DOT +
		    ver[2].asString());
		// clang-format on
		return false;
	}

	// clang-format off
	DBG(path + "found version" + PFX +
	    std::to_string(array[0]) + DOT +
	    std::to_string(array[1]) + DOT +
	    std::to_string(array[2]));
	// clang-format on

	return true;
}

static bool parse_json_attrib(const Json::Value& att, const std::string& path, unit array[NUM_ATTRIBUTES])
{
	if (att.size() != NUM_ATTRIBUTES) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX +
		    ERR_REQ + PFX + std::to_string(NUM_ATTRIBUTES) + SEP +
		    ERR_CURR + PFX + std::to_string(att.size()));
		// clang-format on
		return false;
	}

	for (uint32_t i = 0; i < NUM_ATTRIBUTES; i++) {
		std::string str = path + get_att_name(i);
		if (!att[i].isDouble()) {
			// clang-format off
			ERR(str + ERR_REAL);
			// clang-format on
			return false;
		}

		DBG(str + set_precision(att[i].asFloat()));
		array[i] = att[i].asFloat();
	}

	return true;
}

static bool parse_json_vector(const Json::Value& vector, const std::string& path, unit array[NUM_COORDS])
{
	if (vector.size() != NUM_COORDS) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX +
		    ERR_REQ + PFX + std::to_string(NUM_COORDS) + SEP +
		    ERR_CURR + PFX + std::to_string(vector.size()));
		// clang-format on
		return false;
	}

	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		// clang-format off
		std::string str = path + STR_CORD + LB + get_coord_name(i) + RB;
		// clang-format on
		if (!vector[i].isDouble()) {
			ERR(str + PFX + ERR_REAL);
			return false;
		}

		array[i] = vector[i].asFloat();
		DBG(str + PFX + set_precision(array[i]));
	}

	return true;
}

static bool parse_json_polygon(const Json::Value& polygon, const std::string path, polygon_def& pd)
{
	bool rc;

	if (polygon.isMember(JSON_STR_NAME)) {
		if (!polygon[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		pd.name = polygon[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + pd.name + QT);
	}

	if (polygon.isMember(JSON_STR_COLOR)) {
		if (!polygon[JSON_STR_COLOR].isUInt()) {
			ERR(path + JSON_STR_COLOR + PFX + ERR_UINT);
			return false;
		}

		pd.color_index = polygon[JSON_STR_COLOR].asUInt();
		DBG(path + JSON_STR_COLOR + PFX + std::to_string(pd.color_index));
	}

	if (polygon.isMember(JSON_STR_FORCE)) {
		if (!polygon[JSON_STR_FORCE].isBool()) {
			ERR(path + JSON_STR_FORCE + PFX + ERR_BOOL);
			return false;
		}

		pd.force = polygon[JSON_STR_FORCE].asBool();
		DBG(path + JSON_STR_FORCE + PFX << BOOL(pd.force));
	}

	if (polygon.isMember(JSON_STR_VECS)) {
		if (!polygon[JSON_STR_VECS].isArray()) {
			ERR(path + JSON_STR_VECS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& vectors = polygon[JSON_STR_VECS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_VECS +
		    PFX + std::to_string(vectors.size()));
		// clang-format on

		int i = 1;
		for (const auto& vector : vectors) {
			std::string str = path + STR_VEC + LB + std::to_string(i) + RB;
			unit coords[NUM_COORDS];

			if (!vector.isArray()) {
				ERR(str + PFX + ERR_ARR);
				return false;
			}

			rc = parse_json_vector(vector, str + DOT, coords);
			if (!rc) {
				return false;
			}
			i++;

			vector_3 v(coords);
			pd.vectors.push_back(v);
		}
	}

	return true;
}

static bool parse_json_mesh(const Json::Value& mesh, const std::string path, mesh_def& md)
{
	if (mesh.isMember(JSON_STR_NAME)) {
		if (!mesh[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		md.name = mesh[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + md.name + QT);
	}

	if (mesh.isMember(JSON_STR_POLYS)) {
		if (!mesh[JSON_STR_POLYS].isArray()) {
			ERR(path + JSON_STR_POLYS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& polygons = mesh[JSON_STR_POLYS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_POLYS +
		    PFX + std::to_string(polygons.size()));
		// clang-format on

		int i = 1;
		for (const auto& polygon : polygons) {
			if (!polygon.isString()) {
				// clang-format off
				ERR(path + STR_POLY +
				    LB + std::to_string(i) + RB +
				    PFX + ERR_STR);
				// clang-format on
				return false;
			}

			md.polygons.push_back(polygon.asString());
			// clang-format off
			DBG(path + STR_POLY +
			    LB + std::to_string(i) + RB +
			    PFX + QT + polygon.asString() + QT);
			// clang-format on
		}
	}

	return true;
}

static bool parse_json_lib_elem_def(const Json::Value& element, const std::string path, lib_elem_def& ed)
{
	if (element.isMember(JSON_STR_NAME)) {
		if (!element[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + ed.name + QT);
	}

	if (element.isMember(JSON_STR_MESH)) {
		if (!element[JSON_STR_MESH].isString()) {
			ERR(path + JSON_STR_MESH + PFX + ERR_STR);
			return false;
		}

		ed.mesh = element[JSON_STR_MESH].asString();
		DBG(path + JSON_STR_MESH + PFX + QT + ed.mesh + QT);
	}

	return true;
}

static bool parse_json_scene_elem_def(const Json::Value& element, const std::string path, scene_elem_def& ed)
{
	std::string str;
	bool rc;

	if (element.isMember(JSON_STR_NAME)) {
		str = path + JSON_STR_NAME;
		if (!element[JSON_STR_NAME].isString()) {
			ERR(str + PFX + ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + ed.name + QT);
	}

	if (element.isMember(JSON_STR_PAREN)) {
		str = path + JSON_STR_PAREN;
		if (element[JSON_STR_PAREN].isNull()) {
			DBG(str + ERR_NULL);
			ed.parent = std::nullopt;
		}
		else if (element[JSON_STR_PAREN].isString()) {
			DBG(str + QT + element[JSON_STR_PAREN].asString() + QT);
			ed.parent = std::optional<std::string> {element[JSON_STR_PAREN].asString()};
		}
		else {
			ERR(str + PFX + ERR_STR);
			return false;
		}
	}

	if (element.isMember(JSON_STR_ACT)) {
		str = path + JSON_STR_ACT;
		if (!element[JSON_STR_ACT].isBool()) {
			ERR(str + PFX + ERR_BOOL);
			return false;
		}

		ed.active = element[JSON_STR_ACT].asBool();
		DBG(str + PFX + BOOL(ed.active));
	}

	if (element.isMember(JSON_STR_REF)) {
		str = path + JSON_STR_REF;
		if (!element[JSON_STR_REF].isString()) {
			ERR(str + PFX + ERR_STR);
			return false;
		}

		DBG(str + PFX + QT + element[JSON_STR_REF].asString() + QT);
		ed.ref = std::optional<std::string> {element[JSON_STR_REF].asString()};
	}

	if (element.isMember(JSON_STR_ORIG)) {
		str = path + JSON_STR_ORIG;
		unit array[NUM_ATTRIBUTES];

		if (!element[JSON_STR_ORIG].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_attrib(element[JSON_STR_ORIG], str + DOT, array);
		if (!rc) {
			return false;
		}

		attrib att(array);
		ed.origin = att;
		att.print();
	}

	if (element.isMember(JSON_STR_MOTN)) {
		str = path + JSON_STR_MOTN;
		unit array[NUM_ATTRIBUTES];

		if (!element[JSON_STR_MOTN].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_attrib(element[JSON_STR_MOTN], str + DOT, array);
		if (!rc) {
			return false;
		}

		attrib att(array);
		ed.motion = att;
		att.print();
	}

	return true;
}

static bool parse_json_resolution(const Json::Value& res, const std::string& path, uint32_t array[2])
{
	if (res.size() != 2) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX +
		    ERR_REQ + PFX + std::to_string(2) + SEP +
		    ERR_CURR + PFX + std::to_string(res.size()));
		// clang-format on
		return false;
	}

	if (!res[0].isDouble() || !res[1].isDouble()) {
		ERR(path + PFX + STR_WID + " or " + STR_HGT + " are " + ERR_UINT);
		return false;
	}

	array[0] = res[0].asUInt();
	array[1] = res[1].asUInt();

	return true;
}

static bool parse_json_environment(const Json::Value& env, const std::string path, environ_def& ed)
{
	std::string str;
	bool rc;

	if (env.isMember(JSON_STR_RES)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_RES);

		str = path + JSON_STR_RES;
		uint32_t res[2];

		if (!env[JSON_STR_RES].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_resolution(env[JSON_STR_RES], str + DOT, res);
		if (!rc) {
			return false;
		}

		DBG(str + PFX + std::to_string(res[0]) + 'x' + std::to_string(res[1]));
		ed.resolution = config_ns::resolution_def {res[0], res[1]};
	}

	if (env.isMember(JSON_STR_COLOR)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_COLOR);

		str = path + JSON_STR_COLOR;
		if (!env[JSON_STR_COLOR].isUInt()) {
			ERR(str + PFX + ERR_UINT);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_COLOR].asUInt()));
		ed.color_idx = (graphics::color_idx)env[JSON_STR_COLOR].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_TYPE)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LIGHT_TYPE);

		str = path + JSON_STR_LIGHT_TYPE;
		if (!env[JSON_STR_LIGHT_TYPE].isUInt()) {
			ERR(str + PFX + ERR_UINT);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_LIGHT_TYPE].asUInt()));
		ed.light_type = env[JSON_STR_LIGHT_TYPE].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_DIR)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LIGHT_DIR);

		str = path + JSON_STR_LIGHT_DIR;
		unit coords[NUM_COORDS];

		if (!env[JSON_STR_LIGHT_DIR].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_vector(env[JSON_STR_LIGHT_DIR], str + DOT, coords);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ed.light_direction = v;
		v.print();
	}

	if (env.isMember(JSON_STR_LIGHT_POS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LIGHT_POS);

		str = path + JSON_STR_LIGHT_DIR;
		unit coords[NUM_COORDS];

		if (!env[JSON_STR_LIGHT_POS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_vector(env[JSON_STR_LIGHT_POS], str + DOT, coords);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ed.light_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_CAM_POS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_CAM_POS);

		str = path + JSON_STR_CAM_POS;
		unit coords[NUM_COORDS];

		if (!env[JSON_STR_CAM_POS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_vector(env[JSON_STR_CAM_POS], str + DOT, coords);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ed.camera_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_FOCAL_LEN)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_FOCAL_LEN);

		str = path + JSON_STR_FOCAL_LEN;
		if (!env[JSON_STR_FOCAL_LEN].isDouble()) {
			ERR(str + PFX + ERR_REAL);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_FOCAL_LEN].asFloat()));
		ed.focal_len = env[JSON_STR_FOCAL_LEN].asFloat();
	}

	if (env.isMember(JSON_STR_NEAR_EPS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_NEAR_EPS);

		str = path + JSON_STR_NEAR_EPS;
		if (!env[JSON_STR_NEAR_EPS].isDouble()) {
			ERR(str + PFX + ERR_REAL);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_NEAR_EPS].asFloat()));
		ed.near_eps = env[JSON_STR_NEAR_EPS].asFloat();
	}

	return true;
}

static bool parse_json_runtime(const Json::Value& run, const std::string path, runtime_def& rd)
{
	std::string str;

	if (run.isMember(JSON_STR_LOOPS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LOOPS);
		str = path + JSON_STR_LOOPS;
		if (!run[JSON_STR_LOOPS].isInt()) {
			ERR(str + PFX + ERR_INT);
			return false;
		}

		DBG(str + PFX + std::to_string(run[JSON_STR_LOOPS].asInt()));
		rd.loops = run[JSON_STR_LOOPS].asInt();
	}

	if (run.isMember(JSON_STR_LOOP_DELAY)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LOOP_DELAY);
		str = path + JSON_STR_LOOP_DELAY;
		if (!run[JSON_STR_LOOP_DELAY].isUInt()) {
			ERR(str + PFX + ERR_UINT);
			return false;
		}

		DBG(str + PFX + std::to_string(run[JSON_STR_LOOP_DELAY].asUInt()));
		rd.loop_delay = run[JSON_STR_LOOP_DELAY].asUInt();
	}

	return true;
}

static bool parse_json_library(const Json::Value& library, const std::string path, library_def& ld)
{
	bool rc;

	if (library.isMember(JSON_STR_POLYS)) {
		if (!library[JSON_STR_POLYS].isArray()) {
			ERR(path + JSON_STR_POLYS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& polygons = library[JSON_STR_POLYS];

		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_POLYS +
		    PFX + std::to_string(polygons.size()));
		// clang-format on

		int i = 1;
		for (const auto& polygon : polygons) {
			polygon_def pd {};
			// clang-format off
			std::string str = path + JSON_STR_POLYS +
					  LB + std::to_string(i) + RB;
			// clang-format on

			if (!polygon.isObject()) {
				ERR(str + PFX + ERR_OBJ);
				return false;
			}

			rc = parse_json_polygon(polygon, str + DOT, pd);
			if (!rc) {
				return false;
			}
			i++;

			ld.polygons.push_back((std::move(pd)));
		}
	}

	if (library.isMember(JSON_STR_MESHS)) {
		if (!library[JSON_STR_MESHS].isArray()) {
			ERR(path + JSON_STR_MESHS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& meshes = library[JSON_STR_MESHS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_MESHS +
		    PFX  + std::to_string(meshes.size()));
		// clang-format on

		int i = 1;
		for (const auto& mesh : meshes) {
			mesh_def md {};
			// clang-format off
			std::string str = path + JSON_STR_MESHS +
					  LB + std::to_string(i) + RB;
			// clang-format on

			if (!mesh.isObject()) {
				ERR(str + PFX + ERR_OBJ);
				return false;
			}

			rc = parse_json_mesh(mesh, str + DOT, md);
			if (!rc) {
				return false;
			}
			i++;

			ld.meshes.push_back((std::move(md)));
		}
	}

	if (library.isMember(JSON_STR_ELEMS)) {
		if (!library[JSON_STR_ELEMS].isArray()) {
			ERR(path + JSON_STR_ELEMS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& elements = library[JSON_STR_ELEMS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_ELEMS +
		    PFX  + std::to_string(elements.size()));
		// clang-format on

		int i = 1;
		for (const auto& element : elements) {
			lib_elem_def ed {};
			// clang-format off
			std::string str = path + JSON_STR_ELEMS +
					  LB + std::to_string(i) + RB;
			// clang-format on

			if (!element.isObject()) {
				ERR(str + PFX + ERR_OBJ);
				return false;
			}

			rc = parse_json_lib_elem_def(element, str + DOT, ed);
			if (!rc) {
				return false;
			}
			i++;

			ld.lib_elem_defs.push_back(std::move(ed));
		}
	}

	// clang-format off
	DBG(path + STR_FNLN + SPC +
	    JSON_STR_POLYS + PFX + std::to_string(ld.polygons.size()));
	DBG(path + STR_FNLN + SPC +
	    JSON_STR_MESHS + PFX + std::to_string(ld.meshes.size()));
	DBG(path + STR_FNLN + SPC +
	    JSON_STR_ELEMS + PFX + std::to_string(ld.lib_elem_defs.size()));
	// clang-format on

	return true;
}

static bool parse_json_scene(const Json::Value& scene, const std::string path, scene_def& sd)
{
	bool rc;

	if (scene.isMember(JSON_STR_NAME)) {
		if (!scene[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		sd.name = scene[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + sd.name);
	}

	if (scene.isMember(JSON_STR_ELEMS)) {
		std::string str = path + JSON_STR_ELEMS;
		// DBG(str);

		if (!scene[JSON_STR_ELEMS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		const Json::Value elements = scene[JSON_STR_ELEMS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_ELEMS +
		    PFX + std::to_string(elements.size()));
		// clang-format on

		int i = 1;
		for (const auto& element : elements) {
			std::string err = str + LB + std::to_string(i) + RB;
			// DBG(str + PFX);
			if (!element.isObject()) {
				ERR(err + PFX + ERR_OBJ);
				return false;
			}

			scene_elem_def ed {};
			rc = parse_json_scene_elem_def(element, err + DOT, ed);
			if (!rc) {
				return false;
			}
			i++;

			sd.scene_elem_defs.push_back(std::move(ed));
		}

		// clang-format off
		DBG(path + STR_FNLN + SPC + JSON_STR_ELEMS + PFX +
		    std::to_string(sd.scene_elem_defs.size()));
		// clang-format on
	}

	if (scene.isMember(JSON_STR_ENV)) {
		std::string str = path + JSON_STR_ENV;

		if (!scene[JSON_STR_ENV].isObject()) {
			ERR(str + PFX + ERR_OBJ);
			return false;
		}

		const Json::Value& env = scene[JSON_STR_ENV];
		// clang-format off
		DBG(str + SPC + STR_NUM + SPC + STR_FLD +
		    PFX + std::to_string(env.size()));
		// clang-format on

		environ_def ed {};
		rc = parse_json_environment(env, str + DOT, ed);
		if (!rc) {
			return false;
		}

		sd.env = std::move(ed);
	}

	if (scene.isMember(JSON_STR_RUN)) {
		std::string str = path + JSON_STR_RUN;

		if (!scene[JSON_STR_RUN].isObject()) {
			ERR(str + PFX + ERR_OBJ);
			return false;
		}

		const Json::Value& run = scene[JSON_STR_RUN];
		DBG(str + SPC + STR_NUM + SPC + STR_FLD + PFX + std::to_string(run.size()));

		runtime_def rd {};
		rc = parse_json_runtime(run, str + DOT, rd);
		if (!rc) {
			return false;
		}

		sd.run = std::move(rd);
	}

	return true;
}

bool parse_json(std::ifstream& ifs, config_ns::AST& ast)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	std::string json_errs;
	config_ns::errors_t errs;
	bool rc;

	builder["collectComments"] = false;
	builder["rejectDupKeys"] = true;
	builder["allowTrailingCommas"] = false;
	builder["emitUTF8"] = false;

	DBG(STR_PRS);

	std::string path = STR_PRS + PFX;
	std::string str;

	rc = parseFromStream(builder, ifs, &root, &json_errs);
	if (!rc) {
		// clang-format off
		str = path + STR_JSON + SPC + STR_PRSR +
		    ERR_FAILW + PFX + json_errs;
		ERR(str);
		// clang-format on
		return false;
	}

	check_valid_field_name(root, errs);
	if (errs.size()) {
		PRN(WARN_PFX + STR_PRS + PFX + ERR_UNK + NL);
		print_errors(errs);
		return false;
	}

	if (!root.isObject()) {
		ERR(path + STR_JSON + PFX + STR_ROOT + ERR_OBJ);
		return false;
	}

	str = path + JSON_STR_VER;

	// Mandatory. checked during parse
	if (!root.isMember(JSON_STR_VER)) {
		ERR(str + PFX + ERR_MISS);
		return false;
	}

	Json::Value& ver = root[JSON_STR_VER];
	if (!ver.isArray()) {
		ERR(str + PFX + ERR_ARR);
		return false;
	}

	uint32_t version[JSON_VER_FIELDS] = {JSON_VER_MAJ, JSON_VER_MIN, JSON_VER_REV};
	rc = check_json_version(ver, str + DOT, version);
	if (!rc) {
		return false;
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_LIB)) {
		str = path + JSON_STR_LIB;
		DBG(str);

		if (!root[JSON_STR_LIB].isObject()) {
			ERR(str + PFX + ERR_OBJ);
			return false;
		}

		const Json::Value& library = root[JSON_STR_LIB];
		library_def ld {};

		rc = parse_json_library(library, str + DOT, ld);
		if (!rc) {
			return false;
		}

		ast.library_defs = ld;
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_SCNS)) {
		str = path + JSON_STR_SCNS;
		DBG(str);

		if (!root[JSON_STR_SCNS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& scenes = root[JSON_STR_SCNS];
		// clang-format off
		DBG(str + PFX + STR_NUM + SPC + JSON_STR_SCNS +
		    PFX + std::to_string(scenes.size()));
		// clang-format on

		int i = 1;
		for (const auto& scene : scenes) {
			scene_def sd {};

			str += DOT + STR_SCN + LB + std::to_string(i) + RB;

			if (!scene.isObject()) {
				ERR(str + PFX + ERR_ARR);
				return false;
			}

			rc = parse_json_scene(scene, str + DOT, sd);
			if (!rc) {
				return false;
			}
			i++;

			ast.scene_defs.push_back(std::move(sd));
		}
	}

	// clang-format off
	DBG(path + STR_FNLN + SPC + JSON_STR_SCNS +
	    PFX + std::to_string(ast.scene_defs.size()));
	// clang-format on

	return true;
}

} // namespace config_ns
