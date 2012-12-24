#include "stdafx.h"

#include "tools.h"

#include "3Dmesh.h"

#include "CSG_line.h"

#include "CSG_face.h"

#include "CSG_segment.h"

#include "CSG_shape.h"


#include "SC_func.h"
#include "SC_env.h"

#include "vec_transformation.h"


//////////////////////////////////////////////////////////////////
// CSGVertexFinder
//////////////////////////////////////////////////////////////////

CSGVertexFinder::CSGVertexFinder()
{
	count=0;
	isactive=false;
}

void CSGVertexFinder::reset()
{
	cells.reset();
	count=0;
	isactive=false;
}

void CSGVertexFinder::start(const CSGBBox &ibbox)
{
	reset();
	bbox=ibbox;
	count=20;
	for (int i=0; i<count*count*count; i++) cells.add(new TVertexFinderCell);
	isactive=true;
}

void CSGVertexFinder::addvertex(CSGVertex *ivertex)
{
	if (!isactive) throw QError(_text("VertexFinder is not activated"));
	int cellidx=G_cellidx(ivertex->x,ivertex->y,ivertex->z);
	cells[cellidx]->add(ivertex);
}

void CSGVertexFinder::delvertex(CSGVertex *ivertex)
{
	if (!isactive) throw QError(_text("VertexFinder is not activated"));
	int cellidx=G_cellidx(ivertex->x,ivertex->y,ivertex->z);
	TVertexFinderCell &cell=G_cell(cellidx);
	for (int i=0; i<cell.G_count(); i++)
		if (ivertex==cell[i])
		{
			cell.del(i);
			return;
		}
}


int CSGVertexFinder::G_cellidx(double x, double y, double z) const
{
	if (!isactive) throw QError(_text("VertexFinder is not activated"));
	int ix=(int)((x-bbox.xmin)/(bbox.xmax-bbox.xmin)*count);
	if (ix<0) ix=0; if (ix>=count) ix=count-1;
	int iy=(int)((y-bbox.ymin)/(bbox.ymax-bbox.ymin)*count);
	if (iy<0) iy=0; if (iy>=count) iy=count-1;
	int iz=(int)((z-bbox.zmin)/(bbox.zmax-bbox.zmin)*count);
	if (iz<0) iz=0; if (iz>=count) iz=count-1;
	return (ix*count+iy)*count+iz;
}

TVertexFinderCell& CSGVertexFinder::G_cell(int cellidx)
{
	if ((cellidx<0)||(cellidx>=cells.G_count())) throw QError(_text("Invalid VertexFinder cell index"));
	return *cells[cellidx];
}




//////////////////////////////////////////////////////////////////
// CSGShape
//////////////////////////////////////////////////////////////////

CSGVertex* CSGShape::addvertex(const CSGVertex &ivertex)
{
	bbox.AddVertex(ivertex);
	vertices.add(new CSGVertex(ivertex));
	vertices[vertices.G_count()-1]->idx=vertices.G_count()-1;
	if (finder.G_isactive()) finder.addvertex(vertices[vertices.G_count()-1]);
	return vertices[vertices.G_count()-1];
}

CSGVertex* CSGShape::addvertexifnew(const CSGVertex &ivertex)
{
	int nr=findvertexnr(CSGVector(ivertex.x,ivertex.y,ivertex.z));
	if (nr>=0)
	{
//		vertices[nr]->status=ivertex.status;
		return vertices[nr];
	}

	bbox.AddVertex(ivertex);
	return addvertex(ivertex);
}


void CSGShape::startfinder(const CSGBBox &ibbox)
{
	finder.start(ibbox);
	for (int i=0; i<vertices.G_count(); i++) finder.addvertex(vertices[i]);
	int i=0;
}

void CSGShape::stopfinder()
{
	finder.reset();
}


int CSGShape::findvertexnr(const CSGVector &pos)
{
	if (finder.G_isactive())
	{
		int cellidx=finder.G_cellidx(pos.x,pos.y,pos.z);
		TVertexFinderCell& cellcontent=finder.G_cell(cellidx);
		for (int i=0; i<cellcontent.G_count(); i++)
			if (cellcontent[i]->equals(pos)) return cellcontent[i]->idx;
		return -1;
	}
	else
	{
		for(int i=0;i<vertices.G_count();i++)
			if(vertices[i]->equals(pos)) return i;			
		return -1;
	}
}


CSGVertex* CSGShape::addVertex(const CSGVector &pos, int status)
{
	int nr=findvertexnr(pos);

	if(nr<0)
	{
		CSGVertex vertex(pos.x,pos.y,pos.z, status);
		return addvertex(vertex);
	}
	else
	{
		vertices[nr]->Set_Status(status);
		return vertices[nr];
	}
}




void CSGShape::delvertex(int nr)
{
	if ((nr<0)||(nr>=vertices.G_count())) throw QError(_text("Invalid vertex nr"));
	if (finder.G_isactive()) finder.delvertex(vertices[nr]);
	vertices.del(nr);
	for (int i=nr; i<vertices.G_count(); i++) vertices[i]->idx=i;
}


