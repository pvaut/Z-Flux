#pragma once

#include "CSG_components.h"


class CSGFace;

class CSGLine
{
public:

	CSGVector point;
	CSGVector direction;
	
	CSGLine();

	CSGLine(CSGFace * face1, CSGFace * face2);
	CSGLine(const CSGVector & direction, const CSGVector & point);
	
	CSGVector getPoint();
	CSGVector getDirection();
	void setPoint(const CSGVector & point);
	void setDirection(const CSGVector & direction);
		
	double computePointToPointDistance(const CSGVector & otherPoint);
	
	CSGVector computeLineIntersection(CSGLine * otherLine);
	CSGVector computePlaneIntersection(const CSGVector & normal, const CSGVector & planePoint, bool & bResult);

	void perturbDirection();
};
