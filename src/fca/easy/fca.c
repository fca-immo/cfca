/**
 * fca.c, (c) 2013, Immanuel Albrecht; Dresden University of
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

#include "private.h"

/** @file
 *  this file contains general formal context related operations and routines
 *
 */

/**
 * create a new formal context
 *
 * @param objects       object count
 * @param attributes    attribute count
 * @return a new FormalContext object
 */

FormalContext newFormalContext(int objects, int attributes)
{
	myFormalContext *ctx = malloc(sizeof(myFormalContext));

	ctx->attributes = attributes;
	ctx->objects = objects;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	ctx->attributeNames = calloc(attributes, sizeof(char*));
	ctx->objectNames = calloc(objects, sizeof(char*));

#pragma GCC diagnostic pop

	for (int var = 0; var < attributes; ++var)
	{
		ctx->attributeNames[var] = calloc(1, sizeof(char));
	}

	for (int var = 0; var < objects; ++var)
	{
		ctx->objectNames[var] = calloc(1, sizeof(char));
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	ctx->incidence = calloc(objects * attributes, sizeof(IncidenceCell));

#pragma GCC diagnostic pop

	return (FormalContext) ctx;
}

/**
 * create a new formal context object from a .cxt file
 *
 * @param filename
 * @return  the formal context that has been read from the given file
 */

FormalContext newFormalContextFromFile(const char* filename)
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

	int line_nbr;
	line_nbr = 0;

	int objects;
	int attributes;

	attributes = 0;
	objects = 0;

	myFormalContext *ctx;
	ctx = 0;

	while ((read = getline(&line, &len, file)) != -1)
	{

		/*
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
			objects = atoi(line);
		}
		else if (line_nbr == 3)
		{
			attributes = atoi(line);
			ctx = (myFormalContext *) newFormalContext(objects, attributes);
		}
		else if (line_nbr == 4)
		{
			//empty line
		}
		else if (line_nbr < objects + 5)
		{

			int i;
			i = line_nbr - 5;

			free(ctx->objectNames[i]);
			ctx->objectNames[i] = strdup(line);

		}
		else if (line_nbr < objects + attributes + 5)
		{
			int i;
			i = line_nbr - 5 - objects;

			free(ctx->attributeNames[i]);
			ctx->attributeNames[i] = strdup(line);
		}
		else if (line_nbr < objects * 2 + attributes + 5)
		{
			int i;
			i = line_nbr - 5 - objects - attributes;

			int width;
			width = MIN((signed)strlen(line),attributes);

			for (int var = 0; var < width; ++var)
			{
				if ((line[var] == 'x') || (line[var] == 'X')
						|| (line[var] == '1'))
				{
					CROSS(CELL (i, ctx, var));
				}
			}

		}
		else
		{
			/*
			 * we read all data
			 */
			break;
		}

		line_nbr++;
	}

	/*
	 * free memory and return
	 */

	grace: if (file != stdin)
	{
		fclose(file);
	}

	free(line);

	return (FormalContext) ctx;
}

/**
 * save the context ctx at the given file location
 * @param ctx
 * @param filename
 */

void writeFormalContext(FormalContext ctx, const char* filename)
{
	RETURN_IF_ZERO(ctx);
	RETURN_IF_ZERO(filename);

	FILE* file;
	file = fopen(filename, "w");

	RETURN_IF_ZERO(file);

	myFormalContext *c;
	c = (myFormalContext*) ctx;

	fprintf(file, "B\n\n%d\n%d\n\n", c->objects, c->attributes);

	for (int var = 0; var < c->objects; ++var)
	{
		fputs(c->objectNames[var], file);
		fputs("\n", file);
	}

	for (int var = 0; var < c->attributes; ++var)
	{
		fputs(c->attributeNames[var], file);
		fputs("\n", file);
	}

	for (int g = 0; g < c->objects; ++g)
	{
		for (int m = 0; m < c->attributes; ++m)
		{
			if ( gIm(g, c, m))
				fputs("X", file);
			else
				fputs(".", file);
		}
		fputs("\n", file);
	}

	fclose(file);
}

/**
 * deletes the formal context *ctx, and sets the pointer to zero
 *
 * @param ctx  pointer to the formal context object to be deleted
 */

