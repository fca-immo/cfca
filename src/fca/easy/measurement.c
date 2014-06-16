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

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "private.h"

/**
 * calculates the likelihood function L
 * @param B   the measurement context
 * @param c   the condition map (mapping objects
 * @param I
 * @param eta
 * @param L
 */

void calculateLikelihood(const FormalContext B, const ConditionMap c,
		const FormalContext I, const EtaFunction eta, CommutativeProduct L)
{
	RETURN_IF_ZERO(B);
	RETURN_IF_ZERO(c);
	RETURN_IF_ZERO(I);
	RETURN_IF_ZERO(eta);
	RETURN_IF_ZERO(L);

	const myFormalContext* restrict b;
	b = (const myFormalContext*) B;

	const myFormalContext* restrict i;
	i = (const myFormalContext*) I;

	WARN_IF_UNEQUAL_DO(b->objects, (int )c->objects, return);
	WARN_IF_UNEQUAL_DO(b->attributes, i->attributes, return);
	WARN_IF_UNEQUAL_DO(i->attributes, (int ) eta->measurements, return);
	WARN_IF_UNEQUAL_DO(2, eta->types, return);
	WARN_IF_UNEQUAL_DO(eta->constants, L->constants,);

	memset(L->match, 0, sizeof(size_t) * L->constants);
	memset(L->mismatch, 0, sizeof(size_t) * L->constants);

	for (int g = 0; g < b->objects; ++g)
	{
		int c_g;
		c_g = (int) c->c[g];

		WARN_IF_GEQ_DO(c_g, i->objects, continue);

		for (int m = 0; m < b->attributes; ++m)
		{
			if (gIm(c_g,i,m))
			{
				if (gIm(g,b,m))
				{
					L->match[eta->eta[b->attributes + m]]++;
				}
				else
				{
					L->mismatch[eta->eta[b->attributes + m]]++;
				}
			}
			else
			{
				if (gIm(g,b,m))
				{
					L->mismatch[eta->eta[m]]++;
				}
				else
				{
					L->match[eta->eta[m]]++;
				}
			}
		}
	}
}

/**
 * generates a new formal context that comprises of a pseudo-random fake
 * measurement obtained from I with the given error probabilites and experiment count
 *
 * @param I            experimental condition context
 * @param eta          error probabilities
 * @param experiments  number of conducted experiments
 * @param output_c     a new ConditionMap will be generated to this pointer, if nonzero
 * @return  a new FormalContext object
 */
