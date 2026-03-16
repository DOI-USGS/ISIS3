/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <QFile>

#include "gdal_priv.h"
#include "cpl_string.h"

#include "IException.h"
#include "IString.h"
#include "JP2Encoder.h"

using namespace std;

namespace Isis {

  /**
   * Constructs a JPEG2000 encoder object
   *
   * @param jp2file Name of file where the encoded JP2 data will be stored.
   * @param nsamps  Sample dimension of image that will be encoded.
   * @param nlines  Line dimension of image that will be encoded.
   * @param nbands  Band dimension of image that will be encoded.
   * @param type    Pixel type of data that will be encoded.
   */
  JP2Encoder::JP2Encoder(const QString &jp2file, const unsigned int nsamps,
                         const unsigned int nlines, const unsigned int nbands,
                         const Isis::PixelType type) {
    p_jp2File = jp2file;
    p_numSamples = nsamps;
    p_numLines = nlines;
    p_numBands = nbands;

    if (p_numSamples == 0 || p_numLines == 0 || p_numBands == 0) {
      string msg = "Invalid sample/line/band dimensions specified for output file";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (type == Isis::SignedWord) {
      p_signedData = true;
      p_pixelBytes = 2;
      p_gdalType = GDT_Int16;
    }
    else if (type == Isis::UnsignedWord) {
      p_signedData = false;
      p_pixelBytes = 2;
      p_gdalType = GDT_UInt16;
    }
    else if (type == Isis::UnsignedByte) {
      p_signedData = false;
      p_pixelBytes = 1;
      p_gdalType = GDT_Byte;
    }
    else {
      string msg = "Invalid pixel type specified for output file";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    GDALAllRegister();
  }

  /**
   * Open the JPEG2000 file and initialize it.
   * Creates a temporary GeoTIFF for staging pixel data. The actual JP2
   * encoding happens in the destructor via GDAL CreateCopy.
   */
  void JP2Encoder::OpenFile() {
    if (p_dataset != nullptr)
      return;

    // Create a temporary GeoTIFF for staging pixel data
    p_tmpFile = p_jp2File + ".tmp.tif";
    GDALDriver *tifDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (tifDriver == nullptr) {
      QString msg = "GDAL GTiff driver not available";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    p_dataset = tifDriver->Create(p_tmpFile.toLatin1().data(),
                                  p_numSamples, p_numLines, p_numBands,
                                  p_gdalType, nullptr);
    if (p_dataset == nullptr) {
      QString msg = "Unable to create temporary file [" + p_tmpFile + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_currentLine = 0;
  }

  /**
   * Write 8-bit data to JP2 file
   *
   * @param inbuf The array of pointers to byte buffers that will be used to write
   *              out the image data. One byte buffer is required for each band in
   *              the image. Data is written in a BIL manner.
   */
  void JP2Encoder::Write(unsigned char **inbuf) {
    if (p_dataset == nullptr || p_currentLine >= (int)p_numLines)
      return;

    for (unsigned int b = 0; b < p_numBands; b++) {
      GDALRasterBand *band = p_dataset->GetRasterBand(b + 1);
      CPLErr err = band->RasterIO(GF_Write,
                                  0, p_currentLine, p_numSamples, 1,
                                  inbuf[b], p_numSamples, 1,
                                  GDT_Byte, 0, 0);
      if (err != CE_None) {
        QString msg = "Error writing line " + toString(p_currentLine)
                      + " to [" + p_jp2File + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    p_currentLine++;
  }

  /**
   * Write 16-bit data to JP2 file
   *
   * @param inbuf The array of pointers to byte buffers that will be used to write
   *              out the image data. One byte buffer is required for each band in
   *              the image. Data is written in a BIL manner.
   */
  void JP2Encoder::Write(short int **inbuf) {
    if (p_dataset == nullptr || p_currentLine >= (int)p_numLines)
      return;

    for (unsigned int b = 0; b < p_numBands; b++) {
      GDALRasterBand *band = p_dataset->GetRasterBand(b + 1);
      CPLErr err = band->RasterIO(GF_Write,
                                  0, p_currentLine, p_numSamples, 1,
                                  inbuf[b], p_numSamples, 1,
                                  p_gdalType, 0, 0);
      if (err != CE_None) {
        QString msg = "Error writing line " + toString(p_currentLine)
                      + " to [" + p_jp2File + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    p_currentLine++;
  }

  /**
   * JP2Encoder destructor. Converts the temporary GeoTIFF to JP2 via
   * GDAL CreateCopy, then removes the temp file.
   */
  JP2Encoder::~JP2Encoder() {
    if (p_dataset != nullptr) {
      GDALClose(p_dataset); // flush and close the temp GeoTIFF
      p_dataset = nullptr;

      // Convert temp GeoTIFF to JP2 via CreateCopy
      GDALDriver *jp2Driver =
        GetGDALDriverManager()->GetDriverByName("JP2OpenJPEG");
      if (jp2Driver != nullptr) {
        GDALDataset *srcDs = (GDALDataset *)GDALOpen(
          p_tmpFile.toLatin1().data(), GA_ReadOnly);
        if (srcDs != nullptr) {
          // Lossless compression (reversible wavelet transform)
          char **options = nullptr;
          options = CSLSetNameValue(options, "REVERSIBLE", "YES");
          options = CSLSetNameValue(options, "QUALITY", "100");
          GDALDataset *jp2Ds = jp2Driver->CreateCopy(
            p_jp2File.toLatin1().data(), srcDs, false, options,
            nullptr, nullptr);
          CSLDestroy(options);
          if (jp2Ds != nullptr)
            GDALClose(jp2Ds);
          GDALClose(srcDs);
        }
      }

      // Remove the temporary GeoTIFF
      QFile::remove(p_tmpFile);
    }
  }
}
