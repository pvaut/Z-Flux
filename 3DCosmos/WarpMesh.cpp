#include"stdafx.h"
#include<math.h>

#include "opengl.h"

#include"tools.h"
#include "WarpMesh.h"
#include "3Dmesh.h"


//void warp(double x, double y, double &xt, double &yt)
//{
//	double dst=sqrt((x-0.5)*(x-0.5)+(y-0.5)*(y-0.5));
//	dst=-0.1*dst;
//	xt=(1-dst)*(x-0.5)+0.5;
//	yt=(1+dst)*(y-0.5)+0.5;
//	//xt=x+0.2*y*y;
//	//yt=y-0.2*x*x;
//
//	//xt=x;
//	//yt=y;
//}


TWarpMesh::TWarpMesh()
{
	themesh=new T3Dmesh;
}

TWarpMesh::~TWarpMesh()
{
	delete themesh;
}

void TWarpMesh::init(const TMatrix &warpx, const TMatrix &warpy)
{
	if ((warpx.G_dim1()!=warpy.G_dim1())||(warpx.G_dim2()!=warpy.G_dim2()))
		throw QError(_text("Incompatible warp dimensions"));

	double xsize=1.0;
	double ysize=1.0;

	int dimx=warpx.G_dim1();
	int dimy=warpx.G_dim2();
	themesh->reset();
	int ct=0;
	for (int i=0; i<dimx; i++)
	{
		double x=i*1.0/(dimx-1);
		for (int j=0; j<dimy; j++)
		{
			double y=j*1.0/(dimy-1);
			themesh->addvertex(
				&Tvertex(x*xsize,y*ysize,0.0),
				ct,
				&Tvector(0.0,0.0,1.0),
				warpx.Get(i,j),warpy.Get(i,j));
			ct++;
		}
	}

	themesh->Make_stripbased(dimy,false);

}


void TWarpMesh::apply(Trendercontext *rc)
{
	themesh->render(rc,true,false);
	return;
}
