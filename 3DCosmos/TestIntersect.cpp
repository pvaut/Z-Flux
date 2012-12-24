#include "stdafx.h"

#define EPSILON 0.000001
#define CROSS(dest,v1,v2)           dest[0]=v1[1]*v2[2]-v1[2]*v2[1];           dest[1]=v1[2]*v2[0]-v1[0]*v2[2];           dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2)           dest[0]=v1[0]-v2[0];           dest[1]=v1[1]-v2[1];           dest[2]=v1[2]-v2[2]; 

int intersect_triangle(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v)
{
   double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   double det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
#else                    /* the non-culling branch */
   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
#endif
   return 1;
}

/*
int main(int argc, _TCHAR* argv[])
{
	double t1vert1[3] = {0,0,0};
	double t1vert2[3] = {2,0,0};
	double t1vert3[3] = {2,0,2};

	double t2vert1[3] = {0,2,0};
	double t2vert2[3] = {2,2,0};
	double t2vert3[3] = {2,2,2};

	double ray1orig[3] = {1,1,1};
	double ray1dir[3] = {0,-1,0};
	double ray2dir[3] = {0,1,0};

	double t,u,v;

	std::cout << intersect_triangle(ray1orig, ray1dir, t1vert1,t1vert2,t1vert3,&t,&u,&v) << std::endl;
	std::cout << intersect_triangle(ray1orig, ray2dir, t1vert1,t1vert2,t1vert3,&t,&u,&v) << std::endl;

	std::cout << intersect_triangle(ray1orig, ray1dir, t2vert1,t2vert2,t2vert3,&t,&u,&v) << std::endl;
	std::cout << intersect_triangle(ray1orig, ray2dir, t2vert1,t2vert2,t2vert3,&t,&u,&v) << std::endl;
	return 0;
}

*/
