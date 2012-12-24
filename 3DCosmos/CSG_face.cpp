#include "stdafx.h"

#include "tools.h"

#include "CSG_line.h"

#include "CSG_face.h"

#include "CSG_shape.h"

/////////////////////////////////////////////////////////////////////
// CSGFace
/////////////////////////////////////////////////////////////////////


int CSGFace::UNKNOWN     = 1;
int CSGFace::INSIDE      = 2;
int CSGFace::OUTSIDE     = 3;
int CSGFace::SAME        = 4;
int CSGFace::OPPOSITE    = 5;

int CSGFace::UP          = 6;
int CSGFace::DOWN        = 7;
int CSGFace::ON          = 8;
int CSGFace::NONE        = 9;


CSGFace::CSGFace()
{
	label=0;
	facenormcalculated=false;
	v1=NULL;
	v2=NULL;v3=NULL;
	status=UNKNOWN;
}

CSGFace::CSGFace(CSGVertex *iv1, CSGVertex *iv2, CSGVertex *iv3)
{
	label=0;
	facenormcalculated=false;
	v1=iv1;
	v2=iv2;
	v3=iv3;
	bbox=CSGBBox(*v1,*v2,*v3);
	status=UNKNOWN;
	calcfacenormal();
}

CSGFace::CSGFace(const CSGFace &inp)
{
	copyfrom(inp);
}

void CSGFace::operator=(const CSGFace &inp)
{
	copyfrom(inp);
}


void CSGFace::copyfrom(const CSGFace &inp)
{
	label=inp.label;
	v1=inp.v1;
	v2=inp.v2;
	v3=inp.v3;
	n1=inp.n1;
	n2=inp.n2;
	n3=inp.n3;
	status = inp.status;
	bbox=inp.bbox;
	facenormal=inp.facenormal;
	facenormcalculated=inp.facenormcalculated;
}

bool CSGFace::equals_pointersmatch(CSGFace * pFace)
{
	if (v1!=pFace->v1) return false;
	if (v2!=pFace->v2) return false;
	if (v3!=pFace->v3) return false;
	return true;
}

bool CSGFace::equals(const CSGFace &iface) const
{
	if (v1->issamepos(*iface.v1) && v2->issamepos(*iface.v2) && v3->issamepos(*iface.v3)) return true;
	if (v1->issamepos(*iface.v2) && v2->issamepos(*iface.v3) && v3->issamepos(*iface.v1)) return true;
	if (v1->issamepos(*iface.v3) && v2->issamepos(*iface.v1) && v3->issamepos(*iface.v2)) return true;
	return false;
}

CSGBBox CSGFace::G_BBox()
{
	//return CSGBBox(*v1,*v2,*v3);
	return bbox;
}

CSGVector CSGFace::G_Normal() const
{
	return facenormal;
}

void CSGFace::calcfacenormal()
{
	CSGVector p1=v1->G_Position();
	CSGVector p2=v2->G_Position();
	CSGVector p3=v3->G_Position();
	CSGVector xy;xy.lincomb(p2,+1,p1,-1);
	CSGVector xz;xz.lincomb(p3,+1,p1,-1);
	facenormal.vecprod(xy, xz);
	area=facenormal.G_size()/2.0;
	facenormal.Normalise();
	facenormcalculated=true;
}

int CSGFace::G_Status()
{
	return status;
}

double CSGFace::G_Area()
{
	CSGVector p1=v1->G_Position();
	CSGVector p2=v2->G_Position();
	CSGVector p3=v3->G_Position();
	CSGVector xy;xy.lincomb(p2,+1,p1,-1);
	CSGVector xz;xz.lincomb(p3,+1,p1,-1);
	CSGVector n;n.vecprod(xy,xz);
	return n.G_size()/2.0;
}

void CSGFace::invert()
{
	CSGVertex * vertexTemp = v2;
	v2 = v1;
	v1 = vertexTemp;
	
	CSGVector ntemp;ntemp=n2;
	n2=n1;
	n1=ntemp;

	n1.Invert();
	n2.Invert();
	n3.Invert();
}

