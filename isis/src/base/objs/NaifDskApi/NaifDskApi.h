#ifndef NaifDskApi_h
#define NaifDskApi_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/09 18:49:25 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

//  These are the current set of NAIF DSK include files.  Note that the 
//  extern "C" wrapper is required as those files are not yet C++ ready.

#include <QDebug>

#include <SpiceUsr.h>
#include <SpiceZfc.h>

// Specs for convenient NAIF vectors and matrices
#include <tnt/tnt_array1d.h>
#include <tnt/tnt_array1d_utils.h>
#include <tnt/tnt_array2d.h>
#include <tnt/tnt_array2d_utils.h>

namespace Isis {

  /**
   * @brief Namespace to contain type definitions of NAIF DSK fundamentals 
   *  
   * This NAIF DSK utility provides the means to a TIN.  Needed are types that 
   * define efficient implemenations of vectors, vertices and triangles (plates). 
   * The TNT library is used to specify these fundamental types due to its 
   * efficeint passing and memory manaegement mechanisms.
   * 
   * @author 2013-12-05 Kris Becker 
   * @internal 
   *   @history 2013-12-05 Kris Becker  Original Version 
   *   @history 2015-03-08 Jeannie Backer - Moved implementation of validate() methods to cpp file.
   *                           Added qdebug formatters for typedefs so that they can be easily
   *                           printed in unitTests. Added class to ISIS trunk. References #2035
   *   @history 2017-06-28 Kris Becker - Updated DSK includes for NAIF N0066 release that now
   *                           includes the DSK formally. The includes are now all in SpiceUsr.h.
   *                           Removed SPICE includes from the cpp file as well. Fixes #4947.
   */
  
  // Basic type definitions
  typedef TNT::Array1D<SpiceDouble> NaifVector;       //!<  1-D Buffer[3]
  typedef TNT::Array1D<SpiceDouble> NaifVertex;       //!<  1-D Buffer[3]
  typedef TNT::Array2D<SpiceDouble> NaifTriangle;     //!<  3-D triangle[3][3]

  // Provide (dimensionality) validation routines
  bool validate(const NaifVertex &v);
  bool validate(const NaifTriangle &t);

  QDebug operator<<(QDebug dbg, const TNT::Array1D<SpiceDouble> &tntArray);
  QDebug operator<<(QDebug dbg, const TNT::Array2D<SpiceDouble> &tntMatrix);

  QString toString(const TNT::Array1D<SpiceDouble> &tntArray, int precision=15);

};  // namespace Isis

#endif
