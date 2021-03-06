#pragma once

#include "includes.h"
#include "simplexnoise.h"
#define TRUE    1
#define FALSE   0


/*
* Procedural fBm evaluated at "point"; returns value stored in "value".
*
* Copyright 1994 F. Kenton Musgrave
*
* Parameters:
*    ``H''  is the fractal increment parameter
*    ``lacunarity''  is the gap between successive frequencies
*    ``octaves''  is the number of frequencies in the fBm
*/
double
fBm(double3 point, double H, double lacunarity, double octaves)
{

	double            value, frequency, remainder, Noise3();
	int               i;
	static int        first = TRUE;
	static double     *exponent_array;

	/* precompute and store spectral weights */
	if (first) {
		/* seize required memory for exponent_array */
		exponent_array =
			(double *)malloc((octaves + 1) * sizeof(double));
		frequency = 1.0;
		for (i = 0; i <= octaves; i++) {
			/* compute weight for each frequency */
			exponent_array[i] = pow(frequency, -H);
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	value = 0.0;            /* initialize vars to proper values */
	frequency = 1.0;

	/* inner loop of spectral construction */
	for (i = 0; i<octaves; i++) {
		value += raw_noise_3d(point.x, point.y, point.z) * exponent_array[i];
		point.x *= lacunarity;
		point.y *= lacunarity;
		point.z *= lacunarity;
	} /* for */

	remainder = octaves - (int)octaves;
	if (remainder)      /* add in ``octaves''  remainder */
						/* ``i''  and spatial freq. are preset in loop above */
		value += remainder * raw_noise_3d(point.x, point.y, point.z) * exponent_array[i];

	return(value);

} /* fBm() */


  /*
  * Procedural multifractal evaluated at "point";
  * returns value stored in "value".
  *
  * Copyright 1994 F. Kenton Musgrave
  *
  * Parameters:
  *    ``H''  determines the highest fractal dimension
  *    ``lacunarity''  is gap between successive frequencies
  *    ``octaves''  is the number of frequencies in the fBm
  *    ``offset''  is the zero offset, which determines multifractality
  */
double
multifractal(double3 point, double H, double lacunarity,
	double octaves, double offset)
{
	double            value, frequency, remainder, Noise3();
	int               i;
	static int        first = TRUE;
	static double     *exponent_array;

	/* precompute and store spectral weights */
	if (first) {
		/* seize required memory for exponent_array */
		exponent_array =
			(double *)malloc((octaves + 1) * sizeof(double));
		frequency = 1.0;
		for (i = 0; i <= octaves; i++) {
			/* compute weight for each frequency */
			exponent_array[i] = pow(frequency, -H);
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	value = 1.0;            /* initialize vars to proper values */
	frequency = 1.0;

	/* inner loop of multifractal construction */
	for (i = 0; i<octaves; i++) {
		value *= offset * frequency * raw_noise_3d(point.x, point.y, point.z);
		point.x *= lacunarity;
		point.y *= lacunarity;
		point.z *= lacunarity;
	} /* for */

	remainder = octaves - (int)octaves;
	if (remainder)      /* add in ``octaves''  remainder */
						/* ``i''  and spatial freq. are preset in loop above */
		value += remainder * raw_noise_3d(point.x, point.y, point.z) * exponent_array[i];

	return value;

} /* multifractal() */


  /*
  * Heterogeneous procedural terrain function: stats by altitude method.
  * Evaluated at "point"; returns value stored in "value".
  *
  * Copyright 1994 F. Kenton Musgrave
  *
  * Parameters:
  *       ``H''  determines the fractal dimension of the roughest areas
  *       ``lacunarity''  is the gap between successive frequencies
  *       ``octaves''  is the number of frequencies in the fBm
  *       ``offset''  raises the terrain from `sea level'
  */
double
Hetero_Terrain(double3 point,
	double H, double lacunarity, double octaves, double offset)
{
	double          value, increment, frequency, remainder, Noise3();
	int             i;
	static int      first = TRUE;
	static double   *exponent_array;

	/* precompute and store spectral weights, for efficiency */
	if (first) {
		/* seize required memory for exponent_array */
		exponent_array =
			(double *)malloc((octaves + 1) * sizeof(double));
		frequency = 1.0;
		for (i = 0; i <= octaves; i++) {
			/* compute weight for each frequency */
			exponent_array[i] = pow(frequency, -H);
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	/* first unscaled octave of function; later octaves are scaled */
	value = offset + raw_noise_3d(point.x, point.y, point.z);
	point.x *= lacunarity;
	point.y *= lacunarity;
	point.z *= lacunarity;

	/* spectral construction inner loop, where the fractal is built */
	for (i = 1; i<octaves; i++) {
		/* obtain displaced noise value */
		increment = raw_noise_3d(point.x, point.y, point.z) + offset;
		/* scale amplitude appropriately for this frequency */
		increment *= exponent_array[i];
		/* scale increment by current `altitude' of function */
		increment *= value;
		/* add increment to ``value''  */
		value += increment;
		/* raise spatial frequency */
		point.x *= lacunarity;
		point.y *= lacunarity;
		point.z *= lacunarity;
	} /* for */

	  /* take care of remainder in ``octaves''  */
	remainder = octaves - (int)octaves;
	if (remainder) {
		/* ``i''  and spatial freq. are preset in loop above */
		/* note that the main loop code is made shorter here */
		/* you may want to that loop more like this */
		increment = (raw_noise_3d(point.x, point.y, point.z) + offset) * exponent_array[i];
		value += remainder * increment * value;
	}

	return(value);

} /* Hetero_Terrain() */


  /* Hybrid additive/multiplicative multifractal terrain model.
  *
  * Copyright 1994 F. Kenton Musgrave
  *
  * Some good parameter values to start with:
  *
  *      H:           0.25
  *      offset:      0.7
  */
double
HybridMultifractal(double3 point, double H, double lacunarity,
	double octaves, double offset)
{
	double          frequency, result, signal, weight, remainder;
	double          Noise3();
	int             i;
	static int      first = TRUE;
	static double   *exponent_array;

	/* precompute and store spectral weights */
	if (first) {
		/* seize required memory for exponent_array */
		exponent_array =
			(double *)malloc((octaves + 1) * sizeof(double));
		frequency = 1.0;
		for (i = 0; i <= octaves; i++) {
			/* compute weight for each frequency */
			exponent_array[i] = pow(frequency, -H);
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	/* get first octave of function */
	result = (raw_noise_3d(point.x, point.y, point.z) + offset) * exponent_array[0];
	weight = result;
	/* increase frequency */
	point.x *= lacunarity;
	point.y *= lacunarity;
	point.z *= lacunarity;

	/* spectral construction inner loop, where the fractal is built */
	for (i = 1; i<octaves; i++) {
		/* prevent divergence */
		if (weight > 1.0)  weight = 1.0;

		/* get next higher frequency */
		signal = (raw_noise_3d(point.x, point.y, point.z) + offset) * exponent_array[i];
		/* add it in, weighted by previous freq's local value */
		result += weight * signal;
		/* update the (monotonically decreasing) weighting value */
		/* (this is why H must specify a high fractal dimension) */
		weight *= signal;

		/* increase frequency */
		point.x *= lacunarity;
		point.y *= lacunarity;
		point.z *= lacunarity;
	} /* for */

	  /* take care of remainder in ``octaves''  */
	remainder = octaves - (int)octaves;
	if (remainder)
		/* ``i''  and spatial freq. are preset in loop above */
		result += remainder * raw_noise_3d(point.x, point.y, point.z) * exponent_array[i];

	return(result);

} /* HybridMultifractal() */


  /* Ridged multifractal terrain model.
  *
  * Copyright 1994 F. Kenton Musgrave
  *
  * Some good parameter values to start with:
  *
  *      H:           1.0
  *      offset:      1.0
  *      gain:        2.0
  */
double
RidgedMultifractal(double3 point, double H, double lacunarity,
	double octaves, double offset, double gain)
{
	double           result, frequency, signal, weight, Noise3();
	int              i;
	static int       first = TRUE;
	static double    *exponent_array;

	/* precompute and store spectral weights */
	if (first) {
		/* seize required memory for exponent_array */
		exponent_array =
			(double *)malloc((octaves + 1) * sizeof(double));
		frequency = 1.0;
		for (i = 0; i <= octaves; i++) {
			/* compute weight for each frequency */
			exponent_array[i] = pow(frequency, -H);
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	/* get first octave */
	signal = raw_noise_3d(point.x, point.y, point.z);
	/* get absolute value of signal (this creates the ridges) */
	if (signal < 0.0) signal = -signal;
	/* invert and translate (note that "offset" should be ~= 1.0) */
	signal = offset - signal;
	/* square the signal, to increase "sharpness" of ridges */
	signal *= signal;
	/* assign initial values */
	result = signal;
	weight = 1.0;

	for (i = 1; i<octaves; i++) {
		/* increase the frequency */
		point.x *= lacunarity;
		point.y *= lacunarity;
		point.z *= lacunarity;

		/* weight successive contributions by previous signal */
		weight = signal * gain;
		if (weight > 1.0) weight = 1.0;
		if (weight < 0.0) weight = 0.0;
		signal = raw_noise_3d(point.x, point.y, point.z);
		if (signal < 0.0) signal = -signal;
		signal = offset - signal;
		signal *= signal;
		/* weight the contribution */
		signal *= weight;
		result += signal * exponent_array[i];
	}

	return(result);

} /* RidgedMultifractal() */


