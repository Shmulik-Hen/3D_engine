#pragma once

#include <cstdint>

#include "attrib.h"
#include "unit.h"
#include "utils.h"
#include "vector_3.h"

// clang-format off
namespace attrib_ns { class attrib; }
namespace vector_3_ns { class vector_3; }
// clang-format on

namespace matrix_ns
{

using attrib = attrib_ns::attrib;
using vector_3 = vector_3_ns::vector_3;

const uint32_t dim = O_ + 1;

class matrix
{
	class vector_4
	{
	public:

		unit _coords[dim];

		vector_4();
		vector_4(const vector_4&);
		vector_4(const unit[dim]);
		vector_4(const unit, const unit, const unit, const unit);
		~vector_4() {}

		vector_4 operator+(const vector_4&) const;
		vector_4 operator-(const vector_4&) const;
		vector_4& operator=(const vector_4&);
		vector_4& operator+=(const vector_4&);
		vector_4& operator-=(const vector_4&);
		unit operator*(const vector_4&) const;
	};

	unit _mat[dim][dim];
	vector_4 _rows[dim];
	vector_4 _cols[dim];

	void prep_row(uint32_t);
	void prep_cols();

public:

	matrix();
	matrix(const matrix&);
	matrix(unit[dim][dim]);
	~matrix() {}

	void print() const;

	matrix operator*(const matrix&) const;
	matrix& operator=(const matrix& m);
	matrix& operator*=(const matrix&);
	vector_3 operator*(const vector_3&) const;

	void prep_trans_mat(const attrib&);
	void prep_rot_mat(const attrib&);
};

matrix get_unit_mat();

} // namespace matrix_ns
