/**
 * fcaV.c, (c) 2013, Immanuel Albrecht; Dresden University of
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
#include <sys/types.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <float.h>

#include "safeguard.h"
#include "private.h"

/** @file
 *  this file contains general formal context related operations and routines with
 *  IncidenceVector implementation
 *
 */

/**
 * create a new formal context
 *
 * @param objects       object count
 * @param attributes    attribute count
 * @return a new FormalContext object
 */

FormalContextV newFormalContextV(unsigned int objects, unsigned int attributes)
{
	myFormalContextV *ctx = malloc(sizeof(myFormalContextV));

	ctx->attributes = attributes;
	ctx->objects = objects;
	ctx->width = WIDTH(attributes);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	ctx->attributeNames = calloc(attributes, sizeof(char*));
	ctx->objectNames = calloc(objects, sizeof(char*));

#pragma GCC diagnostic pop

	for (unsigned int var = 0; var < attributes; ++var)
	{
		ctx->attributeNames[var] = calloc(1, sizeof(char));
	}

	for (unsigned int var = 0; var < objects; ++var)
	{
		ctx->objectNames[var] = calloc(1, sizeof(char));
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	ctx->incidence = calloc(objects * ctx->width, sizeof(uint64_t));

#pragma GCC diagnostic pop

	return (FormalContextV) ctx;
}

/**
 * deletes the formal context *ctx, and sets the pointer to zero
 *
 * @param ctx  pointer to the formal context object to be deleted
 */

void deleteFormalContextV(FormalContextV* ctx)
{
	RETURN_IF_ZERO(ctx);
	RETURN_IF_ZERO(*ctx);

	myFormalContextV *c;

	c = (myFormalContextV*) *ctx;

	*ctx = 0;

	for (unsigned int var = 0; var < c->attributes; ++var)
	{
		free(c->attributeNames[var]);
	}

	for (unsigned int var = 0; var < c->objects; ++var)
	{
		free(c->objectNames[var]);
	}

	free(c->objectNames);
	free(c->attributeNames);
	free(c->incidence);
	free(c);
}

/**
 * create a new formal context object from a .cxt file
 *
 * @param filename
 * @return  the formal context that has been read from the given file
 */

FormalContextV newFormalContextFromFileV(const char* filename)
{
	char *line;
	size_t len;

	len = (INPUTBUFFERSIZE);
	line = malloc(sizeof(char) * len);

	FILE *file;

	if (strcmp(filename, "-") == 0)
	{
		file = stdin;
	}
	else
	{
		file = fopen(filename, "r");
		RETURN_ZERO_IF_ZERO(file);
	}

	ssize_t read;

	unsigned int line_nbr;
	line_nbr = 0;

	unsigned int objects;
	unsigned int attributes;

	attributes = 0;
	objects = 0;

	myFormalContextV *ctx;
	ctx = 0;

	while ((read = getline(&line, &len, file)) != -1)
	{

		/**
		 * this should never happen, right?
		 */
		if (read == 0)
			break;
		line[read - 1] = 0;

		if (line_nbr == 0)
		{
			if (strcmp(line, "B"))
			{
				fprintf(stderr, "File '%s' is not a .cxt file\n", filename);
				goto grace;
			}
		}
		else if (line_nbr == 1)
		{
			//empty line
		}
		else if (line_nbr == 2)
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
			objects = atoi(line);
#pragma GCC diagnostic pop
		}
		else if (line_nbr == 3)
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
			attributes = atoi(line);
			ctx = (myFormalContextV *) newFormalContextV(objects, attributes);
#pragma GCC diagnostic pop
		}
		else if (line_nbr == 4)
		{
			//empty line
		}
		else if (line_nbr < objects + 5)
		{

			unsigned int i;
			i = line_nbr - 5;

			free(ctx->objectNames[i]);
			ctx->objectNames[i] = strdup(line);

		}
		else if (line_nbr < objects + attributes + 5)
		{
			unsigned int i;
			i = line_nbr - 5 - objects;

			free(ctx->attributeNames[i]);
			ctx->attributeNames[i] = strdup(line);
		}
		else if (line_nbr < objects * 2 + attributes + 5)
		{
			unsigned int i;
			i = line_nbr - 5 - objects - attributes;

			unsigned int width;
			width = MIN((unsigned int)strlen(line),attributes);

			for (unsigned int var = 0; var < width; ++var)
			{
				if ((line[var] == 'x') || (line[var] == 'X')
						|| (line[var] == '1'))
				{
					CROSSV(ROW(i, ctx), var);
				}
			}

		}
		else
		{
			/**
			 * we read all data
			 */
			break;
		}

		line_nbr++;
	}

	/**
	 * free memory and return
	 */

	grace: if (file != stdin)
	{
		fclose(file);
	}

	free(line);

	return (FormalContextV) ctx;
}

