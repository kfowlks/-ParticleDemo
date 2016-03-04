#ifndef __3DUTIL_H
#define __3DUTIL_H

#include "math.h"




typedef struct{
	unsigned char numiden3[12];
	unsigned char width[2];
	unsigned char height[2];
	unsigned char bpp;
	unsigned char imagedes;
} _tga_header;



static unsigned texture_pool = 0;
static float trans_coords[9000][4];

void show_tex(float ,float ,float ,float ,float ,float ,float ,float ,float,float);
void tri_strip(float ,float ,float,float ,_3dvector ,float );

void print_string(char *,short ,short ,float);
void print_char(char ,short ,short ,char ,float);
unsigned int texture_alloc(uint32 );
unsigned int tex_load(unsigned short *,int32 );
void draw_tri(float *, float *, float *,_object ,int );
void draw_tri_color(float *, float *, float *,_object ,int );
void draw_3ds(_object ,uint32 ,int , int ,_camera);
void destroy_obj(_object *);
void texture_release_all();

#endif