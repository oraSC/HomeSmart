#include "font.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>



//字库点阵信息

//车
unsigned char  che_16x16[] 	= {
0x02,0x00,0x02,0x00,0x02,0x00,0x7F,0xFC,0x04,0x00,0x09,0x00,0x11,0x00,0x21,0x00,
0x3F,0xF8,0x01,0x00,0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00};
unsigned char che_32x32[] 	= {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x06,0x00,0x00,
0x00,0x0E,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x1C,0x00,0x60,0x0F,0xFF,0xFF,0xF0,
0x00,0x18,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x31,0xC0,0x00,0x00,0x61,0x80,0x00,
0x00,0xC1,0x80,0x00,0x00,0xC1,0x80,0x00,0x01,0x81,0x80,0x00,0x03,0x81,0x80,0xC0,
0x07,0xFF,0xFF,0xE0,0x03,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,
0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x18,0x3F,0xFF,0xFF,0xFC,0x00,0x01,0x80,0x00,
0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,
0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00};

//位
unsigned char wei_16x16[] 	= {
0x08,0x80,0x08,0x40,0x08,0x40,0x10,0x00,0x17,0xFC,0x30,0x00,0x30,0x08,0x52,0x08,
0x92,0x08,0x11,0x10,0x11,0x10,0x11,0x10,0x11,0x20,0x10,0x20,0x1F,0xFE,0x10,0x00};
unsigned char wei_32x32[]	= {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0xE0,0x40,0x00,
0x00,0xC0,0x30,0x00,0x00,0xC0,0x30,0x00,0x01,0x80,0x38,0x00,0x01,0x80,0x10,0x00,
0x01,0x00,0x00,0x30,0x03,0x3F,0xFF,0xF8,0x02,0x00,0x00,0x00,0x07,0x80,0x00,0x00,
0x07,0x00,0x00,0x80,0x0F,0x04,0x01,0xC0,0x0B,0x02,0x01,0xC0,0x1B,0x02,0x01,0x80,
0x13,0x03,0x01,0x80,0x23,0x03,0x01,0x00,0x43,0x01,0x83,0x00,0x03,0x01,0x83,0x00,
0x03,0x01,0x82,0x00,0x03,0x01,0x82,0x00,0x03,0x01,0x82,0x00,0x03,0x01,0x84,0x00,
0x03,0x00,0x04,0x00,0x03,0x00,0x04,0x00,0x03,0x00,0x08,0x00,0x03,0x00,0x08,0x18,
0x03,0x3F,0xFF,0xFC,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


//0
unsigned char _0_8x16[]	= {
0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00,0x00};
unsigned char _0_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x06,0x20,
0x0C,0x30,0x18,0x18,0x18,0x18,0x18,0x08,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,
0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x18,0x08,0x18,0x18,
0x18,0x18,0x0C,0x30,0x06,0x20,0x03,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//1
unsigned char _1_8x16[]	= {
0x00,0x00,0x00,0x08,0x38,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00};
unsigned char _1_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x01,0x80,
0x1F,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x03,0xC0,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//2
unsigned char _2_8x16[] = {
0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x02,0x04,0x08,0x10,0x20,0x42,0x7E,0x00,0x00};
unsigned char _2_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x08,0x38,
0x10,0x18,0x20,0x0C,0x20,0x0C,0x30,0x0C,0x30,0x0C,0x00,0x0C,0x00,0x18,0x00,0x18,
0x00,0x30,0x00,0x60,0x00,0xC0,0x01,0x80,0x03,0x00,0x02,0x00,0x04,0x04,0x08,0x04,
0x10,0x04,0x20,0x0C,0x3F,0xF8,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


//3
unsigned char _3_8x16[] = {
0x00,0x00,0x00,0x3C,0x42,0x42,0x02,0x04,0x18,0x04,0x02,0x42,0x42,0x3C,0x00,0x00};
unsigned char _3_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x18,0x60,
0x30,0x30,0x30,0x18,0x30,0x18,0x30,0x18,0x00,0x18,0x00,0x18,0x00,0x30,0x00,0x60,
0x03,0xC0,0x00,0x70,0x00,0x18,0x00,0x08,0x00,0x0C,0x00,0x0C,0x30,0x0C,0x30,0x0C,
0x30,0x08,0x30,0x18,0x18,0x30,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


//4
unsigned char _4_8x16[] = {
0x00,0x00,0x00,0x04,0x0C,0x0C,0x14,0x24,0x24,0x44,0x7F,0x04,0x04,0x1F,0x00,0x00};
unsigned char _4_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x60,
0x00,0xE0,0x00,0xE0,0x01,0x60,0x01,0x60,0x02,0x60,0x04,0x60,0x04,0x60,0x08,0x60,
0x08,0x60,0x10,0x60,0x30,0x60,0x20,0x60,0x40,0x60,0x7F,0xFC,0x00,0x60,0x00,0x60,
0x00,0x60,0x00,0x60,0x00,0x60,0x03,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//5
unsigned char _5_8x16[] = {
0x00,0x00,0x00,0x7E,0x40,0x40,0x40,0x78,0x44,0x02,0x02,0x42,0x44,0x38,0x00,0x00};
unsigned char _5_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFC,0x0F,0xFC,
0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x13,0xE0,0x14,0x30,
0x18,0x18,0x10,0x08,0x00,0x0C,0x00,0x0C,0x00,0x0C,0x00,0x0C,0x30,0x0C,0x30,0x0C,
0x20,0x18,0x20,0x18,0x18,0x30,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//6
unsigned char _6_8x16[] = {
0x00,0x00,0x00,0x18,0x24,0x40,0x40,0x5C,0x62,0x42,0x42,0x42,0x22,0x1C,0x00,0x00};
unsigned char _6_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x06,0x18,
0x0C,0x18,0x08,0x18,0x18,0x00,0x10,0x00,0x10,0x00,0x30,0x00,0x33,0xE0,0x36,0x30,
0x38,0x18,0x38,0x08,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x18,0x0C,
0x18,0x08,0x0C,0x18,0x0E,0x30,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//7
unsigned char _7_8x16[] = {
0x00,0x00,0x00,0x7E,0x42,0x04,0x04,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x00,0x00};
unsigned char _7_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFC,0x1F,0xFC,
0x10,0x08,0x30,0x10,0x20,0x10,0x20,0x20,0x00,0x20,0x00,0x40,0x00,0x40,0x00,0x40,
0x00,0x80,0x00,0x80,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x03,0x00,0x03,0x00,
0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//8
unsigned char _8_8x16[] = {
0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x42,0x3C,0x00,0x00};
unsigned char _8_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x0C,0x30,
0x18,0x18,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x38,0x0C,0x38,0x08,0x1E,0x18,0x0F,0x20,
0x07,0xC0,0x18,0xF0,0x30,0x78,0x30,0x38,0x60,0x1C,0x60,0x0C,0x60,0x0C,0x60,0x0C,
0x60,0x0C,0x30,0x18,0x18,0x30,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//9
unsigned char _9_8x16[] = {
0x00,0x00,0x00,0x38,0x44,0x42,0x42,0x42,0x46,0x3A,0x02,0x02,0x24,0x18,0x00,0x00};
unsigned char _9_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x18,0x20,
0x30,0x10,0x30,0x18,0x60,0x08,0x60,0x0C,0x60,0x0C,0x60,0x0C,0x60,0x0C,0x60,0x0C,
0x70,0x1C,0x30,0x2C,0x18,0x6C,0x0F,0x8C,0x00,0x0C,0x00,0x18,0x00,0x18,0x00,0x10,
0x30,0x30,0x30,0x60,0x30,0xC0,0x0F,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


unsigned char _a_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x44,0x0C,0x34,0x44,0x4C,0x36,0x00,0x00};

