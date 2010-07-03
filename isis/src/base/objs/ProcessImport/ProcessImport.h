#ifndef ProcessImport_h
#define ProcessImport_h
/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2010/02/22 02:25:02 $
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

#include "Process.h"
#include "Buffer.h"
#include "PixelType.h"
#include "SpecialPixel.h"
#include "EndianSwapper.h"
#include "PvlTranslationManager.h"
#include "CubeAttribute.h"
#include "JP2Decoder.h"

namespace Isis {
/**                                                                       
 * @brief  Byte swapper               
 *                                                                        
 * This class is used to import an image into Isis. It is a powerful base class 
 * which can be used to skip over header bytes, trailer bytes, prefix and/or 
 * suffix bytes. By deriving off of this class it can be used to Import vicar, 
 * PDS, fits, or many other standard format images. 
 *                                                                        
 * Here is an example of how to use ProcessImport                         
 * @code                                                                  
 *   Import p; 
 *   p.SetInputFile("myfile.raw"); 
 *   p.SetDimensions(512,512,1);
 *   p.SetPixelType(Isis::SignedWord); 
 *   p.SetByteOrder(Isis::Lsb);
 *   p.SetOutputCube("TO"); 
 *   p.StartProcess(); 
 *   p.EndProcess();                                  
 * @endcode
 * 
 * If you would like to see ProcessImport being used in implementation, 
 * see raw2isis.cpp or for a class that implements ProcessImport, 
 * see ProcessImportPds
 * 
 * @ingroup HighLevelCubeIO                                                  
 *                                                                        
 * @author 2002-08-06 Tracie Sucharski                                                                                                                                                     
 *                                                                        
 * @internal
 *  @history 2003-01-24 Tracie Sucharski - Fixed bug in processing 8bit data. In 
 *                                         the Swap method needed to return 
 *                                         unsigned char, not char.
 *  @history 2003-02-13 Stuart Sides - Added a unit test.
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...
 *  @history 2003-05-30 Stuart Sides - Fixed compiler error after -O1 flag was 
 *                                     added to g++
 *  @history 2003-09-10 Tracie Sucharski - Complete redesign to handle different 
 *                                         raw file formats.
 *  @history 2003-10-09 Stuart Sides - Added capabilities for reading PDS files.
 *  @history 2003-10-10 Stuart Sides - Added capabilities to get the translation 
 *                                     files from the user preferences BASE
 *                                     directory.
 *  @history 2003-10-10 Stuart Sides - Fixed bug for PDS files. When the image 
 *                                     name was explicitly given the open  
 *                                     statement was attempting to open the 
 *                                     label file.
 *  @history 2003-10-16 Stuart Sides - Added a section for debuging all the 
 *                                     parameters which can be set before
 *                                     processing starts.
 *  @history 2003-10-16 Stuart Sides - Added a second parameter to the Pvl 
 *                                     constructor in SetVicarFile. This allows 
 *                                     the vicar label to be read into a Pvl  
 *                                     without modifying the repeated "PROPERTY" 
 *                                     keyword.
 *  @history 2003-10-23 Stuart Sides - Added new member functions  
 *                                    "AddImportLabel()"and"AddImportLabel(Pvl). 
 *                                     AddImportLabel() uses the original label
 *                                     file to create a Pvl and write it to the  
 *                                     output cube label. AddImportLabel(Pvl)
 *                                     uses the Pvl argument to write the import
 *                                     label to the output cube.
 *  @history 2004-01-21 Jeff Anderson - Modified SetOutputCube method to 
 *                                      propagate the input pixel type, base,and
 *                                      multipliers to the output cube. The old 
 *                                      method always generated real pixel 
 *                                      values in the output.
 *  @history 2004-01-22 Jeff Anderson - Modified the SetVicarFile method to 
 *                                      return the vicar labels
 *                                      internalized in a Pvl object.
 *  @history 2004-02-04 Jeff Anderson - Modified SetPdsFile method to allow for 
 *                                      SPECTRAL_QUBE objects to handle Themis
 *                                      data.
 *  @history 2004-02-05 Jeff Anderson - Refactored the class
 *  @history 2004-02-10 Stuart Sides - Added new member AddLabel
 *  @history 2004-02-10 Stuart Sides - Modified the SetOutputCube member to 
 *                                     create a real output file by default if
 *                                     the number of core base and multipliers 
 *                                     is more than one.
 *  @history 2005-02-09 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation
 *  @history 2005-04-08 Leah Dahmer - added missing documentation.
 * 
 *  @history 2006-04-21 Brendan George - Implemented ProcessBil()
 * 
 *  @history 2006-10-26 Stuart Sides - Added unsigned 16 bit ability
 * 
 *  @history 2007-10-05 Steven Lambright - Added StartProcess method that
 *                                   passes data back to a function instead of writing it
 *                                   out to a file.
 *  @history 2007-11-13 Janet Barrett - Added the ProcessBip capability 
 *  @history 2008-02-22 Steven Lambright - Fixed a memory leak and improved the way the
 *                                    destructor detects allocated pointers.
 *  @history 2008-08-18 Jeannie Walldren - Modified
 *                                    documentation and added method
 *                                    SetFileTrailerBytes().
 *  @history 2008-09-16 Christopher Austin - Added error throwing for the
 *           setting of special pixel ranges that overlap by adding
 *           CheckPixelRange().
 *  @history 2009-12-14 Janet Barrett - Added capability to import JPEG2000 files.
 * 
 */                                                                       

class ProcessImport : public Isis::Process {
  public:
    ProcessImport();
    ~ProcessImport ();
    void StartProcess ();
    void StartProcess (void funct(Isis::Buffer &out));
    void SetInputFile (const std::string &file);
    Isis::Cube* SetOutputCube (const std::string &parameter);
    Isis::Cube* SetOutputCube (const std::string &parameter, 
                                   Isis::CubeAttributeOutput &att);
    void SetPixelType (const Isis::PixelType type);
    /**
     * Returns the pixel type
     * @return The pixel type of input data
     */
    Isis::PixelType PixelType() { return p_pixelType; }
    void SetDimensions (const int ns, const int nl, const int nb);