/**
 * save the context ctx at the given file location
 * @param ctx
 * @param filename
 */

void writeFormalContextV(FormalContextV ctx, const char* filename)
{
	RETURN_IF_ZERO(ctx);
	RETURN_IF_ZERO(filename);

	FILE* file;
	file = fopen(filename, "w");

	RETURN_IF_ZERO(file);

	myFormalContextV *c;
	c = (myFormalContextV*) ctx;

	fprintf(file, "B\n\n%zu\n%zu\n\n", c->objects, c->attributes);

	for (unsigned int var = 0; var < c->objects; ++var)
	{
		fputs(c->objectNames[var], file);
		fputs("\n", file);
	}

	for (unsigned int var = 0; var < c->attributes; ++var)
	{
		fputs(c->attributeNames[var], file);
		fputs("\n", file);
	}

	for (unsigned int g = 0; g < c->objects; ++g)
	{
		for (unsigned int m = 0; m < c->attributes; ++m)
		{
			if ( INCIDESV(ROW(g, c), m))
				fputs("X", file);
			else
				fputs(".", file);
		}
		fputs("\n", file);
	}

	fclose(file);
}

/**
 * counts the concepts in the concept lattice of ctx, using next closure algorithm
 *
 * @param ctx   formal context
 * @return   number of concepts in context
 */
int countContextConceptsV(FormalContextV ctx)
{
	RETURN_ZERO_IF_ZERO(ctx);

	myFormalContextV *restrict c;
	c = (myFormalContextV*) ctx;

	IncidenceVector restrict M;
	IncidenceVector restrict Y;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	Y = calloc(c->width, sizeof(uint64_t));
	M = malloc(c->width * sizeof(uint64_t));

#pragma GCC diagnostic pop

	/**
	 * calculate the bottom intent of the concept lattice, i.e. {}''
	 */
	closeIntentV(ctx, Y, M);

	int count;

	count = 1;

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
				 * we found the next intent
				 */
				count++;

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

	/**
	 * free up memory
	 */

	free(M);
	free(Y);

	return count;
}

/**
 * close an attribute set, i.e. add further attributes
 *
 * @param ctx    formal context
 * @param input  the intent set that is to be closed
 * @param output  the closure intent'' wrt. ctx
 */

void closeIntentV(FormalContextV restrict ctx,
		const IncidenceVector restrict input, IncidenceVector restrict output)
{

	myFormalContextV* restrict I;
	I = (myFormalContextV*) ctx;
	for (size_t var = 0; var < I->width; ++var)
	{
		output[var] = ~0ULL;
	}

	MASKVECTOR(output, I->attributes);

	for (size_t g = 0; g < I->objects; ++g)
	{
		int good;
		good = 1;

		for (size_t i = 0; i < I->width; ++i)
		{
			if ((input[i]) & (~(ROW(g,I)[i])))
			{
				/**
				 * some attribute is not present for this object -> next object
				 */
				good = 0;
				break;
			}

		}

		if (good)
		{
			/**
			 * remove attributes that are not common among all objects that have the input
			 * attributes
			 */
			for (size_t i = 0; i < I->width; ++i)
			{
				output[i] &= ROW(g,I)[i];
			}
		}
	}
}

/**
 * compare two intent vectors
 *
 * @param attributes   attribute count
 * @param minus        "left" operand
 * @param plus         "right" operand
 * @return -1 if minus is bigger, 1 if plus is bigger,
 *        0 if minus and plus is the same
 */

