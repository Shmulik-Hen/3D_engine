#include <cmath>

// #define DEBUG_PRINTS
#include "matrix.h"

namespace matrix_ns
{

// clang-format off
unit unit_mat[dim][dim] = {
	{ UNIT, ZERO, ZERO, ZERO },
	{ ZERO, UNIT, ZERO, ZERO },
	{ ZERO, ZERO, UNIT, ZERO },
	{ ZERO, ZERO, ZERO, UNIT }
};
// clang-format on

matrix::vector_4::vector_4()
{
	for (uint32_t i = X_; i <= O_; i++) {
		_coords[i] = ZERO;
	}
}

matrix::vector_4::vector_4(const vector_4& v)
{
	for (uint32_t i = X_; i <= O_; i++) {
		_coords[i] = v._coords[i];
	}
}

matrix::vector_4::vector_4(const unit v[dim])
{
	for (uint32_t i = X_; i <= O_; i++) {
		_coords[i] = v[i];
	}
}

matrix::vector_4::vector_4(const unit x, const unit y, const unit z, const unit o)
{
	_coords[X_] = x;
	_coords[Y_] = y;
	_coords[Z_] = z;
	_coords[O_] = o;
}

matrix::vector_4 matrix::vector_4::operator+(const vector_4& v) const
{
	return vector_4(_coords[X_] + v._coords[X_],
	                _coords[Y_] + v._coords[Y_],
	                _coords[Z_] + v._coords[Z_],
	                _coords[O_] + v._coords[O_]);
}

matrix::vector_4 matrix::vector_4::operator-(const vector_4& v) const
{
	return vector_4(_coords[X_] - v._coords[X_],
	                _coords[Y_] - v._coords[Y_],
	                _coords[Z_] - v._coords[Z_],
	                _coords[O_] - v._coords[O_]);
}

matrix::vector_4& matrix::vector_4::operator=(const vector_4& v)
{
	if (this != &v) {
		for (uint32_t i = X_; i <= O_; i++) {
			_coords[i] = v._coords[i];
		}
	}
	return *this;
}

matrix::vector_4& matrix::vector_4::operator+=(const vector_4& v)
{
	_coords[X_] += v._coords[X_];
	_coords[Y_] += v._coords[Y_];
	_coords[Z_] += v._coords[Z_];
	_coords[O_] += v._coords[O_];

	return *this;
}

matrix::vector_4& matrix::vector_4::operator-=(const vector_4& v)
{
	_coords[X_] -= v._coords[X_];
	_coords[Y_] -= v._coords[Y_];
	_coords[Z_] -= v._coords[Z_];
	_coords[O_] -= v._coords[O_];

	return *this;
}

unit matrix::vector_4::operator*(const vector_4& v) const
{
	return unit(_coords[X_] * v._coords[X_] +
	            _coords[Y_] * v._coords[Y_] +
	            _coords[Z_] * v._coords[Z_] +
	            _coords[O_] * v._coords[O_]);
}

// ---------------------- end of class matrix::vector_4 -------------------------

matrix::matrix()
{
	for (uint32_t i = X_; i <= O_; i++) {
		for (uint32_t j = X_; j <= O_; j++) {
			_mat[i][j] = ZERO;
		}
		prep_row(i);
	}

	prep_cols();
}

matrix::matrix(const matrix& m)
{
	for (uint32_t i = X_; i <= O_; i++) {
		for (uint32_t j = 0; j <= O_; j++) {
			_mat[i][j] = m._mat[i][j];
		}
		prep_row(i);
	}

	prep_cols();
}

matrix::matrix(unit u[dim][dim])
{
	for (uint32_t i = X_; i <= O_; i++) {
		for (uint32_t j = X_; j <= O_; j++) {
			_mat[i][j] = u[i][j];
		}
		prep_row(i);
	}

	prep_cols();
}

void matrix::prep_row(uint32_t r)
{
	_rows[r] = _mat[r];
}

void matrix::prep_cols()
{
	for (uint32_t i = X_; i <= O_; i++) {
		_cols[i] = {_rows[X_]._coords[i],
		            _rows[Y_]._coords[i],
		            _rows[Z_]._coords[i],
		            _rows[O_]._coords[i]};
	}
}

void matrix::print() const
{
#ifdef DEBUG_PRINTS
	DBG("matrix:");
	for (uint32_t i = X_; i <= O_; i++) {
		for (uint32_t j = X_; j <= O_; j++) {
			DBG("  [" << i << "][" << j << "] : " << _mat[i][j]);
		}
	}
	DBG("");
#endif
}

matrix matrix::operator*(const matrix& m) const
{
	matrix temp;

	temp._mat[X_][X_] = _rows[X_] * m._cols[X_];
	temp._mat[X_][Y_] = _rows[X_] * m._cols[Y_];
	temp._mat[X_][Z_] = _rows[X_] * m._cols[Z_];
	temp._mat[X_][O_] = _rows[X_] * m._cols[O_];
	temp._mat[Y_][X_] = _rows[Y_] * m._cols[X_];
	temp._mat[Y_][Y_] = _rows[Y_] * m._cols[Y_];
	temp._mat[Y_][Z_] = _rows[Y_] * m._cols[Z_];
	temp._mat[Y_][O_] = _rows[Y_] * m._cols[O_];
	temp._mat[Z_][X_] = _rows[Z_] * m._cols[X_];
	temp._mat[Z_][Y_] = _rows[Z_] * m._cols[Y_];
	temp._mat[Z_][Z_] = _rows[Z_] * m._cols[Z_];
	temp._mat[Z_][O_] = _rows[Z_] * m._cols[O_];
	temp._mat[O_][X_] = _rows[O_] * m._cols[X_];
	temp._mat[O_][Y_] = _rows[O_] * m._cols[Y_];
	temp._mat[O_][Z_] = _rows[O_] * m._cols[Z_];
	temp._mat[O_][O_] = _rows[O_] * m._cols[O_];

	for (uint32_t i = X_; i <= O_; i++) {
		temp.prep_row(i);
	}
	temp.prep_cols();

	return temp;
}

matrix& matrix::operator=(const matrix& m)
{
	if (this != &m) {
		for (uint32_t i = X_; i <= O_; i++) {
			for (uint32_t j = X_; j <= O_; j++) {
				_mat[i][j] = m._mat[i][j];
			}
			prep_row(i);
		}
		prep_cols();
	}
	return *this;
}

matrix& matrix::operator*=(const matrix& m)
{
	*this = *this * m;
	return *this;
}

vector_3 matrix::operator*(const vector_3& v) const
{
	vector_3 v1(_mat[X_][X_], _mat[X_][Y_], _mat[X_][Z_]);
	vector_3 v2(_mat[Y_][X_], _mat[Y_][Y_], _mat[Y_][Z_]);
	vector_3 v3(_mat[Z_][X_], _mat[Z_][Y_], _mat[Z_][Z_]);

	return vector_3(vector_3::dot(v, v1) + _mat[X_][O_],
	                vector_3::dot(v, v2) + _mat[Y_][O_],
	                vector_3::dot(v, v3) + _mat[Z_][O_]);
}

void matrix::prep_trans_mat(const attrib& a)
{
	unit rx = a.get_rot_x() * DEG_TO_RAD;
	unit ry = a.get_rot_y() * DEG_TO_RAD;
	unit rz = a.get_rot_z() * DEG_TO_RAD;
	unit tx = a.get_trans_x();
	unit ty = a.get_trans_y();
	unit tz = a.get_trans_z();
	unit sc = a.get_scale();

	DBG("prep_trans_mat:");
	DBG("      rx:  " << FLT(rx, 6) << " ry: " << FLT(ry, 6) << " rz: " << FLT(rz, 6));
	DBG("      tx:  " << FLT(tx, 6) << " ty: " << FLT(ty, 6) << " tz: " << FLT(tz, 6));
	DBG("    scale: " << FLT(sc, 6));

	unit srx = std::sin(rx);
	unit sry = std::sin(ry);
	unit srz = std::sin(rz);
	unit crx = std::cos(rx);
	unit cry = std::cos(ry);
	unit crz = std::cos(rz);

	DBG("     srx: " << FLT(srx, 6) << " sry: " << FLT(sry, 6) << " srz: " << FLT(srz, 6));
	DBG("     crx: " << FLT(crx, 6) << " cry: " << FLT(cry, 6) << " crz: " << FLT(crz, 6) << ENDL);

	_mat[X_][X_] = (sc * cry * crz);
	_mat[X_][Y_] = (sc * -cry * srz);
	_mat[X_][Z_] = (sc * -sry);
	_mat[X_][O_] = tx;

	_mat[Y_][X_] = (sc * (crx * srz - srx * sry * crz));
	_mat[Y_][Y_] = (sc * (crx * crz + srx * sry * srz));
	_mat[Y_][Z_] = (sc * -srx * cry);
	_mat[Y_][O_] = ty;

	_mat[Z_][X_] = (sc * (srx * srz + crx * sry * crz));
	_mat[Z_][Y_] = (sc * (srx * crz - crx * sry * srz));
	_mat[Z_][Z_] = (sc * crx * cry);
	_mat[Z_][O_] = tz;

	_mat[O_][X_] = ZERO;
	_mat[O_][Y_] = ZERO;
	_mat[O_][Z_] = ZERO;
	_mat[O_][O_] = UNIT;

	for (uint32_t i = X_; i <= O_; i++) {
		prep_row(i);
	}
	prep_cols();

#ifdef DEBUG_PRINTS
	print();
#endif
}

void matrix::prep_rot_mat(const attrib& a)
{
	unit rx = a.get_rot_x() * DEG_TO_RAD;
	unit ry = a.get_rot_y() * DEG_TO_RAD;
	unit rz = a.get_rot_z() * DEG_TO_RAD;

	DBG("prep_rot_mat:");
	DBG("      rx: " << FLT(rx, 6) << " ry: " << FLT(ry, 6) << " rz: " << FLT(rz, 6));

	unit srx = std::sin(rx);
	unit sry = std::sin(ry);
	unit srz = std::sin(rz);
	unit crx = std::cos(rx);
	unit cry = std::cos(ry);
	unit crz = std::cos(rz);

	DBG("     srx: " << FLT(srx, 6) << " sry: " << FLT(sry, 6) << " srz: " << FLT(srz, 6));
	DBG("     crx: " << FLT(crx, 6) << " cry: " << FLT(cry, 6) << " crz: " << FLT(crz, 6) << ENDL);

	_mat[X_][X_] = (cry * crz);
	_mat[X_][Y_] = (-cry * srz);
	_mat[X_][Z_] = (-sry);
	_mat[X_][O_] = ZERO;

	_mat[Y_][X_] = (crx * srz - srx * sry * crz);
	_mat[Y_][Y_] = (crx * crz + srx * sry * srz);
	_mat[Y_][Z_] = (-srx * cry);
	_mat[Y_][O_] = ZERO;

	_mat[Z_][X_] = (srx * srz + crx * sry * crz);
	_mat[Z_][Y_] = (srx * crz - crx * sry * srz);
	_mat[Z_][Z_] = (crx * cry);
	_mat[Z_][O_] = ZERO;

	_mat[O_][X_] = ZERO;
	_mat[O_][Y_] = ZERO;
	_mat[O_][Z_] = ZERO;
	_mat[O_][O_] = UNIT;

	for (uint32_t i = X_; i <= O_; i++) {
		prep_row(i);
	}
	prep_cols();

#ifdef DEBUG_PRINTS
	print();
#endif
}

matrix get_unit_mat()
{
	return unit_mat;
}

} // namespace matrix_ns
