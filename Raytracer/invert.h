#pragma once
class invert
{
public:
	void invert_matrix(double A[4][4], double Ainv[4][4]);
	void adjoint(double in[4][4], double out[4][4]);
	double det4x4(double m[4][4]);
	double det3x3(double a1, double a2, double a3, double b1, double b2, double b3, double c1,
		double c2, double c3);
	double det2x2(double a, double b, double c, double d);
};

