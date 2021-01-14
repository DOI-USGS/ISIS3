#ifndef CubeStretch_h
#define CubeStretch_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Stretch.h"

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
      CubeStretch(QString name="DefaultStretch", QString stretchType="Default", int bandNumber = 1);
      ~CubeStretch();

      CubeStretch(Stretch const& stretch);
      CubeStretch(Stretch const& stretch, QString type);

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

