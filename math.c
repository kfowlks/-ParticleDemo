#include "math.h"





unsigned long seed=123;

#define RNDC 1013904223
#define RNDM 1164525


/* Floating-point Sin/Cos; 256 angles, -1.0 to 1.0 */

#define SIN(x) (sinus_tab[(x)&2047])
#define COS(x) (sinus_tab[((x)+512)&2047])


void setseed(int i) { if(i) seed=i;}



int rand() {

	seed = seed * RNDM + RNDC;

	return seed;

}



int randnum(int limit) { return (rand() & 0x7fffffff) % limit; }





/* matrices for transforming world coordinates to

   screen coordinates (with perspective)        */







void translate(float x, float y, float z)
{
    static float matrix[4][4] = {
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
    };
    
    matrix[3][0] = x;
    matrix[3][1] = y;
    matrix[3][2] = z;
    apply_matrix(&matrix);
}

void rotate_x(int n)
{
    static float matrix[4][4] = {
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
    };
    matrix[1][1] = matrix[2][2] = COS(n);
    matrix[1][2] = -(matrix[2][1] = SIN(n));
    apply_matrix(&matrix);
}

void rotate_y(int n)
{
    static float matrix[4][4] = {
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
    };
    matrix[0][0] = matrix[2][2] = COS(n);
    matrix[2][0] = -(matrix[0][2] = SIN(n));
    apply_matrix(&matrix);
}

void rotate_z(int n)
{
    static float matrix[4][4] = {
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
    };
    matrix[0][0] = matrix[1][1] = COS(n);
    matrix[0][1] = -(matrix[1][0] = SIN(n));
    apply_matrix(&matrix);
}

void
lib3ds_vector_sub(_3dvector c, _3dvector a, _3dvector b)
{
  int i;
  for (i=0; i<3; ++i) {
    c.v[i]=a.v[i]-b.v[i];
  }
}


void
lib3ds_vector_cross(_3dvector c, _3dvector a, _3dvector b)
{
  c.v[0]=a.v[1]*b.v[2] - a.v[2]*b.v[1];
  c.v[1]=a.v[2]*b.v[0] - a.v[0]*b.v[2];
  c.v[2]=a.v[0]*b.v[1] - a.v[1]*b.v[0];
}


void
set_camera_view( _3dvector pos,
  _3dvector tgt, float roll)
{

  float M[4][4] = {
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
    };

 float R[4][4] = {
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 },
    };


  
  _3dvector x, y, z;

  lib3ds_vector_sub(y, tgt, pos);
//  lib3ds_vector_normalize(y);
  
  z.v[0] = 0;
  z.v[1] = 0;
  z.v[2] = 1.0;
  
  lib3ds_vector_cross(x, y, z);
  lib3ds_vector_cross(z, x, y);

//  lib3ds_vector_normalize(x);
 // lib3ds_vector_normalize(y);

  //lib3ds_matrix_identity(M);
  M[0][0] = x.v[0];
  M[1][0] = x.v[1];
  M[2][0] = x.v[2];
  M[0][1] = y.v[0];
  M[1][1] = y.v[1];
  M[2][1] = y.v[2];
  M[0][2] = z.v[0];
  M[1][2] = z.v[1];
  M[2][2] = z.v[2];

  //lib3ds_matrix_identity(R);
  //lib3ds_matrix_rotate_y(R, roll);
 // lib3ds_matrix_mul(matrix, R,M);
  
  apply_matrix(&R);
  rotate_y(roll);

  apply_matrix(&M);
  //lib3ds_matrix_translate_xyz(matrix, );

  translate(-pos.v[0],-pos.v[1],-pos.v[2]);
}


float dot_product(const _3dvector *a,const _3dvector *b)

{

   return (a->x*b->x + a->y*b->y + a->z*b->z);



}



 float vec_length(const _3dvector *a)

{

	return ((float ) int_sqrt(a->x*a->x + a->y*a->y + a->z*a->z));

}





void calc_norm( const _3dvector *a, const _3dvector *b, const _3dvector *c, _3dvector *norm )

{

	_3dvector v,w;

	float len;



	//calc vectors for two of the sides of the triangle

	v.x = c->x - a->x;

	v.y = c->y - a->y;

	v.z = c->z - a->z;

	w.x = b->x - a->x;

	w.y = b->y - a->y;

	w.z = b->z - a->z;



	//normal is the cross product of the two sides

	norm->x = v.y*w.z - w.y*v.z;

	norm->y = w.x*v.z - v.x*w.z;

	norm->z = v.x*w.y - w.x*v.y;



	//normalize the vector

	// err.. no native square yet will add later

	// len = sqrt( norm->x*norm->x + norm->y*norm->y + norm->z*norm->z );

	

	len= int_sqrt(norm->x*norm->x + norm->y*norm->y + norm->z*norm->z);

	

	norm->x /= len;

	norm->y /= len;

	norm->z /= len;

}







float vector_length(const _3dvector *a)

{

  float len;

  len*=(a->x*a->x) + (a->y*a->y) + (a->z*a->z);

  return len;

}



void set_camera(float x,float y,float z)

{

	camera_matrix[3][0] = -x;

	camera_matrix[3][1] = -y;

	camera_matrix[3][2] = -z;

}









int int_sqrt(long x) {



  long sub = -1;



  if (x > 4096) {               /* start it off with accuracy of 128 */

    if (x > 16384) {

      if (x > 36864) {

        if (x > 65536) {

          if (x > 119716) {

            sub = 693;

            x -= 119716;

          }

          else {

            sub = 511;

            x -= 65536;

          }

        }

        else {

          sub = 383;

          x -= 36864;

        }

      }

      else {

        sub = 255;

        x -= 16384;

      }

    }

    else {

      sub = 127;

      x -= 4096;

    }

  }

  do {

    sub +=2;

    x -= sub;

    sub +=2;

    x -= sub;

    if (x<0)             /* a jump takes longer than no jump, */

      goto EndLoop;      /* so I unrolled it and used gotos.  */

    sub +=2;

    x -= sub;

    sub +=2;

    x -= sub;

    if (x<0)

      goto EndLoop;

    sub +=2;

    x -= sub;

    sub +=2;

    x -= sub;

    if (x<0)

      goto EndLoop;

    sub +=2;

    x -= sub;

    sub +=2;

    x -= sub;

  } while(x>=0);

EndLoop:

  if (x < -sub)

    return (unsigned)(sub>>1) - 1;

  return (unsigned)sub>>1;

}





