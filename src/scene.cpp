#include <algorithm>
#include <thread>

// #define DEBUG_PRINTS
#include "scene.h"
#include "parse_json.h"
#include "validate.h"

namespace scene_ns
{

using namespace element_ns;
using namespace graphics_ns;
using namespace polygon_ns;
using element = element_ns::element;
using graphics = graphics_ns::graphics;
using polygon = polygon_ns::polygon;
using vector_3 = vector_3_ns::vector_3;

bool scene_ns::my_scene::_keep_going = true;

const std::string STR_LABEL = "Software 3D Engine";
const std::string STR_DEFS = "set defaults";

#ifdef DEBUG_PRINTS
// clang-format off
constexpr std::string_view DBG_LIGHT = "light";
constexpr std::string_view DBG_CAM   = "camera";
constexpr std::string_view DBG_PROJ  = "projection";
constexpr std::string_view DBG_BLD   = "build";
constexpr std::string_view DBG_SCR   = "screen";
constexpr std::string_view DBG_RUN   = "run";
constexpr std::string_view DBG_OVRD  = "override";
constexpr std::string_view DBG_MIN_POS  = "min_pos";
constexpr std::string_view DBG_MAX_POS  = "max_pos";
constexpr std::string_view DBG_MID_POS  = "mid_pos";

static void print_vector_values(const vector_3& vec, const std::string path)
{
	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		unit coord = vec.get(i);
		// clang-format off
		std::string str = path + config_ns::STR_CORD +
				  LB + config_ns::get_coord_name(i) + RB +
				  PFX;
		// clang-format on

		DBG(str + config_ns::set_precision(coord));
	}
}
// clang-format on
#endif // DEBUG_PRINTS

my_scene::my_scene()
{
	frame_ctx.draw_vec = std::make_unique<drawvec_t>();
	frame_ctx.state = std::make_unique<scene_state>();
	set_defaults();
}

void my_scene::init_graphics()
{
	// clang-format off
	frame_ctx.state->grfx.gfx =
		std::make_unique<graphics>(STR_LABEL.c_str(),
	                                   frame_ctx.state->grfx.width,
	                                   frame_ctx.state->grfx.height,
	                                   DEFAULT_SCALE);
	// clang-format on

	if (!frame_ctx.state->grfx.gfx.get()) {
		sys_error("failed to initialize graphics");
	}

	frame_ctx.state->grfx.bkg_color = frame_ctx.state->grfx.gfx.get()->get_color_val(frame_ctx.state->win.bkg_color_idx);
}

polygon* my_scene::add_polygon()
{
	polygons_owned.push_back(std::make_unique<polygon>());
	polygon* p = polygons_owned.back().get();
	poly_list.push_back(p);
	return p;
}

element* my_scene::add_element()
{
	elements_owned.push_back(std::make_unique<element>());
	return elements_owned.back().get();
}

bool my_scene::parse(std::ifstream& ifs)
{
	return config_ns::parse_json(ifs, ast);
}

bool my_scene::validate()
{
	return config_ns::validate_ast(ast);
}

config_ns::scene_def* my_scene::find_scene(const std::string& name)
{
	for (auto& scn : ast.scene_defs) {
		if (scn.name == name) {
			return &scn;
		}
	}

	return nullptr;
}

