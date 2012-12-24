#include "stdafx.h"

#include "tools.h"

#include "CSG_line.h"

#include "CSG_face.h"

/////////////////////////////////////////////////////////////////////
// CSGLine
/////////////////////////////////////////////////////////////////////

CSGLine::CSGLine()
{
	point = CSGVector(0,0,0);
	direction = CSGVector(1,0,0);
}

CSGLine::CSGLine(CSGFace *face1, CSGFace *face2)
{
	CSGVector normalFace1 = face1->G_Normal();
	CSGVector normalFace2 = face2->G_Normal();
	
	//direction: cross product of the faces normals
	
	direction.vecprod(normalFace1, normalFace2);

	//if direction lenght is not zero (the planes aren't parallel )...
	if (!direction.IsZero())
	{
		//getting a line point, zero is set to a coordinate whose direction 
		//component isn't zero (line intersecting its origin plan)
		double d1 = -(normalFace1.x*face1->v1->x + normalFace1.y*face1->v1->y + normalFace1.z*face1->v1->z);
		double d2 = -(normalFace2.x*face2->v1->x + normalFace2.y*face2->v1->y + normalFace2.z*face2->v1->z);
		if(fabs(direction.x)>TOL)
		{
			point.x = 0;
			point.y = (d2*normalFace1.z - d1*normalFace2.z)/direction.x;
			point.z = (d1*normalFace2.y - d2*normalFace1.y)/direction.x;
		}
		else if(fabs(direction.y)>TOL)
		{
			point.x = (d1*normalFace2.z - d2*normalFace1.z)/direction.y;
			point.y = 0;
			point.z = (d2*normalFace1.x - d1*normalFace2.x)/direction.y;
		}
		else
		{
			point.x = (d2*normalFace1.y - d1*normalFace2.y)/direction.z;
			point.y = (d1*normalFace2.x - d2*normalFace1.x)/direction.z;
			point.z = 0;
		}
	}
			
	direction.Normalise();
}

CSGLine::CSGLine(const CSGVector & directioni, const CSGVector & pointi)
{
	direction = directioni;
	point = pointi;
	direction.Normalise();
}


CSGVector CSGLine::getPoint()
{
	return point;
}

CSGVector CSGLine::getDirection()
{
	return direction;
}

void CSGLine::setPoint(const CSGVector &pointi)
{
	point = pointi;
}

void CSGLine::setDirection(const CSGVector &directioni)
{
	direction = directioni;
}

double CSGLine::computePointToPointDistance(const CSGVector &otherPoint)
{
	double distance = (point - otherPoint).G_size();
	CSGVector vec = CSGVector(otherPoint.x - point.x, otherPoint.y - point.y, otherPoint.z - point.z);
	vec.Normalise();
	if (CSGVector::dotprod(vec,direction)<0)
		return -distance;			
	else
		return distance;
}

CSGVector CSGLine::computeLineIntersection(CSGLine * otherLine)
{
	//x = x1 + a1*t = x2 + b1*s
	//y = y1 + a2*t = y2 + b2*s
	//z = z1 + a3*t = z2 + b3*s
	
	CSGVector linePoint = otherLine->getPoint(); 
	CSGVector lineDirection = otherLine->getDirection();

	double t;
	if(fabs(direction.y*lineDirection.x-direction.x*lineDirection.y)>TOL)
	{
		t = (-point.y*lineDirection.x+linePoint.y*lineDirection.x+lineDirection.y*point.x-lineDirection.y*linePoint.x)/(direction.y*lineDirection.x-direction.x*lineDirection.y);
	}
	else if (fabs(-direction.x*lineDirection.z+direction.z*lineDirection.x)>TOL)
	{
		t=-(-lineDirection.z*point.x+lineDirection.z*linePoint.x+lineDirection.x*point.z-lineDirection.x*linePoint.z)/(-direction.x*lineDirection.z+direction.z*lineDirection.x);
	}
	else if (fabs(-direction.z*lineDirection.y+direction.y*lineDirection.z)>TOL)
	{
		t = (point.z*lineDirection.y-linePoint.z*lineDirection.y-lineDirection.z*point.y+lineDirection.z*linePoint.y)/(-direction.z*lineDirection.y+direction.y*lineDirection.z);
	}
	else
	{
		return CSGVector(0,0,0);
	}

	CSGVector vResult;
	vResult.lincomb(point,1,direction,t);
	
	return vResult;
}

CSGVector CSGLine::computePlaneIntersection(const CSGVector &normal, const CSGVector &planePoint, bool & bResult)
{
	bResult = true;

	//Ax + By + Cz + D = 0
	//x = x0 + t(x1 – x0)
	//y = y0 + t(y1 – y0)
	//z = z0 + t(z1 – z0)
	//(x1 - x0) = dx, (y1 - y0) = dy, (z1 - z0) = dz
	//t = -(A*x0 + B*y0 + C*z0 )/(A*dx + B*dy + C*dz)

	
	double A = normal.x;
	double B = normal.y;
	double C = normal.z;
	double D = CSGVector::dotprod(normal,planePoint) * -1.0f;
		
	double numerator = CSGVector::dotprod(normal,point) + D;
	double denominator = CSGVector::dotprod(normal,direction);
			
	//if line is paralel to the plane...
	if(fabs(denominator)<TOL)
	{
		//if line is contained in the plane...
		if(fabs(numerator)<TOL)
		{
			return point;
		}
		else
		{
			bResult = false;
			return CSGVector(0,0,0);
		}
	}
	//if line intercepts the plane...
	else
	{
		double t = -numerator/denominator;
		CSGVector resultPoint;
		resultPoint.lincomb(point,1,direction,t);
		
		return resultPoint;
	}
}


double LineRandomNum()
{
	int nRand = rand() % 10000;
	double fRand = (double)nRand;
	fRand *= 0.0001;
	fRand *= 2.0f;
	fRand -= 1.0f;
	return fRand;
}

void CSGLine::perturbDirection()
{
	direction.x += LineRandomNum() * 0.001f;			
	direction.y += LineRandomNum() * 0.001f;			
	direction.z += LineRandomNum() * 0.001f;
}