unsigned char _b_8x16[] = {
0x00,0x00,0x00,0x00,0xC0,0x40,0x40,0x58,0x64,0x42,0x42,0x42,0x64,0x58,0x00,0x00};

unsigned char _c_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x22,0x40,0x40,0x40,0x22,0x1C,0x00,0x00};

unsigned char _d_8x16[] = {
0x00,0x00,0x00,0x00,0x06,0x02,0x02,0x3E,0x42,0x42,0x42,0x42,0x46,0x3B,0x00,0x00};

unsigned char _e_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x42,0x7E,0x40,0x42,0x3C,0x00,0x00};

unsigned char _f_8x16[] = {
0x00,0x00,0x00,0x00,0x0C,0x12,0x10,0x7C,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00};
unsigned char _F_8x16[] = {
0x00,0x00,0x00,0xFC,0x42,0x48,0x48,0x78,0x48,0x48,0x40,0x40,0x40,0xE0,0x00,0x00};

unsigned char _g_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x44,0x44,0x38,0x40,0x3C,0x42,0x42,0x3C};

unsigned char _h_8x16[] = {
0x00,0x00,0x00,0x00,0xC0,0x40,0x40,0x5C,0x62,0x42,0x42,0x42,0x42,0xE7,0x00,0x00};

unsigned char _i_8x16[] = {
0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00};

