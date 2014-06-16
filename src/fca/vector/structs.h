/**
 * vector/structs.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

#ifndef FCA__VECTOR_STRUCTS_H_
#define FCA__VECTOR_STRUCTS_H_

/**
 * @def CHUNKSIZEV
 * size of a chunk of vector concepts
 */
#define CHUNKSIZEV (1024)

/**
 * @def BULKSIZEV
 * size of chunks per bulk (vector version)
 */
#define BULKSIZEV (1024)


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
 *
 * for the vector implementation, we have the variable width which codes the width of each object's
 * IncidenceVector
 */

typedef struct smyFormalContextV
{
	size_t attributes, objects;
	size_t width;
	char** attributeNames;
	char** objectNames;
	IncidenceVector incidence;
} myFormalContextV;


/**
 * A chunk of at most CHUNKSIZEV formal concept intent vectors
 */

typedef struct smyFormalConceptIntentChunkV
{
	/**
	 * number of attributes
	 */
	size_t attributes;
	/**
	 * width of each attribute vector
	 */
	size_t width;
	/**
	 * how many formal concepts are in this chunk
	 */
	size_t size;
	/**
	 * the intents of the concepts
	 */
	IncidenceVector incidence;
} myFormalConceptIntentChunkV;


/**
 * a node of a single linked list of concept chunks.
 * bulk nodes are filled chunk wise, but a bulk node may have non-empty
 * successor nodes even if it is not entire full.
 */

typedef struct sFormalConceptIntentBulkNodeV
{
	/**
	 * number of attributes of the concept intents
	 */
	size_t attributes;
	/**
	 * width of each attribute vector
	 */
	size_t width;
	/**
	 * number of chunks used
	 */
	size_t size;
	/**
	 * array to at most BULKSIZEV chunks
	 */
	myFormalConceptIntentChunkV** chunks;
	/**
	 * pointer to the next BulkNode, or 0
	 */
	struct sFormalConceptIntentBulkNodeV* next;
}* FormalConceptIntentBulkListV;

#endif /* STRUCTS_H_ */
