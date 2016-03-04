#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <kallisti/limits.h>
//#include <kallisti/fs/iso9660.h>
#include <kallisti/abi/fs.h>

#include "math.h"

extern abi_fs_t	*fs;

void vertex_list(uint32 filehnd,_object *obj)
{
	float x,y,z; // Temp float var's 
	int i=0; // index
	int cnt=1;

	while (i < obj->numverts && cnt != 0) 
	{
	  
		cnt = fs->read(filehnd, &x, sizeof(float));
		cnt = fs->read(filehnd, &y, sizeof(float));
		cnt = fs->read(filehnd, &z, sizeof(float));
		
	/*
		obj->tri[i].vec.x = x;
		obj->tri[i].vec.y = y;
		obj->tri[i].vec.z = z;
	*/


		obj->vert_ary[(i)*3] =	 x;
		obj->vert_ary[(i)*3+1] = y;
		obj->vert_ary[(i)*3+2] = z;


		i++;
	}
	
	return;
}


void uv_list(uint32 filehnd,_object *obj)
{
	float u=0,v=0; // Temp float var's 
	int i=0; // index
	int cnt=1;

	while (i < obj->numuv && cnt != 0) 
	{
		cnt = fs->read(filehnd, &u, sizeof(float));
		cnt = fs->read(filehnd, &v, sizeof(float));
		
		obj->texmap[i].u = u;
		obj->texmap[i].v = v;

		i++;
	}
	
	return;
}


void chunk_name(uint32 filehnd,int debug)
{
	int cnt=1;
	char c=1;
	int i=0;
	char temp[100]; // This should be enough
	
	while ((c != 0)  && (cnt !=0)) {
				
		cnt = fs->read(filehnd, &c, sizeof(char));
		
		temp[i] = c;	
		i++;

	} 
			
	temp[i+1]='\n';
	
	if (debug)
	   printf("%s\r\n",temp); 
	   
	return;
}

void face_list(uint32 filehnd,_object *obj)
{
	unsigned short a,b,c,flag; // Temp float var
	int i=0; // index
	int cnt=1;

	while (i < obj->numpolys && cnt != 0) 
	{
		cnt = fs->read(filehnd, &a, sizeof(short));
		cnt = fs->read(filehnd, &b, sizeof(short));
		cnt = fs->read(filehnd, &c, sizeof(short));
		cnt = fs->read(filehnd, &flag, sizeof(short));
		obj->facelist[i].a = a;
		obj->facelist[i].b = b;
		obj->facelist[i].c = c;
		i++;
	}
	
	return;
}