unsigned char _j_8x16[] = {
0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x1C,0x04,0x04,0x04,0x04,0x04,0x04,0x44,0x78};

unsigned char _k_8x16[] = {
0x00,0x00,0x00,0x00,0xC0,0x40,0x40,0x4E,0x48,0x50,0x70,0x48,0x44,0xEE,0x00,0x00};

unsigned char _l_8x16[] = {
0x00,0x00,0x00,0x10,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00};

unsigned char _m_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x49,0x49,0x49,0x49,0x49,0xED,0x00,0x00};

unsigned char _n_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDC,0x62,0x42,0x42,0x42,0x42,0xE7,0x00,0x00};

unsigned char _o_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00};

unsigned char _p_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8,0x64,0x42,0x42,0x42,0x64,0x58,0x40,0xE0};

unsigned char _q_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1A,0x26,0x42,0x42,0x42,0x26,0x1A,0x02,0x07};

unsigned char _r_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE,0x32,0x20,0x20,0x20,0x20,0xF8,0x00,0x00};

unsigned char _s_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x42,0x40,0x3C,0x02,0x42,0x7C,0x00,0x00};
unsigned char _S_8x16[] = {
0x00,0x00,0x00,0x3E,0x42,0x42,0x40,0x20,0x18,0x04,0x02,0x42,0x42,0x7C,0x00,0x00};

unsigned char _t_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x7C,0x10,0x10,0x10,0x10,0x12,0x0C,0x00,0x00};
unsigned char _T_8x16[] = {
0x00,0x00,0x00,0xFE,0x92,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x38,0x00,0x00};

unsigned char _u_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x42,0x42,0x42,0x42,0x46,0x3B,0x00,0x00};

unsigned char _v_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE,0x44,0x44,0x28,0x28,0x10,0x10,0x00,0x00};

unsigned char _w_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDB,0x89,0x4A,0x5A,0x54,0x24,0x24,0x00,0x00};
unsigned char _W_8x16[] = {
0x00,0x00,0x00,0xD6,0x54,0x54,0x54,0x54,0x54,0x6C,0x28,0x28,0x28,0x28,0x00,0x00};

unsigned char _x_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0x24,0x18,0x18,0x18,0x24,0x6E,0x00,0x00};

unsigned char _y_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x18,0x18,0x10,0x10,0x60};

unsigned char _z_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x44,0x08,0x10,0x10,0x22,0x7E,0x00,0x00};



