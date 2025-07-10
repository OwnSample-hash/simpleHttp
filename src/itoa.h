/**
 * @file
 * @brief Int to ASCII(string) conv.
 */

#ifndef __ITOA_HTTP__
#define __ITOA_HTTP__

#include <limits.h>

/**
 * @def ITOA_BASE_N
 * @brief The size of the buffer for the itoa_base function.
 *
 * This is calculated as the number of bits in an unsigned integer plus 2
 * (for the sign and null terminator).
 */
#define ITOA_BASE_N (sizeof(unsigned) * CHAR_BIT + 2)

/**
 * @def TO_BASE(x, b)
 * @brief Converts an integer to a string in a given base.
 *
 * This macro uses the itoa_base function to convert the integer `x` to a string
 * in base `b`, storing the result in a temporary character array of size
 * ITOA_BASE_N.
 *
 * @param x The integer to convert.
 * @param b The base to convert to, must be between 2 and 36.
 */
#define TO_BASE(x, b) itoa_base((char[ITOA_BASE_N]){0}, (x), (b))

/**
 * @brief Converts an integer to a string in a given base.
 *
 * @param s Pointer to a character array where the result will be stored.
 * @param x Nunber to convert
 * @param base Base to convert to, must be between 2 and 36
 * @return Pointer to the resulting string.
 */
char *itoa_base(char *s, int x, int base);

#endif // !__ITOA_HTTP__
