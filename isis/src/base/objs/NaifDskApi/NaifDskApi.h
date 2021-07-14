#ifndef NaifDskApi_h
#define NaifDskApi_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
