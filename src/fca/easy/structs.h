/**
 * easy/structs.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

#ifndef FCA__EASY_STRUCTS_H_
#define FCA__EASY_STRUCTS_H_


/**
 * @def CHUNKSIZE
 * size of a chunk of concepts
 */
#define CHUNKSIZE (64)



/**
 * @def BULKSIZE
 * size of chunks per bulk
 */
#define BULKSIZE (1024)

/**
 * @def INPUTBUFFERSIZE
 * maximal (initial) size of a line (getline will resize buffers if necessary)
 *   (including delimiter)
 */
#define INPUTBUFFERSIZE (1024)


/**
 * each formal context has a finite number of objects and attributes,
 * which may have names (though we do not require them to be unique or given),
 * and an incidence relation which is represented by an objects×attributes-IncidenceCell matrix
 */

typedef struct smyFormalContext
{
	int attributes, objects;
	char** attributeNames;
	char** objectNames;
	IncidenceCell* incidence;
} myFormalContext;



/**
 * A chunk of at most CHUNKSIZE formal concept intents
 */

typedef struct smyFormalConceptIntentChunk
{
	/**
	 * number of attributes
	 */
	int attributes;
	/**
	 * how many formal concepts are in this chunk
	 */
	int size;
	/**
	 * the intents of the concepts
	 */
	IncidenceCell* incidence;
} myFormalConceptIntentChunk;



/**
 * a node of a single linked list of concept chunks.
 * bulk nodes are filled chunk wise, but a bulk node may have non-empty
 * successor nodes even if it is not entire full.
 */

typedef struct sFormalConceptIntentBulkNode
{
	/**
	 * number of attributes of the concept intents
	 */
	int attributes;
	/**
	 * number of chunks used
	 */
	int size;
	/**
	 * array to at most BULKSIZE chunks
	 */
	myFormalConceptIntentChunk** chunks;
	/**
	 * pointer to the next BulkNode, or 0
	 */
	struct sFormalConceptIntentBulkNode* next;
}* FormalConceptIntentBulkList;




#endif /* STRUCTS_H_ */