CSGFace* CSGShape::addFace(CSGVertex *v1, CSGVertex *v2, CSGVertex *v3)
{
	if (v1->equals(v2)||v1->equals(v3)||v2->equals(v3))
	{
		//ASSERT(false);
		return NULL;
	}

	CSGFace fc(v1,v2,v3);
	if (fc.G_Area()<TOL)
	{
		ASSERT(false);
		return NULL;
	}

	faces.add(fc);
	return &faces[faces.G_count()-1];
}


CSGFace* CSGShape::addFace(CSGVertex *v1, CSGVertex *v2, CSGVertex *v3, const CSGVector &in)
{
	CSGFace *fc=addFace(v1,v2,v3);
	if (fc==NULL) return NULL;

	fc->n1=in;
	fc->n2=in;
	fc->n3=in;
	return fc;
}

CSGFace* CSGShape::addFace(CSGVertex *v1, CSGVertex *v2, CSGVertex *v3, const CSGVector &in1, const CSGVector &in2, const CSGVector &in3, int ilabel)
{
	CSGFace *fc=addFace(v1,v2,v3);
	if (fc==NULL) return NULL;
	fc->label=ilabel;
	fc->n1=in1;
	fc->n2=in2;
	fc->n3=in3;
	return fc;
}


void CSGShape::addFace(int vidx1, int vidx2, int vidx3)
{
	addFace(vertices[vidx1],vertices[vidx2],vertices[vidx3],vertices[vidx1]->normal,vertices[vidx2]->normal,vertices[vidx3]->normal,0);
}


void CSGShape::addquad(int vidx1, int vidx2, int vidx3, int vidx4)
{
	addFace(vertices[vidx1],vertices[vidx2],vertices[vidx3],vertices[vidx1]->normal,vertices[vidx2]->normal,vertices[vidx3]->normal,0);
	addFace(vertices[vidx3],vertices[vidx4],vertices[vidx1],vertices[vidx3]->normal,vertices[vidx4]->normal,vertices[vidx1]->normal,0);
}




const CSGVertex& CSGShape::G_vertex(int nr) const
{
	if ((nr<0)||(nr>=vertices.G_count())) throw QError(_text("Invalid vertex nr"));
	return *vertices[nr];
}

const CSGFace& CSGShape::G_face(int nr) const
{
	if ((nr<0)||(nr>=faces.G_count())) throw QError(_text("Invalid vertex nr"));
	return faces[nr];
}

CSGFace& CSGShape::G_face(int nr)
{
	if ((nr<0)||(nr>=faces.G_count())) throw QError(_text("Invalid vertex nr"));
	return faces[nr];
}





//-------------------------FACES_SPLITTING_METHODS------------------------------//

/**
 * Split faces so that none face is intercepted by a face of other object
 * 
 * @param object the other object 3d used to make the split 
 */
