/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2009/12/28 21:09:46 $                                                                 
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

#include <iostream>

#include <vector>
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"
#include "Quaternion.h"
#include "iException.h"

namespace Isis {
  /**
   * Constructs an empty quaternion
   */
  Quaternion::Quaternion () {
    p_quaternion.resize(4);

    for (int i = 0;  i < 4;  i++) {
      p_quaternion[i] = 0.;
    }
  }

  /**
   * Construct a Quaternion class from a matrix stored as a vector &lt;
   * double &gt; with 9 elements or from a quaternion stored as a vector
   * &lt; double &gt; with 4 elements
   * 
   * @param rotation   rotation defined as either a matrix or another quaternion
   *                            loaded as a vector
   */
  Quaternion::Quaternion ( const std::vector<double> rotation ) {
    p_quaternion.resize(4);
    Set( rotation );

  }

  //! Destroys the Quaternion object
  Quaternion::~Quaternion() {}

  /**
   * Sets the quaternion value
   * 
   * @param rotation   rotation defined as either a matrix or a set of 3 angles
   */
  void Quaternion::Set ( std::vector<double> rotation ) {

    if (rotation.size () == 9) {      // Matrix initialization
      m2q_c ( &rotation[0], &p_quaternion[0]);
    }
    else if (rotation.size() == 4) {  //quaternion initialization
      p_quaternion = rotation;
    }
    else {
      std::string msg = "Input vector of unexpected size for matrix or quaternion";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    
  }


  //! Converts quaternion to 3x3 rotational matrix
  std::vector<double> Quaternion::ToMatrix () {
    std::vector<double> matrix(9);
	  q2m_c ( &p_quaternion[0], (SpiceDouble (*)[3]) &matrix[0]);
		return matrix;
  }


  /**
   * Assign value of quaternion class to another quaternion.  For 
   * example, 
   * @code
   *  Quaternion q1( matrix );
   *  Quaternion q2();
   *  ...
   *  q2 = q1;
   * @endcode 
   *  
   * @param quat The Quaternion to copy
   *
   */
  Quaternion& Quaternion::operator=(const Quaternion &quat) { 
    p_quaternion = quat.p_quaternion;
    return *this;
  }


  /**
   * Multiply current Naif quaternion by another Naif quaternion, replacing the 
   * current quaternion.  For example, 
   * @code
   *  Quaternion q1(),q(2);
   *  ...
   *  Quaternion q2() *= q1;
   * @endcode
   * More information on quaternions and the multiplication algorithm is
   * available in the Naif routine qxq_c.
   * 
   * @param[in]   quat (const Quaternion &) quaternion to multiply on the right
   * 
   * @return (Quaternion)     product of quaternions
   *
   */
  Quaternion& Quaternion::operator*=(const Quaternion &quat ) {
    std::vector<double> qout(4);

    qxq_c( (SpiceDouble *) &(this->p_quaternion[0]),
           (SpiceDouble *) &(quat.p_quaternion[0]),
           (SpiceDouble *) &(qout[0]) );
    this->p_quaternion[0] = qout[0];
    this->p_quaternion[1] = qout[1];
    this->p_quaternion[2] = qout[2];
    this->p_quaternion[3] = qout[3];
    return *this;
  }


  /**
   * Multiply two Naif quaternions to create a new quaternion.  For example, 
   * @code
   *  Quaternion q1(),q(2);
   *  ...
   *  Quaternion q3() = q1*q2;
   * @endcode
   * More information is available on quaternions and the multiplication 
   * algorithm in the Naif routine qxq_c.c
   * 
   * @param[in]   quat (const Quaternion &) quaternion to multiply on the
   *       right
   * 
   * @return (Quaternion)     product of quaternions
   *
   */
  Quaternion Quaternion::operator*(const Quaternion &quat ) const {
    Isis::Quaternion qout = *this;
    qout *= quat;

    return qout;
   }



  /**
   * Multiply a quaternion by a scalar.  Just multiply the rotation part and
   * polish the resulting quaternion so it is still a unit quaternion with 
   * positive rotation.  For example, 
   * @code
   *  Quaternion q1(),q(2);
   *  double scalar
   *  ...
   *  q2() = scalar*q2;
   * @endcode
   * 
   * @param[in]   scalar (const double &) scalar value to be multiplied times
   *                                    the current quaternion
   * @return (Quaternion) product of scalar and quaternion
   *
   */
  Quaternion Quaternion::operator*(const double &scalar) {
    Isis::Quaternion qout = *this;	

    double scalar2 = scalar*scalar;
    double unitizer =  1 + qout.p_quaternion[0]*qout.p_quaternion[0]*(scalar2 - 1);

    if (unitizer > 0. ) {
      unitizer  =  sqrt( unitizer );
    }
    else {
      std::string msg = "Unable to make quaternion a unit quaternion";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    qout.p_quaternion[0] = (qout.p_quaternion[0] * scalar)/unitizer;

    for (int i = 1;  i < 4;  i++) {
      qout.p_quaternion[i] /= unitizer;
    }
    Polish (qout); 

    return qout;
  }

  //! Returns the conjugate of the quaternion
   Quaternion Quaternion::Conjugate() {
     Quaternion qout;
     qout.p_quaternion[0] = p_quaternion[0];

     for ( int i = 1;  i < 4;  i++) {
       qout.p_quaternion[i] = -p_quaternion[i];
     }
     return qout;
   }



  /**
   * Multiply a vector by a quaternion (rotate the vector)
   * 
   * @param [in] vin (const std::vector<double>(3)) Vector to be multiplied
   *                                                (rotated)
   */
	std::vector<double> Quaternion::Qxv ( const std::vector<double> &vin ) {
    if (vin.size() != 3) {
      std::string msg = "Unexpected vector size -- 3 expected";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    Quaternion qvin;
    qvin.p_quaternion[0] = 0.;
    qvin.p_quaternion[1] = vin[0];
    qvin.p_quaternion[2] = vin[1];
    qvin.p_quaternion[3] = vin[2];

    Quaternion qvout(p_quaternion);
    Quaternion conj(p_quaternion);
    qvout *= qvin;
    qvout *= conj.Conjugate();
    std::vector<double> vout(qvout.p_quaternion.begin()+1,qvout.p_quaternion.end());
    
    return vout;
  }


  /**
   * Polish the quaternion -- make the first component positive
   * 
   */

  void Quaternion::Polish ( Quaternion &quat ) {

    if ( quat.p_quaternion[0] < 0) {
      quat.p_quaternion[0] = -quat.p_quaternion[0];
      quat.p_quaternion[1] = -quat.p_quaternion[1];
      quat.p_quaternion[2] = -quat.p_quaternion[2];
      quat.p_quaternion[3] = -quat.p_quaternion[3];
    }

  }



  /** 
    * Return the camera angles (right ascension, declination, and twist) for
    * the quaternion
    *
    */
  std::vector<double> Quaternion::ToAngles( int axis3, int axis2, int axis1 ) {
    std::vector<double> rotationMatrix = ToMatrix ();
    SpiceDouble ang1, ang2, ang3;
    m2eul_c( (SpiceDouble *) &rotationMatrix[0], axis3, axis2, axis1,
             &ang3, &ang2, &ang1);
    std::vector<double> angles;
    angles.push_back(ang1);
    angles.push_back(ang2);
    angles.push_back(ang3);
    return angles;
  }

}
 // end namespace isis
