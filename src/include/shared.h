#ifndef SHARED_H
#define SHARED_H

#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;
typedef long long llong;

typedef void (*func)();

#define DEBUG 3
#if defined(DEBUG) && DEBUG > 0
#define PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
                                    __FILE__, __LINE__, __func__, ##args)
#else
#define PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#define STREQ(a, b) (strcmp((a), (b)) == 0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define COMP(a, b) (((a) < (b)) - ((a) > (b)))
#define SIGN(a) COMP(0, a)

// fast min/max on ints
// r = y + ((x - y) & ((x - y) >> (sizeof(int) * CHAR_BIT - 1))); // min(x, y)
// r = x - ((x - y) & ((x - y) >> (sizeof(int) * CHAR_BIT - 1))); // max(x, y)

#define B_00000000 0x0
#define B_00000001 0x1
#define B_00000010 0x2
#define B_00000011 0x3
#define B_00000100 0x4
#define B_00000101 0x5
#define B_00000110 0x6
#define B_00000111 0x7
#define B_00001000 0x8
#define B_00001001 0x9
#define B_00001010 0xA
#define B_00001011 0xB
#define B_00001100 0xC
#define B_00001101 0xD
#define B_00001110 0xE
#define B_00001111 0xF
#define B_00010000 0x10
#define B_00010001 0x11
#define B_00010010 0x12
#define B_00010011 0x13
#define B_00010100 0x14
#define B_00010101 0x15
#define B_00010110 0x16
#define B_00010111 0x17
#define B_00011000 0x18
#define B_00011001 0x19
#define B_00011010 0x1A
#define B_00011011 0x1B
#define B_00011100 0x1C
#define B_00011101 0x1D
#define B_00011110 0x1E
#define B_00011111 0x1F
#define B_00100000 0x20
#define B_00100001 0x21
#define B_00100010 0x22
#define B_00100011 0x23
#define B_00100100 0x24
#define B_00100101 0x25
#define B_00100110 0x26
#define B_00100111 0x27
#define B_00101000 0x28
#define B_00101001 0x29
#define B_00101010 0x2A
#define B_00101011 0x2B
#define B_00101100 0x2C
#define B_00101101 0x2D
#define B_00101110 0x2E
#define B_00101111 0x2F
#define B_00110000 0x30
#define B_00110001 0x31
#define B_00110010 0x32
#define B_00110011 0x33
#define B_00110100 0x34
#define B_00110101 0x35
#define B_00110110 0x36
#define B_00110111 0x37
#define B_00111000 0x38
#define B_00111001 0x39
#define B_00111010 0x3A
#define B_00111011 0x3B
#define B_00111100 0x3C
#define B_00111101 0x3D
#define B_00111110 0x3E
#define B_00111111 0x3F
#define B_01000000 0x40
#define B_01000001 0x41
#define B_01000010 0x42
#define B_01000011 0x43
#define B_01000100 0x44
#define B_01000101 0x45
#define B_01000110 0x46
#define B_01000111 0x47
#define B_01001000 0x48
#define B_01001001 0x49
#define B_01001010 0x4A
#define B_01001011 0x4B
#define B_01001100 0x4C
#define B_01001101 0x4D
#define B_01001110 0x4E
#define B_01001111 0x4F
#define B_01010000 0x50
#define B_01010001 0x51
#define B_01010010 0x52
#define B_01010011 0x53
#define B_01010100 0x54
#define B_01010101 0x55
#define B_01010110 0x56
#define B_01010111 0x57
#define B_01011000 0x58
#define B_01011001 0x59
#define B_01011010 0x5A
#define B_01011011 0x5B
#define B_01011100 0x5C
#define B_01011101 0x5D
#define B_01011110 0x5E
#define B_01011111 0x5F
#define B_01100000 0x60
#define B_01100001 0x61
#define B_01100010 0x62
#define B_01100011 0x63
#define B_01100100 0x64
#define B_01100101 0x65
#define B_01100110 0x66
#define B_01100111 0x67
#define B_01101000 0x68
#define B_01101001 0x69
#define B_01101010 0x6A
#define B_01101011 0x6B
#define B_01101100 0x6C
#define B_01101101 0x6D
#define B_01101110 0x6E
#define B_01101111 0x6F
#define B_01110000 0x70
#define B_01110001 0x71
#define B_01110010 0x72
#define B_01110011 0x73
#define B_01110100 0x74
#define B_01110101 0x75
#define B_01110110 0x76
#define B_01110111 0x77
#define B_01111000 0x78
#define B_01111001 0x79
#define B_01111010 0x7A
#define B_01111011 0x7B
#define B_01111100 0x7C
#define B_01111101 0x7D
#define B_01111110 0x7E
#define B_01111111 0x7F
#define B_10000000 0x80
#define B_10000001 0x81
#define B_10000010 0x82
#define B_10000011 0x83
#define B_10000100 0x84
#define B_10000101 0x85
#define B_10000110 0x86
#define B_10000111 0x87
#define B_10001000 0x88
#define B_10001001 0x89
#define B_10001010 0x8A
#define B_10001011 0x8B
#define B_10001100 0x8C
#define B_10001101 0x8D
#define B_10001110 0x8E
#define B_10001111 0x8F
#define B_10010000 0x90
#define B_10010001 0x91
#define B_10010010 0x92
#define B_10010011 0x93
#define B_10010100 0x94
#define B_10010101 0x95
#define B_10010110 0x96
#define B_10010111 0x97
#define B_10011000 0x98
#define B_10011001 0x99
#define B_10011010 0x9A
#define B_10011011 0x9B
#define B_10011100 0x9C
#define B_10011101 0x9D
#define B_10011110 0x9E
#define B_10011111 0x9F
#define B_10100000 0xA0
#define B_10100001 0xA1
#define B_10100010 0xA2
#define B_10100011 0xA3
#define B_10100100 0xA4
#define B_10100101 0xA5
#define B_10100110 0xA6
#define B_10100111 0xA7
#define B_10101000 0xA8
#define B_10101001 0xA9
#define B_10101010 0xAA
#define B_10101011 0xAB
#define B_10101100 0xAC
#define B_10101101 0xAD
#define B_10101110 0xAE
#define B_10101111 0xAF
#define B_10110000 0xB0
#define B_10110001 0xB1
#define B_10110010 0xB2
#define B_10110011 0xB3
#define B_10110100 0xB4
#define B_10110101 0xB5
#define B_10110110 0xB6
#define B_10110111 0xB7
#define B_10111000 0xB8
#define B_10111001 0xB9
#define B_10111010 0xBA
#define B_10111011 0xBB
#define B_10111100 0xBC
#define B_10111101 0xBD
#define B_10111110 0xBE
#define B_10111111 0xBF
#define B_11000000 0xC0
#define B_11000001 0xC1
#define B_11000010 0xC2
#define B_11000011 0xC3
#define B_11000100 0xC4
#define B_11000101 0xC5
#define B_11000110 0xC6
#define B_11000111 0xC7
#define B_11001000 0xC8
#define B_11001001 0xC9
#define B_11001010 0xCA
#define B_11001011 0xCB
#define B_11001100 0xCC
#define B_11001101 0xCD
#define B_11001110 0xCE
#define B_11001111 0xCF
#define B_11010000 0xD0
#define B_11010001 0xD1
#define B_11010010 0xD2
#define B_11010011 0xD3
#define B_11010100 0xD4
#define B_11010101 0xD5
#define B_11010110 0xD6
#define B_11010111 0xD7
#define B_11011000 0xD8
#define B_11011001 0xD9
#define B_11011010 0xDA
#define B_11011011 0xDB
#define B_11011100 0xDC
#define B_11011101 0xDD
#define B_11011110 0xDE
#define B_11011111 0xDF
#define B_11100000 0xE0
#define B_11100001 0xE1
#define B_11100010 0xE2
#define B_11100011 0xE3
#define B_11100100 0xE4
#define B_11100101 0xE5
#define B_11100110 0xE6
#define B_11100111 0xE7
#define B_11101000 0xE8
#define B_11101001 0xE9
#define B_11101010 0xEA
#define B_11101011 0xEB
#define B_11101100 0xEC
#define B_11101101 0xED
#define B_11101110 0xEE
#define B_11101111 0xEF
#define B_11110000 0xF0
#define B_11110001 0xF1
#define B_11110010 0xF2
#define B_11110011 0xF3
#define B_11110100 0xF4
#define B_11110101 0xF5
#define B_11110110 0xF6
#define B_11110111 0xF7
#define B_11111000 0xF8
#define B_11111001 0xF9
#define B_11111010 0xFA
#define B_11111011 0xFB
#define B_11111100 0xFC
#define B_11111101 0xFD
#define B_11111110 0xFE
#define B_11111111 0xFF

#endif