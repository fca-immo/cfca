/**
 * vector/macros.h, (c) 2013, Immanuel Albrecht; Dresden University of
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

#ifndef FCA__VECTOR_MACROS_H_
#define FCA__VECTOR_MACROS_H_


/** @file
 * These macros are used for uint64_t bit-stream arrays
 */

/**
 * @def OFFSET(x)
 *
 * get the offset of the x-th bit in an 64-bit integer vector
 */

#define OFFSET(x) ((unsigned)(x)>>6)
/**
 * @def BITNBR(x)
 *
 * get the remainder of the x-th bit in an 64-bit integer vector, i.e. 65=64+ @a 1
 */
#define BITNBR(x) (((unsigned)(x))&(63))

/**
 * @def WIDTH(x)
 *
 * determine the length of an 64-bit integer vector that can hold x bits
 */
#define WIDTH(x) ((((unsigned)(x))&(63))?((unsigned)(x)/64)+1:((unsigned)(x)/64))

/**
 * @def BITVALUE(x)
 *
 * gives the bit-value of the x-th bit. (Note that bit 0 is the most,
 * and bit 63 is the least significant bit)
 */

#define BITVALUE(x) ((1ULL<<(63-BITNBR(x))))

/*
 * old version
 */

//#define BITVALUEX(x) ((1ULL<<BITNBR(x)))

/**
 * @def CRIMPVALUE(x)
 *
 * gives an 64-bit integer that has set the bits 0 through x.
 *
 * CRIMPVALUE(0) == 0x8000000000000000
 * CRIMPVALUE(1) == 0xc000000000000000
 * CRIMPVALUE(2) == 0xe000000000000000
 * CRIMPVALUE(3) == 0xf000000000000000
 * CRIMPVALUE(4) == 0xf800000000000000
 * etc.
 *
 */

#define CRIMPVALUE(x) ((~(0ULL))>>(63-(BITNBR(x)))<<(63-BITNBR(x)))

/*
 * old version
 */

//#define CRIMPVALUEX(x) ((~(0ULL))<<(63-(BITNBR(x)))>>(63-BITNBR(x)))

/**
 * @def MASKVECTOR(v,x)
 *
 * set the unused attribute bits to zero. (i.e. attributes == 100 -> width == 2, BITNBR(99) == 35)
 * where v is a 64-bit integer vector, and x is the number used bits.
 */
#define MASKVECTOR(v,x) {if (BITNBR((x))) { *((v)+OFFSET((x)-1)) = ( (*((v)+OFFSET((x)-1))>>(63-BITNBR((x)-1))) ) << (63-BITNBR((x)-1));  }}

/*
 * old version
 */
//#define MASKVECTORX(v,x) {if (BITNBR((x))) { *((v)+OFFSET((x)-1)) = ( (*((v)+OFFSET((x)-1))<<(63-BITNBR((x)-1))) ) >> (63-BITNBR((x)-1));  }}

/**
 * @def  CROSSV(v,x)
 * crosses the x-th attribute of an attribute vector
 */
#define CROSSV(v,x) { *((v)+OFFSET(x)) |= BITVALUE(x); }

/**
 * @def  CLEARV(v,x)
 * clears the x-th attribute of an attribute vector
 */
#define CLEARV(v,x) { *((v)+OFFSET(x)) &= ~ (BITVALUE(x)); }

/**
 * @def INCIDESV(v,x)
 * checks whether the x-th attribute of an attribute vector is crossed
 */
#define INCIDESV(v,x) (  ( *((v)+OFFSET(x)) >> (63-BITNBR(x)) ) & 1  )

/*
 * old version
 */
//#define INCIDESVX(v,x) (  ( *((v)+OFFSET(x)) >> BITNBR(x) ) & 1  )

/**
 * @def ROW(g,I)
 * gives the attribute vector for a given object
 */
#define ROW(g,I) ((I)->incidence + ((I)->width * (g)))




#endif /* MACROS_H_ */
