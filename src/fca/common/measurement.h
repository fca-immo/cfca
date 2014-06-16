/**
 * measurement.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

#ifndef FCA__MEASUREMENT_H_
#define FCA__MEASUREMENT_H_

#include <stdlib.h>
#include <float.h>
#include <math.h>

/**
 * type for probabilities
 */

typedef double Probability;
typedef double LogProbability;

#define LOG_PROB_LOWER_BOUND (-INFINITY)
#define LOG_PROB_UPPER_BOUND (1.)

/**
 * this structure contains information on how many cases for measurements there are,
 * and what the probabilities of erroneous measurement there is for each case
 */
typedef struct sEtaFunction
{
	/**
	 * the number of different probability constants
	 */
	size_t constants;

	/**
	 * the constants array.
	 */

	Probability *C;

	/**
	 * the number of error types,
	 * usually this is equal to 2, where:
	 * :                PREDICTED (I) :
	 * : Error type     .   X         :
	 * :(B) MEASURED . (0)  1         :
	 * :(B) MEASURED X  0  (1)        :
	 *
	 */
	size_t types;

	/**
	 * the number of different measurements
	 */

	size_t measurements;

	/**
	 * the error constants, where the rows (t) are types,
	 * and the columns (m) are measurements, i.e.
	 * the error probability for type t error and measurement m is:
	 *
	 * C[eta[t*measurements + m]]
	 */
	size_t * eta;
}*EtaFunction;

EtaFunction
newEtaFunction(size_t types, size_t measurements, size_t constants);

EtaFunction
newUniformEtaFunction(size_t types, size_t measurements);

EtaFunction
newGeneralEtaFunction(size_t types, size_t measurements);

void
deleteEtaFunction(EtaFunction* eta);

/**
 * This struct represents a formal product of formal constants x_i
 * and their counterparts (1-x_i)
 * under the assumption of communtativity of the product. E.g.
 *
 * @f[  \prod_{i=0}^{\mathrm{constants}} x_i^{\mathrm{mismatch}(i)}
 *                               \cdot (1-x_i)^{\mathrm{match}(i)}  @f]
 */

typedef struct sCommutativeProduct
{
	/**
	 * the number of different formal probability constants
	 */
	size_t constants;
	/**
	 * This represents the first term part
	 *
	 * @f[  \prod_{i=0}^{\mathrm{constants}} x_i^{\mathrm{mismatch}(i)}  @f]
	 */
	size_t *mismatch;
	/**
	 * The represents the second term part
	 *
	 * @f[  \prod_{i=0}^{\mathrm{constants}} (1-x_i)^{\mathrm{match}(i)}  @f]
	 */
	size_t *match;
	/**
	 *  This is an intermediate helper structure used to calculate the log
	 *  probabilities: we need to store the intermediate results per constant and
	 *  matchtype.
	 *
	 *  Length of this array: 2*constants
	 */
	LogProbability *intermediate;

}*CommutativeProduct;

CommutativeProduct
newCommutativeProduct(size_t constants);

void
deleteCommutativeProduct(CommutativeProduct* p);

/**
 * This struct represents a map between the object sets of two formal contexts.
 */

typedef struct sConditionMap
{
	/**
	 * cardinality of the domain object set
	 */
	size_t objects;

	/**
	 * the object index of the image, i.e.
	 * c[i] is the index of the image of the object with index i in the domain
	 */
	size_t *c;

}*ConditionMap;

ConditionMap
newConditionMap(size_t objects);

void
deleteConditionMap(ConditionMap* c);

/**
 * This structure stores the logarithms of probabilities and of their complements.
 * We use logarithms to the base 2.
 */

typedef struct sLogCache
{
	/**
	 * number of constants
	 */
	size_t constants;

	/**
	 * logarithms of the probabilities, i.e.
	 * logC[i] = log2(C[i])
	 */
	LogProbability *logC;

	/**
	 * logarithms of the complementary probabilities, i.e.
	 * logNotC[i] = log2(1-C[i])
	 */
	LogProbability *logNotC;

}*LogCache;

LogCache
newLogCache(size_t constants);

void
deleteLogCache(LogCache* log_c);

void
calculateLogs(const EtaFunction eta, LogCache log_c);

LogProbability
logProbabilityFromProduct(const LogCache log_c, CommutativeProduct l);

LogProbability
sumUp(const LogProbability * restrict  V, size_t length,
		LogProbability lower_bound, LogProbability upper_bound);

/**
 * This structure stores the asymmetric distances between objects
 * of some formal context.
 */

typedef struct sDistanceMatrix {
	/**
	 * number of objects of the corresponding context,
	 * the matrix will have the dimension (objects x objects)
	 */
	size_t objects;

	/**
	 * stores the distance between measurements:
	 * let x,y be object indices of some context, then
	 *
	 * d[x * objects + y]
	 *
	 * contains the distance from x to y,
	 * which is the likelihood of measuring the attribute vector of y
	 * under the assumption that the measured object has indeed all
	 * the attributes corresponding to x.
	 */
	LogProbability *d;
} *DistanceMatrix;

DistanceMatrix
newDistanceMatrix(size_t objects);

void
deleteDistanceMatrix(DistanceMatrix* d);

void
writeDistancesToFile(const DistanceMatrix d, const char* filename);

void
normalizeDistanceMatrix(DistanceMatrix restrict d);

#endif /* MEASUREMENT_H_ */
