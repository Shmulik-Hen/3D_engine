#pragma once

#include <cstdint>
#include <string_view>

#include "unit.h"

// constexpr values
constexpr unit UNIT(1.0f); // a '1' in engines units
constexpr unit ZERO(0.0f); // a '0' in engines units
constexpr unit PI(3.14159265358979323846f);
constexpr unit DEG(180.0f);
constexpr unit DEG_TO_RAD(PI / DEG);
constexpr unit EPSILON(1e-5f);    // near zero cutoff point to avoid random sign flips
constexpr unit DIST_ATTEN(3e-5f); // light attenuation factor according to object's distance from light source
constexpr int FORWARD_LOOKING_SIGN(-1);
// Scale factor for EPSILON when computing the squared tolerance
// for detecting nearly-duplicate polygon vectors.
constexpr unit CONSEC_TOLERANCE_SCALE(10.0f);
constexpr unit NORMAL_SCALE_FACTOR(50.0f); // polygon debugging
constexpr std::string_view JSON_FIELD_VALUES_REJECTED_CHARS = "()[]{}'`,.;:\"\\";

// versions
constexpr uint32_t APP_VER_MAJ(0U);
constexpr uint32_t APP_VER_MIN(10U);
constexpr uint32_t APP_VER_REV(1U);
constexpr uint8_t MIN_JSON_VER_MAJ(0U);
constexpr uint8_t MIN_JSON_VER_MIN(0U);
constexpr uint8_t MIN_JSON_VER_REV(3U);
constexpr uint8_t MAX_JSON_VER_MAJ(0U);
constexpr uint8_t MAX_JSON_VER_MIN(0U);
constexpr uint8_t MAX_JSON_VER_REV(4U);
constexpr uint32_t JSON_VER_FIELDS(3U);

// coordinates
constexpr uint32_t X_(0U);
constexpr uint32_t Y_(1U);
constexpr uint32_t Z_(2U);
constexpr uint32_t O_(3U);

// array sizes
constexpr uint32_t NUM_ATTRIBUTES(7U);
constexpr uint32_t NUM_COORDS(O_);
constexpr uint32_t MIN_VECTORS(3U);
constexpr uint32_t MAX_VECTORS(8U);
constexpr uint32_t MIN_POLYGONS(1U);
constexpr uint32_t MIN_MESHES(1U);
constexpr uint32_t MIN_LIB_ELEMS(1U);
constexpr uint32_t MIN_LIBRARY(1U);
constexpr uint32_t MIN_SCN_ELEMS(2U);
constexpr uint32_t MIN_SCENES(1U);

// validation limits

// screen resolution (1 based)
constexpr uint32_t MIN_RES_WIDTH(800U); // DOS era vga (4:3)
constexpr uint32_t MIN_RES_HEIGHT(600U);
constexpr uint32_t MAX_RES_WIDTH(1920U); // full HD 16:9
constexpr uint32_t MAX_RES_HEIGHT(1080U);
// screen pixel positions (zero based)
constexpr uint32_t MIN_X_POS(0U);
constexpr uint32_t MIN_Y_POS(0U);
// zero based; derived
constexpr uint32_t MAX_X_POS(MAX_RES_WIDTH - 1U);
constexpr uint32_t MAX_Y_POS(MAX_RES_HEIGHT - 1U);
// coords/attributes
constexpr unit MIN_REAL_VAL(-1000.0f);
constexpr unit MAX_REAL_VAL(1000.0f);
// environment
constexpr unit MIN_FOCAL_LEN(100.0f);
constexpr unit MAX_FOCAL_LEN(3000.0f);
constexpr unit MIN_NEAR_EPS(0.5f);
constexpr unit MAX_NEAR_EPSN(3.0f);
constexpr unit MIN_SCALE(0.1f);
constexpr unit MAX_SCALE(10.0f);
// runtime
constexpr uint32_t MIN_LOOP_DELAY(1U);
constexpr uint32_t MAX_LOOP_DELAY(200U);
constexpr int MIN_LOOPS(-1);
constexpr int MAX_LOOPS(100);

// default values

// reolution
constexpr uint32_t DEF_RES_WIDTH(1280U); // 16:10
constexpr uint32_t DEF_RES_HEIGHT(800U);
// environment
constexpr unit DEF_LIGHT_DIR_X(ZERO);
constexpr unit DEF_LIGHT_DIR_Y(ZERO);
constexpr unit DEF_LIGHT_DIR_Z(UNIT);
constexpr unit DEF_LIGHT_POS_X(ZERO);
constexpr unit DEF_LIGHT_POS_Y(20.0f);
constexpr unit DEF_LIGHT_POS_Z(75.0f);
constexpr unit DEF_CAM_POS_X(ZERO);
constexpr unit DEF_CAM_POS_Y(ZERO);
constexpr unit DEF_CAM_POS_Z(1000.0f);
constexpr unit DEF_FOCAL_LEN(2000.0f);
constexpr unit DEF_NEAR_EPS(UNIT);
constexpr unit DEF_SCALE(UNIT);
// runtime
constexpr int DEF_LOOP_NUM(-1); // endless
constexpr uint32_t DEF_LOOP_DELAY(20U);