    /**
     * Returns the number of samples
     * @return The number of samples
     */
    int Samples () { return p_ns; }

    /**
     * Returns the number of lines
     * @return The number of lines
     */
    int Lines () { return p_nl; }

    /**
     * Returns the number of bands
     * @return The number of bands
     */
    int Bands () { return p_nb; }

    void SetByteOrder (const Isis::ByteOrder order);

    /**
     * Returns the byte order
     * @return Byte order of the data
     */
    Isis::ByteOrder ByteOrder () { return p_byteOrder; }

    void SetFileHeaderBytes (const int bytes);
    void SetFileTrailerBytes (const int bytes);
    void SetDataHeaderBytes (const int bytes);
    void SetDataTrailerBytes (const int bytes);
    void SetDataPrefixBytes (const int bytes);
    void SetDataSuffixBytes (const int bytes);
    void SaveFileHeader ();
    void SaveFileTrailer ();
    void SaveDataHeader ();
    void SaveDataTrailer ();
    void SaveDataPrefix ();
    void SaveDataSuffix ();
    char* FileHeader ();
    char* FileTrailer ();
    std::vector<char *> DataHeader ();
    std::vector<char *> DataTrailer ();
    std::vector<std::vector<char *> > DataPrefix ();
    std::vector<std::vector<char *> > DataSuffix ();

    /**
     * This enum includes how the document should be read: by BSQ, BIL, BIP,
     * JP2, or InterleaveUndefined.
     */
    enum Interleave {
      InterleaveUndefined, /**<Undefined */
      JP2,                 /**<Jpeg 2000 Format (always band sequential).*/
      BSQ,                 /**<Band Sequential Format (i.e. Band Interleaved).  
                              The first line of data is followed immediately by 
                              the next line in the same spectral band.*/
      BIL,                 /**<Band Interleaved By Line Format (i.e. Row Interleaved).  
                              The first line of data is followed immediately by
                              the first line of data in the next band.*/
      BIP                  /**<Band Interleaved By Pixel Format (i.e. Pixel Interleaved).  
                              The first pixel for all bands is followed by 
                              the second pixel for all bands.*/
    };
    void SetOrganization (const ProcessImport::Interleave org);

    void SetBase (const double base);
    void SetMultiplier (const double mult);
    void SetBase (const std::vector<double> base);
    void SetMultiplier (const std::vector<double> mult);

    void SetSpecialValues (const double null, const double lrs,
                           const double lis, const double hrs,
                           const double his);
    void SetNull(const double null_min, const double null_max);
    void SetLRS(const double lrs_min, const double lrs_max);
    void SetLIS(const double lis_min, const double lis_max);
    void SetHRS(const double hrs_min, const double hrs_max);
    void SetHIS(const double his_min, const double his_max);

    double TestPixel(const double pixel);

#if 0
    void AddImportLabel (Isis::Pvl &importLab);
    void AddLabel (Isis::Pvl &label);
#endif