void my_scene::build(const config_ns::scene_def& scene_def)
{
#ifdef DEBUG_PRINTS
	// clang-format off
	const std::string pfx = config_ns::STR_SCN + PFX + DBG_BLD + PFX;
	// clang-format on
#endif // DEBUG_PRINTS

	DBG(pfx + scene_def.name + DOT);
	// DBG(DBG_BLD << PFX << num polygon defs: " << (int)ast.library_defs.polygons.size());
	// for (const auto& def : ast.library_defs.polygons) {
	// 	polygon* poly = add_polygon();
	// 	poly->init_from_def(frame_ctx, def);
	// 	DBG("build: polygon:");
	// 	poly->print();
	// }

	// DBG(DBG_BLD << PFX << num element defs: " << (int)ast.library_defs.elements.size());
	// for (const auto& def : ast.library_defs.lib_elem_defs) {
	// 	element* elem = add_element();
	// 	if (!root) {
	// 		root = elem;
	// 	}
	// 	elem->init_from_def(poly_list, root, def);
	// 	DBG("build: element:");
	// 	elem->print();
	// }

	if (scene_def.env.has_value()) {
#ifdef DEBUG_PRINTS
		const std::string env_str = pfx + config_ns::JSON_STR_ENV;
		DBG(env_str);
		// clang-format off
		const std::string path = env_str + PFX + DBG_OVRD + PFX;
		// clang-format on
#endif // DEBUG_PRINTS

		config_ns::environ_def env = scene_def.env.value();

		if (env.resolution.has_value()) {
			config_ns::resolution_def res = env.resolution.value();
			bool res_changed = false;

			if (res.width != frame_ctx.state->grfx.width) {
				frame_ctx.state->grfx.width = res.width;
				frame_ctx.state->win.max_pos.x = res.width - 1;
				res_changed = true;
				// clang-format off
				DBG(path + config_ns::JSON_STR_RES + PFX +
				    config_ns::STR_WID + PFX +
				    std::to_string(frame_ctx.state->grfx.width));
				// clang-format on
			}

			if (res.height != frame_ctx.state->grfx.height) {
				frame_ctx.state->grfx.height = res.height;
				frame_ctx.state->win.max_pos.y = res.height - 1;
				res_changed = true;
				// clang-format off
				DBG(path + config_ns::JSON_STR_RES + PFX +
				    config_ns::STR_HGT + PFX +
				    std::to_string(frame_ctx.state->grfx.height));
				// clang-format on
			}

			if (res_changed) {
				frame_ctx.state->win.mid_pos.x = frame_ctx.state->win.max_pos.x / 2;
				frame_ctx.state->win.mid_pos.y = frame_ctx.state->win.max_pos.y / 2;
			}
		}

		if (env.color_idx.has_value() &&
		    frame_ctx.state->win.bkg_color_idx != env.color_idx.value()) {
			frame_ctx.state->win.bkg_color_idx = env.color_idx.value();
			// clang-format off
			DBG(path + config_ns::JSON_STR_COLOR + PFX +
			    std::to_string(frame_ctx.state->win.bkg_color_idx));
			// clang-format on
		}

		if (env.light_type.has_value() &&
		    frame_ctx.state->light.type != env.light_type.value()) {
			// clang-format off
			frame_ctx.state->light.type =
				env.light_type.value() ?
					config_ns::light_type::point :
					config_ns::light_type::directional;
			DBG(path + config_ns::JSON_STR_LIGHT_TYPE + PFX +
			    std::to_string((int)frame_ctx.state->light.type));
			// clang-format on
		}

		if (env.light_direction.has_value() &&
		    frame_ctx.state->light.direction != env.light_direction.value()) {
			frame_ctx.state->light.direction = env.light_direction.value();
#ifdef DEBUG_PRINTS
			// clang-format off
			const std::string loc_path = path +
						     config_ns::JSON_STR_LIGHT_DIR +
						     PFX;
			// clang-format on
			print_vector_values(frame_ctx.state->light.direction, loc_path);
#endif // DEBUG_PRINTS
		}

		if (env.light_position.has_value() &&
		    frame_ctx.state->light.type == config_ns::point &&
		    frame_ctx.state->light.position != env.light_position.value()) {
			frame_ctx.state->light.position = env.light_position.value();
#ifdef DEBUG_PRINTS
			// clang-format off
			const std::string loc_path = path +
						     config_ns::JSON_STR_LIGHT_POS +
						     PFX;
			// clang-format on
			print_vector_values(frame_ctx.state->light.position, loc_path);
#endif // DEBUG_PRINTS
		}

		if (env.camera_position.has_value() &&
		    frame_ctx.state->camera.position != env.camera_position.value()) {
			frame_ctx.state->camera.position = env.camera_position.value();
#ifdef DEBUG_PRINTS
			// clang-format off
			const std::string loc_path = path +
						     config_ns::JSON_STR_CAM_POS +
						     PFX;
			// clang-format on
			print_vector_values(frame_ctx.state->camera.position, loc_path);
#endif // DEBUG_PRINTS
		}

		if (env.focal_len.has_value() &&
		    frame_ctx.state->proj.focal_len != env.focal_len.value()) {
			frame_ctx.state->proj.focal_len = env.focal_len.value();
#ifdef DEBUG_PRINTS
			// clang-format off
			DBG(path + config_ns::JSON_STR_FOCAL_LEN + PFX +
			    config_ns::set_precision(frame_ctx.state->proj.focal_len));
			// clang-format on
#endif // DEBUG_PRINTS
		}

		if (env.near_eps.has_value() &&
		    frame_ctx.state->proj.near_eps != env.near_eps.value()) {
			frame_ctx.state->proj.near_eps = env.near_eps.value();
			// clang-format off
			DBG(path + config_ns::JSON_STR_NEAR_EPS + PFX +
			    config_ns::set_precision(frame_ctx.state->proj.near_eps));
			// clang-format on
		}
	}

	if (scene_def.run.has_value()) {
#ifdef DEBUG_PRINTS
		// clang-format off
		const std::string path = config_ns::STR_SCN + PFX + DBG_BLD +
					 PFX + config_ns::JSON_STR_RUN + PFX +
					 DBG_OVRD + PFX;
		// clang-format on
#endif // DEBUG_PRINTS

		config_ns::runtime_def run = scene_def.run.value();
		if (run.loops.has_value() &&
		    frame_ctx.state->run.loops != run.loops.value()) {
			frame_ctx.state->run.loops = run.loops.value();
			// clang-format off
			DBG(path + config_ns::JSON_STR_LOOPS + PFX +
			    std::to_string(frame_ctx.state->run.loops));
			// clang-format on
		}

		if (run.loop_delay.has_value() &&
		    frame_ctx.state->run.loop_delay != run.loop_delay.value()) {
			frame_ctx.state->run.loop_delay = run.loop_delay.value();
			// clang-format off
			DBG(path + config_ns::JSON_STR_LOOP_DELAY + PFX +
			    std::to_string(frame_ctx.state->run.loop_delay));
			// clang-format on
		}
	}
}

