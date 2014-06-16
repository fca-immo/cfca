/**
 * easy/private.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

#ifndef FCA__EASY_PRIVATE_H_
#define FCA__EASY_PRIVATE_H_

#include "../easy.h"
#include "structs.h"

myFormalConceptIntentChunk*
newConceptChunk(int attributes);

void
deleteConceptChunk(myFormalConceptIntentChunk** c);

FormalConceptIntentBulkList
newConceptBulk(int attributes);

FormalConceptIntentBulkList
newConceptBulkFromContext(FormalContext ctx);

void
writeConceptsToFile(FormalContext ctx, FormalConceptIntentBulkList root,
		const char* filename);

void
deleteConceptBulk(FormalConceptIntentBulkList* rootNode);

int
countConceptsInBulk(FormalConceptIntentBulkList root);

FormalConceptIntentBulkList
addConceptToBulk(FormalConceptIntentBulkList root, const IncidenceCell* intent);

void
closeIntent(FormalContext restrict ctx, const IncidenceCell* restrict input,
		IncidenceCell* restrict output);

void
closeIntent2(FormalContext restrict ctx, const IncidenceCell* restrict input,
		IncidenceCell* restrict outputIntent,
		IncidenceCell* restrict outputExtent);

int
intentCmp(int attributes, const IncidenceCell* minus, const IncidenceCell* plus);

#endif /* PRIVATE_H_ */
