/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
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
#include "NaifDskApi.h"

// Specs for convenient NAIF vectors and matrices
#include <tnt/tnt_array1d.h>
#include <tnt/tnt_array1d_utils.h>
#include <tnt/tnt_array2d.h>
#include <tnt/tnt_array2d_utils.h>

#include "IString.h"

namespace Isis {
  /** 
   *  Verifies that the given NaifVector or NaifVertex is 3 dimensional.
   *  
   *  Also acceptable:
   *     bool validate(const NaifVector &v).
   *  
   * @param v Input can be a NaifVertex or NaifVector to validate.
   *  
   * @return <b>bool</b> Indicates whether the given array is size 3.
   */
  bool validate(const NaifVertex &v) {
    if ( 3 != v.dim1() ) return (false);
    return (true);
  }



  /** 
   *  Verifies that the given NaifTriangle is 3 x 3.
   *  
   * @param t NaifTriangle to validate.
   *  
   * @return <b>bool</b> Indicates whether the given 2D array is 3 x 3.
   */
  bool validate(const NaifTriangle &t) {
    if ( 3 != t.dim1() ) return (false);
    if ( 3 != t.dim2() ) return (false);
    return (true);
  }



  /** 
   *  Enables any TNT array of SpiceDoubles to be passed into qDebug() directly.
   *  Valid inputs include NaifVector and NaifVertex.
   *  Array values will be printed on a single line with precision=15 digits.
   *  
   * @param tntArray TNT array to be printed.
   */
  QDebug operator<<(QDebug dbg, const TNT::Array1D<SpiceDouble> &tntArray) {
    dbg.nospace() << toString(tntArray);
    return dbg;
  }



  /** 
   *  Enables any 2 dimensional TNT array of SpiceDoubles to be passed into
   *  qDebug() directly. Valid inputs include NaifTriangle. Each row will
   *  be indented 4 spaces and each entry will be printed with precision=15
   *  digits.
   *  
   * @param tntMatrix Two dimensional TNT array to be printed.
   */
  QDebug operator<<(QDebug dbg, const TNT::Array2D<SpiceDouble> &tntMatrix) {
    for (int i = 0; i < tntMatrix.dim1(); i++) {
      dbg.nospace() << "    ";
      for (int j = 0; j < tntMatrix.dim2(); j++) {
        dbg.nospace() << toString(tntMatrix[i][j], 15) << "     ";
      }
      dbg.nospace() << endl;
    }
    return dbg;
  }



  /** 
   *  Formats any TNT array of SpiceDoubles as a string with given precision.
   *  Valid inputs include NaifVector and NaifVertex. The array in the output
   *  string will be comma separated and enclosed in parentheses.
   *  
   * @param tntArray TNT array to be formatted as a string.
   * @param precision Number of digits each value in the array will be stored with. 
   *  
   * @return <b>QString</b> A string containing the array values in parentheses and comma separated.
   */
  QString toString(const TNT::Array1D<SpiceDouble> &naifArray, int precision) {
    QString result = "( ";
    for (int i = 0; i < naifArray.dim1(); i++) {
      result += toString(naifArray[i], precision);
      if (i != naifArray.dim1() - 1) result += ", ";
    }
    result += " )";
    return result;
  }


}
// namespace Isis
