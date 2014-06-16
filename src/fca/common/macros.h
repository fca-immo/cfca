/**
 * fca_macros.h, (c) 2013, Immanuel Albrecht; Dresden University of
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
#ifndef FCA__MACROS_H_
#define FCA__MACROS_H_

#include <stdio.h>


/**
 * @def RETURN_IF_ZERO(x)
 * checks whether x == 0, and returns
 */

#define RETURN_IF_ZERO(x) {if (((x) == (void*)0)) {fprintf(stderr, "WARNING: ZERO pointer %s in %s [%s:%u]\n", #x, __FUNCTION__, __FILE__,__LINE__); return;}}

/**
 * @def RETURN_ZERO_IF_ZERO(x)
 * checks whether x == (void*)0, and returns 0;
 */

#define RETURN_ZERO_IF_ZERO(x) {if (((x) == (void*)0)) {fprintf(stderr, "WARNING: ZERO pointer %s in %s [%s:%u]\n", #x, __FUNCTION__, __FILE__,__LINE__); return 0;}}

/**
 * @def RETURN_ZERO_IF_ZEROI(x)
 * checks whether x == 0, and returns 0;
 */
#define RETURN_ZERO_IF_ZEROI(x) {if (((x) == 0)) {fprintf(stderr, "WARNING: ZERO %s in %s [%s:%u]\n", #x, __FUNCTION__, __FILE__,__LINE__); return 0;}}

/**
 * @def WARN_IF_UNEQUAL_DO(x,y,d)
 * if x!=y, prints a warning and calls the statement d
 */

#define WARN_IF_UNEQUAL_DO(x,y,d) {if (((x) != (y))) {fprintf(stderr, "WARNING: %s NOT EQUAL TO %s in %s [%s:%u]\n", #x, #y, __FUNCTION__, __FILE__,__LINE__); d;}}

/**
 * @def WARN_IF_GEQ_DO(x,y,d)
 * if x>=y, prints a warning and calls the statement d
 */

#define WARN_IF_GEQ_DO(x,y,d) {if (((x) >= (y))) {fprintf(stderr, "WARNING: %s NOT EQUAL TO %s in %s [%s:%u]\n", #x, #y, __FUNCTION__, __FILE__,__LINE__); d;}}


/**
 * @def MIN(a,b)
 * gives minimum
 */

#define MIN(a,b) (((a)<(b))?(a):(b))

/**
 * @def MAX(a,b)
 * gives maximum
 */

#define MAX(a,b) (((a)>(b))?(a):(b))

#endif /* FCA_MACROS_H_ */
