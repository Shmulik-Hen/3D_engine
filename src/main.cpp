#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <list>

// #define DEBUG_PRINTS
#include "utils.h"
#include "scene.h"

using namespace attrib_ns;
using namespace element_ns;
using namespace graphics_ns;
using namespace scene_ns;
using namespace config_ns;
using attrib = attrib_ns::attrib;
using element = element_ns::element;
using graphics = graphics_ns::graphics;
using input_state = graphics_ns::graphics::input_state;
namespace fs = std::filesystem;

constexpr std::string_view def_conf_name = "cube";
constexpr std::string_view def_conf_path = "../config/";
constexpr std::string_view def_file_name = "config";
constexpr std::string_view ext = ".json";
constexpr std::string_view ver = "JSON configuration version";
const std::string version = JSON_STR_VER + PFX + std::to_string(APP_VER_MAJ) + DOT + std::to_string(APP_VER_MIN) + DOT + std::to_string(APP_VER_REV);
const std::string json_version(ver + PFX + std::to_string(JSON_VER_MAJ) + DOT + std::to_string(JSON_VER_MIN) + DOT + std::to_string(JSON_VER_REV));
const std::string def_conf_file = def_conf_path + def_file_name + ext;

void usage(std::string str)
{
	// clang-format off
	fprintf(stdout, "\nUsage: %s [OPTIONS]... [CONFIG_FILE] [CONFIG_NAME]\n", str.c_str());
	fprintf(stdout, "Render animated 3D objects\n\n");
	fprintf(stdout, "OPTIONS:\n");
	fprintf(stdout, "  -c, --conf <config name>\tspecify a configuration name to choose (default 'config')\n");
	fprintf(stdout, "  -f, --file <config file>\tspecify a configuration JSON file name (default config.json)\n");
	fprintf(stdout, "  -h, --help\t\t\tdisplay this help and exit\n");
	fprintf(stdout, "  -u, --usage\t\t\tdisplay this help and exit\n");
	fprintf(stdout, "  -v, --version\t\t\toutput version information and exit\n");
	fprintf(stdout, "\n");
	// clang-format on
}

