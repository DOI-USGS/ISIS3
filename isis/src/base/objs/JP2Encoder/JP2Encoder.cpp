/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/02/22 02:16:56 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                    

#include <float.h>
#include <iostream>
#include <string>
#include <sstream>

#include "iException.h"
#include "iString.h"
#include "JP2Encoder.h"
#include "JP2Error.h"

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
  *
  */
  JP2Encoder::JP2Encoder (const std::string &jp2file, const unsigned int nsamps,
                          const unsigned int nlines, const unsigned int nbands,
                          const Isis::PixelType type) {

#if ENABLEJP2K  
    p_jp2File = jp2file;
    p_sampleDimension = nsamps;
    p_lineDimension = nlines;
    p_bandDimension = nbands;

    if (p_sampleDimension == 0 || p_lineDimension == 0 || p_bandDimension == 0) {
      string msg = "Invalid sample/line/band dimensions specified for output file";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    if (type == Isis::SignedWord) {
      p_signedData = true;
      p_pixelBits = 16;
      p_pixelBytes = 2;
    } else if (type == Isis::UnsignedWord) {
      p_signedData = false;
      p_pixelBits = 16;
      p_pixelBytes = 2;
    } else if (type == Isis::UnsignedByte) {
      p_signedData = false;
      p_pixelBits = 8;
      p_pixelBytes = 1;
    } else {
      string msg = "Invalid pixel type specified for output file";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    // Determine number of resolution levels. The sample/line dimension at the
    // smallest resolution must not be smaller than 64. The number of resolution
    // levels must not exceed 32.
    p_resolutionLevels = 1;
    int mindim = 
      (p_sampleDimension > p_lineDimension) ? p_lineDimension : p_sampleDimension;
    while (mindim > 64 && p_resolutionLevels < 32) {
      ++p_resolutionLevels;
      mindim >>= 1;
    }

    // Precinct size will be set to 256 for all resolution levels
    p_precinctSize.resize(p_resolutionLevels,256);

    // The progression order, code block size, and tile size are all set to
    // values that allow incremental flushing to work. Incremental flushing
    // prevents us from having to store the entire compressed codestream in
    // memory before writing it to disk.
    p_progressionOrder = "PCRL";
    p_codeBlockSize = 64;
    p_tileSizeWidth = p_sampleDimension; // untiled - size of image
    p_tileSizeHeight = p_lineDimension;

    // Register the Kakadu error handler
    Kakadu_Error = new JP2Error;
    kdu_customize_errors(Kakadu_Error);
#else
    std::string msg = "JPEG2000 has not been enabled with this build of ISIS3";
    throw iException::Message(iException::System,msg,_FILEINFO_);
#endif
  }


 /**
  * Open the JPEG2000 file and initialize it
  *
  */
  void JP2Encoder::OpenFile () {
#if ENABLEJP2K
    // Open the JP2 file stream
    JP2_Stream = new jp2_family_tgt();
    JP2_Stream->open(p_jp2File.c_str());

    // Open the JP2 boxes
    JP2_Boxes = new jp2_target();
    JP2_Boxes->open(JP2_Stream);

    // Configure and write all required JP2 boxes up to, but not including,
    // the JPEG2000 codestream (jp2c) box. This includes the Signature,
    // File_Type, and JP2_Header boxes with the latter including the 
    // Image_Header and Colour_Specification subboxes.

    // Set the codestream SIZ parameters - this includes the image data
    // organization, resolution levels, reversible (lossless) discrete
    // wavelet transform, and encoding progression order.
    siz_params *codestream_parameters = new siz_params();
    codestream_parameters->set(Sdims,0,0,(int)p_lineDimension);
    codestream_parameters->set(Sdims,0,1,(int)p_sampleDimension);
    codestream_parameters->set(Sprecision,0,0,(int)p_pixelBits);
    codestream_parameters->set(Stiles,0,0,(int)p_tileSizeHeight);
    codestream_parameters->set(Stiles,0,1,(int)p_tileSizeWidth);
    codestream_parameters->set(Ssigned,0,0,p_signedData);
    codestream_parameters->set(Scomponents,0,0,(int)p_bandDimension);
    ostringstream levels;
    levels << "Clevels=" << (p_resolutionLevels - 1);
    codestream_parameters->parse_string(levels.str().c_str());
    codestream_parameters->parse_string("Creversible=yes");
    string progression = "Corder=" + p_progressionOrder;
    codestream_parameters->parse_string(progression.c_str());

    // Determine the number of tile length marker segments. This is
    // necessary if incremental flushing of the codestream is to be
    // employed. Incremental flushing prevents the software from 
    // having to store the entire codestream in memory before writing
    // it to disk. The number of tile length marker segments is
    // estimated by dividing the tile height by the number of lines
    // requested for incremental flushing rounded up to the next whole
    // number.
    int TLM_segments;
    int p_flushLines = 0;
    long long line_bytes = (long long)p_sampleDimension * p_pixelBytes;
    if (INCREMENTAL_FLUSH_BYTES < (p_lineDimension * line_bytes)) {
      p_flushLines = p_tileSizeHeight;
      while ((p_flushLines * line_bytes) > INCREMENTAL_FLUSH_BYTES) {
        p_flushLines -= 1024;
        if (p_flushLines < 0) p_flushLines = 0;
      }
    }
    if (p_flushLines) {
      TLM_segments = (int)(((double)p_tileSizeHeight / (double)p_flushLines) + 0.5);
      if (!TLM_segments) TLM_segments = 1;
    } else {
      TLM_segments = 1;
    }

    ostringstream segments;
    segments << "ORGgen_tlm=" << TLM_segments;
    codestream_parameters->parse_string(segments.str().c_str());

    // Include packet length markers in tile headers
    codestream_parameters->parse_string("ORGgen_plt=yes");

    // Finalize the codestream parameters
    codestream_parameters->finalize_all();

    // Construct the JPEG2000 codestream object
    JPEG2000_Codestream = new kdu_codestream();
    JPEG2000_Codestream->create(codestream_parameters,JP2_Boxes);

    // Some parameters must be set again after creating the codestream.
    // It was not clear if they need to be set twice or only after
    // creating the codestream.
    codestream_parameters = JPEG2000_Codestream->access_siz();
    codestream_parameters->parse_string(levels.str().c_str());
    codestream_parameters->parse_string("Creversible=yes");
    codestream_parameters->parse_string(progression.c_str());
    codestream_parameters->parse_string(segments.str().c_str());
    codestream_parameters->parse_string("ORGgen_plt=yes");

    // Set precinct sizes - vertical dimension gets set first. In our
    // case, both dimensions are the same.
    ostringstream sizes;
    sizes << "Cprecincts=";
    for (unsigned int i=0; i<p_precinctSize.size(); i++) {
      if (i != 0) sizes << ",";
      sizes << "{" << p_precinctSize[i] << "," << p_precinctSize[i] << "}";
    }
    codestream_parameters->parse_string(sizes.str().c_str());

    // Set code block size - vertical dimension gets set first. In our
    // case, both dimensions are the same.
    ostringstream size;
    size << "Cblk={" << p_codeBlockSize << "," << p_codeBlockSize << "}";
    codestream_parameters->parse_string(size.str().c_str());
   
    codestream_parameters->finalize_all();

    // Finalize image dimensions
    jp2_dimensions dimensions = JP2_Boxes->access_dimensions();
    dimensions.init(codestream_parameters);
    dimensions.finalize_compatibility(codestream_parameters);

    // Set colour definition
    jp2_colour colour = JP2_Boxes->access_colour();
    colour.init((p_bandDimension >= 3) ? JP2_sRGB_SPACE : JP2_sLUM_SPACE);

    // Write all JP2 boxes up to, but not including, the codestream box
    JP2_Boxes->write_header();

    // Initialize the encoder
    // Open the JPEG2000 codestream (jp2c) box
    JP2_Boxes->open_codestream();

    // Set number of quality layers to 1
    int layers;
    kdu_params *COD = JPEG2000_Codestream->access_siz()->access_cluster(COD_params);
    if (!(COD->get(Clayers,0,0,layers) && (layers > 0)))
      COD->set(Clayers,0,0,layers=1);
    kdu_long *layer_sizes = new kdu_long[layers];
    memset(layer_sizes,0,sizeof(kdu_long)*layers);

    // Initialize the codestream stripe compressor
    p_compressor.start(*JPEG2000_Codestream,layers,layer_sizes,NULL,0,false,
                       p_pixelBytes==4); // Force precise for 32-bit values

    // Determine optimum stripe heights for accessing data
    p_stripeHeights = new int[p_bandDimension];
    p_maxStripeHeights = new int[p_bandDimension];
    p_precisions = new int[p_bandDimension];
    p_isSigned = new bool[p_bandDimension];
    p_compressor.get_recommended_stripe_heights(MIN_STRIPE_HEIGHT,
       MAX_STRIPE_HEIGHT,p_stripeHeights,p_maxStripeHeights);
    for (unsigned int i=0; i<p_bandDimension; i++) {
      p_precisions[i] = p_pixelBits;
      p_isSigned[i] = p_signedData;
      p_stripeHeights[i] = 1;
    }
#endif
  }

 /**
  * Write 8-bit data to JP2 file
  *
  * @param inbuf The array of pointers to byte buffers that will be used to write
  *              out the image data. One byte buffer is required for each band in
  *              the image. Kakadu writes in a BIL manner.
  *
  */
  void JP2Encoder::Write (unsigned char **inbuf) {
#if ENABLEJP2K
    p_writeStripes = p_compressor.push_stripe(inbuf,p_stripeHeights,NULL,NULL,
                                              p_precisions,p_flushLines);
#endif
  }

 /**
  * Write 16-bit data to JP2 file
  *
  * @param inbuf The array of pointers to byte buffers that will be used to write
  *              out the image data. One byte buffer is required for each band in
  *              the image. Kakadu writes in a BIL manner.
  *
  */
  void JP2Encoder::Write (short int **inbuf) {
#if ENABLEJP2K
    p_writeStripes = p_compressor.push_stripe(inbuf,p_stripeHeights,NULL,NULL,
                                     p_precisions,p_isSigned,p_flushLines);
#endif
  }

 /**
  * JP2Encoder destructor
  *
  */
  JP2Encoder::~JP2Encoder () {
#if ENABLEJP2K
    p_compressor.finish();
    JPEG2000_Codestream->destroy();
    JP2_Boxes->close();
    JP2_Stream->close();
    delete [] p_stripeHeights;
    delete [] p_maxStripeHeights;
    delete [] p_precisions;
    delete [] p_isSigned;
#endif
  }
}
