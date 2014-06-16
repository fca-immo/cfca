/**
 * fcaVnextClosureX.c, (c) 2013, Immanuel Albrecht; Dresden University of
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "safeguard.h"
#include "private.h"


/** @file
 *  this file contains a multi-threading nextClosure implementation using the
 *  IncidenceVector implementation
 */

/**
 * creates a new formal concept intent chunk and fills it with the intents of all formal
 * concepts in the concept lattice of ctx, using next closure algorithm, that are in a given
 * lexicographic interval of the powerset
 *
 * @param ctx   formal context
 * @param start   first attribute vector (not included if it is a concept intent)
 *                if this is zero, start with M={}, in this case, we add the bottom
 *                concept in case of M''=={}
 * @param stop    last attribute vector, or zero to continue until the end
 * @return   concept intents between (start, stop]
 */

FormalConceptIntentBulkListV nextClosureVX1(FormalContextV ctx,
		const IncidenceVector restrict start, const IncidenceVector restrict stop)
{
	RETURN_ZERO_IF_ZERO(ctx);

	myFormalContextV * restrict c;
	c = (myFormalContextV*) ctx;

	IncidenceVector restrict M;
	IncidenceVector restrict Y;

	Y = calloc(c->width, sizeof(uint64_t));
	M = malloc(c->width * sizeof(uint64_t));

	FormalConceptIntentBulkListV root;
	FormalConceptIntentBulkListV last;

	root = newConceptBulkV(c->attributes);

	if (start)
	{
		memcpy(M, start, c->width * sizeof(uint64_t));

		last = root;

	}
	else
	{

		/**
		 * calculate the bottom intent of the concept lattice, i.e. {}''
		 */
		closeIntentV(ctx, Y, M);

		/**
		 * add the bottom element of the concept lattice (a concept lattice is never empty)
		 */

		last = addConceptToBulkV(root, M);
	}

	/**
	 * begin of nextClosure function iteration
	 */
	nextClosure:

	for (size_t i = c->attributes; i > 0;)
	{
		--i;

		if (!INCIDESV(M,i))
		{
			CROSSV(M, i);
			closeIntentV(ctx, M, Y);

			int good;
			good = 1;

			for (unsigned int j = 0; j < OFFSET(i); ++j)
			{
				if (Y[j] & (~(M[j])))
				{
					good = 0;
					break;
				}
			}
			if (good)
			{
				if (Y[OFFSET(i)] & (~M[OFFSET(i)]) & CRIMPVALUE(i))
				{
					good = 0;
				}
			}

			if (good)
			{
				/**
				 * check whether we are still in range
				 */

				if (stop)
				{
					if (intentCmpV(c->attributes, Y, stop) < 0)
					{
						/**
						 * the (pseudo)intent Y is bigger than stop
						 */
						goto gracefulReturn;
					}
				}

				/**
				 * we found the next intent
				 */
				last = addConceptToBulkV(last, Y);

				/**
				 * continue with Y for M
				 */

				IncidenceVector DELTA;
				DELTA = M;
				M = Y;
				Y = DELTA;
				/**
				 * do the nextClosure
				 */
				goto nextClosure;
			}
		}

		CLEARV(M, i);
	}

	gracefulReturn:
	/**
	 * free up memory
	 */

	free(M);
	free(Y);

	return root;
}

typedef struct snextClosureVX1Params
{
	FormalConceptIntentBulkListV rVal;
	FormalContextV ctx;
	IncidenceVector start;
	IncidenceVector stop;
}*nextClosureVX1Params;

void* callNextClosureVX1(void * params)
{
	nextClosureVX1Params p;
	p = (nextClosureVX1Params) params;

	p->rVal = nextClosureVX1(p->ctx, p->start, p->stop);

	return 0;
}

/**
 * creates a new formal concept intent chunk and fills it with the intents of all formal
 * concepts in the concept lattice of ctx, using a parallel next closure algorithm
 * with up to 8 threads
 *
 * @param ctx   formal context
 * @return   concept intents
 */

FormalConceptIntentBulkListV nextClosureVX(FormalContextV ctx)
{
	RETURN_ZERO_IF_ZERO(ctx);

	myFormalContextV *c;
	c = (myFormalContextV*) ctx;

	size_t N;

	N = 1;

	if (c->attributes >= 3)
		N = 8;
	else if (c->attributes >= 2)
		N = 4;
	else if (c->attributes >= 1)
		N = 2;

	if (N < 2)
		return nextClosureVX1(ctx, 0, 0);

	IncidenceVector bounds;
	bounds = calloc(c->width * (N - 1), sizeof(uint64_t));

	if (N == 2)
	{
		CROSSV(bounds, 0);
	}
	else if (N == 4)
	{
		CROSSV(bounds, 1); //01

		CROSSV(bounds + c->width, 0); //10

		CROSSV(bounds + c->width * 2, 1); //11
		CROSSV(bounds + c->width * 2, 0);
	}
	else if (N == 8)
	{
		CROSSV(bounds, 2); // 001

		CROSSV(bounds + c->width, 1); //010

		CROSSV(bounds + c->width * 2, 2); //011
		CROSSV(bounds + c->width * 2, 1);

		CROSSV(bounds + c->width * 3, 0); //100

		CROSSV(bounds + c->width * 4, 0); //101
		CROSSV(bounds + c->width * 4, 2);

		CROSSV(bounds + c->width * 5, 1); //110
		CROSSV(bounds + c->width * 5, 0);

		CROSSV(bounds + c->width * 6, 0); //111
		CROSSV(bounds + c->width * 6, 1);
		CROSSV(bounds + c->width * 6, 2);
	}

//	for (int i = 0; i < N - 1; ++i)
//	{
//		printf("BOUND %16llx\n", *(bounds + i * c->width)&CRIMPVALUE(c->attributes-1));
//		if (i > 0)
//			printf("CMP %d\n",
//					intentCmpV(c->attributes, bounds + (i - 1) * c->width,
//							bounds + i * c->width));
//	}


	nextClosureVX1Params chunks;
	chunks = malloc(N * sizeof(struct snextClosureVX1Params));

	pthread_t *threads;
	threads = malloc(N * sizeof(pthread_t));

	for (size_t i = 0; i < N; ++i)
	{
		chunks[i].ctx = ctx;
		if (i > 0)
			chunks[i].start = (bounds + c->width * (i - 1));
		else
			chunks[i].start = 0;

		if (i < N - 1)
			chunks[i].stop = (bounds + c->width * (i));
		else
			chunks[i].stop = 0;
	}

	for (size_t i = 0; i < N; ++i)
	{
		pthread_create(&threads[i], 0, callNextClosureVX1, (void*) &chunks[i]);
	}

	for (size_t i = 0; i < N; ++i)
	{
		pthread_join(threads[i], 0);
	}

//	for (size_t i = 0; i < N; ++i)
//	{
//		printf("%zu thread: counts %zu\n", i,
//				countConceptsInBulkV(chunks[i].rVal));
//	}

	FormalConceptIntentBulkListV root;
	FormalConceptIntentBulkListV last;

	root = chunks[0].rVal;
	last = root;

	for (size_t var = 1; var < N; ++var)
	{

		while (last->next)
			last = last->next;

		last->next = chunks[var].rVal;
	}

	free(bounds);
	free(chunks);

	return root;
}
