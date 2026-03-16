#ifndef JP2Decoder_h
#define JP2Decoder_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "gdal_priv.h"

namespace Isis {

  /**
   * @brief  JPEG2000 decoder class
   *
   * This class is used to decode a JPEG2000 image with GDAL
   * (OpenJPEG backend).
   *
   * Here is an example of how to use JP2Decoder
   * @code
   *   JP2Decoder *JP2_decoder;
   *   JP2_decoder = new JP2Decoder(QString(ui.GetFileName("FROM")));
   *   JP2_decoder->OpenFile();
   *   int nsamps = JP2_decoder->GetSampleDimension();
   *   int nlines = JP2_decoder->GetLineDimension();
   *   int nbands = JP2_decoder->GetBandDimension();
   *   int pixelbytes = JP2_decoder->GetPixelBytes();
   *   bool is_signed = JP2_decoder->GetSignedData();
   *   delete JP2_decoder;
   *   ProcessImport jp;
   *   jp.SetDimensions(nsamps,nlines,nbands);
   *   if (pixelbytes == 1) {
   *     jp.SetPixelType(Isis::UnsignedByte);
   *   } else if (pixelbytes == 2) {
   *     if (is_signed) {
   *       jp.SetPixelType(Isis::SignedWord);
   *     } else {
   *       jp.SetPixelType(Isis::UnsignedWord);
   *     }
   *   } else {
   *     throw iException::Message(iException::User,
   *       "The file [" + ui.GetFileName("FROM") + "] contains unsupported data type.",
   *       _FILEINFO_);
   *   }
   *   jp.SetInputFile(QString(ui.GetFileName("FROM")));
   *   jp.SetOutputCube("TO");
   *   jp.SetOrganization(ProcessImport::JP2);
   *   jp.StartProcess();
   *   jp.EndProcess();
   * @endcode
   *
   * If you would like to see JP2Decoder being used in implementation,
   * see std2isis.cpp or for a class that implements JP2Decoder,
   * see ProcessImport
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2009-12-18 Janet Barrett
   *
   * @internal
   *  @history 2009-12-18 Janet Barrett - Original version.
   *  @history 2012-04-06 Kris Becker - Fixed condition compilation where
   *                        support for JP2K is disabled
   *  @history 2016-08-28 Kelvin Rodriguez - Moved member variables to be placed properly
   *                        within the if ENABLEJP2K preprocessor block in order to stop
   *                        unused member variable warnings in clang. Part of porting to OS X 10.11.
   *  @history 2017-08-21 Tyler Wilson, Ian Humphrey, Summer Stapleton - Added
   *                       support for new kakadu libraries.  References #4809.
   *  @history 2017-09-15 Ian Humphrey - Modified destructor to call finish() on the decompressor
   *                          before destroying the kdu_codestream. Caused segfault on OSX 10.11
   *                          for the JP2Importer test, and isis2std and std2isis jpeg2000 tests.
   *                          References #4809.
   *  @history 2026-03-16 Oleg Alexandrov - Replaced Kakadu backend with GDAL
   *                          (OpenJPEG). Removed JP2Error dependency.
   */
  class JP2Decoder {
    public:
      JP2Decoder(const QString &jp2file);
      ~JP2Decoder();

      // Open and initialize the JP2 file for reading
      void OpenFile();

      // Get the sample dimension of the JP2 file
      inline int GetSampleDimension() const {
        return (int)p_numSamples;
      }

      // Get the line dimension of the JP2 file
      inline int GetLineDimension() const {
        return (int)p_numLines;
      }

      // Get the band dimension of the JP2 file
      inline int GetBandDimension() const {
        return (int)p_numBands;
      }

      // Get number of bytes per pixel in the JP2 file
      inline int GetPixelBytes() const {
        return p_pixelBytes;
      }

      // Determine if data in JP2 file is signed
      inline bool GetSignedData() const {
        return p_signedData;
      }

      // Read byte data from the JP2 file
      void Read(unsigned char **inbuf);

      // Read 16-bit data from the JP2 file
      void Read(short int **inbuf);

      static bool IsJP2(QString filename);

    private:
      QString p_jp2File;                 //!<Input file name
      unsigned int p_numSamples = 0;     //!<Number of samples in JP2 file
      unsigned int p_numLines = 0;       //!<Number of lines in JP2 file
      unsigned int p_numBands = 0;       //!<Number of bands in JP2 file
      unsigned int p_pixelBytes = 0;     //!<Number of bytes per pixel in JP2 file
      bool p_signedData = false;         //!<Set to true if data in JP2 file is signed

      GDALDataset *p_dataset = nullptr;  //!<GDAL dataset handle
      int p_currentLine = 0;             //!<Next line to read
  };
};
#endif
