#ifndef CentroidApolloPan_h
#define CentroidApolloPan_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
