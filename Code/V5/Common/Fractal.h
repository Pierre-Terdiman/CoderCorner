// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: FRACTAL.H
// Author: Pierre Terdiman
// Date: 08/15/98
// Abstract:
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM CODER CORNER (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

#ifndef __FRACTAL_H__
#define __FRACTAL_H__

#define XYZ_ADD(c,a,b)   { (c).x = (a).x + (b).x; (c).y = (a).y + (b).y;\
                           (c).z = (a).z + (b).z;}
#define XYZ_SUB(c,a,b)   { (c).x = (a).x - (b).x; (c).y = (a).y - (b).y; \
                           (c).z = (a).z - (b).z;}
#define XYZ_MULT(c,a,b)  { (c).x = (a).x * (b).x; (c).y = (a).y * (b).y; \
                           (c).z = (a).z * (b).z;}
#define XYZ_DIV(c,a,b)   { (c).x = (a).x / (b).x; (c).y = (a).y / (b).y; \
                           (c).z = (a).z / (b).z;}

#define XYZ_INC(c,a)     { (c).x += (a).x; (c).y += (a).y;\
                           (c).z += (a).z;}
#define XYZ_DEC(c,a)     { (c).x -= (a).x; (c).y -= (a).y;\
                           (c).z -= (a).z;}
#define XYZ_ADDC(c,a,b)  { (c).x = (a).x + (b); (c).y = (a).y + (b);\
                           (c).z = (a).z + (b);}
#define XYZ_SUBC(c,a,b)  { (c).x = (a).x - (b); (c).y = (a).y - (b); \
                           (c).z = (a).z - (b);}
#define XYZ_MULTC(c,a,b) { (c).x = (a).x * (b); (c).y = (a).y * (b);\
                           (c).z = (a).z * (b);}
#define XYZ_DIVC(c,a,b)  { (c).x = (a).x / (b); (c).y = (a).y / (b);\
                           (c).z = (a).z / (b);}
#define XYZ_COPY(b,a)    { (b).x = (a).x; (b).y = (a).y; (b).z = (a).z; }
#define XYZ_COPYC(b,a)   { (b).x = (a); (b).y = (a); (b).z = (a); }
#define  DOT_XYZ(p1, p2)  ((p1).x * (p2).x + (p1).y * (p2).y + (p1).z * (p2).z)

#define CROSS_XYZ(c,a,b) { (c).x = (a).y * (b).z - (a).z * (b).y; \
			   (c).y = (a).z * (b).x - (a).x * (b).z; \
			   (c).z = (a).x * (b).y - (a).y * (b).x; }
#define CROSS_3(c,a,b)   { (c)[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1]; \
			   (c)[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2]; \
			   (c)[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0]; }

#define NORMALIZE_XYZ(v)  { float __tmpnormval; \
			    __tmpnormval = (double)sqrt((v).x*(v).x +  \
					    (v).y*(v).y + (v).z*(v).z); \
			      (v).x /= __tmpnormval; \
			      (v).y /= __tmpnormval;  \
			      (v).z /= __tmpnormval;}
#define R_NORMALIZE_XYZ(v)  { float __tmpnormval; \
			    (((__tmpnormval = (double)sqrt((v).x*(v).x +  \
					    (v).y*(v).y + (v).z*(v).z)) \
			      == 0.0) ? FALSE : ((v).x /= __tmpnormval,  \
						 (v).y /= __tmpnormval,  \
						 (v).z /= __tmpnormval,TRUE));}

#define RGB_ADD(c,a,e)   { (c).r = (a).r + (e).r; (c).g = (a).g + (e).g; \
                           (c).b = (a).b + (e).b; }
#define RGB_SUB(c,a,e)   { (c).r = (a).r - (e).r; (c).g = (a).g - (e).g; \
                           (c).b = (a).b - (e).b; }
#define RGB_MULT(c,a,e)  { (c).r = (a).r * (e).r; (c).g = (a).g * (e).g; \
                           (c).b = (a).b * (e).b; }
#define RGB_ADDC(c,a,e)  { (c).r = (a).r + (e); (c).g = (a).g + (e); \
                           (c).b = (a).b + (e); }
#define RGB_SUBC(c,a,e)  { (c).r = (a).r - (e); (c).g = (a).g - (e); \
                           (c).b = (a).b - (e); }
