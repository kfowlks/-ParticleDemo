#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "matrix.h"
#include <kallisti/libk.h>
#include <kallisti/abi/fs.h>
#include <kallisti/abi/ta.h>
#include <kallisti/abi/spu.h>
#include <kallisti/abi/maple.h>
#include <kallisti/abi/thread.h>
#include "../kosh/abi/conio.h"
#include "math.h"
#include "3dutil.h"



 /* Library handles */
  abi_fs_t	*fs = NULL;
  abi_ta_t	*ta = NULL;
  abi_spu_t	*spu = NULL;
  abi_maple_t	*maple = NULL;
  

/* 
	Code Released under GPL By Kevin Fowlks aka KamKa Feb 14th 2001
    Portion of this code where taken from NeHe Particle Tutor
	minor editing was done to get it working in my base code..
	I hope this code is of some use to someone :) .

	Code should Compile under KOS .80 
	Just Unzip into the userland dir and type run make.
	You need genroms workign on your system too... 

	kamka@barcode-search.com

 */



#define MAX_PARTICLES 150 /* Transparent Polys are VERY TA INTENSIVE */

typedef struct						// Create A Structure For Particle
{
	int	active;					// Active (Yes/No)
	float	life;					// Particle Life
	float	fade;					// Fade Speed
	float	r;						// Red Value
	float	g;						// Green Value
	float	b;						// Blue Value
	float	x;						// X Position
	float	y;						// Y Position
	float	z;						// Z Position
	float	xi;						// X Direction
	float	yi;						// Y Direction
	float	zi;						// Z Direction
	float	xg;						// X Gravity
	float	yg;						// Y Gravity
	float	zg;						// Z Gravity
}
particles;							// Particles Structure

particles particle[MAX_PARTICLES];	// Particle Array (Room For Particle Info)

float	zoom=-40.0f;
int col=0;
float	slowdown=1.0f;				// Slow Down Particles
float	xspeed;						// Base X Speed (To Allow Keyboard Direction Of Tail)
float	yspeed;	
float  joy_x=0.0;
float joy_y=0.0;
float yr=0.0;

static float colors[12][3]=		// Rainbow Of Colors
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};





#define RGB565(r,g,b) ((r >> 3) << 11)| ((g >> 2) << 5)| ((b >> 3) << 0)



void init_particle()
{

	int loop;


	for (loop=0;loop<MAX_PARTICLES;loop++)				// Initials All The Textures
	{
		particle[loop].active=1;						// Make All The Particles Active
		particle[loop].life=1.0f;

		// Give All The Particles Full Life
		particle[loop].fade= (float)randnum(100) / 1500.0f;
		particle[loop].r=colors[(loop+1)/(MAX_PARTICLES/12)][0];	// Select Red Rainbow Color
		particle[loop].g=colors[(loop+1)/(MAX_PARTICLES/12)][1];	// Select Green Rainbow Color
		particle[loop].b=colors[(loop+1)/(MAX_PARTICLES/12)][2];	// Select Blue Rainbow Color
		particle[loop].xi=(float)(randnum(50)-26.0f)*10.0f;	// Random Speed On X Axis
		particle[loop].yi=(float)(randnum(50)-25.0f)*10.0f;	// Random Speed On Y Axis
		particle[loop].zi=(float)(randnum(50)-25.0f)*10.0f;	// Random Speed On Z Axis
		particle[loop].xg=0.0f;							// Set Horizontal Pull To Zero
		particle[loop].yg=0.8f;						// Set Vertical Pull Downward
		particle[loop].zg=0.0f;							// Set Pull On Z Axis To Zero
	}

}



void draw_part(int x_pos,int y_pos)
{
int loop;
float x;
float y;
float z;
_3dvector rgb;

for (loop=0;loop<MAX_PARTICLES;loop++)					// Loop Through All The Particles
	{
		if (particle[loop].active == 1)							// If The Particle Is Active
		{

			x=particle[loop].x;
			y=particle[loop].y;

			/* Don't really need this cuz z is not being transformed  */
			z=particle[loop].z+zoom;

			rgb.v[0] = particle[loop].r;
			rgb.v[1] = particle[loop].g;
			rgb.v[2] = particle[loop].b;

			/* Main Call this does all the work it's in 3dutil.c */
			tri_strip(x+x_pos,y+y_pos,512,32,rgb,particle[loop].life);

			particle[loop].x+=particle[loop].xi/(slowdown*75);// Move On The X Axis By X Speed
			particle[loop].y+=particle[loop].yi/(slowdown*75);// Move On The Y Axis By Y Speed
			particle[loop].z+=particle[loop].zi/(slowdown*100);// Move On The Z Axis By Z Speed

			particle[loop].xi+=particle[loop].xg;			// Take Pull On X Axis Into Account
			particle[loop].yi+=particle[loop].yg;			// Take Pull On Y Axis Into Account
			particle[loop].zi+=particle[loop].zg;			// Take Pull On Z Axis Into Account
		 	
			particle[loop].life = particle[loop].life - particle[loop].fade; 	// Random Fade Speed
		

			if (particle[loop].life<0.0f)					// If Particle Is Burned Out
			{

				particle[loop].life=1.0f;					// Give It New Life
				particle[loop].fade=(float)randnum(100)/1500.0f+0.003f;	// Random Fade Value
				particle[loop].x=0.0f;						// Center On X Axis
				particle[loop].y=0.0f;						// Center On Y Axis
				particle[loop].z=0.0f;						// Center On Z Axis
				particle[loop].xi=xspeed+(float)(randnum(70)-32.0f);	// X Axis Speed And Direction
				particle[loop].yi=yspeed+(float)(randnum(70)-30.0f);	// Y Axis Speed And Direction
				particle[loop].zi=(float)(randnum(60)-30.0f);	// Z Axis Speed And Direction
				particle[loop].r=colors[col][0];			// Select Red From Color Table
				particle[loop].g=colors[col][1];			// Select Green From Color Table
				particle[loop].b=colors[col][2];			// Select Blue From Color Table
			}
		}

	}
}


