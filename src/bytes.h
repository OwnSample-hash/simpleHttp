/**
 * @file
 * @brief Represents some byte stuff
 *
 * @def KB_1
 * 1 KiB
 *
 * @def KB_10
 * 10 KiB
 *
 * @def KB_100
 * 100 KiB
 *
 * @def MB_1
 * 1 MiB
 *
 * @def MB_10
 * 10 MiB
 *
 * @def MB_100
 * 100 MiB
 *
 * @def GB_1
 * 1 GiB
 *
 * @def GB_10
 * 10 GiB
 *
 * @def GB_100
 * 100 GiB
 *
 * @def ASCII_A
 * Ascii code of 'A'
 *
 * @def ASCII_Z
 * Ascii code of 'Z' by adding 0x19
 *
 * @def ASCII_SPC
 * Ascii code of ' '
 *
 * @def ASCII_AL
 * Ascii code of 'a' by adding spc to 'A'
 *
 * @def ASCII_ZL
 * Ascii code of 'z' by adding spc to 'Z'
 */

#ifndef __BYTES__HTTP_
#define __BYTES__HTTP_

#define KB_1 1024
#define KB_10 10240
#define KB_100 102400

#define MB_1 1048576
#define MB_10 10485760
#define MB_100 104857600

#define GB_1 1073741824
#define GB_10 10737418240
#define GB_100 107374182400

#define ASCII_A 0x41
#define ASCII_Z 0x41 + 0x19
#define ASCII_SPC 0x20
#define ASCII_AL ASCII_A + ASCII_SPC
#define ASCII_ZL ASCII_Z + ASCII_SPC

#endif // !__BYTES__HTTP_
