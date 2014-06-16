/**
 * cfca.c, (c) 2013, Immanuel Albrecht; Dresden University of
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
#include <time.h>

#include "fca/fca.h"
#include "fca/easy/private.h"
#include "fca/vector/private.h"

/**
 * this is the main testing routine for purposes of testing the formal concept analysis
 * implementation for errors
 *
 * @return
 */

int main(void)
{
	puts("Go!");
	/**
	 * initialize pseudo random number generator
	 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

	srandom(time(0));

#pragma GCC diagnostic pop

	/**
	 * start tests
	 */

	FormalContext ctx;
	ctx = newFormalContextFromRandom(15, 30, 0.3f);

	writeFormalContext(ctx, "/home/immo/tmp/test.cxt");

	EtaFunction eta;
	eta = newUniformEtaFunction(2, 30);
	eta->C[0] = 0.10; //Type I error
	eta->C[1] = 0.04; //Type II error

	LogCache logC;
	logC = newLogCache(eta->constants);
	calculateLogs(eta,logC);

	ConditionMap c_gen;
	FormalContext B;
	const size_t experiments = 60;


	B = newFakeMeasurement(ctx,eta,experiments,&c_gen);
	writeFormalContext(B, "/home/immo/tmp/test_B.cxt");

	puts("Calculating DistanceMatrix");

	DistanceMatrix d;
	d = newDistanceMatrixFromContext(B,eta,logC);

	writeDistancesToFile(d,"/home/immo/tmp/test_d.csv");

	normalizeDistanceMatrix(d);
	writeDistancesToFile(d,"/home/immo/tmp/test_dnorm.csv");

	puts("Testing restorability of the condition map...");

	ConditionMap c;
	c = newConditionMap(experiments);

	optimizeConditionMap(B,c,ctx,eta,logC);

	for (int x=0;x<c->objects;++x) {
		if (c->c[x] != c_gen->c[x]) {
			printf("c(%d) = %d != %d\n",x,c->c[x],c_gen->c[x]);
		}
	}

	optimizeApproximationContext(B,c,ctx,eta,logC);
	writeFormalContext(ctx, "/home/immo/tmp/test_C.cxt");

	optimizeConditionMap(B,c,ctx,eta,logC);
	optimizeApproximationContext(B,c,ctx,eta,logC);
	writeFormalContext(ctx, "/home/immo/tmp/test_D.cxt");


	optimizeConditionMap(B,c,ctx,eta,logC);
	optimizeApproximationContext(B,c,ctx,eta,logC);
	writeFormalContext(ctx, "/home/immo/tmp/test_E.cxt");


	optimizeConditionMap(B,c,ctx,eta,logC);
	optimizeApproximationContext(B,c,ctx,eta,logC);
	writeFormalContext(ctx, "/home/immo/tmp/test_F.cxt");

	deleteConditionMap(&c);
	deleteConditionMap(&c_gen);
	deleteLogCache(&logC);
	deleteEtaFunction(&eta);
	deleteFormalContext(&B);
	deleteFormalContext(&ctx);
	deleteDistanceMatrix(&d);

	puts("done.");
	return EXIT_SUCCESS;
}
