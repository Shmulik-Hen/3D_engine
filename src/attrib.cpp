// #define DEBUG_PRINTS
#include "attrib.h"
#include "utils.h"

namespace attrib_ns
{

// clang-format off
attrib::attrib(const unit dx, const unit dy, const unit dz,
	       const unit ox, const unit oy, const unit oz,
	       const unit z) :
	_rot_x {dx}, _rot_y {dy}, _rot_z {dz},
	_trans_x {ox}, _trans_y {oy}, _trans_z {oz},
	_scale {z},
	_arr {_rot_x, _rot_y, _rot_z, _trans_x, _trans_y, _trans_z, _scale}
{
}

attrib::attrib(const unit atts[NUM_ATTRIBUTES]) :
	_rot_x {atts[0]}, _rot_y {atts[1]}, _rot_z {atts[2]},
	_trans_x {atts[3]}, _trans_y {atts[4]}, _trans_z {atts[5]},
	_scale {atts[6]},
	_arr {_rot_x, _rot_y, _rot_z, _trans_x, _trans_y, _trans_z, _scale}

{
}
// clang-format on

void attrib::print() const
{
	DBG("  attrib:" << ENDL
	                << "            rotate x:    " << FLT(_rot_x, 6) << ENDL
	                << "            rotate y:    " << FLT(_rot_y, 6) << ENDL
	                << "            rotate z:    " << FLT(_rot_z, 6) << ENDL
	                << "            translate x: " << FLT(_trans_x, 6) << ENDL
	                << "            translate y: " << FLT(_trans_y, 6) << ENDL
	                << "            translate z: " << FLT(_trans_z, 6) << ENDL
	                << "            scale:       " << FLT(_scale, 6) << ENDL);
}

attrib& attrib::operator=(unit atts[NUM_ATTRIBUTES])
{
	_rot_x = atts[0];
	_rot_y = atts[1];
	_rot_z = atts[2];
	_trans_x = atts[3];
	_trans_y = atts[4];
	_trans_z = atts[5];
	_scale = atts[6];
	return *this;
}

attrib& attrib::operator+=(const attrib& a)
{
	_rot_x += a._rot_x;
	_rot_y += a._rot_y;
	_rot_z += a._rot_z;
	_trans_x += a._trans_x;
	_trans_y += a._trans_y;
	_trans_z += a._trans_z;
	_scale *= a._scale;
	return *this;
}

} // namespace attrib_ns
