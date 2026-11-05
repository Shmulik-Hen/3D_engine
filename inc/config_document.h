#pragma once

#include <optional>
#include <unordered_set>

#include "attrib.h"
#include "graphics.h"
#include "vector_3.h"

namespace config_ns
{

using attrib = attrib_ns::attrib;
using graphics = graphics_ns::graphics;
using vector_3 = vector_3_ns::vector_3;

using errors_t = std::vector<std::string>;
using att_names_t = std::map<uint32_t, std::string_view>;
using coord_names_t = std::map<uint32_t, std::string_view>;
using validate_key_t = std::unordered_set<std::string>;

// clang-format off
// supported field names - jsoncpp uses only std::string
const std::string JSON_STR_VER   = "version";
const std::string JSON_STR_LIB   = "library";
const std::string JSON_STR_VECS  = "vectors";
const std::string JSON_STR_POLYS = "polygons";
const std::string JSON_STR_MESHS = "meshes";
const std::string JSON_STR_ELEMS = "elements";
const std::string JSON_STR_MESH  = "mesh";
const std::string JSON_STR_SCNS  = "scenes";
const std::string JSON_STR_NAME  = "name";
const std::string JSON_STR_COLOR = "color_idx";
const std::string JSON_STR_FORCE = "force";
const std::string JSON_STR_PAREN = "parent";
const std::string JSON_STR_ACT   = "active";
const std::string JSON_STR_REF   = "ref";
const std::string JSON_STR_ORIG  = "origin";
const std::string JSON_STR_MOTN  = "motion";
const std::string JSON_STR_ENV   = "environment";
const std::string JSON_STR_RUN   = "runtime";
// environment field names
const std::string JSON_STR_RES        = "resolution";
const std::string JSON_STR_LIGHT_TYPE = "light_type";
const std::string JSON_STR_LIGHT_DIR  = "light_direction";
const std::string JSON_STR_LIGHT_POS  = "light_position";
const std::string JSON_STR_CAM_POS    = "camera_position";
const std::string JSON_STR_FOCAL_LEN  = "focal_len";
const std::string JSON_STR_NEAR_EPS   = "near_eps";
// runtime field names
const std::string JSON_STR_LOOPS      = "loops";
const std::string JSON_STR_LOOP_DELAY = "loop_delay";
// ignored fields names
const std::string JSON_STR_IGN_ENC    = "encoding";

// error/debug messages
constexpr std::string_view STR_JSON  = "JSON";
constexpr std::string_view STR_ROOT  = "root";
constexpr std::string_view STR_NUM   = "num";
constexpr std::string_view STR_FNLN  = "final num";
constexpr std::string_view STR_PRS   = "parse";
constexpr std::string_view STR_PRSR  = "parser";
constexpr std::string_view STR_VAL   = "validate";
constexpr std::string_view STR_SCN   = "scene";
constexpr std::string_view STR_ATT   = "attrib";
constexpr std::string_view STR_VEC   = "vector";
constexpr std::string_view STR_PAIR  = "pair";
constexpr std::string_view STR_ARR   = "array";
constexpr std::string_view STR_POLY  = "polygon";
constexpr std::string_view STR_ELEM  = "element";
constexpr std::string_view STR_ROTX  = "rotation X:    ";
constexpr std::string_view STR_ROTY  = "rotation Y:    ";
constexpr std::string_view STR_ROTZ  = "rotation Z:    ";
constexpr std::string_view STR_TRNX  = "translation X: ";
constexpr std::string_view STR_TRNY  = "translation Y: ";
constexpr std::string_view STR_TRNZ  = "translation Z: ";
constexpr std::string_view STR_SCL   = "scale factor:  ";
constexpr std::string_view STR_AST   = "ast";
constexpr std::string_view STR_VLDT  = "validate";
constexpr std::string_view STR_VALS  = "values";
constexpr std::string_view STR_REFS  = "references";
constexpr std::string_view STR_HIER  = "hierarchy";
constexpr std::string_view STR_WID   = "width";
constexpr std::string_view STR_HGT   = "height";
constexpr std::string_view STR_CORD  = "coord";
constexpr std::string_view STR_SIZE  = "size";
constexpr std::string_view STR_LEN   = "len";
constexpr std::string_view STR_PATH  = "path";
constexpr std::string_view STR_FOUND = "found";
constexpr std::string_view STR_FLD   = "fields";
constexpr std::string_view STR_NAMES = "names";
constexpr std::string_view STR_LIST  = "list";
constexpr std::string_view STR_DET   = "detected";
constexpr std::string_view STR_IKEY  = "insert key";
constexpr std::string_view STR_FKEY  = "find key";
constexpr std::string_view STR_SUCC  = "succeeded";
constexpr std::string_view STR_IN    = " in ";
constexpr std::string_view ERR_NFND  = "not found";
constexpr std::string_view ERR_VER   = " unsupported version";
constexpr std::string_view ERR_REQ   = "requires";
constexpr std::string_view ERR_CURR  = "current";
constexpr std::string_view ERR_OBJ   = "not an object";
constexpr std::string_view ERR_ARR   = "not an array";
constexpr std::string_view ERR_UINT  = "not an unsigned integer";
constexpr std::string_view ERR_INT   = "not an integer";
constexpr std::string_view ERR_STR   = "not a string";
constexpr std::string_view ERR_REAL  = "not a floating point";
constexpr std::string_view ERR_BOOL  = "not a boolean";
constexpr std::string_view ERR_MISS  = "is missing";
constexpr std::string_view ERR_ILG   = "is illegal";
constexpr std::string_view ERR_NULL  = "is null";
constexpr std::string_view ERR_EMPTY = "is empty";
constexpr std::string_view ERR_ATLST = "at least";
constexpr std::string_view ERR_INV   = "is invalid";
constexpr std::string_view ERR_SIZE  = "illegal array size";
constexpr std::string_view ERR_FAIL  = "failed";
constexpr std::string_view ERR_FAILW = "failed with";
constexpr std::string_view ERR_DET   = "errors detected !";
constexpr std::string_view ERR_UNK   = "unknown fields detected";
constexpr std::string_view ERR_UNS   = "unsupported field";
constexpr std::string_view ERR_DUP   = "duplicate";
// clang-format on

const std::unordered_set<std::string> valid_field_names = {
	JSON_STR_VER,
	JSON_STR_LIB,
	JSON_STR_VECS,
	JSON_STR_MESHS,
	JSON_STR_POLYS,
	JSON_STR_ELEMS,
	JSON_STR_MESH,
	JSON_STR_SCNS,
	JSON_STR_NAME,
	JSON_STR_COLOR,
	JSON_STR_FORCE,
	JSON_STR_PAREN,
	JSON_STR_ACT,
	JSON_STR_REF,
	JSON_STR_ORIG,
	JSON_STR_MOTN,
	JSON_STR_ENV,
	JSON_STR_RES,
	JSON_STR_LIGHT_TYPE,
	JSON_STR_LIGHT_DIR,
	JSON_STR_LIGHT_POS,
	JSON_STR_CAM_POS,
	JSON_STR_FOCAL_LEN,
	JSON_STR_NEAR_EPS,
	JSON_STR_RUN,
	JSON_STR_LOOPS,
	JSON_STR_LOOP_DELAY,
};

const std::unordered_set<std::string> ignored_field_names = {
	JSON_STR_IGN_ENC,
};

const std::unordered_set<std::string> hints_field_names = {
	JSON_STR_VER,
	JSON_STR_LIB,
	JSON_STR_VECS,
	JSON_STR_MESHS,
	JSON_STR_POLYS,
	JSON_STR_ELEMS,
	JSON_STR_MESH,
	JSON_STR_SCNS,
	JSON_STR_NAME,
	JSON_STR_COLOR,
	JSON_STR_FORCE,
	JSON_STR_PAREN,
	JSON_STR_ACT,
	JSON_STR_REF,
	JSON_STR_ORIG,
	JSON_STR_MOTN,
	JSON_STR_ENV,
	JSON_STR_RES,
	JSON_STR_LIGHT_TYPE,
	JSON_STR_LIGHT_DIR,
	JSON_STR_LIGHT_POS,
	JSON_STR_CAM_POS,
	JSON_STR_FOCAL_LEN,
	JSON_STR_NEAR_EPS,
	JSON_STR_RUN,
	JSON_STR_LOOPS,
	JSON_STR_LOOP_DELAY,
	JSON_STR_IGN_ENC,
};

const att_names_t att_names = {
	{0, STR_ROTX},
	{1, STR_ROTY},
	{2, STR_ROTZ},
	{3, STR_TRNX},
	{4, STR_TRNY},
	{5, STR_TRNZ},
	{6,  STR_SCL}
};

const coord_names_t coord_names = {
	{0, "X"},
	{1, "Y"},
	{2, "Z"},
};

/* values validation */
enum light_type
{
	__lt_min__,               // dummy
	directional = __lt_min__, // sun-like
	point,                    // light-bulb
	__lt_max__                // dummy
};

/* library */
struct parse_poly_def
{
	// required fields
	std::string name;
	uint32_t color_index {0};
	bool force {false};
	std::vector<vector_3> vectors;
};

struct parse_mesh_def
{
	// required fields
	std::string name;
	std::vector<std::string> polygons;
};

struct parse_lib_elem_def
{
	// required fields
	std::string name;
	std::string mesh;
};

struct parse_lib_def
{
	// required fields
	std::vector<parse_poly_def> polygons;
	std::vector<parse_mesh_def> meshes;
	std::vector<parse_lib_elem_def> lib_elem_defs;
};
/* library end */

/* scenes */
struct parse_scn_elem_def
{
	// required fields
	std::string name;
	bool active {false};