unsigned char _a_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x18,0x30,0x30,0x18,
0x30,0x18,0x30,0x18,0x00,0x38,0x07,0xD8,0x1C,0x18,0x30,0x18,0x60,0x18,0x60,0x18,
0x60,0x18,0x60,0x19,0x30,0x79,0x1F,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _b_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x78,0x00,0x18,0x00,
0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x19,0xE0,0x1A,0x38,0x1C,0x18,
0x1C,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,
0x18,0x08,0x1C,0x18,0x1C,0x30,0x13,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _c_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE0,0x0E,0x10,0x0C,0x18,
0x18,0x18,0x30,0x18,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x04,
0x18,0x04,0x18,0x08,0x0C,0x10,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _d_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x78,0x00,0x18,
0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x07,0xD8,0x0C,0x38,0x18,0x18,
0x18,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,
0x10,0x18,0x18,0x38,0x0C,0x5E,0x07,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _e_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x0C,0x30,0x08,0x18,
0x18,0x08,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x3F,0xFC,0x30,0x00,0x30,0x00,0x30,0x00,
0x18,0x04,0x18,0x08,0x0E,0x18,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _f_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x01,0x86,
0x01,0x06,0x03,0x06,0x03,0x00,0x03,0x00,0x03,0x00,0x3F,0xF8,0x03,0x00,0x03,0x00,
0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,
0x03,0x00,0x03,0x00,0x03,0x00,0x1F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char _F_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFC,0x18,0x1C,
0x18,0x04,0x18,0x02,0x18,0x02,0x18,0x00,0x18,0x00,0x18,0x10,0x18,0x10,0x18,0x30,
0x1F,0xF0,0x18,0x30,0x18,0x10,0x18,0x10,0x18,0x10,0x18,0x00,0x18,0x00,0x18,0x00,
0x18,0x00,0x18,0x00,0x18,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _g_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xEE,0x0C,0x36,0x08,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x08,0x18,0x0C,0x30,0x0F,0xE0,0x18,0x00,0x18,0x00,
0x1F,0xC0,0x0F,0xF8,0x18,0x1C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x18,0x18,0x07,0xE0};

unsigned char _h_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x78,0x00,0x18,0x00,
0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x19,0xE0,0x1A,0x30,0x1C,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _i_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,0xC0,
0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x1F,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _j_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x78,
0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xF0,0x00,0x30,0x00,0x30,
0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,
0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x18,0x60,0x18,0x40,0x0F,0x80};

unsigned char _k_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x78,0x00,0x18,0x00,
0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x18,0x7C,0x18,0x30,0x18,0x20,
0x18,0x40,0x18,0x80,0x19,0x80,0x1B,0x80,0x1E,0xC0,0x1C,0xC0,0x18,0x60,0x18,0x30,
0x18,0x30,0x18,0x18,0x18,0x1C,0x7E,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _l_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x1F,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _m_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0xEF,0x3C,0x71,0xC6,0x61,0x86,
0x61,0x86,0x61,0x86,0x61,0x86,0x61,0x86,0x61,0x86,0x61,0x86,0x61,0x86,0x61,0x86,
0x61,0x86,0x61,0x86,0x61,0x86,0xF3,0xCF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _n_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0xE0,0x7A,0x30,0x1C,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _o_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x0C,0x30,0x08,0x18,
0x18,0x18,0x10,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,
0x18,0x18,0x18,0x18,0x0C,0x30,0x03,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _p_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0xE0,0x7A,0x30,0x1C,0x18,
0x18,0x08,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x0C,
0x18,0x18,0x1C,0x18,0x1E,0x30,0x19,0xE0,0x18,0x00,0x18,0x00,0x18,0x00,0x7E,0x00};

unsigned char _q_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC8,0x0C,0x78,0x18,0x38,
0x18,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x18,
0x10,0x18,0x18,0x38,0x0C,0x78,0x07,0x98,0x00,0x18,0x00,0x18,0x00,0x18,0x00,0x7E};