void CSGShape::splitFaces(CSGShape *object)
{
	CSGLine line;
	CSGFace face1;
	CSGFace face2;

	CSGSegment segment1, segment2;

	double distFace1Vert1, distFace1Vert2, distFace1Vert3, distFace2Vert1, distFace2Vert2, distFace2Vert3;
	int signFace1Vert1, signFace1Vert2, signFace1Vert3, signFace2Vert1, signFace2Vert2, signFace2Vert3;
	int numFacesBefore = G_facecount();
	int numFacesStart = G_facecount();
	int facesIgnored = 0;

	//--!!--dit kan versneld worden door gebruik te maken van slimme structuren om deze n x n face test te acceleren
					
	if(G_BBox().overlap(object->G_BBox()))
	{			
		for(int i=0;i<G_facecount();i++)
		{
			face1 = G_face(i);

			if(face1.G_BBox().overlap(object->G_BBox()))
			{
				for(int j=0;j<object->G_facecount();j++)
				{
					face2 = object->G_face(j);

					if(face1.G_BBox().overlap(face2.G_BBox()))
					{
						//PART I - DO TWO POLIGONS INTERSECT?
						//POSSIBLE RESULTS: INTERSECT, NOT_INTERSECT, COPLANAR
						
						//distance from the face1 vertices to the face2 plane
						distFace1Vert1 = computeDistance(*(face1.v1), face2);
						distFace1Vert2 = computeDistance(*(face1.v2), face2);
						distFace1Vert3 = computeDistance(*(face1.v3), face2);
						
						//distances signs from the face1 vertices to the face2 plane 
						signFace1Vert1 = (distFace1Vert1>TOL? 1 :(distFace1Vert1<-TOL? -1 : 0)); 
						signFace1Vert2 = (distFace1Vert2>TOL? 1 :(distFace1Vert2<-TOL? -1 : 0));
						signFace1Vert3 = (distFace1Vert3>TOL? 1 :(distFace1Vert3<-TOL? -1 : 0));
						
						//if all the signs are zero, the planes are coplanar
						//if all the signs are positive or negative, the planes do not intersect
						//if the signs are not equal...
						if (!(signFace1Vert1==signFace1Vert2 && signFace1Vert2==signFace1Vert3))
						{
							//distance from the face2 vertices to the face1 plane
							distFace2Vert1 = computeDistance(*(face2.v1), face1);
							distFace2Vert2 = computeDistance(*(face2.v2), face1);
							distFace2Vert3 = computeDistance(*(face2.v3), face1);
							
							//distances signs from the face2 vertices to the face1 plane
							signFace2Vert1 = (distFace2Vert1>TOL? 1 :(distFace2Vert1<-TOL? -1 : 0)); 
							signFace2Vert2 = (distFace2Vert2>TOL? 1 :(distFace2Vert2<-TOL? -1 : 0));
							signFace2Vert3 = (distFace2Vert3>TOL? 1 :(distFace2Vert3<-TOL? -1 : 0));
						
							//if the signs are not equal...
							if (!(signFace2Vert1==signFace2Vert2 && signFace2Vert2==signFace2Vert3))
							{
								line = CSGLine(&face1, &face2);
						
								//intersection of the face1 and the plane of face2
								segment1 = CSGSegment(line, face1, signFace1Vert1, signFace1Vert2, signFace1Vert3);
																
								//intersection of the face2 and the plane of face1
								segment2 = CSGSegment(line, face2, signFace2Vert1, signFace2Vert2, signFace2Vert3);
															
								//if the two segments intersect...
								if(segment1.intersect(segment2))
								{
									//PART II - SUBDIVIDING NON-COPLANAR POLYGONS
									int lastNumFaces = G_facecount();
									this->splitFace(i, segment1, segment2);
																		
									//prevent from infinite loop (with a loss of faces...)
									if(numFacesStart*20<G_facecount())
									{
										//System.out.println("possible infinite loop situation: terminating faces split");
										//return;
									}
							
									//if the face in the position isn't the same, there was a break 
									// GregS - There is some list jigging going on!
									// Well of course there would be - the number of faces can't be expected to remain constant!!
									// Same goes for vertices!

									// This must be the bit that isn't doing its job properly!!!!!!!!
									// Danilo himself wrote:
									// "if the face in the position isn't the same, there was a break"
									// Thats pretty damn clear!
									if(!face1.equals_pointersmatch(&G_face(i))) 
//									if(!(face1->equals_pointersmatch(&face1Orig)))
									{
/*										if(false)
										{
											DiagnosticEvent * pEvent = g_pTool->AddEvent();
											if(pEvent)
											{
												//pEvent->AddTriangle(face1->v1->getPosition(), face1->v2->getPosition(), face1->v3->getPosition(), gxColorWhite);
												//pEvent->AddTriangle(face2->v1->getPosition(), face2->v2->getPosition(), face2->v3->getPosition(), gxColorBlue);
												//pEvent->SetObjects(this, object);
												pEvent->SetObjects(this, 0);
												//pEvent->SetObjectWithHighlightedTriangle(this, i);
												pEvent->SetText("Face %d was cut with %d", i,j);
											}
										}*/

										//if the generated solid is equal the origin...
										//if(face1->equals(getFace(getNumFaces()-1)))
										if(face1.equals(G_face(G_facecount()-1)))
										{
											//return it to its position and jump it
											if(i!=(G_facecount()-1))
											{
												// Add a before and after diagnostic event.
												// Here is a call to remove!!!!!!!!!!!!!
												faces.del(G_facecount()-1);
												//faces->InsertFace(i, *face1);
												faces.insertbefore(i, face1);
												// I vaguely remember there being a problem with this interpretation....
												// Indices have to remain the same???
											}
											else
											{
												// Is this technically necessary? This just means do the next loop, which would have happened anyway.
												continue;
											}
										}
										//else: test next face
										// This is because the whole face list was shunted down when the current face was removed, so
										// the next face is the face at the current position in the list.
										else
										{
											i--;
											break;
										}
									}
								}
							}
						}
					}

/*					if(false)
					{
						DiagnosticEvent * pEvent = g_pTool->AddEvent();
						if(pEvent)
						{
							//pEvent->AddTriangle(face1->v1->getPosition(), face1->v2->getPosition(), face1->v3->getPosition(), gxColorWhite);
							//pEvent->AddTriangle(face2->v1->getPosition(), face2->v2->getPosition(), face2->v3->getPosition(), gxColorBlue);
							//pEvent->SetObjects(this, object);
							pEvent->SetObjects(this, 0);
							//pEvent->SetObjectWithHighlightedTriangle(this, i);
							pEvent->SetText("After cut Face %d x %d", i,j);
						}
					}*/
				}
			}
		}
	}

	extern int g_CSGDiagToolNumEventAdds; 

/*	if(false)
	{
		for(int i = 0; i < getNumFaces(); i++)
		{
			DiagnosticEvent * pEvent = g_pTool->AddEvent();
			if(pEvent)
			{
				pEvent->SetObjectWithHighlightedTriangle(this, i);
				pEvent->SetText("After splitFaces - Face %d of %d", i+1, getNumFaces());
			}
		}
	}*/

	// Generate events here, so we can look at all the triangles.
	// Also, want to see how the number of faces grew to be soooo big

//	printf("\nNum events = %d\n", g_CSGDiagToolNumEventAdds);
	//printf("\nNum events = %d\n", g_pTool->GetNumEvents());

	// Sort the triangles based on height?
	// Heal the triangulation - will require new vertices
}



/**
 * Computes closest distance from a vertex to a plane
 * 
 * @param vertex vertex used to compute the distance
 * @param face face representing the plane where it is contained
 * @return the closest distance from the vertex to the plane
 */
double CSGShape::computeDistance(const CSGVertex &vertex, const CSGFace &face)
{
	CSGVector normal = face.G_Normal();
	double a = normal.x;
	double b = normal.y;
	double c = normal.z;
	double d = -(a*face.v1->x + b*face.v1->y + c*face.v1->z);
	return a*vertex.x + b*vertex.y + c*vertex.z + d;
}

