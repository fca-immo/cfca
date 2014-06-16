/**
 * measurement.c, (c) 2013, Immanuel Albrecht; Dresden University of
 * Technology, Professur für die Psychologie des Lernen und Lehrens
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <string.h>
#include "../common.h"

/** @file
 *  This file contains implementations regarding measurement error probabilities
 *  related structures operations
 */

/**
 * creates a new EtaFunction
 *
 * @param types  number of types, usually 2
 * @param measurements  number of different measurements,
 * @param constants  number of different error constants,
 * <= types*measurements are needed.
 * @return  a new structure
 */

EtaFunction newEtaFunction(size_t types, size_t measurements, size_t constants)
{
	RETURN_ZERO_IF_ZEROI(types);
	RETURN_ZERO_IF_ZEROI(measurements);
	RETURN_ZERO_IF_ZEROI(constants);

	EtaFunction e;
	e = malloc(sizeof(struct sEtaFunction));

	e->types = types;
	e->measurements = measurements;
	e->constants = constants;

	e->C = calloc(constants, sizeof(Probability));
	e->eta = calloc(types * measurements, sizeof(size_t));

	return e;
}

/**
 *   deletes an EtaFunction and sets the pointer to zero
 * @param eta
 */

void deleteEtaFunction(EtaFunction* eta)
{
	RETURN_IF_ZERO(eta);
	RETURN_IF_ZERO(*eta);

	free((*eta)->C);
	free((*eta)->eta);
	free(*eta);

	*eta = 0;
}

/**
 * create a new EtaFunction with the same constant for each type
 *
 * @param types
 * @param measurements
 * @return
 */
EtaFunction newUniformEtaFunction(size_t types, size_t measurements)
{
	EtaFunction e;
	e = newEtaFunction(types, measurements, types);

	for (size_t t = 0; t < types; ++t)
	{
		for (size_t m = 0; m < measurements; ++m)
		{
			e->eta[t * measurements + m] = t;
		}
	}

	return e;
}

/**
 * create a new EtaFunction with the a constant for each type and measurement on its own
 *
 * @param types
 * @param measurements
 * @return
 */
EtaFunction newGeneralEtaFunction(size_t types, size_t measurements)
{
	EtaFunction e;
	e = newEtaFunction(types, measurements, types * measurements);

	for (size_t t = 0; t < types; ++t)
	{
		for (size_t m = 0; m < measurements; ++m)
		{
			e->eta[t * measurements + m] = t * measurements + m;
		}
	}

	return e;
}

/**
 * creates a new commutative product without any factors
 * @param constants   number of formal constants
 * @return new CommutativeProduct object
 */

CommutativeProduct newCommutativeProduct(size_t constants)
{
	RETURN_ZERO_IF_ZEROI(constants);

	CommutativeProduct p;
	p = malloc(sizeof(struct sCommutativeProduct));

	p->constants = constants;

	p->mismatch = calloc(constants, sizeof(size_t));
	p->match = calloc(constants, sizeof(size_t));

	p->intermediate = malloc(constants * 2 * sizeof(LogProbability));

	return p;
}

/**
 * deletes a CommutativeProduct and sets its pointer to zero.
 *
 * @param p
 */

void deleteCommutativeProduct(CommutativeProduct* p)
{
	RETURN_IF_ZERO(p);
	RETURN_IF_ZERO(*p);

	free((*p)->match);
	free((*p)->mismatch);
	free((*p)->intermediate);
	free(*p);

	*p = 0;
}

/**
 * creates a new condition map
 * @param objects   cardinality of the domain
 * @return new ConditionMap object
 */

ConditionMap newConditionMap(size_t objects)
{
	RETURN_ZERO_IF_ZEROI(objects);

	ConditionMap c;
	c = malloc(sizeof(struct sConditionMap));

	c->objects = objects;
	c->c = calloc(objects, sizeof(size_t));

	return c;
}

/**
 * deletes a ConditionMap object
 * @param c
 */

void deleteConditionMap(ConditionMap* c)
{
	RETURN_IF_ZERO(c);
	RETURN_IF_ZERO(*c);

	free((*c)->c);
	free(*c);

	*c = 0;
}

/**
 * creates a new cache for logs of constants
 * @param constants number of constants
 * @return new LogCache
 */

LogCache newLogCache(size_t constants)
{
	LogCache l;

	l = malloc(sizeof(struct sLogCache));

	l->constants = constants;

	l->logC = calloc(constants, sizeof(LogProbability));
	l->logNotC = calloc(constants, sizeof(LogProbability));

	return l;
}

/**
 * deletes a LogCache and sets its pointer to zero
 * @param log_c
 */
void deleteLogCache(LogCache* log_c)
{
	RETURN_IF_ZERO(log_c);
	RETURN_IF_ZERO(*log_c);

	free((*log_c)->logC);
	free((*log_c)->logNotC);
	free(*log_c);

	*log_c = 0;
}

/**
 * calculates the logarithms of probabilities given by constants, and their complements
 *
 * @param eta    input constants
 * @param log_c  output structure to update the constants
 */