#define RGB_MULTC(c,a,e) { (c).r = (a).r * (e); (c).g = (a).g * (e); \
                           (c).b = (a).b * (e); }
#define RGB_DIVC(c,a,e)  { (c).r = (a).r / (e); (c).g = (a).g / (e); \
                           (c).b = (a).b / (e); }
#define RGB_COPY(c,a)    { (c).r = (a).r; (c).g = (a).g; (c).b = (a).b; }
#define RGB_COPYC(c,a)   { (c).r = (a); (c).g = (a); (c).b = (a); }

#undef MIN
#undef MAX
#define    MIN(a, b)       ((a) < (b) ? (a) : (b))
#define    MAX(a, b)       ((a) > (b) ? (a) : (b))




#ifndef GRAPHICS_H
#define GRAPHICS_H
#endif

#define       ABS(a)          ((a) > 0 ? (a) : -(a))
double fabs(), sqrt(), sin(), cos(), tan();


  typedef struct xyz_td
          {
              float    x,
                       y,
                       z;
	  } xyz_td;



  typedef struct rgb_td
          {
              float    r,
                       g,
                       b;
	  } rgb_td;


typedef float tran_mat_td[4][4];


/*
 *****************************************************************************
 *                  FUNCTION FIELD DEFINITIONS
 *****************************************************************************
 */

typedef unsigned char flow_func_type;

typedef struct flow_func_td
    {
      short         func_type;
      xyz_td        center, axis;
      float         distance;
      float         falloff_start;
      short         falloff_type;
      float         parms[20];
    } flow_func_td;

typedef struct vol_shape_td
    {
      xyz_td         center;
      xyz_td         inv_rad_sq;
      xyz_td         inv_rad;
    } vol_shape_td;

typedef struct vol_td
    {
      short          obj_type;
      float          y_obj_min,    /* min & max y per volume             */
	             y_obj_max;
      float          x_obj_min,    /* min & max x per volume             */
	             x_obj_max;
      int            shape_type;   /* 0=sphere, 1=box;                   */
      int            self_shadow;  /* 0=no, 1=yes                        */
      xyz_td         scale_obj;   /*how to scale obj to get into -1:1 space*/
      xyz_td         trans_obj;   /*how to trans obj to get into -1:1 space*/
      vol_shape_td   shape;
      int            funct_name;
      rgb_td         color;
      float          amb_coef;
      float          diff_coef;
      float          spec_coef;
      float          spec_power;
      int            illum_type;  /* illumination - phong, blinn, c-t, gas */
      int            color_type;  /* constant, solid*/
      float          indx_refrac;
    } vol_td;


#endif // __FRACTAL_H__





#define TRUE    1
#define FALSE   0

typedef struct {
 Float64 x;
 Float64 y;
 Float64 z;
} FRACTALVECTOR;

Float64 FRAC_fBm(FRACTALVECTOR vectar, Float64 H, Float64 lacunarity, Float64 octaves);
Float64 FRAC_Multifractal(FRACTALVECTOR vectar, Float64 H,
        Float64 lacunarity, Float64 octaves, Float64 offset);
Float64 FRAC_Hetero_Terrain(FRACTALVECTOR vectar,
        Float64 H, Float64 lacunarity, Float64 octaves, Float64 offset);
Float64 FRAC_HybridMultifractal( FRACTALVECTOR vectar, Float64 H, Float64 lacunarity,
                    Float64 octaves, Float64 offset );
Float64 FRAC_RidgedMultifractal( FRACTALVECTOR vectar, Float64 H, Float64 lacunarity,
        Float64 octaves, Float64 offset, Float64 gain );
Float32 FRAC_Turbulence(FRACTALVECTOR v, Float32 freq);
Float32 FRAC_Bias(Float32 a, Float32 b);
Float32 FRAC_Gain(Float32 a, Float32 b);
Float32 FRAC_Noise1(Float32 arg);
Float32 FRAC_Noise2(Float32 vec[2]);
Float32 FRAC_Noise3(Float32 vec[3]);
static void FRAC_Normalize2(Float32 v[2]);
static void FRAC_Normalize3(Float32 v[3]);
static void FRAC_Init(void);

