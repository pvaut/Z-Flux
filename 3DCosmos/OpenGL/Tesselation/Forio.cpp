#include "stdafx.h"
#include "iostream.h"
#include "ctype.h"  
#include "forio.h"                        

FBOOL::operator BOOL() const { return value ; }
FBOOL::FBOOL( BOOL b ) { value = b ; }
FBOOL::FBOOL() { value = FALSE ; }
    
istream& operator>> ( istream& is, FBOOL& b ) {
  is >> ws ;          
  if ( ( is.peek() == 'f' ) || ( is.peek() == 'F' ) ) b = FALSE ;
  if ( ( is.peek() == 't' ) || ( is.peek() == 'T' ) ) b = TRUE ;
  while ( isalpha( is.peek() ) ) { is.get() ; } 
  return is ;
}

istream& ofc( istream& is ) 
{ 
	//9/23/00: handles space-delimited as well as comma-delimited values
	is >> ws ; //1/19/01 the 'ws' is a istream function that eats white space
	if ( is.peek() == ',' ) is.get() ;
	return is ;
}


ostream& ofc( ostream& os ) { 
  os << ", " ;
  return os ;
}

istream& feol( istream& is ) 
{
  //Extracts from stream until \r, \n, or EOF is encountered.
	//Then it extracts the '\n' or '\r' (but not an EOF).
  while( ( is.peek() != '\n' ) && 
  			 ( is.peek() != '\r' ) && 
  			 ( is.peek() != EOF ) ) is.get() ;
  while( is.peek() == '\n' || is.peek() == '\r' ) is.get() ;
  return is ;
}

ostream& operator<< ( ostream& os, FBOOL& b ) {
  if ( b ) {
    os << "T " ;
  } else {
    os << "F " ;
  }  
  return os ;
}

