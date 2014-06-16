/**
 * easy.h, (c) 2013, Immanuel Albrecht; Dresden University of
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
 * This header file provides interfaces with the easy IncidenceCell* implementations
 */

#ifndef FCA__EASY_H_
#define FCA__EASY_H_

#include <inttypes.h>

#include "common.h"
#include "easy/macros.h"

/**
 * type of the incidence relation matrix cells
 */
typedef int8_t IncidenceCell;

/**
 * abstract formal context object type used for interface purposes
 */

struct sFormalContext;
typedef struct sFormalContext *FormalContext;

/**
 * intent structure of a formal concept
 */

typedef struct sFormalIntent
{
	size_t attributes;
	IncidenceCell* incidence;
} FormalIntent;

FormalContext
newFormalContext(int objects, int attributes);

FormalContext
newFormalContextFromRandom(int objects, int attributes, float p);

FormalContext
newFormalContextFromFile(const char* filename);

int
countContextConcepts(FormalContext ctx);

void
writeFormalContext(FormalContext ctx, const char* filename);

void
deleteFormalContext(FormalContext* ctx);

#include "easy/measurement.h"

#endif /* EASY_H_ */
