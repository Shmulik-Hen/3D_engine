#pragma once

#include "utils.h"
#include "unit.h"

namespace attrib_ns
{

class attrib
{
	unit _rot_x {ZERO};   // x-axis rotation
	unit _rot_y {ZERO};   // y-axis rotation
	unit _rot_z {ZERO};   // z-axis rotation
	unit _trans_x {ZERO}; // x-axis translation
	unit _trans_y {ZERO}; // y-axis translation
	unit _trans_z {ZERO}; // z-axis translation
	unit _scale {UNIT};   // scale factor
	unit _arr[NUM_ATTRIBUTES];

public:

	attrib() {}
	~attrib() {}
	attrib(const unit, const unit, const unit, const unit, const unit, const unit, const unit);
	attrib(const unit[NUM_ATTRIBUTES]);

	void print() const;

	unit get_rot_x() const { return _rot_x; };
	unit get_rot_y() const { return _rot_y; };
	unit get_rot_z() const { return _rot_z; };
	unit get_trans_x() const { return _trans_x; };
	unit get_trans_y() const { return _trans_y; };
	unit get_trans_z() const { return _trans_z; };
	unit get_scale() const { return _scale; };

	attrib& operator=(unit[NUM_ATTRIBUTES]);
	attrib& operator+=(const attrib&);
	inline unit operator[](uint32_t i) const { return (i < NUM_ATTRIBUTES) ? _arr[i] : ZERO; };
};

} // namespace attrib_ns
