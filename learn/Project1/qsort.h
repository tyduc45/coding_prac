#pragma once

#include<iostream>
#include<random>
#include<assert.h>

double sqrt_self(double a)
{
	assert(a >= 0, "Input must be greater than 0");
	if (a == 0) return 0;

	double eps = 1e-10;
	double x = a / 2.0;

	while (fabs(x*x - a) > eps)
	{
		x = (x + a / x) * 0.5;
	}
	return x;
}

double Q_InvSqrt(double a)
{
	assert(a >= 0 && "Input must be greater than 0");
	long long i; double y;
	memcpy(&i, &a, sizeof(i));// evil floating point bit level hacking
	i = 0x5FE6EB50C7B537A9LL - (i >> 1); // what the fuck
	memcpy(&y, &i, sizeof(y));
	y = y * (1.5 - (a * 0.5 * y * y)); // 1st iteration
	return y;
}

double Q_sqrt(double a)
{
	assert(a >= 0 && "Input must be greater than 0");
	long long i; double y;
	memcpy(&i, &a, sizeof(i));// evil floating point bit level hacking
	i = 0x1FF7A31BEA2C429CLL + (i >> 1); // what the fuck
	memcpy(&y, &i, sizeof(y));
	y = 0.5 * (y + a / y); // 1st iteration
	return y;
}