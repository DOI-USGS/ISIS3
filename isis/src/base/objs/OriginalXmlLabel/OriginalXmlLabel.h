#ifndef OriginalXmlLabel_h
#define OriginalXmlLabel_h

/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/05/14 19:20:28 $
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