unsigned char _r_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x1C,0x7E,0x66,0x06,0x86,
0x07,0x80,0x07,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00,
0x06,0x00,0x06,0x00,0x06,0x00,0x7F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _s_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE4,0x06,0x1C,0x0C,0x0C,
0x0C,0x04,0x0C,0x04,0x0E,0x00,0x07,0xC0,0x01,0xF0,0x00,0x78,0x00,0x1C,0x10,0x0C,
0x10,0x0C,0x18,0x0C,0x1C,0x18,0x13,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char _S_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xC8,0x18,0x78,
0x30,0x18,0x60,0x18,0x60,0x08,0x60,0x08,0x60,0x00,0x70,0x00,0x3C,0x00,0x1F,0x00,
0x07,0xC0,0x01,0xF0,0x00,0x78,0x00,0x18,0x00,0x1C,0x40,0x0C,0x40,0x0C,0x60,0x0C,
0x20,0x0C,0x30,0x18,0x38,0x30,0x27,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _t_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x00,0x01,0x00,0x01,0x00,0x03,0x00,0x07,0x00,0x3F,0xF8,0x03,0x00,0x03,0x00,
0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,
0x03,0x04,0x03,0x04,0x01,0x88,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char _T_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x31,0x84,
0x21,0x86,0x41,0x82,0x41,0x82,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
0x01,0x80,0x01,0x80,0x01,0x80,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _u_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x78,0x78,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x38,0x0C,0x5E,0x07,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _v_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x3E,0x18,0x0C,0x18,0x08,
0x18,0x18,0x0C,0x10,0x0C,0x10,0x04,0x20,0x06,0x20,0x06,0x20,0x03,0x40,0x03,0x40,
0x03,0xC0,0x01,0x80,0x01,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _w_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFB,0xCF,0x61,0x86,0x21,0x84,
0x31,0x84,0x31,0x84,0x31,0xC8,0x11,0xC8,0x1A,0xC8,0x1A,0x48,0x1A,0x70,0x0E,0x70,
0x0C,0x70,0x0C,0x30,0x0C,0x20,0x04,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char _W_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xCF,0x61,0x86,
0x61,0x84,0x21,0x84,0x20,0x84,0x30,0xC4,0x31,0xC4,0x31,0xC4,0x31,0xC8,0x31,0xC8,
0x11,0xC8,0x12,0x48,0x1A,0x68,0x1A,0x68,0x1A,0x70,0x1C,0x70,0x0C,0x70,0x0C,0x70,
0x0C,0x30,0x0C,0x20,0x08,0x20,0x08,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _x_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x7C,0x0C,0x10,0x0E,0x10,
0x06,0x20,0x03,0x40,0x03,0x40,0x01,0x80,0x01,0x80,0x01,0xC0,0x02,0x60,0x04,0x60,
0x04,0x30,0x08,0x18,0x18,0x18,0x7C,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char _y_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x3E,0x18,0x18,0x18,0x10,
0x08,0x10,0x0C,0x10,0x04,0x20,0x06,0x20,0x06,0x20,0x02,0x40,0x03,0x40,0x01,0x40,
0x01,0x80,0x01,0x80,0x01,0x00,0x01,0x00,0x01,0x00,0x02,0x00,0x3E,0x00,0x3C,0x00};

unsigned char _z_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xF8,0x30,0x38,0x30,0x30,
0x20,0x60,0x20,0xE0,0x00,0xC0,0x01,0x80,0x03,0x80,0x03,0x00,0x06,0x00,0x0E,0x04,
0x0C,0x04,0x18,0x0C,0x30,0x18,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


//:
unsigned char _maohao_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00};

