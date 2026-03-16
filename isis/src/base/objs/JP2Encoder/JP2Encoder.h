#ifndef JP2Encoder_h
#define JP2Encoder_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "gdal_priv.h"

#include "PixelType.h"

namespace Isis {

  /**
   * @brief  JPEG2000 encoder class
   *
   * This class is used to convert image data into JPEG2000 format
   * with GDAL (OpenJPEG backend).
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2009-01-11 Janet Barrett
   *
   * @internal
   *  @history 2009-01-11 Janet Barrett - Original version.
   *  @history 2012-04-06 Kris Becker - Fixed condition compilation where
   *                        support for JP2K is disabled
   *  @history 2017-08-21 Tyler Wilson, Ian Humphrey, Summer Stapleton - Added
   *                        support for new kakadu libraries.  References #4809.
   *  @history 2026-03-16 Oleg Alexandrov - Replaced Kakadu backend with GDAL
   *                          (OpenJPEG). Removed JP2Error dependency.
   */
  class JP2Encoder {
    public:
      JP2Encoder(const QString &jp2file, const unsigned int nsamps,
                 const unsigned int nlines, const unsigned int nbands,
                 const Isis::PixelType type);
      ~JP2Encoder();

      // Open and initialize the JP2 file for writing
      void OpenFile();

      // Write byte data to the JP2 file
      void Write(unsigned char **inbuf);

      // Write 16-bit data to the JP2 file
      void Write(short int **inbuf);

    private:
      QString p_jp2File;                   //!<Output JP2 file name
      QString p_tmpFile;                   //!<Temporary GeoTIFF for staging
      unsigned int p_numSamples = 0;       //!<Sample dimension of output file
      unsigned int p_numLines = 0;         //!<Line dimension of output file
      unsigned int p_numBands = 0;         //!<Band dimension of output file
      unsigned int p_pixelBytes = 0;       //!<Number of bytes per pixel in output file
      bool p_signedData = false;           //!<Set to true if output data is signed
      GDALDataType p_gdalType = GDT_Byte;  //!<GDAL pixel type

      GDALDataset *p_dataset = nullptr;    //!<Temp GeoTIFF dataset handle
      int p_currentLine = 0;               //!<Next line to write
  };
};
#endif
