#ifndef JP2Decoder_h
#define JP2Decoder_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/02/22 02:15:24 $
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

#include <string>

#if ENABLEJP2K
#include "jp2.h"
#include "kdu_stripe_decompressor.h"
#endif

#define MIN_STRIPE_HEIGHT 256
#define MAX_STRIPE_HEIGHT 8192

namespace Isis {
/**                                                                       
 * @brief  JPEG2000 decoder class
 *                                                                        
 * This class is used to decode a JPEG2000 image.
 *                                                                        
 * Here is an example of how to use JP2Decoder
 * @code
 *   JP2Decoder *JP2_decoder;
 *   JP2_decoder = new JP2Decoder(iString(ui.GetFilename("FROM")));
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
 *       "The file [" + ui.GetFilename("FROM") + "] contains unsupported data type.",
 *       _FILEINFO_);
 *   }
 *   jp.SetInputFile(iString(ui.GetFilename("FROM")));
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
 * 
 */                                                                       

class JP2Error;
class JP2Decoder {
  public:
    JP2Decoder (const std::string &jp2file);
    ~JP2Decoder ();

    // Register with the Kakadu error facility
    JP2Error *kakadu_error () const {return Kakadu_Error;};

    // Open and initialize the JP2 file for reading
    void OpenFile ();

    // Get the sample dimension of the JP2 file
    inline int GetSampleDimension() const { return ((int) p_numSamples); }

    // Get the line dimension of the JP2 file
    inline int GetLineDimension() const { return ((int) p_numLines); }

    // Get the band dimension of the JP2 file
    inline int GetBandDimension() const { return ((int) p_numBands); }

    // Get number of bytes per pixel in the JP2 file
    inline int GetPixelBytes() const { return (p_pixelBytes); }

    // Determine if data in JP2 file is signed
    inline bool GetSignedData() const { return (p_signedData); }

    // Read byte data from the JP2 file
    void Read (unsigned char **inbuf);

    // Read 16-bit data from the JP2 file
    void Read (short int **inbuf);

  private:
    std::string p_jp2File;          //!<Input file name
    unsigned int p_resolutionLevel; //!<Resolution level that file will be decompressed
                                    //!<at. Always full resolution.
    unsigned int p_numSamples;      //!<Number of samples in JP2 file
    unsigned int p_numLines;        //!<Number of lines in JP2 file
    unsigned int p_numBands;        //!<Number of bands in JP2 file
    unsigned int p_pixelBits;       //!<Number of bits per pixel in JP2 file.
    unsigned int p_pixelBytes;      //!<Number of bytes per pixel in JP2 file.
    bool p_signedData;              //!<Set to true if data in JP2 file is signed.
    unsigned int p_highestResLevel; //!<Total number of available resolution levels in
                                    //!<JP2 file.
    int *p_stripeHeights;           //!<Determines how many lines are read at a time
                                    //!<from the JP2 file.
    int *p_maxStripeHeights;        //!<Determines the maximum number of lines that can
                                    //!<be read at a time from the JP2 file.
    int *p_precisions;              //!<Determines the bit precision of each band in 
                                    //!<the JP2 file.
    bool *p_isSigned;               //!<Determines if the data is signed/unsigned for
                                    //!<each band in the JP2 file.

#if ENABLEJP2K
    kdu_dims p_imageDims;           //!<Image dimensions of JP2 file
    jp2_family_src *JP2_Stream;     //!<JP2 file input stream
    jp2_source *JP2_Source;         //!<JP2 content source
    kdu_codestream *JPEG2000_Codestream;    //!<Allow access to JP2 file codestream.
    kdu_stripe_decompressor p_decompressor; //!<High level interface to decompression of
                                            //!<JP2 file.
#endif
    JP2Error *Kakadu_Error;         //!<JP2 Error handling facility
    bool p_readStripes;             //!<Number of lines read per call to Read methods

    void SetResolutionAndRegion (); //!<Sets resolution of data that will be decompressed.
                                    //!<Also determines the image dimensions at the requested
                                    //!<resolution.
};
};
#endif
