#include "CRC.h"

//unknown data
const unsigned char ver1[28]={0xa5,0x53,0x00,0xe1,0x00,0x00,0x00,0x05,0xfa,
							0x76,0x31,0x2e,0x30,0x30,0x00,0x67,0xda,
							0xa5,0x32,0x00,0xe1,0x00,0x00,0x00,0x00,0x01,0x43,0x23};
const unsigned char ver1s[17]={0xa5,0x53,0x00,0xe1,0x00,0x00,0x00,0x05,0xfa,
							0x76,0x31,0x2e,0x30,0x30,0x00,0x67,0xda};
const unsigned char ver2[18]={0xa5,0x54,0x00,0x00,0x00,0x00,0x00,0x07,0xf8,0x76,0x31,
							0x2e,0x31,0x37,0x63,0x00,0xdd,0x31};
const unsigned char macack[16]={0xa5,0x07,0x00,0x00,0x00,0x00,0x06,0xf9,0x00,0x60,0x9f,
							0x80,0x07,0x54,0x18,0x99};

const unsigned char crc21[512]=
	{0x8B,0xB7, 0x9B,0x96, 0xAB,0xF5, 0xBB,0xD4, 0xCB,0x33, 0xDB,0x12, 0xEB,0x71, 0xFB,0x50,
	 0x0A,0xBF, 0x1A,0x9E, 0x2A,0xFD, 0x3A,0xDC, 0x4A,0x3B, 0x5A,0x1A, 0x6A,0x79, 0x7A,0x58,
	 0x99,0x86, 0x89,0xA7, 0xB9,0xC4, 0xA9,0xE5, 0xD9,0x02, 0xC9,0x23, 0xF9,0x40, 0xE9,0x61,
	 0x18,0x8E, 0x08,0xAF, 0x38,0xCC, 0x28,0xED, 0x58,0x0A, 0x48,0x2B, 0x78,0x48, 0x68,0x69,
	 0xAF,0xD5, 0xBF,0xF4, 0x8F,0x97, 0x9F,0xB6, 0xEF,0x51, 0xFF,0x70, 0xCF,0x13, 0xDF,0x32,
	 0x2E,0xDD, 0x3E,0xFC, 0x0E,0x9F, 0x1E,0xBE, 0x6E,0x59, 0x7E,0x78, 0x4E,0x1B, 0x5E,0x3A,
	 0xBD,0xE4, 0xAD,0xC5, 0x9D,0xA6, 0x8D,0x87, 0xFD,0x60, 0xED,0x41, 0xDD,0x22, 0xCD,0x03,
	 0x3C,0xEC, 0x2C,0xCD, 0x1C,0xAE, 0x0C,0x8F, 0x7C,0x68, 0x6C,0x49, 0x5C,0x2A, 0x4C,0x0B,
	 0xC3,0x73, 0xD3,0x52, 0xE3,0x31, 0xF3,0x10, 0x83,0xF7, 0x93,0xD6, 0xA3,0xB5, 0xB3,0x94,
	 0x42,0x7B, 0x52,0x5A, 0x62,0x39, 0x72,0x18, 0x02,0xFF, 0x12,0xDE, 0x22,0xBD, 0x32,0x9C,
	 0xD1,0x42, 0xC1,0x63, 0xF1,0x00, 0xE1,0x21, 0x91,0xC6, 0x81,0xE7, 0xB1,0x84, 0xA1,0xA5,
	 0x50,0x4A, 0x40,0x6B, 0x70,0x08, 0x60,0x29, 0x10,0xCE, 0x00,0xEF, 0x30,0x8C, 0x20,0xAD,
	 0xE7,0x11, 0xF7,0x30, 0xC7,0x53, 0xD7,0x72, 0xA7,0x95, 0xB7,0xB4, 0x87,0xD7, 0x97,0xF6,
	 0x66,0x19, 0x76,0x38, 0x46,0x5B, 0x56,0x7A, 0x26,0x9D, 0x36,0xBC, 0x06,0xDF, 0x16,0xFE,
	 0xF5,0x20, 0xE5,0x01, 0xD5,0x62, 0xC5,0x43, 0xB5,0xA4, 0xA5,0x85, 0x95,0xE6, 0x85,0xC7,
	 0x74,0x28, 0x64,0x09, 0x54,0x6A, 0x44,0x4B, 0x34,0xAC, 0x24,0x8D, 0x14,0xEE, 0x04,0xCF,

	 0x1A,0x3F, 0x0A,0x1E, 0x3A,0x7D, 0x2A,0x5C, 0x5A,0xBB, 0x4A,0x9A, 0x7A,0xF9, 0x6A,0xD8,
	 0x9B,0x37, 0x8B,0x16, 0xBB,0x75, 0xAB,0x54, 0xDB,0xB3, 0xCB,0x92, 0xFB,0xF1, 0xEB,0xD0,
	 0x08,0x0E, 0x18,0x2F, 0x28,0x4C, 0x38,0x6D, 0x48,0x8A, 0x58,0xAB, 0x68,0xC8, 0x78,0xE9,
	 0x89,0x06, 0x99,0x27, 0xA9,0x44, 0xB9,0x65, 0xC9,0x82, 0xD9,0xA3, 0xE9,0xC0, 0xF9,0xE1,
	 0x3E,0x5D, 0x2E,0x7C, 0x1E,0x1F, 0x0E,0x3E, 0x7E,0xD9, 0x6E,0xF8, 0x5E,0x9B, 0x4E,0xBA,
	 0xBF,0x55, 0xAF,0x74, 0x9F,0x17, 0x8F,0x36, 0xFF,0xD1, 0xEF,0xF0, 0xDF,0x93, 0xCF,0xB2,
	 0x2C,0x6C, 0x3C,0x4D, 0x0C,0x2E, 0x1C,0x0F, 0x6C,0xE8, 0x7C,0xC9, 0x4C,0xAA, 0x5C,0x8B,
	 0xAD,0x64, 0xBD,0x45, 0x8D,0x26, 0x9D,0x07, 0xED,0xE0, 0xFD,0xC1, 0xCD,0xA2, 0xDD,0x83,
	 0x52,0xFB, 0x42,0xDA, 0x72,0xB9, 0x62,0x98, 0x12,0x7F, 0x02,0x5E, 0x32,0x3D, 0x22,0x1C,
	 0xD3,0xF3, 0xC3,0xD2, 0xF3,0xB1, 0xE3,0x90, 0x93,0x77, 0x83,0x56, 0xB3,0x53, 0xA3,0x14,
	 0x40,0xCA, 0x50,0xEB, 0x60,0x88, 0x70,0xA9, 0x00,0x4E, 0x10,0x6F, 0x20,0x0C, 0x30,0x2D,
	 0xC1,0xC2, 0xD1,0xE3, 0xE1,0x80, 0xF1,0xA1, 0x81,0x46, 0x91,0x67, 0xA1,0x04, 0xB1,0x25,
	 0x76,0x99, 0x66,0xB8, 0x56,0xDB, 0x46,0xFA, 0x36,0x1D, 0x26,0x3C, 0x16,0x5F, 0x06,0x7E,
	 0xF7,0x91, 0xE7,0xB0, 0xD7,0xD3, 0xC7,0xF2, 0xB7,0x15, 0xA7,0x34, 0x97,0x57, 0x87,0x76,
	 0x64,0xA8, 0x74,0x89, 0x44,0xEA, 0x54,0xCB, 0x24,0x2C, 0x34,0x0D, 0x04,0x6E, 0x14,0x4F,
	 0xE5,0xA0, 0xF5,0x81, 0xC5,0xE2, 0xD5,0xC3, 0xA5,0x24, 0xB5,0x05, 0x85,0x66, 0x95,0x47};