unsigned char _maohao_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,0xC0,0x03,0xC0,
0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x80,0x03,0xC0,0x03,0xC0,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//￥
unsigned char _yuan_16x16[] = {
0x00,0x00,0x00,0x00,0x10,0x10,0x08,0x20,0x04,0x40,0x02,0x80,0x01,0x00,0x01,0x00,
0x01,0x00,0x0F,0xE0,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x00,0x00};
unsigned char _yuan_32x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFC,0x07,0xF8,0x01,0xC0,0x01,0xC0,
0x00,0xE0,0x01,0x00,0x00,0x60,0x03,0x00,0x00,0x30,0x06,0x00,0x00,0x18,0x0C,0x00,
0x00,0x0C,0x18,0x00,0x00,0x06,0x30,0x00,0x00,0x03,0x60,0x00,0x00,0x03,0xC0,0x00,
0x00,0x01,0x80,0x00,0x00,0xFF,0xFF,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,
0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,
0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x03,0xC0,0x00,0x00,0x1F,0xFC,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//-
unsigned char _gang_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char _gang_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//%
unsigned char _baifenhao_8x16[] = {
0x00,0x00,0x00,0x44,0xA4,0xA8,0xA8,0xB0,0x54,0x1A,0x2A,0x2A,0x4A,0x44,0x00,0x00};
unsigned char _baifenhao_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x10,0x6C,0x10,
0x44,0x20,0xC6,0x20,0xC6,0x40,0xC6,0x40,0xC6,0x40,0xC6,0x80,0xC6,0x80,0x44,0x80,
0x6D,0x38,0x39,0x6C,0x02,0x44,0x02,0xC6,0x02,0xC6,0x04,0xC6,0x04,0xC6,0x08,0xC6,
0x08,0xC6,0x08,0x44,0x10,0x6C,0x10,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//.
unsigned char _dain_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00};
unsigned char _dain_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x18,0x00,0x3C,0x00,0x3C,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// (空格)
unsigned char _kongge_8x16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char _kongge_16x32[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};



typedef struct FonT{
	
	unsigned char str[4];
	unsigned char *array_16;
	unsigned char *array_32; 

}Font_t , *pFont_t;



Font_t 	FONTLIB[TOTAL];
int	LIB_SIZE;



unsigned char *search_font_array();
int Print();
int add_FONTLIB();

int font_lib_init()
{
	
	//初始化
	bzero(&FONTLIB, sizeof(FONTLIB));
	LIB_SIZE = 0;

	add_FONTLIB("车", che_16x16, che_32x32);
	add_FONTLIB("位", wei_16x16, wei_32x32);
	
	add_FONTLIB("0", _0_8x16, _0_16x32);	
	add_FONTLIB("1", _1_8x16, _1_16x32);	
	add_FONTLIB("2", _2_8x16, _2_16x32);	
	add_FONTLIB("3", _3_8x16, _3_16x32);	
	add_FONTLIB("4", _4_8x16, _4_16x32);	
	add_FONTLIB("5", _5_8x16, _5_16x32);	
	add_FONTLIB("6", _6_8x16, _6_16x32);	
	add_FONTLIB("7", _7_8x16, _7_16x32);	
	add_FONTLIB("8", _8_8x16, _8_16x32);	
	add_FONTLIB("9", _9_8x16, _9_16x32);	
	
	add_FONTLIB("a", _a_8x16, _a_16x32);	
	add_FONTLIB("b", _b_8x16, _b_16x32);	
	add_FONTLIB("c", _c_8x16, _c_16x32);	
	add_FONTLIB("d", _d_8x16, _d_16x32);	
	add_FONTLIB("e", _e_8x16, _e_16x32);	
	add_FONTLIB("f", _f_8x16, _f_16x32);
	add_FONTLIB("F", _F_8x16, _F_16x32);	
	add_FONTLIB("g", _g_8x16, _g_16x32);	
	add_FONTLIB("h", _h_8x16, _h_16x32);	
	add_FONTLIB("i", _i_8x16, _i_16x32);	
	add_FONTLIB("j", _j_8x16, _j_16x32);
	add_FONTLIB("k", _k_8x16, _k_16x32);	
	add_FONTLIB("l", _l_8x16, _l_16x32);	
	add_FONTLIB("m", _m_8x16, _m_16x32);	
	add_FONTLIB("n", _n_8x16, _n_16x32);	
	add_FONTLIB("o", _o_8x16, _o_16x32);
	add_FONTLIB("p", _p_8x16, _p_16x32);	
	add_FONTLIB("q", _q_8x16, _q_16x32);	
	add_FONTLIB("r", _r_8x16, _r_16x32);	
	add_FONTLIB("s", _s_8x16, _s_16x32);
	add_FONTLIB("S", _S_8x16, _S_16x32);
	add_FONTLIB("t", _t_8x16, _t_16x32);	
	add_FONTLIB("T", _T_8x16, _T_16x32);
	add_FONTLIB("u", _u_8x16, _u_16x32);	
	add_FONTLIB("v", _v_8x16, _v_16x32);	
	add_FONTLIB("w", _w_8x16, _w_16x32);
	add_FONTLIB("W", _W_8x16, _W_16x32);	
	add_FONTLIB("x", _x_8x16, _x_16x32);	
	add_FONTLIB("y", _y_8x16, _y_16x32);	
	add_FONTLIB("z", _z_8x16, _z_16x32);	
	

	add_FONTLIB(":", 	_maohao_8x16, 		_maohao_16x32);	
	add_FONTLIB("￥", 	_yuan_16x16, 		_yuan_32x32);	
	add_FONTLIB("-", 	_gang_8x16, 		_gang_16x32);
	add_FONTLIB("%", 	_baifenhao_8x16,	_baifenhao_16x32);
	add_FONTLIB(".",	_dain_8x16,			_dain_16x32);
	add_FONTLIB(" ",	_kongge_8x16,		_kongge_16x32);
}


