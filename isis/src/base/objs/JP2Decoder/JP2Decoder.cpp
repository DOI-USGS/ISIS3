/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "gdal_priv.h"

#include "IException.h"
#include "IString.h"
#include "JP2Decoder.h"

using namespace std;

namespace Isis {

  /**
   * Constructs a JPEG2000 decoder object
   *
   * @param jp2file The name of the JP2 file that needs to be decoded.
   */
  JP2Decoder::JP2Decoder(const QString &jp2file) {
    p_jp2File = jp2file;
    GDALAllRegister();
  }

  /**
   * Open the JPEG2000 file
   */
  void JP2Decoder::OpenFile() {
    if (p_dataset != nullptr)
      return;

    p_dataset = (GDALDataset *)GDALOpen(p_jp2File.toLatin1().data(),
                                        GA_ReadOnly);
    if (p_dataset == nullptr) {
      QString msg = "Unable to open JP2 file [" + p_jp2File + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_numSamples = p_dataset->GetRasterXSize();
    p_numLines = p_dataset->GetRasterYSize();
    p_numBands = p_dataset->GetRasterCount();

    if (p_numBands == 0) {
      GDALClose(p_dataset);
      p_dataset = nullptr;
      QString msg = "JP2 file has no bands [" + p_jp2File + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Get pixel type from the first band
    GDALRasterBand *band = p_dataset->GetRasterBand(1);
    GDALDataType dt = band->GetRasterDataType();

    switch (dt) {
      case GDT_Byte:
        p_pixelBytes = 1;
        p_signedData = false;
        break;
      case GDT_UInt16:
        p_pixelBytes = 2;
        p_signedData = false;
        break;
      case GDT_Int16:
        p_pixelBytes = 2;
        p_signedData = true;
        break;
      default: {
        GDALClose(p_dataset);
        p_dataset = nullptr;
        QString msg = "The source file has unsupported pixel type ["
                      + p_jp2File + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // Verify all bands have the same data type
    for (unsigned int b = 2; b <= p_numBands; b++) {
      GDALRasterBand *otherBand = p_dataset->GetRasterBand(b);
      if (otherBand->GetRasterDataType() != dt) {
        GDALClose(p_dataset);
        p_dataset = nullptr;
        std::string msg = "The source file does not have bands with matching "
                          "characteristics";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    p_currentLine = 0;
  }

  /**
   * Read data from JP2 file containing 8-bit data
   *
   * @param inbuf The array of pointers to byte buffers that will be used to read
   *              in the image data. One byte buffer is required for each band in
   *              the image. Data is read in a BIL manner. It is up to the calling
   *              routine to do the casting necessary to convert the byte data to
   *              the actual pixel type (UnsignedByte, UnsignedWord, SignedWord).
   */
  void JP2Decoder::Read(unsigned char **inbuf) {
    if (p_dataset == nullptr || p_currentLine >= (int)p_numLines)
      return;

    for (unsigned int b = 0; b < p_numBands; b++) {
      GDALRasterBand *band = p_dataset->GetRasterBand(b + 1);
      CPLErr err = band->RasterIO(GF_Read,
                                  0, p_currentLine, p_numSamples, 1,
                                  inbuf[b], p_numSamples, 1,
                                  GDT_Byte, 0, 0);
      if (err != CE_None) {
        QString msg = "Error reading line " + toString(p_currentLine)
                      + " from [" + p_jp2File + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    p_currentLine++;
  }

  /**
   * Read data from JP2 file containing 16-bit data
   *
   * @param inbuf The array of pointers to byte buffers that will be used to read
   *              in the image data. One byte buffer is required for each band in
   *              the image. Data is read in a BIL manner. It is up to the calling
   *              routine to do the casting necessary to convert the byte data to
   *              the actual pixel type (UnsignedByte, UnsignedWord, SignedWord).
   */
  void JP2Decoder::Read(short int **inbuf) {
    if (p_dataset == nullptr || p_currentLine >= (int)p_numLines)
      return;

    GDALDataType readType = p_signedData ? GDT_Int16 : GDT_UInt16;

    for (unsigned int b = 0; b < p_numBands; b++) {
      GDALRasterBand *band = p_dataset->GetRasterBand(b + 1);
      CPLErr err = band->RasterIO(GF_Read,
                                  0, p_currentLine, p_numSamples, 1,
                                  inbuf[b], p_numSamples, 1,
                                  readType, 0, 0);
      if (err != CE_None) {
        QString msg = "Error reading line " + toString(p_currentLine)
                      + " from [" + p_jp2File + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    p_currentLine++;
  }

  /**
   * JP2Decoder destructor
   */
  JP2Decoder::~JP2Decoder() {
    if (p_dataset != nullptr) {
      GDALClose(p_dataset); // closes dataset and frees all memory
      p_dataset = nullptr;
    }
  }

  /**
   * Check if a file is a valid JP2 file
   */
  bool JP2Decoder::IsJP2(QString filename) {
    GDALAllRegister();
    GDALDataset *ds = (GDALDataset *)GDALOpen(filename.toLatin1().data(),
                                              GA_ReadOnly);
    if (ds == nullptr)
      return false;

    const char *driverName = ds->GetDriver()->GetDescription();
    bool isJP2 = (strcmp(driverName, "JP2OpenJPEG") == 0 ||
                  strcmp(driverName, "JP2ECW") == 0 ||
                  strcmp(driverName, "JP2MrSID") == 0 ||
                  strcmp(driverName, "JP2KAK") == 0);
    GDALClose(ds);
    return isJP2;
  }
}
