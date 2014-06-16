/**
 * gen-concepts.c, (c) 2014, Immanuel Albrecht; Dresden University of
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <float.h>

#include "fca/fca.h"
#include "fca/easy/private.h"
#include "fca/vector/private.h"

int main(int argc, const char *argv[])
{
    int mode = 0; /** use single threaded vector mode */
    int only_count = 0;
    int i;
    
    /**
     * variables for next closure applications
     */

    FormalContext cxt;
    FormalConceptIntentBulkList bulk;

    FormalContextV cxtV;
    FormalConceptIntentBulkListV bulkV;
    

    char output_name[1024];
    


    puts("Calculate Concepts\n");


    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i],"--help") == 0) {
            puts("...generates all intents of formal concepts for a given formal context");
            printf("Usage: %s [-E | -V | -Vx] [-v | -o] input1.txt input2.txt ....\n",argv[0]);
            puts(" where input*.txt are the input contexts in Burmeister format");
            puts(" All concepts are written to $FILENAME.concepts unless in count mode.");
            puts("");
            puts("Optional Flags:");
            puts("   -E     use easy implementation");
            puts("   -V     use vector implementation");
            puts("   -Vx    use parallelized vector implementation");
            puts("");
            puts("   -c     only count the number of concepts, do not write output");
            puts("   -v     write all concepts to $INPUT.concepts file");
        }
        else if (strcmp(argv[i],"-E") == 0) {
            mode = 2;
        } 
        else if (strcmp(argv[i],"-Vx") == 0) {
            mode = 1;
        } 
        else if (strcmp(argv[i],"-c") == 0) {
            only_count = 1;
        } 
        else if (strcmp(argv[i],"-v") == 0) {
            only_count = 0;
        } 
        else if (strcmp(argv[i],"-V") == 0) {
            mode = 0;
        } else {

            printf("Reading %s...\n",argv[i]);
            if (mode==2) {
                cxt = newFormalContextFromFile(argv[i]);
                bulk = newConceptBulkFromContext(cxt);
                
                if (only_count)
                    printf("Concepts: %d\n",countConceptsInBulk(bulk));
                else {
                    strncpy(output_name, argv[i], 1000);
                    strcat(output_name, ".concepts");
                    printf("Writing %s...\n",output_name);
                    writeConceptsToFile(cxt, bulk, output_name);
                }

                deleteConceptBulk(&bulk);
                deleteFormalContext(&cxt);
            } else if (mode == 0) {
                cxtV = newFormalContextFromFileV(argv[i]);

                bulkV = newConceptBulkFromContextV(cxtV);
                
                if (only_count)
                    printf("Concepts: %d\n",countConceptsInBulkV(bulkV));
                else {
                    strncpy(output_name, argv[i], 1000);
                    strcat(output_name, ".concepts");
                    printf("Writing %s...\n",output_name);
                    writeConceptsToFileV(cxtV, bulkV, output_name);
                }

                deleteConceptBulkV(&bulkV);

                deleteFormalContextV(&cxtV);

            } else if (mode == 1) {
                cxtV = newFormalContextFromFileV(argv[i]);

                bulkV = nextClosureVX(cxtV);
                
                if (only_count)
                    printf("Concepts: %d\n",countConceptsInBulkV(bulkV));
                else {
                    strncpy(output_name, argv[i], 1000);
                    strcat(output_name, ".concepts");
                    printf("Writing %s...\n",output_name);
                    writeConceptsToFileV(cxtV, bulkV, output_name);
                }

                deleteConceptBulkV(&bulkV);

                deleteFormalContextV(&cxtV);
            }
        }
    }
    
    return 0;
}
