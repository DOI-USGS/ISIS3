#ifndef Centroid_h
#define Centroid_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Chip.h"
#include "Selection.h"
#include "Statistics.h"

namespace Isis {
  /**                                                                       
   * @brief Selection class derived from the Pure Virtual Parent Class for all Selection classes
   *                                                                        
   * Description coming soon
   *                                                                        
   * @author 2011-10-12 Orrin Thomas
   *                                                                        
   * @internal                                                              
   *   @history 2011-10-12 Orrin Thomas - Original version
   *   @history 2012-02-14 Orrin Thomas - updated to Centroid::select(..) to start the floodfill
   *                          algorithim at [Chip::p_chipSample, Chip::p_chipLine]. Thus, the
   *                          starting pixel of the floodfill/centroid is set by calling the
   *                          Chip::SetChipPosition() before Centroid::select.
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   */
  class Centroid : public Selection
  {
  public:
    Centroid();
    virtual ~Centroid();
    //pure virtual function to be defined in this child class
    int select(Chip *inputChip,Chip *selectionChip);
    int setDNRange( double minimumDN,double maximumDN );
    double getMinDN();
    double getMaxDN();
  private:
    double m_maxDN;    //!< The max DN value to be included in the selection
    double m_minDN;    //!< The min DN value to be included in the selection
  };
}
#endif
