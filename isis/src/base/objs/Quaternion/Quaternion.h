#ifndef Quaternion_h
#define Quaternion_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2009/12/28 19:16:01 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include <string>
#include <vector>
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

namespace Isis {
  /**
   * @brief Provide operations for quaternion arithmetic
   * 
   * This class provides a wrapper for existing Naif quaternion functions and
   * also includes other operators and methods useful for working with 
   * quaternions.
   * 
   * @ingroup Rotations                                       
   *                                                    
   * @author 2005-12-07 Debbie A. Cook                    
   *
   * @internal 
   *   @history Steven Lambright Fixed documentation 
   *      
   */

  class Quaternion {
    public:
    // constructors
      Quaternion ();
      Quaternion ( const std::vector<double> matrix );

    // destructor
      ~Quaternion();

    //Methods

      std::vector<double> ToMatrix ();
	
      std::vector<double> ToAngles( int axis3, int axis2, int axis1 );

      void Set ( std::vector<double> );

      //! Return the quaternion as a vector
      std::vector<double> GetQuaternion() const { return p_quaternion; }
      ;
      Quaternion& operator=(const Quaternion &quat);
 
      Quaternion& operator*=(const Quaternion &quat ); 

      Quaternion operator*(const Quaternion &quat ) const; 
 
      Quaternion operator*(const double &scalar);

  /**
   * Return a member of a quaternion.  For example, 
   * @code
   *  Quaternion q();
   * ...
   *  double angle = q[0];
   * @endcode
   * 
   * @param[in] index (const int &) quaternion member to return
   * 
   * @return (double&) value pointed to by iter
   * 
   */
      double& operator[]( int index) {return p_quaternion.at(index); };



      std::vector<double> Qxv ( const std::vector<double> &vin );

      Quaternion Conjugate();


  

   private:
      std::vector<double> p_quaternion;     //!< Quaternion
      void Polish( Quaternion &quat ); 
  };
};

#endif

