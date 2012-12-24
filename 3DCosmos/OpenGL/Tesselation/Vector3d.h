
#ifndef __VECTOR3D__
#define __VECTOR3D__

#include "iostream.h"

class Vector3D {
public:
  float x ;
  float y ;
  float z ;
  
  Vector3D() ;  
  Vector3D( const float, const float, const float ) ;  
  void AnglesDeg( const float, const float ) ;
  void GetVector( float&, float&, float& ) ;  
  float Abs() ;   
  void Normalize() ; 
  
  // monadic negate
  friend Vector3D operator-( const Vector3D& ) ;    
             
  // add
  friend Vector3D operator+( const Vector3D&, const Vector3D& ) ;
  friend Vector3D operator+( const float, const Vector3D& ) ;
  friend Vector3D operator+( const Vector3D&, const float& ) ;
                
  // subtract
  friend Vector3D operator-( const Vector3D&, const Vector3D& ) ;
  friend Vector3D operator-( const float, const Vector3D& ) ;
  friend Vector3D operator-( const Vector3D&, const float ) ;
  
  // multiply (dot products and multiply by constants)
  friend float    operator*( const Vector3D&, const Vector3D& ) ;
  friend Vector3D operator*( const float, const Vector3D& ) ;
  friend Vector3D operator*( const Vector3D&, const float ) ;
             
  // divide           
  friend Vector3D operator/( const Vector3D&, const float ) ;
  
  // cross product
  friend Vector3D operator%( const Vector3D&, const Vector3D& ) ;
                                                     
  // stream input/output
  friend istream& operator>> ( istream&, Vector3D& ) ;
  friend ostream& operator<< ( ostream&, Vector3D& ) ;

  friend CArchive& operator>> ( CArchive&, Vector3D& ) ;
  friend CArchive& operator<< ( CArchive&, Vector3D& ) ;
  void Serialize( CArchive& ) ;
  
  Vector3D Cart2Sphere() ;
  Vector3D Sphere2Cart() ;
  
} ;              


#endif 
