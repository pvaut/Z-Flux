#pragma once

#include "tobject.h"
#include "valnames.h"

#include "CSG_components.h"

#include "CSG_face.h"




class QBinTagWriter;
class QBinTagReader;


class CSGFace;
class CSGSegment;

class Taffinetransformation;
class T2DContourset;
class Tspacewarper;
class TSC_functor;
class TBlob;


typedef Tcopyarray<CSGVertex> TVertexFinderCell;

class CSGVertexFinder
{
private:
	int count;//number of cells per dimension
	CSGBBox bbox;
	Tarray<TVertexFinderCell> cells;
	bool isactive;
public:
	CSGVertexFinder();
	bool G_isactive() const { return isactive; }
	void reset();
	void start(const CSGBBox &ibbox);
	void addvertex(CSGVertex *ivertex);
	void delvertex(CSGVertex *ivertex);
	int G_cellidx(double x, double y, double z) const;//returns the cell a point belongs to
	TVertexFinderCell& G_cell(int cellidx);
};

class CSGShape : public TObject
{
private:
	CSGVertexList vertices;
	Titemarray<CSGFace> faces;
	CSGBBox bbox;
	CSGVertexFinder finder;
public:
	static StrPtr GetClassName() { return SC_valname_shape; }
	CSGShape();
	CSGShape(const CSGShape &ishape);
	~CSGShape();

	void operator=(const CSGShape &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	static bool compare(CSGShape *v1, CSGShape *v2) { return false; }
	static bool comparesize(CSGShape *v1, CSGShape *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}

	void reset();

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);


	void copyfrom(const CSGShape &ishape);

	void addpolygon(Tcopyarray<Tvertex> &vertexlist, int ilabel);

	void createbar(const CSGVector &pos, const CSGVector &size);
	void createsphere(const CSGVector &cent, double radius, int resol);
	void createcylinder(const CSGVector &start, const CSGVector &axis, double radius , int resol);
	void create_extrudedpolygon(T2DContourset *contourset0, double height, int layercount);
	void create_revolvedpolygon(T2DContourset *contourset, int resolution);
	void create_conepolygon(T2DContourset *contourset,const Tvertex &top);

	void create_polyhedron(Tcopyarray<Tplane> &planelist, Tintarray &indexlist);

	void create_surface(TSC_functor *functor, double gridsize, double totalsize, Tvertex *ipt, bool dotet);

	void create_blob(const TBlob &blob, double gridsize);

	void SetFaceLabels(int ilabel);

	void calcfacenormals();

	void createflatnormals();

	void subsample(double maxdist);

	void smoothen(double maxdist, double alpha);


	void transform(const Taffinetransformation &tf);

	void warp(Tspacewarper &warper);

	bool Intersect(const Tvertex &raypos, const Tvector &raydir, double &t0, Tvector &normdir);



public:
	int findvertexnr(const CSGVector &pos);//returns the vertex at this position, -1 if no match

	void startfinder(const CSGBBox &ibbox);
	void stopfinder();

	CSGVertex* addvertex(const CSGVertex &ivertex);
	CSGVertex* addvertexifnew(const CSGVertex &ivertex);
	CSGVertex* addVertex(const CSGVector &ipos, int istatus);


	void delvertex(int nr);
	CSGFace* addFace(CSGVertex *v1, CSGVertex *v2, CSGVertex *v3);
	CSGFace* addFace(CSGVertex *v1, CSGVertex *v2, CSGVertex *v3, const CSGVector &in);
	CSGFace* addFace(CSGVertex *v1, CSGVertex *v2, CSGVertex *v3, const CSGVector &in1, const CSGVector &in2, const CSGVector &in3, int ilabel);

	void addFace(int vidx1, int vidx2, int vidx3);
	void addquad(int vidx1, int vidx2, int vidx3, int vidx4);

	int G_vertexcount() const { return vertices.G_count(); }
	const CSGVertex& G_vertex(int nr) const;
	int G_facecount() const { return faces.G_count(); }
	const CSGFace& G_face(int nr) const;
	CSGFace& G_face(int nr);



	CSGBBox& G_BBox() { return bbox; }

	void splitFaces(CSGShape *pObject);

//private:

	void determine_memberfaces();
	void clear_memberfaces();

	void determine_vertexneighbors();
	void forget_vertexneighbors();


	double computeDistance(const CSGVertex &vertex, const CSGFace &face);

	void splitFace(int facePos, CSGSegment &segment1, CSGSegment &segment2);
	  
	void breakFaceInTwo(int facePos, const CSGVector &newPos, int splitEdge);
	
	void breakFaceInTwo(int facePos, const CSGVector &newPos, CSGVertex &endVertex);
	
	void breakFaceInThree(int facePos, const CSGVector &newPos1, const CSGVector &newPos2, CSGVertex &startVertex, CSGVertex &endVertex);
	void breakFaceInThree(int facePos, const CSGVector &newPos1, const CSGVector &newPos2, int splitEdge);
	void breakFaceInThree(int facePos, const CSGVector &newPos, CSGVertex &endVertex);
	void breakFaceInThree(int facePos, const CSGVector &newPos);

	void breakFaceInFour(int facePos, const CSGVector &newPos1, const CSGVector &newPos2, CSGVertex &endVertex);
	
	void breakFaceInFive(int facePos, const CSGVector &newPos1, const CSGVector &newPos2, int linedVertex);
	
	void classifyFaces(CSGShape &pObject);
	
	void invertInsideFaces();

	void accum(CSGShape & pObject, int faceStatus1, int faceStatus2);

};



class CSGShapeCombiner
{
public:

	CSGShape m_pObject1;
	CSGShape m_pObject2;
	
	CSGShapeCombiner(const CSGShape &io1, const CSGShape &io2);

	void getUnion(CSGShape &output);
	void getIntersection(CSGShape &output);
	void getDifference(CSGShape &output);

private:
	
	void compose(int faceStatus1, int faceStatus2, int faceStatus3, CSGShape &output);
};