int main() {


  poly_hdr_t poly; /* Our Basic Textured Poly  */
  
  unsigned int particle_txr,font_txr,bk_txr;
 
  uint8 mcont=0;
  abi_conio_t *conio;  
  cont_cond_t cond;
 
  /* Make a BIG BUFFER THE SIZE OF OUR MAX TEXTURE THIS IS JUST TEMP */
  unsigned short *temp = malloc(512*512*sizeof(unsigned short));
  unsigned  char *data = malloc(512*512*4); 

  
	fs = lib_open("fs");
	if (fs == NULL) { printf("Can't open FS lib\r\n"); return -1; }
	spu = lib_open("spu");
	if (spu == NULL) { printf("Can't open SPU lib\r\n"); return -1; }
	ta = lib_open("ta");
	if (ta == NULL) { printf("Can't open TA lib\r\n"); return -1; }
	maple = lib_open("maple");
	if (maple == NULL) { printf("Can't open Maple lib\r\n"); return -1; }

	conio = lib_open("conio");

	if (conio != NULL) {
		printf("Kosh detected; freezing\r\n");
		conio->freeze();
	}


  if (!LoadTexture("/rd/part.tga",data,temp)) {return -1;} 
   
	  particle_txr = tex_load(temp,(32*32)*sizeof(unsigned short));


  if (!LoadTexture("/rd/FONT1.TGA",data,temp)) {return -1;} 
   
	  font_txr = tex_load(temp,(256*256)*sizeof(unsigned short));


   /* Ok lets free some memory */
   if (temp) free(temp);
   if (data) free(data);


   setseed(1232); /*Sets Seed Value for Random Functions */


   init_particle();



  while(1)
  {
  
    ta->begin_render(); 
	/* IMPORTANT: YOU DON"T NEED A HEADER JUST MAKE SURE AN EOL IS SENT */
    ta->commit_eol();


    /* TA freaks out at having a completely empty list */
	ta->poly_hdr_txr(&poly, TA_TRANSLUCENT,TA_ARGB4444,256, 256, font_txr,TA_NO_FILTER);
    ta->commit_poly_hdr(&poly);
	
    print_string(" THIS IS A SAMPLE DREAMCAST PROGRAM",15,2,1.0);
	
	print_string("USE JOYPAD TO CHANGE PARTICLE DIRECTION",0,435,1.0);

	print_string("   KAMKA PRESENTS PARTICLE WORLD ",15,450,1.0);


	ta->poly_hdr_txr(&poly, TA_TRANSLUCENT,TA_ARGB4444,32, 32, particle_txr,TA_NO_FILTER);
    ta->commit_poly_hdr(&poly);

	draw_part(640/2,480/2); /* Draw Particle's and Center then on the screen */
	
    ta->commit_eol();


    /* Must Call to Finish Rendering Process*/
    ta->finish_frame();


	if (maple->cont_get_cond(maple->first_controller(), &cond) < 0)
			break;

	if (!(cond.buttons & CONT_START))
			break;

	
	if (!(cond.buttons & CONT_DPAD_UP)) {
		    joy_y = joy_y - 1.0;
				if (yspeed>-200) yspeed-= 1.0f;
	}
	
	if (!(cond.buttons & CONT_DPAD_DOWN)) {
		  joy_y =joy_y + 1.0;
		  if (yspeed<200) yspeed+=1.0f;

		 
	}
	
	if (!(cond.buttons & CONT_DPAD_LEFT)) {
		joy_x=joy_x-1.0;
			if (xspeed<200) xspeed-= 1.0f;

	}

	if (!(cond.buttons & CONT_DPAD_RIGHT)) {
		joy_x=joy_x+1.0;
			if (xspeed>-200) xspeed+= 1.0f;
	}


	
		if (col>11)	col=0;				// If Color Is To High Reset It

			col++;

  }

   printf("Exiting Main Loop \r\n");


  
  if (conio != NULL) {
		conio->thaw();
	}
  
      
  return 0;

}
