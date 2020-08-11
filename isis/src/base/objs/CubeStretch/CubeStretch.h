#ifndef CubeStretch_h
#define CubeStretch_h
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

#include "Stretch.h"
#include "StretchBlob.h"

namespace Isis {
  /**
   * @brief Stores stretch information for a cube. Stores stretch pairs, 
   * band number associated with the stretch, and the stretch type from 
   * the Advanced Stretch Tool (or 'Default' if not specified) 
   *
   * @ingroup Utility
   *
   * @author 2020-07-28 Kristin Berry
   *
   * @internal
   *  @history 2020-07-28 Kristin Berry - Original Version
   */
  class CubeStretch : public Stretch { 
    public: 
      CubeStretch();
      CubeStretch(QString name);
      CubeStretch(QString name, QString stretchType, int bandNumber=1);
      ~CubeStretch();

      CubeStretch(Stretch const& stretch);
      CubeStretch(Stretch const& stretch, QString type);
      CubeStretch(Stretch const& stretch, QString type, QString name, int bandNumber=1);

      bool operator==(CubeStretch& stretch2);

      QString getType();
      void setType(QString stretchType);

      QString getName();
      void setName(QString name);

      int getBandNumber();
      void setBandNumber(int bandNumber);
           
    private:                      
      QString m_name; //! The name of the stretch. 
      QString m_type; //! Type of  stretch. This is only currently used in the AdvancedStretchTool.
      int m_bandNumber; //! The band number associated with this stretch
  };
};                                

#endif