FormalContext newFakeMeasurement(const FormalContext I, const EtaFunction eta,
		int experiments, ConditionMap* output_c)
{
	RETURN_ZERO_IF_ZERO(I);
	RETURN_ZERO_IF_ZERO(eta);

	const myFormalContext* restrict i;
	i = (const myFormalContext*) I;

	WARN_IF_UNEQUAL_DO(i->attributes, (int ) eta->measurements, return 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	if (output_c)
		*output_c = newConditionMap(experiments);

#pragma GCC diagnostic pop

	myFormalContext *b;
	b = (myFormalContext *) newFormalContext(experiments, i->attributes);

	for (int x = 0; x < experiments; ++x)
	{
		int c_x;
		c_x =
				MIN((int) floor((double) (random()) /
								(double) RAND_MAX * (double)i->objects), i->objects-1);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

		if (output_c)
			(*output_c)->c[x] = c_x;

#pragma GCC diagnostic pop

		for (int m = 0; m < i->attributes; ++m)
		{
			Probability rnd;
			rnd = (Probability) random() / (Probability) RAND_MAX;

			if (gIm(c_x,i,m))
			{
				if (rnd >= eta->C[eta->eta[i->attributes + m]])
					CROSS(CELL(x,b,m));
			}
			else
			{
				if (rnd < eta->C[eta->eta[m]])
					CROSS(CELL(x,b,m));
			}
		}
	}

	return (FormalContext) b;
}

/**
 * recalculate the condition map C, such that the likelihood of (B,c,I,eta) is maximal
 *
 * @param B       the measurement context
 * @param c       output: ConditionMap that will be filled with c_{I,eta}
 * @param I       the condition context
 * @param eta     the error probabilities
 * @param log_c   log constants corresponding to eta
 */

void optimizeConditionMap(const FormalContext B, ConditionMap c,
		const FormalContext I, const EtaFunction restrict eta,
		const LogCache log_c)
{
	RETURN_IF_ZERO(B);
	RETURN_IF_ZERO(c);
	RETURN_IF_ZERO(I);
	RETURN_IF_ZERO(eta);
	RETURN_IF_ZERO(log_c);

	const myFormalContext* restrict b;
	b = (const myFormalContext*) B;

	const myFormalContext* restrict i;
	i = (const myFormalContext*) I;

	WARN_IF_UNEQUAL_DO(b->objects, (int )c->objects, return);
	WARN_IF_UNEQUAL_DO(b->attributes, i->attributes, return);
	WARN_IF_UNEQUAL_DO(b->attributes, (int )eta->measurements, return);
	WARN_IF_UNEQUAL_DO(2, eta->types, return);
	WARN_IF_UNEQUAL_DO(eta->constants, log_c->constants, return);

	size_t attributes;
	attributes = (size_t) b->attributes;

	CommutativeProduct lp;
	lp = newCommutativeProduct(eta->constants);

	CommutativeProduct restrict l;
	l = lp;

	/*
	 * For every object index x of B, find the last object index of I, such that
	 * the likelihood that experiment x was conducted under that condition is maximal.
	 */

	for (size_t x = 0; x < (size_t) b->objects; ++x)
	{

		size_t best_cx;
		best_cx = 0;

		/*
		 * initialize the current maximal probability with the bottom value of
		 * LogProbability
		 */

		LogProbability maximal_prob;
		maximal_prob = LOG_PROB_LOWER_BOUND;

		for (size_t cx = 0; cx < (size_t) i->objects; ++cx)
		{
			/*
			 * calculate (partial) likelihood if c(x) = cx
			 */
			memset(l->match, 0, sizeof(size_t) * l->constants);
			memset(l->mismatch, 0, sizeof(size_t) * l->constants);

			for (size_t m = 0; m < attributes; ++m)
			{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
				if (gIm(x,b,m))
				{
					if (gIm(cx,i,m))
					{
#pragma GCC diagnostic pop
						/*
						 * 1-eta(m,1)
						 */
						l->match[eta->eta[1 * eta->constants + m]]++;
						//printf("X");
					}
					else
					{
						/*
						 * eta(m,0)
						 */
						l->mismatch[eta->eta[0 * eta->constants + m]]++;
						//printf("O");
					}
				}
				else
				{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
					if (gIm(cx,i,m))
					{
#pragma GCC diagnostic pop
						/*
						 * eta(m,1)
						 */
						l->mismatch[eta->eta[1 * eta->constants + m]]++;
						//printf("_");
					}
					else
					{
						/*
						 * 1-eta(m,0)
						 */
						l->match[eta->eta[0 * eta->constants + m]]++;
						//printf(".");
					}
				}
			}

			LogProbability p;
			p = logProbabilityFromProduct(log_c, l);

			//printf("%f\t", p);

			/*
			 * cx is more likely than best_cx
			 * so, update cx -> best_cx
			 */

			if (maximal_prob <= p)
			{
				maximal_prob = p;
				best_cx = cx;
			}
		}

		/*
		 * choose the most likely match
		 */

		c->c[x] = best_cx;
		//printf("c(%4zu) = %4zu\n", x, c->c[x]);
	}

	deleteCommutativeProduct(&lp);
}

/**
 * optimize the approximated context for a given measurement and condition map
 * @param B    the measurement context
 * @param cmap    condition map
 * @param I    output: new approximated context
 * @param eta  the error probabilities
 * @param log_c log constants corresponding to eta
 */

void optimizeApproximationContext(const FormalContext B,
		const ConditionMap cmap, FormalContext I,
		const EtaFunction restrict eta, const LogCache log_c)
{
	RETURN_IF_ZERO(B);
	RETURN_IF_ZERO(cmap);
	RETURN_IF_ZERO(I);
	RETURN_IF_ZERO(eta);
	RETURN_IF_ZERO(log_c);

	const myFormalContext* restrict b;
	b = (const myFormalContext*) B;

	myFormalContext* i;
	i = (myFormalContext*) I;

	WARN_IF_UNEQUAL_DO(b->objects, (int )cmap->objects, return);
	WARN_IF_UNEQUAL_DO(b->attributes, i->attributes, return);
	WARN_IF_UNEQUAL_DO(b->attributes, (int )eta->measurements, return);
	WARN_IF_UNEQUAL_DO(2, eta->types, return);
	WARN_IF_UNEQUAL_DO(eta->constants, log_c->constants, return);

	size_t attributes;
	attributes = (size_t) b->attributes;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	size_t measurements;
	measurements = b->objects;

	size_t conditions;
	conditions = i->objects;

#pragma GCC diagnostic pop

	CommutativeProduct *l_array;

	l_array = malloc(sizeof(CommutativeProduct) * conditions * 2);

	CommutativeProduct restrict * l_cross;
	l_cross = (CommutativeProduct restrict *) l_array;

	CommutativeProduct restrict * l_gap;
	l_gap = (CommutativeProduct restrict *) (l_array + conditions);

	for (size_t c = 0; c < conditions; ++c)
	{
		l_cross[c] = newCommutativeProduct(eta->constants);
		l_gap[c] = newCommutativeProduct(eta->constants);
	}

	/*
	 * check column wise, whether there should be a cross in the incidence matrix I
	 * or not.
	 */

	for (size_t a = 0; a < attributes; ++a)
	{
		size_t eta_a0, eta_a1;

		eta_a0 = eta->eta[0 * eta->constants + a];
		eta_a1 = eta->eta[1 * eta->constants + a];

		for (size_t c = 0; c < conditions; ++c)
		{
			for (size_t i = 0; i < eta->constants; ++i)
			{
				l_cross[c]->match[i] = 0;
				l_cross[c]->mismatch[i] = 0;
				l_gap[c]->match[i] = 0;
				l_gap[c]->mismatch[i] = 0;
			}
		}

		/*
		 * count how often we measured attribute a for each condition cmap
		 * and fill it in the corresponding CommutativeProduct
		 */

		for (size_t o = 0; o < measurements; ++o)
		{
			size_t condition;
			condition = cmap->c[o];

			/*
			 * add up in parallel both error vectors for both cross and gap case
			 * in the approximation
			 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

			if (gIm(o, b, a))
			{
				l_cross[condition]->match[eta_a1]++;
				l_gap[condition]->mismatch[eta_a0]++;
			}
			else
			{
				l_cross[condition]->mismatch[eta_a1]++;
				l_gap[condition]->match[eta_a0]++;
			}

#pragma GCC diagnostic pop
		}

		/*
		 * now each l_cross[c] and l_gap[c] contain the same indexed factors for
		 * the likelihoods with and without a cross for c I a
		 * we sum up both and the bigger wins
		 */

		for (size_t c = 0; c < conditions; ++c)
		{
			LogProbability cross;
			LogProbability gap;

			cross = logProbabilityFromProduct(log_c, l_cross[c]);
			gap = logProbabilityFromProduct(log_c, l_gap[c]);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

			if (gap <= cross)
			{
				CROSS(CELL(c,i,a));
			}
			else
			{
				CLEAR(CELL(c,i,a));
			}

#pragma GCC diagnostic pop
		}
	}

	for (size_t c = 0; c < conditions; ++c)
	{
		deleteCommutativeProduct(l_array + c);
		deleteCommutativeProduct(l_array + c + conditions);
	}

	free(l_array);
}

/**
 * calculates the asymmetric distances for some measurement context
 *
 * @param B     the measurement context
 * @param eta   the error probabilities
 * @param log_c the logarithms corresponding to eta
 * @return  a new DistanceMatrix
 */

DistanceMatrix newDistanceMatrixFromContext(const FormalContext B,
		const EtaFunction restrict eta, const LogCache log_c)
{
	RETURN_ZERO_IF_ZERO(B);
	RETURN_ZERO_IF_ZERO(eta);
	RETURN_ZERO_IF_ZERO(log_c);

	const myFormalContext* restrict b;
	b = (const myFormalContext*) B;

	WARN_IF_UNEQUAL_DO(b->attributes, (int )eta->measurements, return 0);
	WARN_IF_UNEQUAL_DO(2, eta->types, return 0);
	WARN_IF_UNEQUAL_DO(eta->constants, log_c->constants, return 0);

	size_t attributes;
	attributes = (size_t) b->attributes;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	size_t measurements;
	measurements = b->objects;

#pragma GCC diagnostic pop

	DistanceMatrix d;
	d = newDistanceMatrix(measurements);

	CommutativeProduct dxy, dyx;

	dxy = newCommutativeProduct(eta->constants);
	dyx = newCommutativeProduct(eta->constants);

	/*
	 * calculate the distances d(x,y) and d(y,x) in parallel
	 */

	for (size_t x = 0; x < measurements; ++x)
	{
		for (size_t y = x; y < measurements; ++y)
		{
			/*
			 * set dxy and dyx to zero
			 */
			for (size_t i = 0; i < eta->constants; ++i)
			{
				dxy->match[i] = 0;
				dxy->mismatch[i] = 0;

				dyx->match[i] = 0;
				dyx->mismatch[i] = 0;
			}

			/*
			 * check where the attribute vectors coincide and where not
			 */

			for (size_t m = 0; m < attributes; ++m)
			{
				size_t eta_m0, eta_m1;

				eta_m0 = eta->eta[0 * eta->constants + m];
				eta_m1 = eta->eta[1 * eta->constants + m];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

				if (gIm(x,b,m))
				{
					if (gIm(y,b,m))
					{

						dxy->match[eta_m1]++;
						dyx->match[eta_m1]++;
					}
					else
					{
						dxy->mismatch[eta_m1]++;
						dyx->mismatch[eta_m0]++;
					}
				}
				else
				{
					if (gIm(y,b,m))
					{
						dxy->mismatch[eta_m0]++;
						dyx->mismatch[eta_m1]++;
					}
					else
					{
						dxy->match[eta_m0]++;
						dyx->match[eta_m0]++;
					}
				}

#pragma GCC diagnostic pop
			}

			/*
			 * calculate the logarithms of the likelihoods
			 */

			d->d[x * measurements + y] = logProbabilityFromProduct(log_c, dxy);
			d->d[y * measurements + x] = logProbabilityFromProduct(log_c, dyx);
		}
	}

	deleteCommutativeProduct(&dxy);
	deleteCommutativeProduct(&dyx);

	return d;
}
