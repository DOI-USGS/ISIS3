#ifndef JP2Exporter_h
#define JP2Exporter_h

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

#include "StreamExporter.h"

namespace Isis {
  class JP2Encoder;

  /**
   * @brief Exports cubes into JPEG 2000 images
   *
   * A streamed exporter for JPEG 2000 images.  Can write an arbitrarily large
   * set of single-band Isis cubes to an arbitrarily large JPEG 2000 image with
   * the given pixel type.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *   @history 2012-04-04 Travis Addair - Added documentation.
   *   @history 2013-06-05 Jeannie Backer - Removed "get" prefix from ImageExporter method calls.
   *                           References #1380.
   *
   */
  class JP2Exporter : public StreamExporter {
    public:
      JP2Exporter();
      virtual ~JP2Exporter();

      virtual void write(FileName outputName, int quality=100);

      static bool canWriteFormat(QString format);

    protected:
      virtual void createBuffer();

      virtual void setBuffer(int s, int b, int dn) const;
      virtual void writeLine(int l) const;

    private:
      //! Object responsible for writing data to the output image
      JP2Encoder *m_encoder;

      //! Two dimensional array containing all color channels for a line
      char **m_buffer;
  };
};


#endif