// This just worked...
#if 1

/**
 * Split an individual face
 * 
 * @param facePos face position on the array of faces
 * @param segment1 segment representing the intersection of the face with the plane
 * of another face
 * @return segment2 segment representing the intersection of other face with the
 * plane of the current face plane
 */	  
void CSGShape::splitFace(int facePos, CSGSegment &segment1, CSGSegment &segment2)
{
	CSGVertex startPosVertex, endPosVertex;
	CSGVector startPos, endPos;
	int startType, endType, middleType;
	double startDist, endDist;
	
	CSGFace & face = G_face(facePos);
	CSGVertex *startVertex = segment1.getStartVertex();
	CSGVertex *endVertex = segment1.getEndVertex();
	
	//starting point: deeper starting point 		
	if (segment2.getStartDistance() > segment1.getStartDistance()+TOL)
	{
		startDist = segment2.getStartDistance();
		startType = segment1.getIntermediateType();
		startPos = segment2.getStartPosition();
	}
	else
	{
		startDist = segment1.getStartDistance();
		startType = segment1.getStartType();
		startPos = segment1.getStartPosition();
	}
	
	//ending point: deepest ending point
	if (segment2.getEndDistance() < segment1.getEndDistance()-TOL)
	{
		endDist = segment2.getEndDistance();
		endType = segment1.getIntermediateType();
		endPos = segment2.getEndPosition();
	}
	else
	{
		endDist = segment1.getEndDistance();
		endType = segment1.getEndType();
		endPos = segment1.getEndPosition();
	}		
	middleType = segment1.getIntermediateType();
	
	//set vertex to BOUNDARY if it is start type		
	if (startType == CSGSegment::VERTEX)
	{
		startVertex->Set_Status(CSGVertex::BOUNDARY);
	}
			
	//set vertex to BOUNDARY if it is end type
	if (endType == CSGSegment::VERTEX)
	{
		endVertex->Set_Status(CSGVertex::BOUNDARY);
	}
	
	//VERTEX-_______-VERTEX 
	if (startType == CSGSegment::VERTEX && endType == CSGSegment::VERTEX)
	{
		return;
	}
	
	//______-EDGE-______
	else if (middleType == CSGSegment::EDGE)
	{
		//gets the edge 
		int splitEdge;
		if ((startVertex == face.v1 && endVertex == face.v2) || (startVertex == face.v2 && endVertex == face.v1))
		{
			splitEdge = 1;
		}
		else if ((startVertex == face.v2 && endVertex == face.v3) || (startVertex == face.v3 && endVertex == face.v2))
		{	  
			splitEdge = 2; 
		} 
		else
		{
			splitEdge = 3;
		} 
		
		//VERTEX-EDGE-EDGE
		if (startType == CSGSegment::VERTEX)
		{
			breakFaceInTwo(facePos, endPos, splitEdge);
			return;
		}
		
		//EDGE-EDGE-VERTEX
		else if (endType == CSGSegment::VERTEX)
		{
			breakFaceInTwo(facePos, startPos, splitEdge);
			return;
		}
    
		// EDGE-EDGE-EDGE
		else if (startDist == endDist)
		{
			breakFaceInTwo(facePos, endPos, splitEdge);
		}
		else
		{
			if((startVertex == face.v1 && endVertex == face.v2) || (startVertex == face.v2 && endVertex == face.v3) || (startVertex == face.v3 && endVertex == face.v1))
			{
				breakFaceInThree(facePos, startPos, endPos, splitEdge);
			}
			else
			{
				breakFaceInThree(facePos, endPos, startPos, splitEdge);
			}
		}
		return;
	}
	
	//______-FACE-______
	
	//VERTEX-FACE-EDGE
	else if (startType == CSGSegment::VERTEX && endType == CSGSegment::EDGE)
	{
		breakFaceInTwo(facePos, endPos, *endVertex);
	}
	//EDGE-FACE-VERTEX
	else if (startType == CSGSegment::EDGE && endType == CSGSegment::VERTEX)
	{
		breakFaceInTwo(facePos, startPos, *startVertex);
	}
	//VERTEX-FACE-FACE
	else if (startType == CSGSegment::VERTEX && endType == CSGSegment::FACE)
	{
		breakFaceInThree(facePos, endPos, *startVertex);
	}
	//FACE-FACE-VERTEX
	else if (startType == CSGSegment::FACE && endType == CSGSegment::VERTEX)
	{
		breakFaceInThree(facePos, startPos, *endVertex);
	}
	//EDGE-FACE-EDGE
	else if (startType == CSGSegment::EDGE && endType == CSGSegment::EDGE)
	{
		breakFaceInThree(facePos, startPos, endPos, *startVertex, *endVertex);
	}
	//EDGE-FACE-FACE
	else if (startType == CSGSegment::EDGE && endType == CSGSegment::FACE)
	{
		breakFaceInFour(facePos, startPos, endPos, *startVertex);
	}
	//FACE-FACE-EDGE
	else if (startType == CSGSegment::FACE && endType == CSGSegment::EDGE)
	{
		breakFaceInFour(facePos, endPos, startPos, *endVertex);
	}
	//FACE-FACE-FACE
	else if (startType == CSGSegment::FACE && endType == CSGSegment::FACE)
	{
		CSGVector segmentVector(startPos.x-endPos.x, startPos.y-endPos.y, startPos.z-endPos.z);
					
		//if the intersection segment is a point only...
		if (fabs(segmentVector.x)<TOL && fabs(segmentVector.y)<TOL && fabs(segmentVector.z)<TOL)
		{
			breakFaceInThree(facePos, startPos);
			return;
		}
		
		//gets the vertex more lined with the intersection segment
		int linedVertex;
		CSGVector linedVertexPos;
		CSGVector vertexVector(endPos.x-face.v1->x, endPos.y-face.v1->y, endPos.z-face.v1->z);
		vertexVector.Normalise();
		//double dot1 = fabs(segmentVector.dot(vertexVector));
		double dot1 = fabs(CSGVector::dotprod(segmentVector,vertexVector));
		vertexVector = CSGVector(endPos.x-face.v2->x, endPos.y-face.v2->y, endPos.z-face.v2->z);
		vertexVector.Normalise();
		//double dot2 = fabs(segmentVector.dot(vertexVector));
		double dot2 = fabs(CSGVector::dotprod(segmentVector,vertexVector));
		vertexVector = CSGVector(endPos.x-face.v3->x, endPos.y-face.v3->y, endPos.z-face.v3->z);
		vertexVector.Normalise();
		//double dot3 = fabs(segmentVector.dot(vertexVector));
		double dot3 = fabs(CSGVector::dotprod(segmentVector,vertexVector));
		if (dot1 > dot2 && dot1 > dot3)
		{
		 	linedVertex = 1;
			linedVertexPos = face.v1->G_Position();
		}
		else if (dot2 > dot3 && dot2 > dot1)
		{
			linedVertex = 2;
			linedVertexPos = face.v2->G_Position();
		}
		else
		{
			linedVertex = 3;
			linedVertexPos = face.v3->G_Position();
		}
    
		// Now find which of the intersection endpoints is nearest to that vertex.
		if ((linedVertexPos - startPos).G_size() > (linedVertexPos - endPos).G_size())
		{
			breakFaceInFive(facePos, startPos, endPos, linedVertex);
		}
		else
		{
			breakFaceInFive(facePos, endPos, startPos, linedVertex);
		}
	}
}