int intentCmpV(size_t attributes, const IncidenceVector minus,
		const IncidenceVector plus)
{
	for (size_t var = 0; var < OFFSET(attributes); ++var)
	{
		if (minus[var] > plus[var])
			return -1;
		if (plus[var] > minus[var])
			return 1;
	}

	if (BITNBR(attributes))
	{
		/**
		 *  in this case, OFFSET(attributes) == OFFSET(attributes-1)
		 *
		 *  we only check the lower bits 0 through (attributes-1)
		 *
		 */
		uint64_t l, r;

		l = minus[OFFSET(attributes)] & CRIMPVALUE(attributes-1);
		r = plus[OFFSET(attributes)] & CRIMPVALUE(attributes-1);

		if (l > r)
			return -1;

		if (r > l)
			return 1;
	}
	/**
	 * ELSE: attributes has 64 as factor, so we have done all necessary comparisons in the first loop.
	 */
	return 0;
}

/**
 * create a new formal concept chunk
 *
 * @param attributes  number of attributes of the hosting formal context
 * @return  a new concept chunk object
 */

myFormalConceptIntentChunkV* newConceptChunkV(size_t attributes)
{

	myFormalConceptIntentChunkV *c;

	c = malloc(sizeof(myFormalConceptIntentChunkV));

	c->attributes = attributes;
	c->width = WIDTH(attributes);
	c->size = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	c->incidence = calloc(c->width * CHUNKSIZEV, sizeof(uint64_t));

#pragma GCC diagnostic pop

	return c;
}

/**
 * deletes a concept chunk object and sets its pointer to zero
 *
 * @param c   pointer to the concept chunk to be deleted
 */

void deleteConceptChunkV(myFormalConceptIntentChunkV** c)
{
	RETURN_IF_ZERO(c);
	RETURN_IF_ZERO(*c);

	free((*c)->incidence);

	free(*c);
	*c = 0;
}

/**
 * creates a new formal concept intent bulk list
 *
 * @param attributes  number of attributes of the concept intents
 * @return new formal concept intent bulk list's first node
 */

FormalConceptIntentBulkListV newConceptBulkV(size_t attributes)
{
	FormalConceptIntentBulkListV l;
	l = malloc(sizeof(struct sFormalConceptIntentBulkNodeV));

	l->attributes = attributes;
	l->width = WIDTH(attributes);
	l->size = 0;
	l->chunks = calloc(BULKSIZEV, sizeof(myFormalConceptIntentChunkV*));
	l->next = 0;
	return l;
}

/**
 * creates a new formal concept intent chunk and fills it with the intents of all formal
 * concepts in the concept lattice of ctx, using next closure algorithm
 *
 * @param ctx   formal context
 * @return   concept intents
 */

FormalConceptIntentBulkListV newConceptBulkFromContextV(FormalContextV ctx)
{
	RETURN_ZERO_IF_ZERO(ctx);

	myFormalContextV * restrict c;
	c = (myFormalContextV*) ctx;

	IncidenceVector restrict M;
	IncidenceVector restrict Y;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	Y = calloc(c->width, sizeof(uint64_t));
	M = malloc(c->width * sizeof(uint64_t));

#pragma GCC diagnostic pop

	/**
	 * calculate the bottom intent of the concept lattice, i.e. {}''
	 */
	closeIntentV(ctx, Y, M);

	FormalConceptIntentBulkListV root;
	FormalConceptIntentBulkListV last;

	root = newConceptBulkV(c->attributes);

	/**
	 * add the bottom element of the concept lattice (a concept lattice is never empty)
	 */

	last = addConceptToBulkV(root, M);

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

	/**
	 * free up memory
	 */

	free(M);
	free(Y);

	return root;
}

/**
 * write a list of concept intents into a .cxt file
 *
 * @param ctx    formal context (or 0, is used for attribute names)
 * @param root   the first node of the formal concept intent bulk
 * @param filename   output file name (.cxt)
 */

