// forio.h  iostream manipulators for generic fortran io
//           
//#include "strstrea.h"

#ifndef __FORIO__
#define __FORIO__

#include "stdafx.h" //added 3/17/00
#include "strstrea.h" //moved here 10/3/98
 
class FBOOL {
  BOOL value ;          
public:
  operator BOOL() const ;  // FBOOL to BOOL 
  FBOOL() ;                
  FBOOL( BOOL ) ;          // BOOL to FBOOL 
} ;



//istream& operator>> ( istream& is, BOOL& b ) ;
istream& operator>> ( istream& is, FBOOL& b ) ;
istream& ofc( istream& is ) ; 
istream& feol( istream& is ) ; 
ostream& ofc( ostream& os ) ; 
ostream& operator<< ( ostream& os, FBOOL& b ) ;

#endif

