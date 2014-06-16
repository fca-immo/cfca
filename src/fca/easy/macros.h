/**
 * easy/macros.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

#ifndef FCA__EASY_MACROS_H_
#define FCA__EASY_MACROS_H_


/** @file
 * These macros are used for IncidenceCell array implementations of
 * formal contexts. Such implementations are easier to debug, but
 * take up far too much memory for big scale contexts
 */


/**
 * @def INCIDES(x)
 * checks whether something incides by testing the 1-bit
 */

#define INCIDES(x) (((x)&1))

/**
 * @def CLEAR(x)
 * clears the mark
 */

#define CLEAR(x) { (x) = 0; }

//#define CLEAR(x) { (x) &= ~1; }

/**
 * @def CROSS(x)
 * sets the mark
 */
#define CROSS(x) { (x) = 1; }
//#define CROSS(x) { (x) |= 1; }

/**
 * @def CELL(g,I,m)
 * results in the cell that encodes whether g incides with m
 *
 *  I may be a formal context, then g refers to the object number, or
 *  I may be a chunk of formal concepts, then g refers to the concept number.
 */

#define CELL(g,I,m) ((I)->incidence[(I)->attributes * (g) + (m)])

/**
 * @def gIm(g,I,m)
 * test whether g and m incides
 */

#define gIm(g,I,m) INCIDES(CELL( (g) , (I) , (m)))




#endif /* MACROS_H_ */
