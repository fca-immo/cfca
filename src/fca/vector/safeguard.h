/**
 * vector/safeguard.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

#ifndef FCA__VECTOR_SAFEGUARD_H_
#define FCA__VECTOR_SAFEGUARD_H_

/** @file
 *  this file contains safe-guard definitions that prevent using fca/easy functions
 *  with vector code
 *
 *  this file helps a lot when porting easy-fca code to vector-fca code by creating
 *  compiler errors when using the wrong functions
 */

#define ERROR_TOKEN EASY_VERSION_CODE_WITH_VECTOR_FUNCTIONS

#define CHUNKSIZE ERROR_TOKEN
#define BULKSIZE ERROR_TOKEN

#define smyFormalContext ERROR_TOKEN
#define myFormalContext ERROR_TOKEN
#define smyFormalConceptIntentChunk ERROR_TOKEN
#define myFormalConceptIntentChunk ERROR_TOKEN

#define newConceptChunk ERROR_TOKEN
#define deleteConceptChunk ERROR_TOKEN
#define newConceptBulk ERROR_TOKEN
#define newConceptBulkFromContext ERROR_TOKEN
#define writeConceptsToFile ERROR_TOKEN
#define deleteConceptBulk ERROR_TOKEN
#define countConceptsInBulk ERROR_TOKEN
#define addConceptToBulk ERROR_TOKEN
#define closeIntent ERROR_TOKEN
#define closeIntent2 ERROR_TOKEN
#define intentCmp ERROR_TOKEN

#define IncidenceCell ERROR_TOKEN
#define sFormalContext ERROR_TOKEN
#define FormalContext ERROR_TOKEN
#define sFormalIntent ERROR_TOKEN
#define FormalIntent ERROR_TOKEN
#define newFormalContext ERROR_TOKEN
#define newFormalContextFromRandom ERROR_TOKEN
#define newFormalContextFromFile ERROR_TOKEN
#define countContextConcepts ERROR_TOKEN
#define writeFormalContext ERROR_TOKEN
#define deleteFormalContext ERROR_TOKEN


#endif /* SAFEGUARD_H_ */
