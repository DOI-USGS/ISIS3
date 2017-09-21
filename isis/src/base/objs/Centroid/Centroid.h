#ifndef Centroid_h
#define Centroid_h
/**
 * @file
 * $Revision: 1.0 $
 * $Date: 2011/10/12 12:52:30 $
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