void calculateLogs(const EtaFunction eta, LogCache log_c)
{
	WARN_IF_UNEQUAL_DO(eta->constants, log_c->constants, return);

	/*
	 * we could use just any logarithm, so 2 seems reasonable...
	 */

	for (size_t i = 0; i < eta->constants; ++i)
	{
		log_c->logC[i] = log2(eta->C[i]);
		log_c->logNotC[i] = log2(1. - eta->C[i]);
	}
}

/**
 * calculate the log probability from a power vector
 *
 * @param log_c  logarithms of constants
 * @param l   vector of constant and complementary powers
 * @return the log probability of the product given by the vector of powers, l
 */
LogProbability logProbabilityFromProduct(const LogCache log_c,
		CommutativeProduct l)
{
	RETURN_ZERO_IF_ZERO(log_c);
	RETURN_ZERO_IF_ZERO(l);

	WARN_IF_UNEQUAL_DO(log_c->constants, l->constants, return 0);
	size_t constants = log_c->constants;

	for (size_t c = 0; c < constants; ++c)
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
		/*
		 * 0*-inf is still -inf; but we want to enforce 0*-inf = 0
		 */
		if (l->match[c] > 0)
			l->intermediate[c] = log_c->logNotC[c]
					* (LogProbability) l->match[c];
		else
			l->intermediate[c] = 0.;

		if (l->mismatch[c] > 0)
			l->intermediate[c + constants] = log_c->logC[c]
					* (LogProbability) l->mismatch[c];
		else
			l->intermediate[c + constants] = 0.;
#pragma GCC diagnostic pop
	}

	return sumUp(l->intermediate, constants * 2, LOG_PROB_LOWER_BOUND,
	LOG_PROB_UPPER_BOUND);
}

/**
 * This routine calculates the sum of elements of a vector.
 *
 * @param V                ptr to array of LogProbabilities
 * @param length           length of array V
 * @param lower_bound      lower bound to include in this sum
 * @param upper_bound      upper bound to include in this sum
 * @return @f[ \sum_{i=0, \mathrm{lower\_bound}\leq V(i) < \mathrm{upper\_bound}}^{
 *                            \mathrm{length}} V(i) @f]
 */
LogProbability sumUp(const LogProbability * restrict V, size_t length,
		LogProbability lower_bound, LogProbability upper_bound)
{
	//TODO: Make this routine smarter with respect to LARGE sums

	LogProbability sum;
	sum = 0.;

	for (size_t i = 0; i < length; ++i)
	{
		LogProbability summand;
		summand = V[i];

		if ((lower_bound <= summand) && (summand < upper_bound))
		{
			//printf("+%.2f",summand);
			sum += summand;
		}
	}
	//printf("=");

	return sum;
}

/**
 * creates a new DistanceMatrix of the given size
 *
 * @param objects    square matrix dimension
 * @return   the new DistanceMatrix object
 */

DistanceMatrix newDistanceMatrix(size_t objects)
{
	RETURN_ZERO_IF_ZEROI(objects);

	DistanceMatrix d;

	d = malloc(sizeof(struct sDistanceMatrix));

	d->objects = objects;

	d->d = calloc(objects * objects, sizeof(LogProbability));

	return d;
}

/**
 * deletes the given object and sets its pointer to zero
 *
 * @param d   DistanceMatrix
 */

void deleteDistanceMatrix(DistanceMatrix* d)
{
	RETURN_IF_ZERO(d);

	free((*d)->d);
	free(*d);

	d = 0;
}

/**
 * Writes the contents of a distance matrix to the given file in CSV format:
 * d(x,y) will be written on the (x+1)th line to the (y+1)th column.
 *
 * @param d    the distance matrix
 * @param filename  output file name
 */

void writeDistancesToFile(const DistanceMatrix d, const char* filename)
{
	RETURN_IF_ZERO(d);
	RETURN_IF_ZERO(filename);

	FILE* file;
	file = fopen(filename, "w");

	RETURN_IF_ZERO(file);

	/*
	 * header row
	 */

	fputs("\"d(x,y)\"", file);

	for (size_t y = 0; y < d->objects; ++y)
	{
		fprintf(file, ",y=%zu", y);
	}

	fputs("\n", file);

	/*
	 * contents
	 */

	for (size_t x = 0; x < d->objects; ++x)
	{
		fprintf(file, "x=%zu", x);

		for (size_t y = 0; y < d->objects; ++y)
		{
			fprintf(file, ",%f", d->d[x * d->objects + y]);
		}

		fputs("\n", file);
	}

	fclose(file);
}

/**
 * normalizes the distance matrix by replacing d with d', which is defined to be
 *
 * @f[     d'(x,y) = d(x,y) - d(y,y)     @f]
 *
 * @param d  distance matrix
 */

void normalizeDistanceMatrix(DistanceMatrix restrict d)
{
	RETURN_IF_ZERO(d);

	for (size_t y = 0; y < d->objects; ++y)
	{
		LogProbability dyy;
		dyy = d->d[y*d->objects + y];

		for (size_t x = 0; x < d->objects; ++x)
		{
			/*
			 * d(x,y) <- d(x,y) - d(y,y)
			 */
			d->d[x*d->objects + y] -= dyy;
		}
	}
}