void my_scene::update()
{
	// draw list is per-frame (non-owning)
	frame_ctx.draw_vec->clear();

	// update tree + polygons; element/polygon code appends to draw_vec
	root->update_all(frame_ctx);

#ifdef DEBUG_PRINTS
	// DBG("print tree");
	// root->print_all();
#endif
}

void my_scene::sort()
{
	// clang-format off
	std::sort(frame_ctx.draw_vec->begin(), frame_ctx.draw_vec->end(),
		  [](polygon* a, polygon* b) {
			return a->get_depth() > b->get_depth();
		  });
	// clang-format on
}

void my_scene::draw()
{
	for (polygon* poly : *frame_ctx.draw_vec) {
		if (poly) {
			poly->draw(frame_ctx);
		}
	}
}

void my_scene::render()
{
	static int loops = frame_ctx.state->run.loops;

	graphics* gfx = frame_ctx.state->grfx.gfx.get();
	if (!gfx) {
		sys_error("graphics system has not been initialized yet");
	}

	// Acquire backbuffer for this frame and clear it
	frame_ctx.state->grfx.fb = gfx->get_backbuffer();
	gfx->fill_buffer(frame_ctx.state->grfx.fb, frame_ctx.state->grfx.bkg_color);

	if (!frame_ctx.draw_vec->empty()) {
		// DBG("sort");
		sort();
	}

	// DBG("draw");
	draw();

	// DBG("show");
	gfx->present();

	std::this_thread::sleep_for(std::chrono::milliseconds(frame_ctx.state->run.loop_delay));

	if (frame_ctx.state->run.loops > 0) {
		if (--loops < 1) {
			_keep_going = false;
		}
	}
}

bool my_scene::keep_going()
{
	return _keep_going;
}

