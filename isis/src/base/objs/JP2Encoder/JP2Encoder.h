#ifndef JP2Encoder_h
#define JP2Encoder_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include "PixelType.h"

#if ENABLEJP2K
#include "jp2.h"
#include "kdu_stripe_compressor.h"
#endif

#define MIN_STRIPE_HEIGHT 256
#define MAX_STRIPE_HEIGHT 8192
#define INCREMENTAL_FLUSH_BYTES                         (256 * 1024 * 1024)

namespace Isis {
  class JP2Error;

  /**
   * @brief  JPEG2000 encoder class
   *
   * This class is used to convert image data into JPEG2000 format.
   *
   * Here is an example of how to use JP2Encoder
   * @code
   *   char **jp2buf;
   *   ProcessExport p;
   *   Cube *icube;
   *   JP2Encoder *JP2_encoder;
   *   icube = p.SetInputCube("FROM",Isis::OneBand);
   *   p.SetInputRange();
   *   p.setFormat(ProcessExport::BIL);
   *   jp2buf = new char* [1];
   *   jp2buf[0] = new char[icube->Samples()];
   *   p.SetOutputType(Isis::UnsignedByte);
   *   p.SetOutputRange(1.0,255.0);
   *   p.SetOutputNull(0.0);
   *   JP2_encoder = new JP2Encoder(ui.GetFileName("TO"),icube->Samples(),
   *                 icube->Lines(),icube->Bands(),Isis::UnsignedByte);
   *   JP2_encoder->OpenFile();
   *   p.StartProcess(toJP2);
   *   p.EndProcess();
   *   delete JP2_encoder;
   * @endcode
   *
   * If you would like to see JP2Encoder being used in implementation,
   * see isis2std.cpp
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
   *
   */
  class JP2Encoder {
    public:
      JP2Encoder(const QString &jp2file, const unsigned int nsamps,
                 const unsigned int nlines, const unsigned int nbands,
                 const Isis::PixelType type);
      ~JP2Encoder();

      // Register with the Kakadu error facility
      JP2Error *kakadu_error() const {
        return Kakadu_Error;
      };

      // Open and initialize the JP2 file for writing
      void OpenFile();

      // Write byte data to the JP2 file
      void Write(unsigned char **inbuf);

      // Write 16-bit data to the JP2 file
      void Write(short int **inbuf);

    private:
      QString p_jp2File;           //!<Output file name
      std::string p_progressionOrder;  //!<Progression order used to create output file
      std::vector<unsigned int> p_precinctSize; //!<Precinct size(s) used to create output file
      JP2Error *Kakadu_Error;          //!<JP2 Error handling facility

#if ENABLEJP2K
      unsigned int p_sampleDimension;  //!<Sample dimension of output file
      unsigned int p_lineDimension;    //!<Line dimension of output file
      unsigned int p_bandDimension;    //!<Band dimension of output file
      unsigned int p_resolutionLevels; //!<Number of resolution levels used to make output file
      unsigned int p_codeBlockSize;    //!<Code block size used to create output file
      bool p_signedData;               //!<Determines if output file will contain signed/unsigned
      //!<data
      unsigned int p_tileSizeWidth;    //!<Width of tiles used to create output file
      unsigned int p_tileSizeHeight;   //!<Height of tiles used to create output file
      unsigned int p_pixelBits;        //!<Number of bits per pixel used to create output file
      unsigned int p_pixelBytes;       //!<Number of bytes per pixel used to create output file
      int p_flushLines;                //!<Number of lines to store in memory before flushing the
      //!<buffer that is written to the codestream
      bool p_writeStripes;             //!<Number of lines written per call to Write methods
      int *p_stripeHeights;            //!<Determines the number of lines to be written at a time
      //!<to the codestream
      int *p_maxStripeHeights;         //!<Determines the maximum number of lines that can be
      //!<written at a time to the codestream
      int *p_precisions;               //!<Determines bit precision of each band in the output
      //!<JP2 file
      bool *p_isSigned;                //!<Determines if the data is signed/unsigned for each
      //!<band in the JP2 file

      kdu_supp::jp2_family_tgt *JP2_Stream;      //!<JP2 file output stream
      kdu_supp::jp2_target *JP2_Boxes;           //!<JP2 boxes for the JP2 file output stream
      kdu_core::kdu_codestream *JPEG2000_Codestream; //!<Allow access to JP2 file codestream
      kdu_supp::kdu_stripe_compressor p_compressor;  //!<High level interface to compression of JP2
      //!<file
#endif
  };
};
#endif
