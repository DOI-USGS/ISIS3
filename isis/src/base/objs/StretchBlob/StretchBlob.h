#ifndef StretchBlob_h
#define StretchBlob_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <istream>
#include <fstream>
#include <QString>

#include "Blob.h"
#include "CubeStretch.h"

namespace Isis {
  /**
   * @brief Blob to store stretch information for a cube, on a cube.
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
      CubeStretch m_stretch; //! Stretch associated with the blob
  };
};

#endif