  private:
    std::string p_inFile;        //!<Input file name
    Isis::PixelType p_pixelType; //!<Pixel type of input data
    int p_ns;                    //!<Number of samples
    int p_nl;                    //!<Number of lines
    int p_nb;                    //!<Number of bands
    Isis::ByteOrder p_byteOrder; //!<Byte order of data   
    int p_fileHeaderBytes;       /**<The number of bytes of non-image data at  
                                     the beginning of a file. This does not 
                                     include any section headers such as band
                                     headers.*/
    int p_fileTrailerBytes;      /**<The number of bytes of non-image data at  
                                     the end of a file. This does not 
                                     include any section trailers such as band
                                     trailers.*/
    int p_dataHeaderBytes;       /**<Number of bytes of non-image data after the 
                                     file header and before the image data of 
                                     each data block, such as band header data 
                                     in a band sequential file.*/
    int p_dataTrailerBytes;      /**<Number of bytes of non-image data after the 
                                     image data of each data block, such as 
                                     band trailer data in a band sequential file.*/
    int p_dataPreBytes;          /**<Number of bytes of non-image data preceding 
                                     each data record, such as line prefix data 
                                     in a band sequential file.*/
    int p_dataPostBytes;         /**<Number of bytes of non-image data following 
                                     each data record, such as line suffix data 
                                     in a band sequential file.*/
    bool p_saveFileHeader;       /**<Flag indicating whether to save the file header 
                                     or not. */
    bool p_saveDataHeader;       /**<Flag indicating whether to save the data header
                                     or not. */
    bool p_saveDataTrailer;      /**<Flag indicating whether to save the data trailer
                                     or not. */
    bool p_saveDataPre;          /**<Flag indicating whether to save the data prefix
                                     or not. */
    bool p_saveDataPost;         /**<Flag indicating whether to save the data suffix
                                     or not */
    bool p_saveFileTrailer;      /**<Flag indicating whether to save the file trailer
                                     or not. */
    char *p_fileHeader;                          //!<The file header
    std::vector<char *>p_dataHeader;             //!<The data header
    std::vector<char *>p_dataTrailer;            //!<The data trailer
    std::vector<std::vector<char *> >p_dataPre;  //!<The data prefix
    std::vector<std::vector<char *> >p_dataPost; //!<The data suffix
    char *p_fileTrailer;                         //!<The file trailer
  
    ProcessImport::Interleave p_organization; /**<The format of the input file. 
                                                  Possible values are BSQ for band 
                                                  sequential, BIL for band interleaved 
                                                  by line and BIP for band interleaved 
                                                  by pixel.*/   

    std::vector<double> p_base; /**<An array containing the core base for each 
                                    band. If there is only one value in the 
                                    vector then that base applies to all bands.*/
    std::vector<double> p_mult; /**<An array containing the core multiplier for 
                                    each band. If there is only one value in the
                                    vector then that multiplier applies to all 
                                    bands.*/

    double p_null_min;     /**<The pixel value which is the lower bound of NULL data. All  
                           pixels between this value and the max will be converted to the 
                           Isis NULL value.*/
    double p_null_max;     /**<The pixel value which is the upper bound of NULL data. All  
                           pixels between this value and the min will be converted to the 
                           Isis NULL value.*/
    double p_hrs_min;     /**<The pixel value which is the lower bound of HRS data. All  
                           pixels between this value and the max will be converted to the 
                           Isis HRS value.*/
    double p_hrs_max;     /**<The pixel value which is the upper bound of HRS data. All  
                           pixels between this value and the min will be converted to the 
                           Isis HRS value.*/
    double p_his_min;     /**<The pixel value which is the lower bound of HIS data. All  
                           pixels between this value and the max will be converted to the 
                           Isis HIS value.*/
    double p_his_max;     /**<The pixel value which is the upper bound of HIS data. All  
                           pixels between this value and the min will be converted to the 
                           Isis HIS value.*/
    double p_lrs_min;     /**<The pixel value which is the lower bound of LRS data. All  
                           pixels between this value and the max will be converted to the 
                           Isis LRS value.*/
    double p_lrs_max;     /**<The pixel value which is the upper bound of LRS data. All  
                           pixels between this value and the min will be converted to the 
                           Isis LRS value.*/
    double p_lis_min;     /**<The pixel value which is the lower bound of LIS data. All  
                           pixels between this value and the max will be converted to the 
                           Isis LIS value.*/
    double p_lis_max;     /**<The pixel value which is the upper bound of LIS data. All  
                           pixels between this value and the min will be converted to the 
                           Isis LIS value.*/

    void ProcessBsq (void funct(Isis::Buffer &out) = NULL);
    void ProcessBil (void funct(Isis::Buffer &out) = NULL);
    void ProcessBip (void funct(Isis::Buffer &out) = NULL);
    void ProcessJp2 (void funct(Isis::Buffer &out) = NULL);

    void CheckPixelRange( std::string pixelName, double min, double max );
};
};
#endif