void my_scene::set_defaults()
{
	std::string pfx = config_ns::STR_SCN + PFX + STR_DEFS;
	DBG(pfx);
	pfx += PFX;

	// drawing vector
	frame_ctx.draw_vec->reserve(256);

	// light
	frame_ctx.state->light.type = config_ns::light_type::directional;
	frame_ctx.state->light.direction = {DEF_LIGHT_DIR_X, DEF_LIGHT_DIR_Y, DEF_LIGHT_DIR_Z};
	frame_ctx.state->light.position = {DEF_LIGHT_POS_X, DEF_LIGHT_POS_Y, DEF_LIGHT_POS_Z};
	// camera
	frame_ctx.state->camera.position = {DEF_CAM_POS_X, DEF_CAM_POS_Y, DEF_CAM_POS_Z};
#ifdef DEBUG_PRINTS
	// clang-format off
	std::string path = pfx + DBG_LIGHT + PFX;
	DBG(path + config_ns::JSON_STR_LIGHT_TYPE + PFX +
	    std::to_string((int)frame_ctx.state->light.type));
	print_vector_values(frame_ctx.state->light.direction, path +
			    config_ns::JSON_STR_LIGHT_POS + PFX);
	print_vector_values(frame_ctx.state->light.position, path +
			    config_ns::JSON_STR_LIGHT_DIR + PFX);
	print_vector_values(frame_ctx.state->camera.position, path +
			    config_ns::JSON_STR_CAM_POS + PFX);
	// clang-format on
#endif // DEBUG_PRINTS

	// projection
	frame_ctx.state->proj.focal_len = DEF_FOCAL_LEN;
	frame_ctx.state->proj.near_eps = DEF_NEAR_EPS;
#ifdef DEBUG_PRINTS
	path = pfx + DBG_PROJ + PFX;
	// clang-format off
	DBG(path + config_ns::JSON_STR_FOCAL_LEN + PFX +
	    config_ns::set_precision(frame_ctx.state->proj.focal_len));
	DBG(path + config_ns::JSON_STR_NEAR_EPS + PFX +
	     config_ns::set_precision(frame_ctx.state->proj.near_eps));
	// clang-format on
#endif // DEBUG_PRINTS

	// screen
	frame_ctx.state->win.min_pos = {MIN_X_POS, MIN_Y_POS};
	frame_ctx.state->win.max_pos = {DEF_RES_WIDTH - 1, DEF_RES_HEIGHT - 1};
	frame_ctx.state->win.mid_pos.x = frame_ctx.state->win.max_pos.x / 2;
	frame_ctx.state->win.mid_pos.y = frame_ctx.state->win.max_pos.y / 2;
	frame_ctx.state->win.bkg_color_idx = graphics::color_idx::black;
#ifdef DEBUG_PRINTS
	path = pfx + DBG_SCR + PFX;
	// clang-format off
	DBG(path + DBG_MIN_POS + PFX  + LB +
	    std::to_string(frame_ctx.state->win.min_pos.x) + SEP +
	    std::to_string(frame_ctx.state->win.min_pos.y) + RB);
	DBG(path + DBG_MAX_POS + PFX  + LB +
	    std::to_string(frame_ctx.state->win.max_pos.x) + SEP +
	    std::to_string(frame_ctx.state->win.max_pos.y) + RB);
	DBG(path + DBG_MID_POS + PFX  + LB +
	    std::to_string(frame_ctx.state->win.mid_pos.x) + SEP +
	    std::to_string(frame_ctx.state->win.mid_pos.y) + RB);
	DBG(path + config_ns::JSON_STR_COLOR + PFX +
	    std::to_string(frame_ctx.state->win.bkg_color_idx));
	// clang-format on
#endif // DEBUG_PRINTS

	// runtime
	frame_ctx.state->run.loops = DEF_LOOP_NUM;
	frame_ctx.state->run.loop_delay = DEF_LOOP_DELAY;
#ifdef DEBUG_PRINTS
	path = pfx + DBG_RUN + PFX;
	// clang-format off
	DBG(path + config_ns::JSON_STR_LOOPS +
	    PFX + std::to_string(frame_ctx.state->run.loops));
	DBG(path + config_ns::JSON_STR_LOOP_DELAY +
	    PFX + std::to_string(frame_ctx.state->run.loop_delay));
	// clang-format on
#endif // DEBUG_PRINTS

	// limits for validations
	ast.valids.max_color_idx = (graphics::color_idx)((uint32_t)graphics::color_idx::__last_color__ - 1);
}

} // namespace scene_ns
