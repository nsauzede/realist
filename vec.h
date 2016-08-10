#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <iostream>

template<int n> class vec;
typedef vec<3> v3;
template<int n> class vec {
public:
	vec( double d1 = 0, double d2 = 0, double d3 = 0) {
		m_d[0] = d1;
		m_d[1] = d2;
		m_d[2] = d3;
	}
	vec( const double *d) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] = *d++;
		}
	}
	const double& at( unsigned i) const {
		assert( (i < n));
		return m_d[i];
	}
	vec operator^( const vec& r) const {
		vec res;
		res.m_d[0] = this->m_d[1] * r.m_d[2] - this->m_d[2] * r.m_d[1];
		res.m_d[1] = this->m_d[2] * r.m_d[0] - this->m_d[0] * r.m_d[2];
		res.m_d[2] = this->m_d[0] * r.m_d[1] - this->m_d[1] * r.m_d[0];
		return res;
	}
	const vec& operator/=( const double& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] /= r;
		}
		return *this;
	}
	vec operator/( const double& r) const {
		vec res = *this;
		res /= r;
		return res;
	}
	const vec& operator*=( const double& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] *= r;
		}
		return *this;
	}
	vec operator*( const double& r) const {
		vec res = *this;
		res *= r;
		return res;
	}
	const vec& operator+=( const vec& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] += r.m_d[ii];
		}
		return *this;
	}
	vec operator+( const vec& r) const {
		vec res = *this;
		res += r;
		return res;
	}
	const vec& operator-=( const vec& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] -= r.m_d[ii];
		}
		return *this;
	}
	vec operator-( const vec& r) const {
		vec res = *this;
		res -= r;
		return res;
	}
	double operator!() const {
		return sqrt( *this % *this);
	}
	const double& operator[]( unsigned i) const {
		assert( (i < n));
		return m_d[i];
	}
	double& operator[]( unsigned i) {
		assert( (i < n));
		return m_d[i];
	}
	vec operator~() const {
		vec res = *this;
		res /= !res;
		return res;
	}
	double operator%( const vec& r) const {
		double result = 0;
		for (unsigned ii = 0; ii < n; ii++) {
			result += m_d[ii] * r.m_d[ii];
		}
		return result;
	}
	void Print() const {
		for (unsigned ii = 0; ii < n; ii++) {
			if (ii > 0)
				printf( ",");
			printf( "%f", m_d[ii]);
		}
		printf( "\n");
	}
	void Print( std::ostream& out) const {
		for (unsigned ii = 0; ii < n; ii++) {
			if (ii > 0)
				out << ", ";
			out << m_d[ii];
		}
	}
	void Json( std::ostream& out) const {
		out << "[";
		for (unsigned ii = 0; ii < n; ii++) {
			if (ii > 0)
				out << ", ";
			out << m_d[ii];
		}
		out << "]";
	}
	friend std::ostream& operator<<( std::ostream& out, const vec<n>& v) {
		for (unsigned ii = 0; ii < n; ii++) {
			if (ii > 0)
				out << " ";
			out << v[ii];
		}
		return out;
	}
private:
	double m_d[n];
};

inline void vprint( const v3 &v) {
	printf( "%f,%f,%f\n", v[0], v[1], v[2]);
}

inline void vprint( const char *t, const v3 &v) {
	printf( "%s={%f,%f,%f}\n", t, v[0], v[1], v[2]);
}