void writeConceptsToFileV(FormalContextV ctx, FormalConceptIntentBulkListV root,
		const char* filename)
{
	RETURN_IF_ZERO(root);

	myFormalContextV* c;

	if (ctx != 0)
	{
		c = (myFormalContextV*) ctx;

		WARN_IF_UNEQUAL_DO(c->attributes, root->attributes, c = 0);
	}
	else
	{
		c = 0;
	}

	RETURN_IF_ZERO(filename);

	FILE* file;
	file = fopen(filename, "w");

	RETURN_IF_ZERO(file);

	size_t objects;
	objects = countConceptsInBulkV(root);

	fprintf(file, "B\n\n%zu\n%zu\n\n", objects, root->attributes);

	for (size_t var = 0; var < objects; ++var)
	{
		fprintf(file, "C%8zu\n", (var + 1));
	}

	if (c != 0)
	{
		for (size_t var = 0; var < c->attributes; ++var)
		{
			fputs(c->attributeNames[var], file);
			fputs("\n", file);
		}
	}
	else
	{
		for (size_t var = 0; var < root->attributes; ++var)
		{
			fprintf(file, "m%8zu\n", (var + 1));
		}
	}

	for (; root != 0; root = root->next)
	{
		for (size_t chunk = 0; chunk < root->size; ++chunk)
		{
			for (size_t g = 0; g < root->chunks[chunk]->size; ++g)
			{
				for (size_t m = 0; m < root->attributes; ++m)
				{
					if (INCIDESV(ROW(g, root->chunks[chunk]), m))
						fputs("X", file);
					else
						fputs(".", file);
				}
				fputs("\n", file);
			}
		}
	}

	fclose(file);
}

/**
 * deletes the entire bulk list
 * @param rootNode   pointer to the first node
 */

void deleteConceptBulkV(FormalConceptIntentBulkListV* rootNode)
{
	RETURN_IF_ZERO(rootNode);
	RETURN_IF_ZERO(*rootNode);

	FormalConceptIntentBulkListV l;
	l = *rootNode;
	*rootNode = 0;

	do
	{
		for (size_t var = 0; var < l->size; ++var)
		{
			deleteConceptChunkV(&(l->chunks[var]));
		}

		FormalConceptIntentBulkListV next;
		next = l->next;

		free(l->chunks);
		free(l);

		l = next;
	} while (l != 0);
}

/**
 * use this for bulks that are filled in order
 *
 * @param root
 * @return  number of concepts in bulk
 */
size_t countConceptsInBulkV(FormalConceptIntentBulkListV root)
{
	RETURN_ZERO_IF_ZERO(root);

	size_t count = 0;

	while (root != 0)
	{
		if (root->size > 0)
		{
			/**
			 * count the full chunks
			 */
			count += CHUNKSIZEV * (root->size - 1);
			/**
			 * and the last chunk
			 */
			count += root->chunks[root->size - 1]->size;
		}
		root = root->next;
	}

	return count;
}

/**
 * copies the given intent to the bulk denoted by the root node.
 *
 *
 * @param root    root node of the bulk
 * @param intent  read-only pointer to an array of IncidenceCell[root->attributes]
 *
 * @return the node where the intent was added to the last chunk
 */

FormalConceptIntentBulkListV addConceptToBulkV(
		FormalConceptIntentBulkListV root, const IncidenceVector intent)
{
	RETURN_ZERO_IF_ZERO(root);

	do
	{

		if (root->size == 0)
		{
			root->chunks[0] = newConceptChunkV(root->attributes);
			root->size = 1;
		}

		size_t last_index;
		last_index = root->size - 1;

		if (root->chunks[last_index]->size == CHUNKSIZEV)
		{
			if (root->size == BULKSIZEV)
			{
				if (root->next == 0)
				{
					root->next = newConceptBulkV(root->attributes);
				}
				root = root->next;
				continue;
			}
			else
			{
				last_index = root->size++;
				root->chunks[last_index] = newConceptChunkV(root->attributes);
			}
		}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

		memcpy( ROW(root->chunks[last_index]->size, root->chunks[last_index]),
				intent, sizeof(uint64_t) * root->width);

#pragma GCC diagnostic pop

		root->chunks[last_index]->size++;

		break;

	} while (1);

	return root;
}