//
int print_string(pLcdInfo_t plcdinfo, int x, int y, unsigned char *str, int size, int color)
{
	
	//遍历字符串，匹配字符 UTF-8中文字符占用三个字节
	unsigned char *font_array = NULL;
	for(int i = 0; i < strlen(str); )
	{
		if(str[i] > 127)
		{
			unsigned char chinese[4] = {0};
			strncpy(chinese, str + i, 3);
			//printf("%s\n", chinese);
			i = i + 3;
			
			//寻找字库数据
			font_array = search_font_array(chinese, size);
			Print(plcdinfo, x, y, font_array, size, size, color);
			
			//位置右移
			x = x + size;
		}
		else 
		{
			unsigned char english[2] = {0};
			english[0] = str[i];
			//printf("%s\n", english);
			i++;
		
			//寻找字库数据
			font_array = search_font_array(english, size);
			Print(plcdinfo, x, y, font_array, size / 2, size, color);
	
			//位置右移
			x = x + size / 2;
		}

	}
	





}

int add_FONTLIB(unsigned char *str, unsigned char *array_16, unsigned char *array_32)
{
	strcpy(FONTLIB[LIB_SIZE].str, str);
	FONTLIB[LIB_SIZE].array_16 = array_16;
	FONTLIB[LIB_SIZE].array_32 = array_32;
	LIB_SIZE++;
}




unsigned char *search_font_array(unsigned char *str, int size)
{
	
	for(int i = 0; i < LIB_SIZE; i++)
	{
		
		if(strcmp(FONTLIB[i].str, str) == 0)
		{
			if(size == 16)
			{
				return FONTLIB[i].array_16;
			
			}
			else if(size == 32)
			{
				return FONTLIB[i].array_32;
			}
		}
	}

	printf("find no this %s in fontlib\n", str);
}







//
int Print(pLcdInfo_t plcdinfo, int x, int y, unsigned char *font_array, int width, int height, 
	 unsigned int color)
{
	
	//保存起始地址	
	unsigned int *base = plcdinfo->base + x + plcdinfo->width*y;

	//计算一行用多少位数据表示,仅支持 1位数据 代表 8个像素点格式
	int rowsize = width / 8;

	//每一次读取一行
	for(int rows = 0; rows < height*rowsize; rows = rows + rowsize)
	{

		int judge = 0x80;
		
		for(int cols = 0; cols < rowsize; cols++)
		{
			for(int i = 0; i < 8; i++)
			{
				if((font_array[rows + cols] & (judge >> i)))
				{
					//printf("1");
					*(base + i) = color; 
				}
				//else printf("0");
			}

			//前进八个像素点
			base = base + 8;
		}
		//printf("\n");

		//换行	
		base = base + plcdinfo->width - 8 * rowsize;
	}

}