int main(int argc, char** argv)
{
	try {
		// Define the long/short options
		const char* shortops = "c:f:huv";
		// clang-format off
		const struct option longopts[] = {
			{ "conf",    required_argument, 0, 'c'},
			{ "file",    required_argument, 0, 'f'},
			{ "help",    no_argument,       0, 'h'},
			{ "usage",   no_argument,       0, 'u'},
			{ "version", no_argument,       0, 'v'},
			{ 0, 0, 0, 0}  // Terminator
		};
		// clang-format on

		std::string filename;
		std::string conf_name;
		std::string given_file;
		std::string given_conf;
		std::string full_path = argv[0];
		fs::path p(full_path);
		std::string path = p.parent_path().string();
		std::string prog = p.filename().string();
		int index;
		int c;

		while ((c = getopt_long(argc, argv, shortops, longopts, &index)) != -1) {
			DBG("getopt() returned '" << (char)c << "' index:" << index);
			switch (c) {
			case 'c':
				given_conf = optarg;
				DBG("got a configuration name: " << given_conf);
				break;
			case 'f':
				given_file = optarg;
				DBG("got a configuration file: " << given_file);
				break;
			case 'h':
			case 'u':
			case '?':
				usage(prog);
				return 0;
			case 'v':
				std::cout << version << std::endl
					  << json_version << std::endl;
				return 0;
			default:
				std::cerr << "?? getopt_long returned character code 0"
					  << std::oct << c << std::endl;
				usage(prog);
				return 0;
			}
		}

		// Process any remaining non-option arguments
		if (optind < argc) {
			DBG("Non-option arguments: ");
			while (optind < argc) {
				DBG(argv[optind]);
				if (!given_file.length()) {
					given_file = argv[optind];
					DBG("got a configuration file: " << given_file);
				}
				else if (!given_conf.length()) {
					given_conf = argv[optind];
					DBG("got a configuration name: " << given_conf);
				}
				optind++;
			}
		}

		if (argc == 1) {
			filename = def_conf_file;
			conf_name = def_conf_name;
			DBG("  using default config file '" << filename << "'");
			DBG("  using default config name '" << conf_name << "'");
		}
		else {
			if (given_file.length()) {
				filename = given_file;
				DBG("  using config file '" << filename << "'");
			}
			else {
				filename = def_conf_file;
				DBG("  using default config file '" << filename << "'");
			}

			if (given_conf.length()) {
				conf_name = given_conf;
				DBG("  using config name '" << conf_name << "'");
			}
			else {
				conf_name = def_conf_name;
				DBG("  using default config name '" << conf_name << "'");
			}
		}

		std::unique_ptr<my_scene> scene_unique = std::make_unique<my_scene>();
		my_scene* scene = scene_unique.get();

		if (!(scene &&
		      scene->frame_ctx.draw_vec.get() &&
		      scene->frame_ctx.state.get())) {
			sys_error("failed to initialize scene");
		}

		DBG("file name" << PFX << filename);
		std::ifstream ifs;
		ifs.open(filename.c_str(), std::ios::in);
		if (!ifs) {
			sys_error("file not found: ", filename.c_str());
		}

		ifs.unsetf(std::ios::skipws);
		bool rc = scene->parse(ifs);
		ifs.close();
		if (!rc) {
			sys_error("scene->parse() failed");
		}

		rc = scene->validate();
		if (!rc) {
			sys_error("scene->validate() failed");
		}

		config_ns::parse_scn_def* scn = scene->find_scene(conf_name);
		if (!scn) {
			sys_error("couldn't find a scene with name ", conf_name.c_str());
		}

		scene->build(*scn);
		scene->init_graphics();
		graphics* gfx = scene->frame_ctx.state->grfx.gfx.get();
		if (!gfx) {
			sys_error("graphics system has not been initialized yet");
		}

#if 0
		if (!scene->root) {
			sys_error("root is null");
		}

		if (scene->poly_list.empty()) {
			sys_error("no polygons");
		}

		// find the relevant elements to work with
		constexpr std::string_view world_name = "world";
		constexpr std::string_view gimbal_name = "gimbal";
		constexpr std::string_view cube_name = "cube";
		constexpr std::string_view pyramid_name = "pyramid";
		element* cube = nullptr;
		element* pyramid = nullptr;

		element* world = scene->root->find(scene->root, world_name);
		if (!world) {
			sys_error("find world failed");
		}

		element* gimbal = scene->root->find(scene->root, gimbal_name);

		if (conf_name == cube_name) {
			cube = scene->root->find(scene->root, cube_name);
		}
		else if (conf_name == pyramid_name) {
			pyramid = scene->root->find(scene->root, pyramid_name);
		}
		else {
			cube = scene->root->find(scene->root, cube_name);
			pyramid = scene->root->find(scene->root, pyramid_name);
		}

#ifdef DEBUG_PRINTS
		DBG("initial:");
		DBG("print tree");
		scene->root->print_all();
		DBG("walk list");
		for (const auto poly : scene->poly_list) {
			poly->print();
		}
#endif // DEBUG_PRINTS

		// attrib(rotationX, rotationY, rotationZ, translationX, translationY, translationZ, scale)
		attrib keep_moving(0, 0, 0, 0, 0, 0, 1);
#endif // 0

		input_state in {};

		while (!in.quit) {
			gfx->poll_events(in);
			DBG("in.quit: " << in.quit << " in.esc: " << in.key_escape);
#if 0

			if (gimbal) {
				if (gimbal->has_motion()) {
					gimbal->update();
				}
				else {
					gimbal->update(keep_moving);
				}
			}

			if (cube) {
				if (cube->has_motion()) {
					cube->update();
				}
				else {
					cube->update(keep_moving);
				}
			}

			if (pyramid) {
				if (pyramid->has_motion()) {
					pyramid->update();
				}
				else {
					pyramid->update(keep_moving);
				}
			}

			scene->update();
#endif // 0
			scene->render();
			if (!scene->keep_going()) {
				break;
			}
		}

		return 0;
	}
	catch (const std::exception& e) {
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
