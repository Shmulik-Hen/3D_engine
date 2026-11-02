#pragma once

#include <optional>
#include <unordered_set>

#include "graphics.h"
#include "attrib.h"
#include "vector_3.h"

namespace config_ns
{

using graphics = graphics_ns::graphics;
using vector_3 = vector_3_ns::vector_3;
using attrib = attrib_ns::attrib;
using errors_t = std::vector<std::string>;
using att_names_t = std::map<uint32_t, std::string_view>;
using coord_names_t = std::map<uint32_t, std::string_view>;

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

enum light_type
{
	__lt_min__,               // dummy
	directional = __lt_min__, // sun-like
	point,                    // light-bulb
	__lt_max__                // dummy
};

struct polygon_def
{
	// required fields
	std::string name;
	uint32_t color_index {0};
	bool force {false};
	std::vector<vector_3> vectors;
};

struct mesh_def
{
	// required fields
	std::string name;
	std::vector<std::string> polygons;
};

struct lib_elem_def
{
	// required fields
	std::string name;
	std::string mesh;
};

struct scene_elem_def
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

// merged during build from lib_elem_def + scene_elem_def
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

struct resolution_def
{
	// required fields
	uint32_t width {0};
	uint32_t height {0};
};

struct runtime_def
{
	// optional fields
	std::optional<int> loops;
	std::optional<uint32_t> loop_delay;
};

struct environ_def
{
	// optional fields
	std::optional<resolution_def> resolution;
	std::optional<graphics::color_idx> color_idx;
	std::optional<uint32_t> light_type;
	std::optional<vector_3> light_direction;
	std::optional<vector_3> light_position;
	std::optional<vector_3> camera_position;
	std::optional<unit> focal_len;
	std::optional<unit> near_eps;
};

struct library_def
{
	// required fields
	std::vector<polygon_def> polygons;
	std::vector<mesh_def> meshes;
	std::vector<lib_elem_def> lib_elem_defs;
};

struct scene_def
{
	// required fields
	std::string name;
	std::vector<scene_elem_def> scene_elem_defs;

	// optional fields
	std::optional<environ_def> env;
	std::optional<runtime_def> run;
};

struct validate_def
{
	graphics::color_idx max_color_idx;
};

struct AST
{
	library_def library_defs;
	std::vector<scene_def> scene_defs;
	validate_def valids;
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