const unsigned char crc22[512]=
	{0x53,0x35, 0x43,0x14, 0x73,0x77, 0x63,0x56, 0x13,0xB1, 0x03,0x90, 0x33,0xF3, 0x23,0xD2,
	 0xD2,0x3D, 0xC2,0x1C, 0xF2,0x7F, 0xE2,0x5E, 0x92,0xB9, 0x82,0x98, 0xB2,0xFB, 0xA2,0xDA,
	 0x41,0x04, 0x51,0x25, 0x61,0x46, 0x71,0x67, 0x01,0x80, 0x11,0xA1, 0x21,0xC2, 0x31,0xE3,
	 0xC0,0x0C, 0xD0,0x2D, 0xE0,0x4E, 0xF0,0x6F, 0x80,0x88, 0x90,0xA9, 0xA0,0xCA, 0xB0,0xEB,
	 0x77,0x57, 0x67,0x76, 0x57,0x15, 0x47,0x34, 0x37,0xD3, 0x27,0xF2, 0x17,0x91, 0x07,0xB0,
	 0xF6,0x5F, 0xE6,0x7E, 0xD6,0x1D, 0xC6,0x3C, 0xB6,0xDB, 0xA6,0xFA, 0x96,0x99, 0x86,0xB8,
	 0x65,0x66, 0x75,0x47, 0x45,0x24, 0x55,0x05, 0x25,0xE2, 0x35,0xC3, 0x05,0xA0, 0x15,0x81,
	 0xE4,0x6E, 0xF4,0x4F, 0xC4,0x2C, 0xD4,0x0D, 0xA4,0xEA, 0xB4,0xCB, 0x84,0xA8, 0x94,0x89,
	 0x1B,0xF1, 0x0B,0xD0, 0x3B,0xB3, 0x2B,0x92, 0x5B,0x75, 0x4B,0x54, 0x7B,0x37, 0x6B,0x16,
	 0x9A,0xF9, 0x8A,0xD8, 0xBA,0xBB, 0xAA,0x9A, 0xDA,0x7D, 0xCA,0x5C, 0xFA,0x3F, 0xEA,0x1E,
	 0x09,0xC0, 0x19,0xE1, 0x29,0x82, 0x39,0xA3, 0x49,0x44, 0x59,0x65, 0x69,0x06, 0x79,0x27,
	 0x88,0xC8, 0x98,0xE9, 0xA8,0x8A, 0xB8,0xAB, 0xC8,0x4C, 0xD8,0x6D, 0xE8,0x0E, 0xF8,0x2F,
	 0x3F,0x93, 0x2F,0xB2, 0x1F,0xD1, 0x0F,0xF0, 0x7F,0x17, 0x6F,0x36, 0x5F,0x55, 0x4F,0x74,
	 0xBE,0x9B, 0xAE,0xBA, 0x9E,0xD9, 0x8E,0xF8, 0xFE,0x1F, 0xEE,0x3E, 0xDE,0x5D, 0xCE,0x7C,
	 0x2D,0xA2, 0x3D,0x83, 0x0D,0xE0, 0x1D,0xC1, 0x6D,0x26, 0x7D,0x07, 0x4D,0x64, 0x5D,0x45,
	 0xAC,0xAA, 0xBC,0x8B, 0x8C,0xE8, 0x9C,0xC9, 0xEC,0x2E, 0xFC,0x0F, 0xCC,0x6C, 0xDC,0x4D,

	 0xC2,0xBD, 0xD2,0x9C, 0xE2,0xFF, 0xF2,0xDE, 0x82,0x39, 0x92,0x18, 0xA2,0x7B, 0xB2,0x5A,
	 0x43,0xB5, 0x53,0x94, 0x63,0xF7, 0x73,0xD6, 0x03,0x31, 0x13,0x10, 0x23,0x73, 0x33,0x52,
	 0xD0,0x8C, 0xC0,0xAD, 0xF0,0xCE, 0xE0,0xEF, 0x90,0x08, 0x80,0x29, 0xB0,0x4A, 0xA0,0x6B,
	 0x51,0x84, 0x41,0xA5, 0x71,0xC6, 0x61,0xE7, 0x11,0x00, 0x01,0x21, 0x31,0x42, 0x21,0x63,
	 0xE6,0xDF, 0xF6,0xFE, 0xC6,0x9D, 0xD6,0xBC, 0xA6,0x5B, 0xB6,0x7A, 0x86,0x19, 0x96,0x38,
	 0x67,0xD7, 0x77,0xF6, 0x47,0x95, 0x57,0xB4, 0x27,0x53, 0x37,0x72, 0x07,0x11, 0x17,0x30,
	 0xF4,0xEE, 0xE4,0xCF, 0xD4,0xAC, 0xC4,0x8D, 0xB4,0x6A, 0xA4,0x4B, 0x94,0x28, 0x84,0x09,
	 0x75,0xE6, 0x65,0xC7, 0x55,0xA4, 0x45,0x85, 0x35,0x62, 0x25,0x43, 0x15,0x20, 0x05,0x01,
	 0x8A,0x79, 0x9A,0x58, 0xAA,0x3B, 0xBA,0x1A, 0xCA,0xFD, 0xDA,0xDC, 0xEA,0xBF, 0xFA,0x9E,
	 0x0B,0x71, 0x1B,0x50, 0x2B,0x33, 0x3B,0x12, 0x4B,0xF5, 0x5D,0xD4, 0x6B,0xB7, 0x7B,0x96,
	 0x98,0x48, 0x88,0x69, 0xB8,0x0A, 0xA8,0x2B, 0xD8,0xCC, 0xC8,0xED, 0xF8,0x8E, 0xE8,0xAF,
	 0x19,0x40, 0x09,0x61, 0x39,0x02, 0x29,0x23, 0x59,0xC4, 0x49,0xE5, 0x79,0x86, 0x69,0xA7,
	 0xAE,0x1B, 0xBE,0x3A, 0x8E,0x59, 0x9E,0x78, 0xEE,0x9F, 0xFE,0xBE, 0xCE,0xDD, 0xDE,0xFC,
	 0x2F,0x13, 0x3F,0x32, 0x0F,0x51, 0x1F,0x70, 0x6F,0x97, 0x7F,0xB6, 0x4F,0xD5, 0x5F,0xF4,
	 0xBC,0x2A, 0xAC,0x0B, 0x9C,0x68, 0x8C,0x49, 0xFC,0xAE, 0xEC,0x8F, 0xDC,0xEC, 0xCC,0xCD,
	 0x3D,0x22, 0x2D,0x03, 0x1D,0x60, 0x0D,0x41, 0x7D,0xA6, 0x6D,0x87, 0x5D,0xE4, 0x4D,0xC5};
