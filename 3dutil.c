#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <kallisti/abi/fs.h>
#include <kallisti/abi/ta.h>
#include "matrix.h"
#include <kallisti/timer.h>
#include <kallisti/fs.h>
#include <kallisti/fs/iso9660.h>
#include "3dutil.h"


extern  abi_ta_t *ta;
extern abi_fs_t	*fs;
//extern _camera curr_cam;
/* 3D Utils Functions like Printing text & and blitting textures
   Kevin aka "KamKa" Fowlks
*/
#define RGB565(r,g,b) ((r >> 3) << 11)| ((g >> 2) << 5)| ((b >> 3) << 0)

unsigned int texture_alloc(uint32 size)
{
    unsigned int rv;
    
    rv = texture_pool;
    
    /* Align to 8 bytes */  
    size = (size + 7) & (~7);
    texture_pool += size;
    
    printf("Allocating texture at %08x\r\n", rv);
    return rv;
}

void texture_release_all() {
	texture_pool = 0;
}

unsigned int tex_load(unsigned short *data,int32 sz) 
{
    unsigned short *tex;
    unsigned int check_tex;

    check_tex = texture_alloc(sz);
    
	tex = (short *)ta->txr_map(check_tex);

	//tex = (short *)ta_texture_map(check_tex);

    memcpy(tex,data, sz);

  return check_tex;
}

/* Draws Textured 3DS Model On the Screen */
 void draw_3ds(_object testdummy,uint32 textureaddr,int tw, int th,_camera curr_cam)
{

     register i;
     poly_hdr_t poly;
     _3dvector d1,d2;
	
	clear_matrix();  
    
	apply_matrix(&screenview_matrix);
    
	apply_matrix(&projection_matrix);

    translate(testdummy.x,testdummy.y,testdummy.z);

	if (testdummy.x_rot > 0)  rotate_x(testdummy.x_rot);
	if (testdummy.y_rot > 0)  rotate_y(testdummy.y_rot);
	if (testdummy.z_rot > 0)  rotate_z(testdummy.z_rot);


	//set_camera( curr_cam.position,curr_cam.target, curr_cam.roll);
	
	translate(	curr_cam.position.v[0],	curr_cam.position.v[1],	curr_cam.position.v[2]);
	rotate_y(curr_cam.roll);

	transform_coords((float*)testdummy.vert_ary, trans_coords, testdummy.numverts);
 
       	   ta->poly_hdr_txr(&poly, TA_OPAQUE,TA_RGB565, tw, th,textureaddr,TA_NO_FILTER);
       	   ta->commit_poly_hdr(&poly);

	   for (i=0;i<testdummy.numpolys;i++) {

			d1.x= trans_coords[testdummy.facelist[i].c][0] - trans_coords[testdummy.facelist[i].a][0]; 
			d1.y= trans_coords[testdummy.facelist[i].c][1] - trans_coords[testdummy.facelist[i].a][1]; 

			d2.x= trans_coords[testdummy.facelist[i].c][0] - trans_coords[testdummy.facelist[i].b][0]; 
			d2.y= trans_coords[testdummy.facelist[i].c][1] - trans_coords[testdummy.facelist[i].b][1]; 
		              
		   if ((d1.x * d2.y) - (d2.x * d1.y) >= 0 )
			{
				draw_tri(trans_coords[testdummy.facelist[i].a], trans_coords[testdummy.facelist[i].b],
				trans_coords[testdummy.facelist[i].c],testdummy,i);
			}

		}


}


void draw_tri_color(float *t1, float *t2, float *t3,_object obj,int i)
 {
	vertex_oc_t vertc;
	 		
	vertc.flags = TA_VERTEX_NORMAL;
	vertc.x = t1[0] ; vertc.y =t1[1] ; vertc.z =t1[3];
	vertc.a = 1.0f; vertc.r = .40f; vertc.g = 0.0f; vertc.b = 1.0f;
	ta->commit_vertex(&vertc, sizeof(vertc));
	
	vertc.flags = TA_VERTEX_NORMAL;
	vertc.x = t2[0] ; vertc.y =t2[1] ; vertc.z =t2[3];
	vertc.a = 1.0f; vertc.r = 0.0; vertc.g = 0.0f; vertc.b = 0.0f;
	ta->commit_vertex(&vertc, sizeof(vertc));

	vertc.flags = TA_VERTEX_EOL;
	vertc.x = t3[0]; vertc.y =t3[1]; vertc.z =t3[3];
	vertc.a = 1.0f; vertc.r = 0.0f; vertc.g = 0.0f; vertc.b = 0.0f;
	ta->commit_vertex(&vertc, sizeof(vertc));
}


