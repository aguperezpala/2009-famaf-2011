/**==========================================================================
 *
 *  MODULE:	Display Table Map.
 *
 *  IMPORTS:	-
 *
 *  EXPORTTYP:	-
 *
 *  EXPORTATTR:	DISPLAY_SIZE, MAX_CHAR_MAP.
 *
 *  EXPORTPROC:	-
 *
 *  PRIVATE:	-
 *
 *  COMMENTS:	Centronic interrupt program example in C.
 *		This is the Display Table Map module.
 *
 *  USAGE:	none.
 *
 *  VERSION:	1.00.
 *
 *  AUTHOR:	E.E. Javier Gaspar.
 *
 *  DATE:	November 30th, 2005.
 *
 *==========================================================================**/

#ifndef TABLE_H
#define TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Define display size.
 */
#ifndef DISPLAY_SIZE
#define DISPLAY_SIZE	8
#endif

/**
 * Define char map size.
 */
#ifndef MAX_CHAR_MAP
#define MAX_CHAR_MAP	128
#endif

/**
 * Map ascii char to display char.
 */
char map_ascii[MAX_CHAR_MAP] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/*   0 -  15 [00 - 0f] */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/*  16 -  31 [10 - 1f] */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/*  32 -  47 [20 - 2f] */
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07,
    0x7f, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/*  48 -  63 [30 - 3f] */
    0x00, 0x77, 0x7f, 0x39, 0x5e, 0x79, 0x71, 0x3d,
    0x76, 0x30, 0x0e, 0x00, 0x38, 0x37, 0x54, 0x3f,	/*  64 -  79 [40 - 4f] */
    0x73, 0x67, 0x50, 0x6d, 0x78, 0x3e, 0x00, 0x00,
    0x00, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/*  80 -  95 [50 - 5f] */
    0x00, 0x5f, 0x7c, 0x58, 0x5e, 0x7b, 0x71, 0x6f,
    0x74, 0x06, 0x1e, 0x00, 0x38, 0x37, 0x54, 0x5c,	/*  96 - 111 [60 - 6f] */
    0x73, 0x67, 0x50, 0x6d, 0x78, 0x1c, 0x00, 0x00,
    0x00, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	/* 114 - 127 [70 - 7f] */
};


/* NOTE
 * Para lo que sigue ver fotocopia "Display 7 segmentos cátodo común",
 * los pines 14, 16 y 17. Y comparar con la filmina del LPT1 (del teórico),
 * los pines 14, 16 y 17.
 */

/**
 * Turn on displays in direct order.
 */
int dir_order[DISPLAY_SIZE] =
{
    10, 8, 14, 12, 2, 0, 6, 4 /* 1010 1000 1110 1100 0010 0000 0110 0100 */
};
/* NOTE
 * Cada secuencia de bits xyzw envía info al registro CONTROL *
 * de la LPT1. Se usan los bits xyz == a3a2a1, que son "auto_feed", *
 * "initialize_printer" y "select_printer" El bit a0 que es el *
 * "strobe" no se usa y es siempre '0' (pero eso las dir. son pares)
 * Initialize_printer es activa por bajo, las otras dos por alto.
 */

/**
 * Turn on displays in reverse order.
 */
int rev_order[DISPLAY_SIZE] =
{
    4, 6, 0, 2, 12, 14, 8, 10
};


#ifdef __cplusplus
}
#endif

#endif