#endif

/**
 * Face breaker for VERTEX-EDGE-EDGE / EDGE-EDGE-VERTEX
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 * @param edge that will be split 
 */		
void CSGShape::breakFaceInTwo(int facePos, const CSGVector & newPos, int splitEdge)
{
	CSGFace face = faces[facePos];
	//faces->RemoveFace(facePos);
	
	CSGVertex * vertex = addVertex(newPos, CSGVertex::BOUNDARY);
	CSGVector nn=face.InterpolNormal(newPos);

	if (splitEdge == 1)
	{
		addFace(face.v1, vertex, face.v3, face.n1, nn, face.n3, face.label);
		addFace(vertex, face.v2, face.v3, nn, face.n2, face.n3, face.label);
	}
	else if (splitEdge == 2)
	{
		addFace(face.v2, vertex, face.v1, face.n2, nn, face.n1, face.label);
		addFace(vertex, face.v3, face.v1, nn, face.n3, face.n1, face.label);
	}
	else
	{
		addFace(face.v3, vertex, face.v2, face.n3, nn, face.n2, face.label);
		addFace(vertex, face.v1, face.v2, nn, face.n1, face.n2, face.label);
	}

	faces.del(facePos);
}

/**
 * Face breaker for VERTEX-FACE-EDGE / EDGE-FACE-VERTEX
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 * @param endVertex vertex used for splitting 
 */		
void CSGShape::breakFaceInTwo(int facePos, const CSGVector & newPos, CSGVertex & endVertex)
{
	CSGFace face = faces[facePos];
	// should I try putting it back???

	//faces->RemoveFace(facePos);

	
	CSGVertex * vertex = addVertex(newPos, CSGVertex::BOUNDARY);
	CSGVector nn=face.InterpolNormal(newPos);
				
	if (endVertex.equals(face.v1))
	{
		addFace(face.v1, vertex, face.v3, face.n1, nn, face.n3, face.label);
		addFace(vertex, face.v2, face.v3, nn, face.n2, face.n3, face.label);
	}
	else if (endVertex.equals(face.v2))
	{
		addFace(face.v2, vertex, face.v1, face.n2, nn, face.n1, face.label);
		addFace(vertex, face.v3, face.v1, nn, face.n3, face.n1, face.label);
	}
	else
	{
		addFace(face.v3, vertex, face.v2, face.n3, nn, face.n2, face.label);
		addFace(vertex, face.v1, face.v2, nn, face.n1, face.n2, face.label);
	}

	faces.del(facePos);
}

/**
 * Face breaker for EDGE-EDGE-EDGE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param splitEdge edge that will be split
 */