	// optional fields
	std::optional<std::string> ref;
	std::optional<std::string> parent; // null means "root"
	std::optional<attrib> origin;
	std::optional<attrib> motion;
};

struct parse_res_def
{
	// required fields
	uint32_t width {0};
	uint32_t height {0};
};

struct parse_run_def
{
	// optional fields
	std::optional<int> loops;
	std::optional<uint32_t> loop_delay;
};

struct parse_env_def
{
	// optional fields
	std::optional<parse_res_def> resolution;
	std::optional<graphics::color_idx> color_idx;
	std::optional<uint32_t> light_type;
	std::optional<vector_3> light_direction;
	std::optional<vector_3> light_position;
	std::optional<vector_3> camera_position;
	std::optional<unit> focal_len;
	std::optional<unit> near_eps;
};

struct parse_scn_def
{
	// required fields
	std::string name;
	std::vector<parse_scn_elem_def> scene_elem_defs;

	// optional fields
	std::optional<parse_env_def> env;
	std::optional<parse_run_def> run;
};

struct valid_values_def
{
	graphics::color_idx max_color_idx;
};
/* scenes end */
/* values validation end*/

/* references validation */
/* library */
struct valid_poly_ref
{
	const std::string* name;
	const parse_poly_def* poly_def_ptr;
};

struct valid_poly_refs
{
	validate_key_t poly_keys;
	std::vector<valid_poly_ref*> poly_ptrs;
};

struct valid_mesh_ref
{
	validate_key_t mesh_poly_keys;
	std::vector<valid_poly_refs*> mesh_polys_ptrs;
	const std::string* name;
	const parse_mesh_def* mesh_def_ptr;
};

struct valid_mesh_refs
{
	validate_key_t mesh_keys;
	std::vector<valid_mesh_ref*> mesh_ptrs;
};

struct valid_lib_elem_ref
{
	valid_mesh_ref* lib_mesh_ptr;
	const std::string* name;
	const parse_lib_elem_def* lib_elem_ptr;
};

struct valid_lib_elem_refs
{
	validate_key_t lib_elem_keys;
	std::vector<valid_lib_elem_ref*> lib_elem_ptrs;
};

struct valid_library_refs
{
	valid_poly_refs poly_refs;
	valid_mesh_refs mesh_refs;
	valid_lib_elem_refs lib_elem_refs;
};
/* library end */

/* scenes */

struct valid_scn_elem_ref
{
	valid_lib_elem_ref* lib_elem_ptr;
	const std::string* name;
	const parse_scn_elem_def* scn_elem_def_ptr;
};

struct valid_scn_elem_refs
{
	validate_key_t scn_elem_keys;
	std::vector<valid_scn_elem_ref*> scn_elem_ptrs;
};

struct valid_scene_ref
{
	validate_key_t scn_elem_keys;
	std::vector<valid_scn_elem_refs*> scn_elem_ptrs;
	const std::string* name;
	const parse_scn_def* scn_def_ptr;
};

struct valid_scene_refs
{
	validate_key_t scene_ref_keys;
	std::vector<valid_scene_ref*> scene_ref_ptrs;
};

struct valid_refs
{
	valid_library_refs valid_lib_refs;
	valid_scene_refs valid_scn_ref;
};
/* scenes end */
/* references validation end */

struct AST
{
	parse_lib_def library_defs;
	std::vector<parse_scn_def> scene_defs;
	valid_values_def valid_values;
	valid_refs valid_references;
};

// merged during build from parse_lib_elem_def + parse_scn_elem_def
struct element_def
{
	// required fields
	std::string name;
	bool active {false};
	std::vector<std::string> polygons;

	// optional fields
	std::string parent; // null means "root"
	std::optional<attrib> origin;
	std::optional<attrib> motion;
};

void add_error(const std::string& err, errors_t& errs);
void print_errors(const config_ns::errors_t errs, const std::string str = "");
std::string_view get_att_name(const uint32_t i);
std::string_view get_coord_name(const uint32_t i);
std::string set_precision(const unit val, const uint32_t digits = 3);
std::string escape_control_chars(const std::string& source);
bool is_valid_field_name(const std::string& field);
bool is_valid_field_value(const std::string& value);

} // namespace config_ns