void deleteFormalContext(FormalContext* ctx)
{
	RETURN_IF_ZERO(ctx);
	RETURN_IF_ZERO(*ctx);

	myFormalContext *c;

	c = (myFormalContext*) *ctx;

	*ctx = 0;

	for (int var = 0; var < c->attributes; ++var)
	{
		free(c->attributeNames[var]);
	}

	for (int var = 0; var < c->objects; ++var)
	{
		free(c->objectNames[var]);
	}

	free(c->objectNames);
	free(c->attributeNames);
	free(c->incidence);
	free(c);
}

/**
 * create a new formal concept chunk
 *
 * @param attributes  number of attributes of the hosting formal context
 * @return  a new concept chunk object
 */

myFormalConceptIntentChunk* newConceptChunk(int attributes)
{
	myFormalConceptIntentChunk *c;
	c = malloc(sizeof(myFormalConceptIntentChunk));
	c->attributes = attributes;
	c->size = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	c->incidence = calloc(attributes * CHUNKSIZE, sizeof(IncidenceCell));

#pragma GCC diagnostic pop

	return c;
}

/**
 * deletes a concept chunk object and sets its pointer to zero
 *
 * @param c   pointer to the concept chunk to be deleted
 */

void deleteConceptChunk(myFormalConceptIntentChunk** c)
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

FormalConceptIntentBulkList newConceptBulk(int attributes)
{
	FormalConceptIntentBulkList l;
	l = malloc(sizeof(struct sFormalConceptIntentBulkNode));

	l->attributes = attributes;
	l->size = 0;
	l->chunks = calloc(BULKSIZE, sizeof(myFormalConceptIntentChunk*));
	l->next = 0;
	return l;
}

/**
 * deletes the entire bulk list
 * @param rootNode   pointer to the first node
 */