void CSGShape::breakFaceInThree(int facePos, const CSGVector & newPos1, const CSGVector & newPos2, int splitEdge)
{
	CSGFace face = faces[facePos];
	// Moved remove to the end
	//faces->remove(facePos);
	
	CSGVertex * vertex1 = addVertex(newPos1, CSGVertex::BOUNDARY);	
	CSGVertex * vertex2 = addVertex(newPos2, CSGVertex::BOUNDARY);
	CSGVector nn1=face.InterpolNormal(newPos1);
	CSGVector nn2=face.InterpolNormal(newPos2);

	if (splitEdge == 1)
	{
		addFace(face.v1, vertex1, face.v3, face.n1, nn1, face.n3, face.label);
		addFace(vertex1, vertex2, face.v3, nn1, nn2, face.n3, face.label);
		addFace(vertex2, face.v2, face.v3, nn2, face.n2, face.n3, face.label);
	}
	else if (splitEdge == 2)
	{
		addFace(face.v2, vertex1, face.v1, face.n2, nn1, face.n1, face.label);
		addFace(vertex1, vertex2, face.v1, nn1, nn2, face.n1, face.label);
		addFace(vertex2, face.v3, face.v1, nn2, face.n3, face.n1, face.label);
	}
	else
	{
		addFace(face.v3, vertex1, face.v2, face.n3, nn1, face.n2, face.label);
		addFace(vertex1, vertex2, face.v2, nn1, nn2, face.n2, face.label);
		addFace(vertex2, face.v1, face.v2, nn2, face.n1, face.n2, face.label);
	}

	// Moved this
	faces.del(facePos);
}

/**
 * Face breaker for VERTEX-FACE-FACE / FACE-FACE-VERTEX
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 * @param endVertex vertex used for the split
 */
void CSGShape::breakFaceInThree(int facePos, const CSGVector & newPos, CSGVertex & endVertex)
{
	CSGFace face = faces[facePos];
	// Moved remove to the end
	//faces->remove(facePos);
	
	CSGVertex * vertex = addVertex(newPos, CSGVertex::BOUNDARY);
	CSGVector nn=face.InterpolNormal(newPos);

	if (endVertex.equals(face.v1))
	{
		addFace(face.v1, face.v2, vertex, face.n1, face.n2, nn, face.label);
		addFace(face.v2, face.v3, vertex, face.n2, face.n3, nn, face.label);
		addFace(face.v3, face.v1, vertex, face.n3, face.n1, nn, face.label);
	}
	else if (endVertex.equals(face.v2))
	{
		addFace(face.v2, face.v3, vertex, face.n2, face.n3, nn, face.label);
		addFace(face.v3, face.v1, vertex, face.n3, face.n1, nn, face.label);
		addFace(face.v1, face.v2, vertex, face.n1, face.n2, nn, face.label);
	}
	else
	{
		addFace(face.v3, face.v1, vertex, face.n3, face.n1, nn, face.label);
		addFace(face.v1, face.v2, vertex, face.n1, face.n2, nn, face.label);
		addFace(face.v2, face.v3, vertex, face.n2, face.n3, nn, face.label);
	}

	// Moved this
	faces.del(facePos);
}

// General translation rules:
// Use references in function parameters as opposed to pointers
// Would use pointers everywhere, but access is through dot operator
// 

/**
 * Face breaker for EDGE-FACE-EDGE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param startVertex vertex used the new faces creation
 * @param endVertex vertex used for the new faces creation
 */
void CSGShape::breakFaceInThree(int facePos, const CSGVector & newPos1, const CSGVector & newPos2, CSGVertex & startVertex, CSGVertex & endVertex)
{
	CSGFace face = faces[facePos];
	// Moved remove to the end
	//faces->remove(facePos);
	
	CSGVertex * vertex1 = addVertex(newPos1, CSGVertex::BOUNDARY);
	CSGVertex * vertex2 = addVertex(newPos2, CSGVertex::BOUNDARY);
	CSGVector nn1=face.InterpolNormal(newPos1);
	CSGVector nn2=face.InterpolNormal(newPos2);
					
	if (startVertex.equals(face.v1) && endVertex.equals(face.v2))
	{
		addFace(face.v1, vertex1, vertex2, face.n1, nn1, nn2, face.label);
		addFace(face.v1, vertex2, face.v3, face.n1, nn2, face.n3, face.label);
		addFace(vertex1, face.v2, vertex2, nn1, face.n2, nn2, face.label);
	}
	else if (startVertex.equals(face.v2) && endVertex.equals(face.v1))
	{
		addFace(face.v1, vertex2, vertex1, face.n1, nn2, nn1, face.label);
		addFace(face.v1, vertex1, face.v3, face.n1, nn1, face.n3, face.label);
		addFace(vertex2, face.v2, vertex1, nn2, face.n2, nn1, face.label);
	}
	else if (startVertex.equals(face.v2) && endVertex.equals(face.v3))
	{
		addFace(face.v2, vertex1, vertex2, face.n2, nn1, nn2, face.label);
		addFace(face.v2, vertex2, face.v1, face.n2, nn2, face.n1, face.label);
		addFace(vertex1, face.v3, vertex2, nn1, face.n3, nn2, face.label);
	}
	else if (startVertex.equals(face.v3) && endVertex.equals(face.v2))
	{
		addFace(face.v2, vertex2, vertex1, face.n2, nn2, nn1, face.label);
		addFace(face.v2, vertex1, face.v1, face.n2, nn1, face.n1, face.label);
		addFace(vertex2, face.v3, vertex1, nn2, face.n3, nn1, face.label);
	}
	else if (startVertex.equals(face.v3) && endVertex.equals(face.v1))
	{
		addFace(face.v3, vertex1, vertex2, face.n3, nn1, nn2, face.label);
		addFace(face.v3, vertex2, face.v2, face.n3, nn2, face.n2, face.label);
		addFace(vertex1, face.v1, vertex2, nn1, face.n1, nn2, face.label);
	}
	else
	{
		addFace(face.v3, vertex2, vertex1, face.n3, nn2, nn1, face.label);
		addFace(face.v3, vertex1, face.v2, face.n3, nn1, face.n2, face.label);
		addFace(vertex2, face.v1, vertex1, nn2, face.n1, nn1, face.label);
	}

	// Moved this
	faces.del(facePos);
}

