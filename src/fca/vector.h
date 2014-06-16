/**
 * vector.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

/**
 * @file
 * This header file provides interfaces with the faster IncidenceVector implementations
 */


#ifndef FCA__VECTOR_H_
#define FCA__VECTOR_H_

#include <inttypes.h>

#include "common.h"
#include "vector/macros.h"

/**
 * type of compressed attribute vectors
 */
typedef uint64_t *IncidenceVector;

/**
 * abstract formal context object with incidence vectors type used
 * for interface purposes
 */

struct sFormalContextV;
typedef struct sFormalContextV *FormalContextV;

/**
 * intent structure of a formal concept
 */

typedef struct sFormalIntentV
{
	/**
	 * nbr of attributes in this vector
	 */
	size_t attributes;
	/**
	 * the width, i.e. floor of (attributes+63)/64
	 */
	size_t width;
	/**
	 * attribute vector
	 */
	IncidenceVector incidence;
} FormalIntentV;


FormalContextV
newFormalContextV(unsigned int objects, unsigned int attributes);

void
deleteFormalContextV(FormalContextV* ctx);

void
writeFormalContextV(FormalContextV ctx, const char* filename);

FormalContextV
newFormalContextFromFileV(const char* filename);

#include "vector/measurement.h"

#endif /* VECTOR_H_ */