CSGVector CSGFace::InterpolNormal(const CSGVector &pos) const
{

	double d1=PointToLineDistance(v1->G_Position(),v2->G_Position(),v3->G_Position());
	double f1=PointToLineDistance(pos,v2->G_Position(),v3->G_Position());
	if (d1<=0) return v1->G_Position();
	f1=f1/d1;

	double d2=PointToLineDistance(v2->G_Position(),v1->G_Position(),v3->G_Position());
	double f2=PointToLineDistance(pos,v1->G_Position(),v3->G_Position());
	if (d2<=0) return v2->G_Position();
	f2=f2/d2;

	double d3=PointToLineDistance(v3->G_Position(),v1->G_Position(),v2->G_Position());
	double f3=PointToLineDistance(pos,v1->G_Position(),v2->G_Position());
	if (d3<=0) return v3->G_Position();
	f3=f3/d3;

	double ft=f1+f2+f3;
	f1/=ft;
	f2/=ft;
	f3/=ft;

	return CSGVector(
		f1*n1.x+f2*n2.x+f3*n3.x,
		f1*n1.y+f2*n2.y+f3*n3.y,
		f1*n1.z+f2*n2.z+f3*n3.z
		);
}


bool CSGFace::simpleClassify()
{
	int status1 = v1->G_Status();
	int status2 = v2->G_Status();
	int status3 = v3->G_Status();
		
	if(status1==CSGVertex::INSIDE)
	{
		status = INSIDE;
		return true; 
	}

	if(status1==CSGVertex::OUTSIDE)
	{
		status = OUTSIDE;
		return true; 
	}

	if(status2==CSGVertex::INSIDE)
	{
		status = INSIDE;
		return true;
	}

	if(status2==CSGVertex::OUTSIDE)
	{
		status = OUTSIDE;
		return true;
	}

	if(status3==CSGVertex::INSIDE)
	{
		status = INSIDE;
		return true;
	}

	if(status3==CSGVertex::OUTSIDE)
	{
		status = OUTSIDE;
		return true;
	}

	return false;
}


void CSGFace::rayTraceClassify(CSGShape &object)
{
	//creating a ray starting starting at the face baricenter going to the normal direction
	CSGVector p0;
	p0.x = (v1->x + v2->x + v3->x)/3.0;
	p0.y = (v1->y + v2->y + v3->y)/3.0;
	p0.z = (v1->z + v2->z + v3->z)/3.0;
	CSGLine ray(G_Normal(),p0);


	if (ray.direction.IsZero()) return;

	bool success;
	double dotProduct, distance; 
	CSGVector intersectionPoint;
	CSGFace * closestFace = 0;
	double closestDistance;

	int ctt=0;
	do
	{
		success = true; ctt++;
		closestDistance = 99999.9f;
		//for each face from the other shape
		for(int i=0;i<object.G_facecount();i++)
		{
			CSGFace & face = object.G_face(i);
			CSGVector facenormal=face.G_Normal();
			if (!facenormal.IsZero())
			{
				dotProduct = CSGVector::dotprod(facenormal,ray.getDirection()); // dot product
				bool bIntersectResult = false;
				intersectionPoint = ray.computePlaneIntersection(face.G_Normal(), face.v1->G_Position(), bIntersectResult);
				// Need to return whether was successful.
								
				//if ray intersects the plane
				if(bIntersectResult)
				{
					distance = ray.computePointToPointDistance(intersectionPoint);
					
					//if ray lies in plane
					if(fabs(distance)<TOL && fabs(dotProduct)<TOL)
					{
						//disturb the ray in order to not lie into another plane 
						ray.perturbDirection();
						success = false;
						break;
					}
					
					//if ray starts in plane
					if(fabs(distance)<TOL && fabs(dotProduct)>TOL)
					{
						//if ray intersects the face
						if(face.hasPoint(intersectionPoint))
						{
							//faces coincide
							closestFace = &face;
							closestDistance = 0;
							break;
						}
					}
					
					//if ray intersects plane.
					else if(fabs(dotProduct)>TOL && distance>TOL)
					{
						if(distance<closestDistance)
						{
							//if ray intersects the face;
							if(face.hasPoint(intersectionPoint))
							{
								//this face is the closest face untill now
								closestDistance = distance;
								closestFace = &face;
							}
						}
					}
				}
			}
		}
	}while((success==false)&&(ctt<1000));

	if (ctt>=999) ASSERT(false);
	
	//none face found: outside face
	if(closestFace==0)
	{
		status = OUTSIDE;
	}
	//face found: test dot product
	else
	{
		dotProduct = CSGVector::dotprod(closestFace->G_Normal(),ray.getDirection());
		
		//distance = 0: coplanar faces
		if(fabs(closestDistance)<TOL)
		{
			if(dotProduct>TOL)
			{
				status = SAME;
			}
			else if(dotProduct<-TOL)
			{
				status = OPPOSITE;
			}
		}
		
		//dot product > 0 (same direction): inside face
		else if(dotProduct>TOL)
		{
			status = INSIDE;
		}
		
		//dot product < 0 (opposite direction): outside face
		else if(dotProduct<-TOL)
		{
			status = OUTSIDE;
		}
	}

}

