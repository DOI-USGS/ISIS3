#ifndef OriginalXmlLabel_h
#define OriginalXmlLabel_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDomDocument>

#include "Blob.h"
#include "FileName.h"

namespace Isis {
  /**
   * @brief Read and store original Xml labels.
   *
   * This class provides a means to read and store the Xml labels from the
   * original source.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2017-01-30 Jesse Mapel
   *
   * @internal
   *   @history 2017-01-30 Jesse Mapel - Original version, adapted from
   *                           OriginalLabel. Fixes #4584.
   *
   */
  class OriginalXmlLabel : public Isis::Blob {
    public:
      OriginalXmlLabel();
      OriginalXmlLabel(const QString &file);
      ~OriginalXmlLabel();

      void readFromXmlFile(const FileName &xmlFileName);
      const QDomDocument &ReturnLabels() const;

    protected:
      void ReadData(std::istream &stream);
      void WriteData(std::fstream &os);
      void WriteInit();

    private:
      QDomDocument m_originalLabel; //!< Original Xml Label.
  };
};

#endif