void draw_tri(float *t1, float *t2, float *t3,_object obj,int i)
 {

	vertex_ot_t vertc;
	
	vertc.flags = TA_VERTEX_NORMAL;
	vertc.x = t1[0] ; vertc.y =t1[1] ; vertc.z =t1[3];
	vertc.a = 1.0f; vertc.r = .63f; vertc.g = .63f; vertc.b = .63f;
	vertc.u = obj.texmap[obj.facelist[i].a].u;
	vertc.v = obj.texmap[obj.facelist[i].a].v;
	vertc.oa = vertc.or = vertc.og = vertc.ob = 0.0f;
	ta->commit_vertex(&vertc, sizeof(vertc));
	
	vertc.flags = TA_VERTEX_NORMAL;
	vertc.x = t2[0] ; vertc.y =t2[1] ; vertc.z =t2[3];
	vertc.a = 1.0f; vertc.r = .63f; vertc.g = .63f; vertc.b = .63f;
	vertc.u = obj.texmap[obj.facelist[i].b].u;
	vertc.v = obj.texmap[obj.facelist[i].b].v;
	ta->commit_vertex(&vertc, sizeof(vertc));

	vertc.flags = TA_VERTEX_EOL;
	vertc.x = t3[0]; vertc.y =t3[1]; vertc.z =t3[3];
	vertc.u = obj.texmap[obj.facelist[i].c].u;
	vertc.v = obj.texmap[obj.facelist[i].c].v;
	ta->commit_vertex(&vertc, sizeof(vertc));

}

void destroy_obj(_object *obj)
{
	if (obj->vert_ary) free(obj->vert_ary);
	if (obj->texmap) free(obj->texmap);
	if (obj->facelist) free(obj->facelist);
	if (obj->tri) free(obj->tri);
}

void show_tex(float sx,float sy,float size,float sizew,float sizeh,float x,float y,float x2,float y2,float alpha)
{
 
/*

	This Function creates & Displays a rectangle polygon(useing a triangle strip) with textures at any position
	you would like.

	sx, sy is the position that you would like to display the rectangle polygon on the
	screen at. 

	Size is the total texture size e.g 256x256 is just 256 this assumes that your textures are square.

	Sizew, Sizeh is the size of the polygon  * Note: This does not have to be
	the same as the above in that case the texture will be stretched to fit the poly.

	x, y start a 1,1 NOT 0, 0  x2, y2 start at 64, 64 not 63, 63
	x, y / x2, y2 is the start/end position of the texture postion given in REAL
	Bitmap pixel format this creates a box and grabs that data from the texture map to display.

*/



 vertex_ot_t vert;
 vert.flags = TA_VERTEX_NORMAL;
 vert.x =  sx;
 vert.y =  sizeh + sy;
 vert.z = 512.0f; /* Puts Blit infront of everything*/
 vert.u = 0.0f + (x / size);
 vert.v = y2 / size;

 /* ? Funny it seems that you must set the RGB value to something in a range .1 .. 1.0 
    This also controls the intensity of the texture. if you don't want your texture to have any
    funny colors then set all RGB values to the same color */
 
 vert.a = alpha; vert.r = 0.9f; vert.g = 0.9f; vert.b = 0.9f;
 vert.oa = vert.or = vert.og = vert.ob = 0.0f;
 
 ta->commit_vertex(&vert, sizeof(vert));

 vert.x = sx;
 vert.y = sy;
 vert.u = (x / size);
 vert.v = (y / size);
 ta->commit_vertex(&vert, sizeof(vert));

 vert.x = sizew+sx;
 vert.y = sizeh+sy;
 vert.u =  (x2 / size);
 vert.v =  (y2 / size);
 ta->commit_vertex(&vert, sizeof(vert));

 vert.flags = TA_VERTEX_EOL;
 vert.x =  sizew + sx;
 vert.y =  sy;
 vert.u =  (x2 / size);
 vert.v =  (y / size);
 ta->commit_vertex(&vert, sizeof(vert));

}