// Checks if the the face contains a point
bool CSGFace::hasPoint(const CSGVector & point)
{
	int result1, result2, result3;
	CSGVector normal = G_Normal(); 

	//if x is constant...	
	if(fabs(normal.x)>TOL) 
	{
		//tests on the x plane
		result1 = linePositionInX(point, v1->G_Position(), v2->G_Position());
		result2 = linePositionInX(point, v2->G_Position(), v3->G_Position());
		result3 = linePositionInX(point, v3->G_Position(), v1->G_Position());
	}
	
	//if y is constant...
	else if(fabs(normal.y)>TOL)
	{
		//tests on the y plane
		result1 = linePositionInY(point, v1->G_Position(), v2->G_Position());
		result2 = linePositionInY(point, v2->G_Position(), v3->G_Position());
		result3 = linePositionInY(point, v3->G_Position(), v1->G_Position());
	}
	else
	{
		//tests on the z plane
		result1 = linePositionInZ(point, v1->G_Position(), v2->G_Position());
		result2 = linePositionInZ(point, v2->G_Position(), v3->G_Position());
		result3 = linePositionInZ(point, v3->G_Position(), v1->G_Position());
	}
	
	//if the point is up and down two lines...		
	if(((result1==UP)||(result2==UP)||(result3==UP))&&((result1==DOWN)||(result2==DOWN)||(result3==DOWN)))
	{
		return true;
	}
	//if the point is on of the lines...
	else if ((result1==ON)||(result2==ON)||(result3==ON))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Gets the position of a point relative to a line in the x plane
int CSGFace::linePositionInX(const CSGVector &point, const CSGVector &pointLine1, const CSGVector &pointLine2)
{
	double a, b, z;
	if((fabs(pointLine1.y-pointLine2.y)>TOL)&&(((point.y>=pointLine1.y)&&(point.y<=pointLine2.y))||((point.y<=pointLine1.y)&&(point.y>=pointLine2.y))))
	{
		a = (pointLine2.z-pointLine1.z)/(pointLine2.y-pointLine1.y);
		b = pointLine1.z - a*pointLine1.y;
		z = a*point.y + b;
		if(z>point.z+TOL)
		{
			return UP;			
		}
		else if(z<point.z-TOL)
		{
			return DOWN;
		}
		else
		{
			return ON;
		}
	}
	else
	{
		return NONE;
	}
}

//Gets the position of a point relative to a line in the y plane
int CSGFace::linePositionInY(const CSGVector &point, const CSGVector &pointLine1, const CSGVector &pointLine2)
{
	double a, b, z;
	if((fabs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
	{
		a = (pointLine2.z-pointLine1.z)/(pointLine2.x-pointLine1.x);
		b = pointLine1.z - a*pointLine1.x;
		z = a*point.x + b;
		if(z>point.z+TOL)
		{
			return UP;			
		}
		else if(z<point.z-TOL)
		{
			return DOWN;
		}
		else
		{
			return ON;
		}
	}
	else
	{
		return NONE;
	}
}

//Gets the position of a point relative to a line in the z plane
int CSGFace::linePositionInZ(const CSGVector & point, const CSGVector & pointLine1, const CSGVector & pointLine2)
{
	double a, b, y;
	if((fabs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
	{
		a = (pointLine2.y-pointLine1.y)/(pointLine2.x-pointLine1.x);
		b = pointLine1.y - a*pointLine1.x;
		y = a*point.x + b;
		if(y>point.y+TOL)
		{
			return UP;			
		}
		else if(y<point.y-TOL)
		{
			return DOWN;
		}
		else
		{
			return ON;
		}
	}
	else
	{
		return NONE;
	}
}