int load3dsfile(char *filename,  _object *model, int debug)
{

/*  
	Programmed by: Kevin Fowlks   Dec 29th 2000
	
	3DS Model Loader for the Dreamcast Compiles under KOS 0.7 with 
	GCC 2.95.2 

	Should work under libdream .95 with some minor changes.
	This code should fit easily in any engine design that you 
	may have. 
	

	This Code was Written By KamKa aka Kevin Fowlks
	Codes theory is based off of an example 3dsloader 
	by Ryan "Ark" Rubley & a 3DS loader found on flipcode
	
	Notes:
	
	   The debug switch controls weather 3ds file information is displayed on the term.
	   debug can either be 1 or 0.
	   
	   This Code only loads the first object in a 3ds file.
*/


	uint32 file;
	int cnt=0;
	char c;
	
	/* This should be a struct but for some reason my compiler switches are forcing a 4 byte align on shorts 
	 Hmm.. KOS doesn't like me malloc stuff in a function without free before exit... */
	
	unsigned short   chunkid;
	unsigned long    chunklen;
	unsigned short numverts = 0,numpolys = 0,numuv=0;
	char tempname[80];
	
	
	
	file = fs->open(filename, O_RDONLY);


	if (!file) {
		printf("Error opening %s\r\n",filename);
		return 1;
	}
	
	
	
	cnt=1;
	
	while (cnt != 0)
	{
	
	  /* Get the the first chunk */	
	  cnt = fs->read(file, &chunkid, sizeof(short)); 
	  cnt = fs->read(file, &chunklen, sizeof(long)); 
	
		switch (chunkid)
		{
		case 0x4D4D:
			if (debug)
				printf("0x%04X   chuck size = %d    ftell= %d\r\n",chunkid,chunklen,fs->tell(file));

			break;
		case 0x3D3D:
			if (debug)
				printf("0x%04X   chuck size = %d    ftell= %d\r\n",chunkid,chunklen,fs->tell(file));
			break;
		case 0x4000:
		
			if (debug) {
				printf("0x%04X   chuck size = %d    ftell= %d\r\n",chunkid,chunklen,fs->tell(file));
				chunk_name(file,1);
			} 
			else
				chunk_name(file,0);
			
			break;
		case 0x4100:
			if (debug)
				printf("0x%04X   chuck size = %d    ftell = %d\r\n",chunkid,chunklen,fs->tell(file));
			break;
		case 0x4110:
			if (numverts == 0) {

				if (debug)
				    printf("0x%04X   chuck size = %d   ftell= %d\r\n",chunkid,chunklen,fs->tell(file));

				cnt = fs->read(file, &numverts, sizeof(short));
				
				model->numverts = numverts;
				
				
			
				/*  BIG Problem.. HERE NOt SUre why Either Hoping it iwill got away*/
		//	model->tri = malloc(numverts * sizeof(_3dtriangle));

			
			model->vert_ary = malloc(3 * (numverts * sizeof(float)));
			
			
			/* Free Wasted Memory*/
		//	if (model->tri)
		//		free (model->tri);


				
				
				if (debug)
					printf("Numverts %d \r\n",model->numverts);
				
				
				vertex_list(file,model);
			}
			else
				fs->seek(file, chunklen-6, SEEK_CUR);	
				
			break;
		
		case 0x4120:
			if (numpolys == 0)
			{        
				cnt = fs->read(file, &numpolys, sizeof(short));

				
				
				model->numpolys = numpolys ;
				model->facelist = malloc(numpolys * sizeof(_trifacelist));    
				
				if (debug)
					printf("Numpolys %d \r\n",model->numpolys);

				   face_list(file,model);
			}
			else
				fs->seek(file, chunklen-6, SEEK_CUR);
				
			break;
			
			
		case 0x4140:
			if (debug)
				printf("0x%04X   chuck size = %d    ftell = %d\r\n",chunkid,chunklen,fs->tell(file));

			if (numuv == 0)
			{        
				cnt = fs->read(file, &numuv, sizeof(short));
				
				
				model->numuv = numuv;
				model->texmap = malloc(numuv * sizeof(_3duvmap)); 
				
				if (debug)
					printf("NumUV %d \r\n",numuv);

				   uv_list(file,model);
			}
			else
				fs->seek(file, chunklen-6, SEEK_CUR);
				
			break;
		
	        case 0xAFFF:  
		
				if (debug)
					printf("0x%04X   chuck size = %d   ftell= %d\r\n",chunkid,chunklen,fs->tell(file)); 
			break;
		case 0xA200: 
		
				if (debug)
					printf("0x%04X   chuck size = %d   ftell= %d\r\n",chunkid,chunklen,fs->tell(file));
			break;
		case 0xA300:		
			   if (debug) {
				printf("0x%04X   chuck size = %d    ftell= %d\r\n",chunkid,chunklen,fs->tell(file));
				chunk_name(file,1);
			   	}
			   else
			   	chunk_name(file,0);
			break;
						
								
		default:
			if (debug)
			    printf("unknown 0x%04X   Chunk size = %d   ftell = %d \r\n",chunkid,chunklen,fs->tell(file));

				fs->seek(file, chunklen-6, SEEK_CUR);
				break;

				}
	
	 
		}

	

	printf("End of 3DS File\r\n");

	fs->close(file);
	
	
	return 0;
	 
}


