#ifndef StretchBlob_h
#define StretchBlob_h
/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/22 19:44:53 $
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

#include "Blob.h"
#include "CubeStretch.h"

namespace Isis {
  class CubeStretch;
  /**
   * @brief Blob to store stretch information to cube.
   *
   * @ingroup Utility
   *
   * @author 2020-07-28 Kristin Berry and Stuart Sides
   *
   * @internal
   *  @history 2020-07-28 Kristin Berry and Stuart Sides - Original Version
   */
  class StretchBlob : public Isis::Blob {
    public: 
      StretchBlob();
      StretchBlob(CubeStretch stretch);
      StretchBlob(QString name);
      ~StretchBlob();

      CubeStretch getStretch(); 

    protected:
      void WriteInit();
      void ReadData(std::istream &is);
      void WriteData(std::fstream &os);

    private:
      CubeStretch *m_stretch; //! Stretch associated with the blob
  };
};

#endif