void tri_strip(float sx,float sy,float sz,float size,_3dvector rgb,float alpha)
{
 
/*

	This Function creates & Displays a rectangle polygon(useing a triangle strip) with textures at any position
	you would like.

	sx, sy is the position that you would like to display the rectangle polygon on the
	screen at. 

	Size is the total texture size e.g 256x256 is just 256 this assumes that your textures are square.

	Sizew, Sizeh is the size of the polygon  * Note: This does not have to be
	the same as the above in that case the texture will be stretched to fit the poly.

	x, y start a 1,1 NOT 0, 0  x2, y2 start at 64, 64 not 63, 63
	x, y / x2, y2 is the start/end position of the texture postion given in REAL
	Bitmap pixel format this creates a box and grabs that data from the texture map to display.

*/
float x=1.0,y=1.0,x2=size,y2=size;


 vertex_ot_t vert;
 vert.flags = TA_VERTEX_NORMAL;
 vert.x =  sx;
 vert.y =  size + sy;
 vert.z = sz; /* Puts Blit infront of everything*/
 vert.u = 0.0f + (x / size);
 vert.v = y2 / size;

 /* ? Funny it seems that you must set the RGB value to something in a range .1 .. 1.0 
    This also controls the intensity of the texture. if you don't want your texture to have any
    funny colors then set all RGB values to the same color */
 
 vert.a = alpha; 
 vert.r = rgb.v[0];
 vert.g = rgb.v[1];
 vert.b = rgb.v[2];
  vert.oa = vert.or = vert.og = vert.ob = 0.0f;

 /*
 vert.oa = rgb.v[2]/alpha;

 vert.or = rgb.v[1];
	 vert.og = rgb.v[2];
		 vert.ob = rgb.v[0];
 */

 ta->commit_vertex(&vert, sizeof(vert));

 vert.x = sx;
 vert.y = sy;
 vert.u = (x / size);
 vert.v = (y / size);
 ta->commit_vertex(&vert, sizeof(vert));

 vert.x = size+sx;
 vert.y = size+sy;
 vert.u =  (x2 / size);
 vert.v =  (y2 / size);
 ta->commit_vertex(&vert, sizeof(vert));

 vert.flags = TA_VERTEX_EOL;
 vert.x =  size + sx;
 vert.y =  sy;
 vert.u =  (x2 / size);
 vert.v =  (y / size);
 ta->commit_vertex(&vert, sizeof(vert));

}




void print_char(char c,short x,short y,char size,float alpha)
{
  int r = 9; /* Start at second Font set and start at numbers*/

  /*ABCDEFGHIJKMLNO r++
  PQRSTUVWXYZ */

 if (c > '?') 
    r++;
 
 if (c > 'O') 
    r++;

   c = c - '0';

   show_tex(x,y,256,size,size,16*c+2,2+16*r,16+16*c,16+16*r,alpha);
}





void print_string(char *str,short x,short y,float alpha)
{
  int len=strlen(str);
  int i;

 /* No Buffer Overflow Here 480 / 16 = 30 Max Char per line */

  for (i=0;i<len;i++) 
     if (str[i] != ' ')
     	print_char(str[i],x+(16*i),y ,16,alpha);
}

// GOOD
int  LoadTexture(char *filename,unsigned char *data,unsigned short *temp)
{
	unsigned char buff[256];
	int ii=0;
	int cnt,hi=0,wi=0;
    int count=0,equ=0;
	uint32 hnd;	
	uint16  w=0,h=0,bpp=0;
	_tga_header tga_header;
	
	
	hnd = fs->open(filename, O_RDONLY);
	
	if (!hnd) {
		printf("Error opening %s\r\n", filename);
		return;
	}

	cnt = fs->read(hnd, &tga_header, sizeof(_tga_header));
	
	w = tga_header.width[0]+tga_header.width[1]*0x0100;

	h = tga_header.height[0]+tga_header.height[1]*0x0100;
	
	bpp = tga_header.bpp;

	printf("Width = %d   Height = %d    Bpp = %d \r\n",w,h,bpp);

	if (bpp != 24)
	{
	    fs->close(hnd);
	    printf("Error: Wrong File format \r\n");
	    return -1;
	}

	
	fs->seek(hnd,0,SEEK_SET);

	while ((cnt = fs->read(hnd, buff, 256)) != 0) 
	{
	int i;
	
	for (i = 0; i < cnt; i++) 
	{
			
		if (count == 0) { i+=15; }

		data[count] = buff[i];
		count++;

	}

	}
	


	printf("closeing filesystem\r\n");

	fs->close(hnd);	
	
	for (hi=(h-1);hi>=0;hi--) 
	    { 
      	     equ = (hi*w);
		
	      for (wi=0;wi<=(w-1);wi++)
     		{
	       	 temp[equ+wi] = RGB565(data[ii+2],data[ii],data[ii+1]);
       		  ii+=3;
	     	}
	    }
	
	

	return -1;
}	

