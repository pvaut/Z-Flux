#pragma once


#include "CSG_components.h"

#include "CSG_line.h"

class CSGSegment
{
public:

	CSGSegment();
	CSGSegment(CSGLine & line, CSGFace & face, int sign1, int sign2, int sign3);

	/** line resulting from the two planes intersection */
	CSGLine line;
	/** shows how many ends were already defined */
	int index;
	
	/** distance from the segment starting point to the point defining the plane */
	double startDist;
	/** distance from the segment ending point to the point defining the plane */
	double endDist;
	
	// These should probably be enums
	/** starting point status relative to the face */
	int startType;
	/** intermediate status relative to the face */
	int middleType;
	/** ending point status relative to the face */
	int endType;
	
	/** nearest vertex from the starting point */
	CSGVertex * startVertex;
	/** nearest vertex from the ending point */
	CSGVertex * endVertex; 
	
	/** start of the intersection point */
	CSGVector startPos;
	/** end of the intersection point */
	CSGVector endPos;

	/** define as vertex one of the segment ends */
	/** define as face one of the segment ends */
	/** define as edge one of the segment ends */
	static int VERTEX;
	static int FACE;
	static int EDGE;
	
	CSGVertex * getStartVertex();
	CSGVertex * getEndVertex();

	double getStartDistance();
	double getEndDistance();

	int getStartType();
	int getIntermediateType();
	int getEndType();

	int getNumEndsSet();

	CSGVector getStartPosition();
	CSGVector getEndPosition();

	bool intersect(const CSGSegment & segment);

private:

	bool setVertex(CSGVertex * vertex);
	bool setEdge(CSGVertex * vertex1, CSGVertex * vertex2);

	void swapEnds();
};