/**
 * Face breaker for FACE-FACE-FACE (a point only)
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 */
void CSGShape::breakFaceInThree(int facePos, const CSGVector & newPos)
{
	CSGFace face = faces[facePos];
	// Moved remove to the end
	//faces->remove(facePos);
	
	CSGVertex * vertex = addVertex(newPos, CSGVertex::BOUNDARY);
	CSGVector nn=face.InterpolNormal(newPos);
		
	addFace(face.v1, face.v2, vertex, face.n1, face.n2, nn, face.label);
	addFace(face.v2, face.v3, vertex, face.n2, face.n3, nn, face.label);
	addFace(face.v3, face.v1, vertex, face.n3, face.n1, nn, face.label);

	// Moved this
	faces.del(facePos);
}

/**
 * Face breaker for EDGE-FACE-FACE / FACE-FACE-EDGE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param endVertex vertex used for the split
 */	
void CSGShape::breakFaceInFour(int facePos, const CSGVector & newPos1, const CSGVector & newPos2, CSGVertex & endVertex)
{
	CSGFace face = faces[facePos];
	// Moved remove to the end
	//faces->remove(facePos);
	
	CSGVertex * vertex1 = addVertex(newPos1, CSGVertex::BOUNDARY);
	CSGVertex * vertex2 = addVertex(newPos2, CSGVertex::BOUNDARY);
	CSGVector nn1=face.InterpolNormal(newPos1);
	CSGVector nn2=face.InterpolNormal(newPos2);
	
	if (endVertex.equals(face.v1))
	{
		addFace(face.v1, vertex1, vertex2, face.n1, nn1, nn2, face.label);
		addFace(vertex1, face.v2, vertex2, nn1, face.n2, nn2, face.label);
		addFace(face.v2, face.v3, vertex2, face.n2, face.n3, nn2, face.label);
		addFace(face.v3, face.v1, vertex2, face.n3, face.n1, nn2, face.label);
	}
	else if (endVertex.equals(face.v2))
	{
		addFace(face.v2, vertex1, vertex2, face.n2, nn1, nn2, face.label);
		addFace(vertex1, face.v3, vertex2, nn1, face.n3, nn2, face.label);
		addFace(face.v3, face.v1, vertex2, face.n3, face.n1, nn2, face.label);
		addFace(face.v1, face.v2, vertex2, face.n1, face.n2, nn2, face.label);
	}
	else
	{
		addFace(face.v3, vertex1, vertex2, face.n3, nn1, nn2, face.label);
		addFace(vertex1, face.v1, vertex2, nn1, face.n1, nn2, face.label);
		addFace(face.v1, face.v2, vertex2, face.n1, face.n2, nn2, face.label);
		addFace(face.v2, face.v3, vertex2, face.n2, face.n3, nn2, face.label);
	}

	// Moved this
	faces.del(facePos);
}

/**
 * Face breaker for FACE-FACE-FACE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param linedVertex what vertex is more lined with the interersection found
 */		
void CSGShape::breakFaceInFive(int facePos, const CSGVector & newPos1, const CSGVector & newPos2, int linedVertex)
{
	CSGFace face = faces[facePos];
	// In the original java code, remove was being called here, it has been moved to the end of the function.
	//faces->remove(facePos);
	
	CSGVertex * vertex1 = addVertex(newPos1, CSGVertex::BOUNDARY);
	CSGVertex * vertex2 = addVertex(newPos2, CSGVertex::BOUNDARY);
	CSGVector nn1=face.InterpolNormal(newPos1);
	CSGVector nn2=face.InterpolNormal(newPos2);

	double cont = 0;
	if (linedVertex == 1)
	{
		addFace(face.v2, face.v3, vertex1, face.n2, face.n3, nn1, face.label);
		addFace(face.v2, vertex1, vertex2, face.n2, nn1, nn2, face.label);
		addFace(face.v3, vertex2, vertex1, face.n3, nn2, nn1, face.label);
		addFace(face.v2, vertex2, face.v1, face.n2, nn2, face.n1, face.label);
		addFace(face.v3, face.v1, vertex2, face.n3, face.n1, nn2, face.label);
	}
	else if(linedVertex == 2)
	{
		addFace(face.v3, face.v1, vertex1, face.n3, face.n1, nn1, face.label);
		addFace(face.v3, vertex1, vertex2, face.n3, nn1, nn2, face.label);
		addFace(face.v1, vertex2, vertex1, face.n1, nn2, nn1, face.label);
		addFace(face.v3, vertex2, face.v2, face.n3, nn2, face.n2, face.label);
		addFace(face.v1, face.v2, vertex2, face.n1, face.n2, nn2, face.label);
	}
	else
	{
		addFace(face.v1, face.v2, vertex1, face.n1, face.n2, nn1, face.label);
		addFace(face.v1, vertex1, vertex2, face.n1, nn1, nn2, face.label);
		addFace(face.v2, vertex2, vertex1, face.n2, nn2, nn1, face.label);
		addFace(face.v1, vertex2, face.v3, face.n1, nn2, face.n3, face.label);
		addFace(face.v2, face.v3, vertex2, face.n2, face.n3, nn2, face.label);
	}

	// Calling it at the end, because the java garbage collection won't destroy it until after the end of the function,
	// (when there is nothing referencing it anymore)
	faces.del(facePos);
}

