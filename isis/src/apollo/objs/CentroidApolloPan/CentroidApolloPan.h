#ifndef CentroidApolloPan_h
#define CentroidApolloPan_h
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

#include "Centroid.h"

namespace Isis {
  /**                                                                       
   * @brief Selection class derived from the Pure Virtual Parent Class for all Selection classes
   *                                                                        
   * Description coming soon
   *
   * @ingroup Selection
   *                                                                        
   * @author 2011-10-22 Orrin Thomas
   *                                                                        
   * @internal                                                              
   *   @history 2011-10-22 Orrin Thomas - Original version
   *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
   */        
  class CentroidApolloPan : public Centroid {
  public:
    
    CentroidApolloPan(double pixel_size_microns);
    virtual ~CentroidApolloPan();
    bool setPixelSize(double microns);
  
    int elipticalReduction(Chip *selectionChip, double percent_selected, double play, int patience_limit);
    int selectAdaptive(Chip *inputChip,Chip *selectionChip);

  private:
    double m_pixelSize;  //!< pixel size in microns
  };
}
#endif