void deleteConceptBulk(FormalConceptIntentBulkList* rootNode)
{
	RETURN_IF_ZERO(rootNode);
	RETURN_IF_ZERO(*rootNode);

	FormalConceptIntentBulkList l;
	l = *rootNode;
	*rootNode = 0;

	do
	{
		for (int var = 0; var < l->size; ++var)
		{
			deleteConceptChunk(&(l->chunks[var]));
		}

		FormalConceptIntentBulkList next;
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
int countConceptsInBulk(FormalConceptIntentBulkList root)
{
	RETURN_ZERO_IF_ZERO(root);

	int count = 0;

	while (root != 0)
	{
		if (root->size > 0)
		{
			/*
			 * count the full chunks
			 */
			count += CHUNKSIZE * (root->size - 1);
			/*
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

FormalConceptIntentBulkList addConceptToBulk(FormalConceptIntentBulkList root,
		const IncidenceCell* intent)
{
	RETURN_ZERO_IF_ZERO(root);

	do
	{

		if (root->size == 0)
		{
			root->chunks[0] = newConceptChunk(root->attributes);
			root->size = 1;
		}

		int last_index;
		last_index = root->size - 1;

		if (root->chunks[last_index]->size == CHUNKSIZE)
		{
			if (root->size == BULKSIZE)
			{
				if (root->next == 0)
				{
					root->next = newConceptBulk(root->attributes);
				}
				root = root->next;
				continue;
			}
			else
			{
				last_index = root->size++;
				root->chunks[last_index] = newConceptChunk(root->attributes);
			}
		}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

		memcpy(
				&(CELL(root->chunks[last_index]->size, root->chunks[last_index],0)),
				intent, sizeof(IncidenceCell) * root->attributes);

#pragma GCC diagnostic pop

		root->chunks[last_index]->size++;

		break;

	} while (1);

	return root;
}

/**
 *
 * close an attribute set, i.e. add further attributes,
 * 1.92 times slower than closeIntent
 *
 * @param ctx    formal context
 * @param input  the intent set that is to be closed
 * @param outputIntent  the closure intent'' wrt. ctx
 * @param outputExtent  the corresponding objects, i.e. intent' wrt. ctx
 */

void closeIntent2(FormalContext restrict ctx, const IncidenceCell* restrict input,
		IncidenceCell* restrict outputIntent, IncidenceCell* restrict outputExtent)
{
	myFormalContext* restrict I;
	I = (myFormalContext*) ctx;

	for (int g = 0; g < I->objects; ++g)
	{
		CROSS(outputExtent[g]);
		for (int m = 0; m < I->attributes; ++m)
		{
			if (INCIDES(input[m]))
				if (!gIm(g,I,m))
				{
					/*
					 * some attribute is not present for this object -> next object
					 */
					CLEAR(outputExtent[g]);
					break;
				}
		}
	}

	for (int m = 0; m < I->attributes; ++m)
	{
		CROSS(outputIntent[m]);
		for (int g = 0; g < I->objects; ++g)
		{
			if (INCIDES(outputExtent[g]))
				if (!gIm(g,I,m))
				{
					CLEAR(outputIntent[m]);
					break;
				}
		}
	}

}

/**
 * close an attribute set, i.e. add further attributes
 *
 * @param ctx    formal context
 * @param input  the intent set that is to be closed
 * @param output  the closure intent'' wrt. ctx
 */

void closeIntent(FormalContext ctx, const IncidenceCell* restrict input,
		IncidenceCell* restrict output)
{

	myFormalContext* I;
	I = (myFormalContext*) ctx;
	for (int var = 0; var < I->attributes; ++var)
	{
		CROSS(output[var]);
	}

	for (int g = 0; g < I->objects; ++g)
	{
		int good;
		good = 1;

		for (int m = 0; m < I->attributes; ++m)
		{
			if (INCIDES(input[m]))
				if (!gIm(g,I,m))
				{
					/*
					 * some attribute is not present for this object -> next object
					 */
					good = 0;
					break;
				}
		}
		if (good)
			/*
			 * remove attributes that are not common among all objects that have the input
			 * attributes
			 */
			for (int m = 0; m < I->attributes; ++m)
			{
				if (!gIm(g,I,m))
				{
					CLEAR(output[m]);
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

int intentCmp(int attributes, const IncidenceCell* minus,
		const IncidenceCell* plus)
{
	for (int var = 0; var < attributes; ++var)
	{
		if (INCIDES(minus[var]))
		{
			if (!INCIDES((plus[var])))
				return -1;
		}
		else if (INCIDES(plus[var]))
		{
			return 1;
		}
	}
	return 0;
}

/**
 * creates a new formal concept intent chunk and fills it with the intents of all formal
 * concepts in the concept lattice of ctx, using next closure algorithm
 *
 * @param ctx   formal context
 * @return   concept intents
 */
FormalConceptIntentBulkList newConceptBulkFromContext(FormalContext ctx)
{
	RETURN_ZERO_IF_ZERO(ctx);

	myFormalContext *c;
	c = (myFormalContext*) ctx;

	IncidenceCell *M;
	IncidenceCell *Y;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	Y = calloc(c->attributes, sizeof(IncidenceCell));
	M = malloc(c->attributes * sizeof(IncidenceCell));

#pragma GCC diagnostic pop

	/*
	 * calculate the bottom intent of the concept lattice, i.e. {}''
	 */
	closeIntent(ctx, Y, M);

	FormalConceptIntentBulkList root;
	FormalConceptIntentBulkList last;

	root = newConceptBulk(c->attributes);

	/*
	 * add the bottom element of the concept lattice (a concept lattice is never empty)
	 */

	last = addConceptToBulk(root, M);

	/*
	 * begin of nextClosure function iteration
	 */
	nextClosure:

	for (int i = c->attributes - 1; i >= 0; --i)
	{

		if (!INCIDES(M[i]))
		{
			CROSS(M[i]);
			closeIntent(ctx, M, Y);

			int good;
			good = 1;

			for (int j = 0; j < i; ++j)
			{
				if (INCIDES(Y[j]))
				{
					if (!INCIDES((M[j])))
					{

						good = 0;
						break;
					}
				}
			}
			if (good)
			{
				/*
				 * we found the next intent
				 */
				last = addConceptToBulk(last, Y);

				/*
				 * continue with Y for M
				 */

				IncidenceCell *DELTA;
				DELTA = M;
				M = Y;
				Y = DELTA;
				/*
				 * do the nextClosure
				 */
				goto nextClosure;
			}
		}

		CLEAR(M[i]);
	}

	/*
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

void writeConceptsToFile(FormalContext ctx, FormalConceptIntentBulkList root,
		const char* filename)
{
	RETURN_IF_ZERO(root);

	myFormalContext* c;

	if (ctx != 0)
	{
		c = (myFormalContext*) ctx;

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

	int objects;
	objects = countConceptsInBulk(root);

	fprintf(file, "B\n\n%d\n%d\n\n", objects, root->attributes);

	for (int var = 0; var < objects; ++var)
	{
		fprintf(file, "C%8d\n", (var + 1));
	}

	if (c != 0)
	{
		for (int var = 0; var < c->attributes; ++var)
		{
			fputs(c->attributeNames[var], file);
			fputs("\n", file);
		}
	}
	else
	{
		for (int var = 0; var < root->attributes; ++var)
		{
			fprintf(file, "m%8d\n", (var + 1));
		}
	}

	for (; root != 0; root = root->next)
	{
		for (int chunk = 0; chunk < root->size; ++chunk)
		{
			for (int g = 0; g < root->chunks[chunk]->size; ++g)
			{
				for (int m = 0; m < root->attributes; ++m)
				{
					if ( gIm(g, root->chunks[chunk], m))
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
 * create a new formal context with random incidence relation
 *
 * @param objects
 * @param attributes
 * @param p  probability of a cross
 * @return  context
 */

FormalContext newFormalContextFromRandom(int objects, int attributes, float p)
{
	FormalContext ctx;
	ctx = newFormalContext(objects, attributes);

	myFormalContext *c;

	c = (myFormalContext*) ctx;

	for (int g = 0; g < c->objects; ++g)
	{
		for (int m = 0; m < c->attributes; ++m)
		{
			float x;
			x = (float) random() / (float) RAND_MAX;
			if (x >= p)
			{
				CROSS(CELL(g,c,m));
			}
		}
	}
	return ctx;
}

/**
 * counts the concepts in the concept lattice of ctx, using next closure algorithm
 *
 * @param ctx   formal context
 * @return   number of concepts in context
 */
int countContextConcepts2(FormalContext ctx)
{
	RETURN_ZERO_IF_ZERO(ctx);

	myFormalContext *c;
	c = (myFormalContext*) ctx;

	IncidenceCell *M;
	IncidenceCell *Y;
	IncidenceCell *extent;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	Y = calloc(c->attributes, sizeof(IncidenceCell));
	M = malloc(c->attributes * sizeof(IncidenceCell));
	extent = malloc(c->objects * sizeof(IncidenceCell));

#pragma GCC diagnostic pop

	/*
	 * calculate the bottom intent of the concept lattice, i.e. {}''
	 */
	closeIntent2(ctx, Y, M, extent);

	int count;

	count = 1;

	/*
	 * begin of nextClosure function iteration
	 */
	nextClosure:

	for (int i = c->attributes - 1; i >= 0; --i)
	{

		if (!INCIDES(M[i]))
		{
			CROSS(M[i]);
			closeIntent2(ctx, M, Y, extent);

			int good;
			good = 1;

			for (int j = 0; j < i; ++j)
			{
				if (INCIDES(Y[j]))
				{
					if (!INCIDES((M[j])))
					{

						good = 0;
						break;
					}
				}
			}
			if (good)
			{
				/*
				 * we found the next intent
				 */
				count++;

				/*
				 * continue with Y for M
				 */

				IncidenceCell *DELTA;
				DELTA = M;
				M = Y;
				Y = DELTA;
				/*
				 * do the nextClosure
				 */
				goto nextClosure;
			}
		}

		CLEAR(M[i]);
	}

	/*
	 * free up memory
	 */

	free(M);
	free(Y);
	free(extent);
	return count;
}

/**
 * counts the concepts in the concept lattice of ctx, using next closure algorithm
 *
 * @param ctx   formal context
 * @return   number of concepts in context
 */
int countContextConcepts(FormalContext ctx)
{
	RETURN_ZERO_IF_ZERO(ctx);

	myFormalContext *c;
	c = (myFormalContext*) ctx;

	IncidenceCell *M;
	IncidenceCell *Y;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

	Y = calloc(c->attributes, sizeof(IncidenceCell));
	M = malloc(c->attributes * sizeof(IncidenceCell));

#pragma GCC diagnostic pop

	/*
	 * calculate the bottom intent of the concept lattice, i.e. {}''
	 */
	closeIntent(ctx, Y, M);

	int count;

	count = 1;

	/*
	 * begin of nextClosure function iteration
	 */
	nextClosure:

	for (int i = c->attributes - 1; i >= 0; --i)
	{

		if (!INCIDES(M[i]))
		{
			CROSS(M[i]);
			closeIntent(ctx, M, Y);

			int good;
			good = 1;

			for (int j = 0; j < i; ++j)
			{
				if (INCIDES(Y[j]))
				{
					if (!INCIDES((M[j])))
					{

						good = 0;
						break;
					}
				}
			}
			if (good)
			{
				/*
				 * we found the next intent
				 */
				count++;

				/*
				 * continue with Y for M
				 */

				IncidenceCell *DELTA;
				DELTA = M;
				M = Y;
				Y = DELTA;
				/*
				 * do the nextClosure
				 */
				goto nextClosure;
			}
		}

		CLEAR(M[i]);
	}

	/*
	 * free up memory
	 */

	free(M);
	free(Y);

	return count;
}
