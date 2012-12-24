#include "stdafx.h"
#include "math.h"                                         
#include "float.h"
#include "vector3d.h"
#include "iostream.h"
#include "forio.h"            

Vector3D::Vector3D() 
{
  x = 0.f ;
  y = 0.f ;
  z = 0.f ;
  
}
  
Vector3D::Vector3D( const float xx, const float yy, const float zz ) {
  x = xx ;
  y = yy ;
  z = zz ;
  
} 

void Vector3D::GetVector( float& xx, float& yy, float& zz ) {
  xx = x ;
  yy = y ;
  zz = z ;
}
  
  
void Vector3D::AnglesDeg( const float theta, const float phi ) 
{
	//spherical to rectangular conversion
	//X (or Z) vect = sin(th)cos(ph)x + sin(th)sin(ph)y + cos(th)z
	float sth = (float)sin( theta / 57.3 ) ;
	float cth = (float)cos( theta / 57.3 ) ;
	float sph = (float)sin( phi / 57.3 ) ;
	float cph = (float)cos( phi / 57.3 ) ;
	x = sth * cph ;
	y = sth * sph ;
	z = cth ; 
}
  
float Vector3D::Abs() {
  return (float)sqrt( x * x + y * y + z * z ) ;
} 

void Vector3D::Normalize() {
  float mag = Abs() ; 
  if ( mag != 0. ) {
    x = x / mag ;
    y = y / mag ;
    z = z / mag ;
  } else {
    x = 0.f ;
    y = 0.f ;
    z = 0.f ;
  }
}

Vector3D operator+( const Vector3D& a, const Vector3D& b ) {  
  return Vector3D( a.x + b.x, a.y + b.y, a.z + b.z ) ;
}

Vector3D operator+( const float a, const Vector3D& b ) {  
  return Vector3D( a + b.x, a + b.y, a + b.z ) ;
}

Vector3D operator+( const Vector3D& b, const float a ) {  
  return Vector3D( a + b.x, a + b.y, a + b.z ) ;
}

Vector3D operator-( const Vector3D& a, const Vector3D& b ) {  
  return Vector3D( a.x - b.x, a.y - b.y, a.z - b.z ) ;
}
Vector3D operator-( const float a, const Vector3D& b ) {  
  return Vector3D( a - b.x, a - b.y, a - b.z ) ;
}
Vector3D operator-( const Vector3D& a, const float b ) {  
  return Vector3D( a.x - b, a.y - b, a.z - b ) ;
}

Vector3D operator-( const Vector3D& a ) {  
  return Vector3D( -a.x, -a.y, -a.z ) ;
}

float operator*( const Vector3D& a, const Vector3D& b ) 
{
  //vector dot-product of a and b
  return a.x * b.x + a.y * b.y + a.z * b.z ;
}


Vector3D operator*( const float a, const Vector3D& b ) {  
  return Vector3D( a * b.x, a * b.y, a * b.z ) ;
}

Vector3D operator*( const Vector3D& b, const float a ) {  
  return Vector3D( a * b.x, a * b.y, a * b.z ) ;
}

Vector3D operator%( const Vector3D& a, const Vector3D& b ) 
{
  //vector cross-product a x b
  float xx, yy, zz ;
  xx = a.y * b.z - a.z * b.y ;
  yy = a.z * b.x - a.x * b.z ;
  zz = a.x * b.y - a.y * b.x ;
  return Vector3D( xx, yy, zz ) ;
}


Vector3D operator/( const Vector3D& a, const float b ) {  
  return Vector3D( a.x / b, a.y / b, a.z / b ) ;
}

istream& operator>> ( istream& is, Vector3D& v ) {
  is >> v.x >> ofc >> v.y >> ofc >> v.z ;
  return is ;
}

ostream& operator<< ( ostream& os, Vector3D& v ) {
  os << v.x << ofc << v.y << ofc << v.z ;   
  return os ;
}

CArchive& operator>> ( CArchive& ar, Vector3D& v ) {
  ar >> v.x >> v.y >> v.z ;
  return ar ;
}

CArchive& operator<< ( CArchive& ar, Vector3D& v ) {
  ar << v.x << v.y << v.z ;   
  return ar ;
} 

void Vector3D::Serialize( CArchive& ar) {
  if ( ar.IsStoring() ) {
    ar << *this ;
  } else {
    ar >> *this ;
  }
}

Vector3D Vector3D::Cart2Sphere()
{
	//Purpose: cartesion to spherical conversion
	//Provenance: Copyright 07/02/04 G. Frank Paynter
	//Inputs:
	//	v = Vector3D object in cartesion coordinates
	//Outputs:
	//	returns Vector3D object in spherical coordinates, where
	//	x = r (local units)
	//	y = theta (radians)
	//	z = phi (radians)

	double r = sqrt( x*x + y*y ) ;
	double theta = atan2( r, z ) ;
	double phi = atan2( y, x ) ;
	float R = Abs() ;

	return Vector3D( R, (float)theta, (float)phi ) ;
}

Vector3D Vector3D::Sphere2Cart()
{
	//Purpose: spherical to cartesion conversion
	//Provenance: Copyright 07/02/04 G. Frank Paynter
	//Inputs:
	//	v = Vector3D object in spherical coordinates, where
	//		x = r (local units)
	//		y = theta (radians)
	//		z = phi (radians)
	//Outputs:
	//	returns Vector3D object in cartesion coordinates

	float cartz = x*(float)cos( y ) ;
	float cartx = x*(float)( sin( y ) * cos( z ) ) ;
	float carty = x*(float)( sin( y ) * sin( z ) ) ;

	return Vector3D( cartx, carty, cartz ) ;
}