// lather, rinse repeat for the other functions.
// lets do one more, then leave it for today.


//-----------------------------------OTHERS-------------------------------------//

/**
 * Classify faces as being inside, outside or on boundary of other object
 * 
 * @param object object 3d used for the comparison
 */
void CSGShape::classifyFaces(CSGShape & object)
{
	//calculate adjacency information
	determine_vertexneighbors();
	
	//for each face
	for(int i=0;i<G_facecount();i++)
	{
		CSGFace & face = G_face(i);
		
		//if the face vertices aren't classified to make the simple classify
		if(face.simpleClassify()==false)
		{
			//makes the ray trace classification
			face.rayTraceClassify(object);
			
			//mark the vertices
			if(face.v1->G_Status()==CSGVertex::UNKNOWN) 
			{
				face.v1->mark(face.G_Status());
			}
			if(face.v2->G_Status()==CSGVertex::UNKNOWN) 
			{
				face.v2->mark(face.G_Status());
			}
			if(face.v3->G_Status()==CSGVertex::UNKNOWN) 
			{
				face.v3->mark(face.G_Status());
			}
		}
	}

	//forget neighbor vertex info
	forget_vertexneighbors();
}


/** Inverts faces classified as INSIDE, making its normals point outside. Usually
 *  used into the second solid when the difference is applied. */
void CSGShape::invertInsideFaces()
{
	CSGFace *face;
	for(int i=0;i<G_facecount();i++)
	{
		face = &G_face(i);
		if(face->G_Status()==CSGFace::INSIDE)
			face->invert();
	}
}



void CSGShape::accum(CSGShape & pObject, int faceStatus1, int faceStatus2)
{
	for(int i=0;i<pObject.G_facecount();i++)
	{
		CSGFace & face = pObject.G_face(i);

		//if the face status fits with the desired status...
		if(face.G_Status()==faceStatus1 || face.G_Status()==faceStatus2)
		{
			CSGVertex *nv1=addvertexifnew(*face.v1);
			CSGVertex *nv2=addvertexifnew(*face.v2);
			CSGVertex *nv3=addvertexifnew(*face.v3);
			nv1->status=CSGVertex::UNKNOWN;
			nv2->status=CSGVertex::UNKNOWN;
			nv3->status=CSGVertex::UNKNOWN;
			CSGFace *fc=addFace(nv1,nv2,nv3);
			if (fc!=NULL)
			{
				fc->label=face.label;
				fc->n1=face.n1;
				fc->n2=face.n2;
				fc->n3=face.n3;
			}
		}
		else
			int jj=0;
	}

	int iii=0;
}



/////////////////////////////////////////////////////////////////////
// CSGShapeCombiner
/////////////////////////////////////////////////////////////////////

CSGShapeCombiner::CSGShapeCombiner(const CSGShape &io1, const CSGShape &io2)
{
	m_pObject1 = io1;
	m_pObject2 = io2;

	m_pObject1.startfinder(m_pObject1.G_BBox());
	m_pObject2.startfinder(m_pObject2.G_BBox());

	//split the faces so that none of them intercepts each other
//	message(_text("Split faces"));
	m_pObject1.splitFaces(&m_pObject2);
	m_pObject2.splitFaces(&m_pObject1);
//	message(_text("Split faces completed"));

	//classify faces as being inside or outside the other solid
//	message(_text("Classify faces"));
	m_pObject1.classifyFaces(m_pObject2);
	m_pObject2.classifyFaces(m_pObject1);
//	message(_text("Classify faces completed"));
}




void CSGShapeCombiner::getUnion(CSGShape &output)
{
	compose(CSGFace::OUTSIDE, CSGFace::SAME, CSGFace::OUTSIDE,output);
}

void CSGShapeCombiner::getIntersection(CSGShape &output)
{
	return compose(CSGFace::INSIDE, CSGFace::SAME, CSGFace::INSIDE,output);
}

void CSGShapeCombiner::getDifference(CSGShape &output)
{
	m_pObject2.invertInsideFaces();
	compose(CSGFace::OUTSIDE, CSGFace::OPPOSITE, CSGFace::INSIDE,output);
	m_pObject2.invertInsideFaces();
}

void CSGShapeCombiner::compose(int faceStatus1, int faceStatus2, int faceStatus3, CSGShape &output) 
{
	CSGBBox ubbox;
	ubbox.add(m_pObject1.G_BBox());
	ubbox.add(m_pObject2.G_BBox());
	output.reset();
	output.startfinder(ubbox);
	output.accum(m_pObject1, faceStatus1, faceStatus2);
	output.accum(m_pObject2, faceStatus3, faceStatus3);
	output.stopfinder();
}



