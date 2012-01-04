#ifndef ProcessMosaic_h
#define ProcessMosaic_h
/**
 * @file
 * $Revision: 1.13 $
 * $Date: 2010/06/21 18:39:22 $
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

#include "Process.h"
#include "Buffer.h"
#include "Portal.h"
/**
 * Enumeration for BandCriteria 
 * Band to be compared must be greater/lesser than 
 */
enum BandCriteria {
  Lesser, Greater
};
/**
 * File Type - Input/Output
 */
enum FileType {
  inFile, outFile
};

namespace Isis {
  /**
   * @brief Mosaic two cubes together
   *
   * This class allows a programmer to develop a program which merges two cubes
   * together. The application sets the position where input (child) cube will be
   * placed in the mosaic (parent) cube and priority. The Mosaic object will merge
   * the overlapping area.
   *  
   * The process has four priorities (input, mosaic, band, average) for how the input 
   * image has to be placed on the mosaic. Priority "input" will place the input image 
   * on top of the mosaic. Priority "mosaic" will place the input image beneath the 
   * mosaic. Priority "band" will place the input image on the mosaic based on the 
   * "Lessser" or "Greater" criteria between user defined band in the input and the 
   * mosaic images. Priority Average will average of valid pixels in the input and 
   * mosaic images. Choosing this priority will cause the mosaic to have twice the 
   * number of bands of the input image,with a count band for each band. The count 
   * band keeps track of number of images involved in the dn value of the mosaic. 
   * In case of special pixels and the special pixel flags being enabled, the details 
   * for each priority is described below.
   *  
   * This class also has the ability to track the origin of the pixel by storing
   * the input image names in a table and assigning an index to each unique image
   * in the order they were placed on the mosaic. If the priority is input or mosaic
   * then tracking is done only for single band input image. Band priority can track
   * the origin for multiple bands based on a specified band based on the criteria
   * (Lesser or Greater). The specified band can be a number or any keyword in the label.
   *
   * There are also options to copy High and Low Saturation both for Instrument and
   * Representation and Null DN values from the input to the mosaic. These options are
   * valid for only input(top) and band priorities.
   *
   * Following are the values for the origin band depending on the pixel type
   * --------------------------------------------------------------
   * Pixel Type  Default     Start Value  Max Value  Total Images
   *  (Bits)    (No  Origin)                           Supported
   * --------------------------------------------------------------
   *   32       -16777216    -16777215    16777216   33549932
   *   16       -32768       -32767       32767      65535
   *   8         0            1           255        255
   *
   *
   * Tags: F(FALSE), T(TRUE), V(VALID), S(SPECIAL PIXEL HS, LS, NULL), N(NULL),
   * I(INPUT), M(MOSAIC), HL(HS,LS)
   *
   * For priority=mosaic (beneath), the input is placed beneath the mosaic i.e. the
   * input pixel will be copied to the mosaic only if the mosaic pixel is NULL.
   * ------------------------
   * Input  Mosaic  Output
   * ------------------------
   *   V      N       I
   *   S,V    HL,V    M
   *
   * For priority=input(on top), following is the criteria for pixel assignment:
   * -------------------------------------
   * ---Options---    ---Images----
   * HS     LS  NULL  Input  Mosaic  Output
   * -------------------------------------
   *  F     F     F     V      S,V     I
   *  F     F     F     S      S,V     M
   *  T OR  T OR  T     V      S,V     I
   *  T OR  T OR  T     S      S,V     I(H,L,N)
   *
   *
   * For priority=band, following is the criteria for pixel assignment:
   * -----------------------------------------------------
   * ---Options---   ---Images----
   * HS    LS  NULL  Input  Mosaic  Output
   * -----------------------------------------------------
   *  F    F     F     V      V      Criteria based
   *  F    F     F     V      S      I
   *  F    F     F     S      S,V    M
   *  T OR T OR  T     S      S,V    I(H,L,N)
   *  T OR T OR  T     V      V      Criteria based
   *  T OR T OR  T     V      S      I
   *
   * For priority=average, following is the criteria for pixel assignment:
   * -----------------------------------------------------
   * ---Options---   ---Images----
   * HS    LS  NULL  Input  Mosaic  Output       Count
   * -----------------------------------------------------
   *  F    F     F     V      V      Average     count++
   *  F    F     F     V      S      I            1
   *  F    F     F     S      S      M            0
   *  F    F     F     S      V      M           count
   *  T OR T OR  T     S      S,V    I(H,L,N)     0
   *  T OR T OR  T     V      V      Average     count++
   *  T OR T OR  T     V      S      I            1
   *  
   *  
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2003-04-28 Stuart Sides
   *
   * @internal
   *  @history 2003-04-28 Stuart Sides  - Modified unitTest.cpp to do a better test
   *  @history 2003-09-04 Jeff Anderson - Added SetInputWorkCube method
   *  @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2006-09-01 Elizabeth Miller - Added BandBinMatch option to
   *                                         propagate the bandbin group to the
   *                                         mosaic and make sure the input cube
   *                                         bandbin groups match the mosaics
   *                                         bandbin group
   *  @history 2006-10-20 Stuart Sides - Fixed bug BandBin group did not get
   *                                     copied to the output mosaic.
   *  @history 2008-10-03 Steven Lambright - Fixed problem where member variables
   *                                         could be corrupted
   *  @history 2009-09-30 Sharmila Prasad - Added capability to track the pixel origin.
   *                                        Priorities Top and Beneath can track origin
   *                                        for a single band input image only. Band
   *                                        priority can track origin of a multi-band
   *                                        input image based on a particular band.
   *                                        Also Band placement is flexible and any bands
   *                                        from the input cube can fit within the output
   *                                        mosaic. Also ability to allow HS, LS or NULL pixels
   *                                        from input to the mosaic(output). Added table for
   *                                        Origin Default values based on pixel type
   *  @history 2010-02-25 Sharmila Prasad - Changed stricmp to use iString function "Equal"
   *  @history 2010-10-21 Sharmila Prasad - The BandBin group must be carried thru to the mosaic
   *                                        at creation time regardless of matchbandbin flag
   *  @history 2011-01-18 Sharmila Prasad - Added "Average" priority feature, to double
   *           the number of mosaic bands to get Count info
   *  @history 2011-01-24 Sharmila Prasad - API to match DEM and also to add new group "mosaic"
   *           to hold ShapeModel attributes for the mosaic if Flag is Enabled
   *  @history 2011-09-23 Steven Lambright - Fixed table resizing code to not
   *                          do nothing and eventually cause very, very bad
   *                          things to happen (writing out of array bounds).
   *                          Fixes #410.
   *  @history 2011-10-20 Sharmila Prasad - Fixes #0000462, allow Band Priority even if Tracking
   *                                        is not enabled
   *  @history 2011-12-30 Sharmila Prasad - Fixed #00587, Disable Tracking for multiband mosaic for
   *                      ontop or beneath priority
   *  @todo 2005-02-11 Stuart Sides - add coded example and implementation example
   *                                  to class documentation
   */

/** 
 * Constants
 */
#define SRC_IMAGE_TBL  "InputImages"
#define FLOAT_MAX      16777216       //!< Max number a Floating point will hold
#define FLOAT_MIN      -16777215      //!< Min number a Floating point will hold

  class ProcessMosaic : public Isis::Process {

    public:

      /** 
       * structure for origin tracking information
       * from the GUI-user set parameters 
       * used for internal processing 
       */ 
      typedef struct {
        bool bTrack;   //!< Tracking Status Flag
        bool bCreate;  //!< Create Mosaic Flag
        int  iBandNum; //!< Band Number for Band Priority
        std::string  sKeyName;  //!< Band Property - Original band, FilterName etc
        std::string  sKeyValue; //!< Band Value
        BandCriteria eCriteria; //!< Lesser or Greater Criteria
        int  iInBand;  //!< input  band index for the corresponding band in KeyValue
        int  iOutBand; //!< output band index for the corresponding band in KeyValue
      }MosaicOptions;

      /**
       * Enumeration for different Mosaic priorities 
       * (input, mosaic, band) 
       */
      enum MosaicPriority {
        input,  //!< ontop priority
        mosaic, //!< beneath priority
        band,   //!< band priority
        average //!< average priority
      };

      //! Constructs a Mosaic object
      ProcessMosaic();

      //!  Destroys the Mosaic object. It will close all opened cubes.
      ~ProcessMosaic();

      //! Line Processing method for one input and output cube
      void StartProcess(const int &piOutSample, const int &piOutLine, const int &piOutBand);

      //! Set input cube to specified image name at the starting and count of 
      //! samples, lines, bands
      Isis::Cube *SetInputCube(const std::string &parameter,
                               const int ss = 1, const int sl = 1,
                               const int sb = 1,
                               const int ns = 0, const int nl = 0,
                               const int nb = 0);

      //! Set input cube to specified image name with specified attributes at the 
      //! starting and count of samples, lines, bands
      Isis::Cube *SetInputCube(const std::string &fname,
                               Isis::CubeAttributeInput &att,
                               const int ss = 1, const int sl = 1,
                               const int sb = 1,
                               const int ns = 0, const int nl = 0,
                               const int nb = 0);

      //! Set output cube to specified image name
      Isis::Cube *SetOutputCube(const std::string &psParameter);

      /**
       * Sets the bandbin match parameter to the input boolean value
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @param pbFlag - The boolean value to set the bandbin match parameter
       */
      void SetBandBinMatch(bool pbFlag) {
        mbBandbinMatch = pbFlag;
      };

      /** 
       * If Tracking is set, create table and add the input file name
       *  into the table to track the mosaic origin
       */
      void SetMosaicOrigin(int &piIndex);

      //! Get the file index offset to be saved in the band by pixel type
      int GetIndexOffsetByPixelType(void);

      //! Reset the origin band
      void ResetOriginBand(void);

      //! Compare the input and mosaic for the specified band based on the 
      //! criteria and update the mosaic origin band
      void BandComparison(int piIndex, int piIns, int piInl, int piIss, 
                          int piIsl, int piOss, int piOsl);

      //! Get the default origin value based on pixel type for the origin band
      int GetOriginDefaultByPixelType(void);

      //! Get the Band Index in an image of type (input/output)
      int GetBandIndex(const FileType &peFileType);

      //! Checks for the table with name "InputImage"
      bool GetTrackStatus(void);

      /**
       * Get the configured Priority
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @return MosaicPriority 
       */
      MosaicPriority GetPriority(void){
        return mePriority;
      };
      
      //! New mosaic, add the Band Bin group specific to the mosaic
      void AddBandBinGroup(int piIsb, int piOsb);

      //! Default BandBin group if Match BandBin is set to False
      void AddDefaultBandBinGroup(void);

      //! Mosaic exists, match the band with the input image
      void MatchBandBinGroup(const int piIsb, const int piOsb, int &piInb);
      
      //! Process average priority
      bool ProcessAveragePriority(int piPixel, Portal& pInPortal, Portal& pOutPortal, Portal& pOrigPortal);

      //! Reset all the count bands to default at the time of creation
      void ResetCountBands(void);
      /** 
       * Get/Set HS,LS, NULL Flags. If set true and if the input image has a
       * Special Pixel then it is copied to the mosaic irrespective of any 
       * condition. Not supported in mosaic priority
       */ 
      
       /**
        * Set HS Flag
        * 
        * @author Sharmila Prasad (1/19/2011)
        * 
        * @param pbFlag 
        */
       void SetHighSaturationFlag(bool pbFlag) {
         mbHighSat = pbFlag;
       } ;
       
       /**
        * Set LS Flag
        * 
        * @author Sharmila Prasad (1/19/2011)
        * 
        * @param pbFlag 
        */
       void SetLowSaturationFlag(bool pbFlag) {
         mbLowSat  = pbFlag;
       } ;
       
       /**
        * Set NULL Flag
        * 
        * @author Sharmila Prasad (1/19/2011)
        * 
        * @param pbFlag 
        */
       void SetNullFlag(bool pbFlag) {
         mbNull    = pbFlag;
       } ;
       
       /**
        * Get HS Flag
        * 
        * @author Sharmila Prasad (1/19/2011)
        * 
        * @return bool 
        */
       bool GetHighSaturationFlag(void) {
         return mbHighSat;
       } ;

       /**
        * Get LS Flag
        * 
        * @author Sharmila Prasad (1/19/2011)
        * 
        * @return bool 
        */
       bool GetLowSaturationFlag(void) {
         return mbLowSat;
       };
       
       /**
        * Get NULL Flag
        * 
        * @author Sharmila Prasad (1/19/2011)
        * 
        * @return bool
        */
       bool GetNullFlag(void) {
         return mbNull;
       };

      /**
       * Set the priority input, mosaic, band, average
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @param pePriority - one of the four priorities
       */
      void SetPriority(MosaicPriority pePriority) {
        mePriority = pePriority;
      };

      /**
       * Enable/Disable Match DEM's
       * 
       * @author Sharmila Prasad (1/24/2011)
       * 
       * @param pbMatchDEM 
       */
      void SetMatchDEM(bool pbMatchDEM) {
        mbMatchDEM = pbMatchDEM;
      }
      
      //! Match DEM between Input & Mosaic if MatchDEM Flag is enabled
      void MatchDEMShapeModel(void);
      
      /**
       * Set/Get the Track Flag
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @param pbFlag - set the tracking flag 
       */
      void SetTrackFlag(bool pbFlag) {
        mMosaicOptions.bTrack = pbFlag;
      };
      
      //! Get Track Flag status
      bool GetTrackFlag(void) {
        return mMosaicOptions.bTrack;
      };

      /**
       * Flag to indicate to the Process that the mosaic is being newly created
       * Indication that the new label specific to the mosaic needs to be created.
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @param pbFlag - Set Create Flag True/False
       */
      void SetCreateFlag(bool pbFlag) {
        mMosaicOptions.bCreate  = pbFlag;
      };

      /**
       * Set the Band Number for priority Band
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @param piBandNum - Band Number 
       */
      void SetBandNumber(int piBandNum) {
        mMosaicOptions.iBandNum = piBandNum;
      };

      /**
       * Set the keyword for priority Band
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @param psKeyName - Band Property
       * @param psKeyValue - Band Value
       */
      void SetBandKeyWord(std::string psKeyName, std::string psKeyValue) {
        mMosaicOptions.sKeyName = psKeyName;
        mMosaicOptions.sKeyValue = psKeyValue;
      };

      /**
       * Set the Band Criteria Lesser/Greater than for Band priority
       * 
       * @author Sharmila Prasad (1/19/2011)
       * 
       * @param peCriteria - Band Criteria
       */
      void SetBandCriteria(BandCriteria peCriteria) {
        mMosaicOptions.eCriteria = peCriteria;
      };

      //! Debugging
      void Test(void);

      //! Get the input file Line location in the mosaic
      int GetInputStartLineInMosaic(void) {
        return miOsl;
      };
      
      //! Get the input file Sample location in the mosaic
      int GetInputStartSampleInMosaic(void) {
        return miOss;
      };
      
      //! Get the input file Band location in the mosaic
      int GetInputStartBandInMosaic(void) {
        return miOsb;
      };

    private:
      int p_iss; //!< The starting sample within the input cube
      int p_isl; //!< The starting line within the input cube
      int p_isb; //!< The starting band within the input cube
      int p_ins; //!< The number of samples from the input cube
      int p_inl; //!< The number of lines from the input cube
      int p_inb; //!< The number of bands from the input cube

      int miOss; //!< The starting sample within the output cube
      int miOsl; //!< The starting line within the output cube
      int miOsb; //!< The starting band within the output cube
      /**
       * True/False value to determine whether to enforce the input cube
       * bandbin matches the mosaic bandbin group
       */
      bool mbBandbinMatch;

      //! Flag to indicate whether DEM of the input and mosaic should match
      bool mbMatchDEM;
      
      //! Set the priority to input(ontop), mosaic(beneath) or band
      MosaicPriority mePriority;

      /**
       * Set the Special Pixels Flags to True/False.
       * True- allow the special pixel to be passed onto the mosaic. 
       * Holds good for input and band priority
       */
      bool mbHighSat; //!< HS Flag
      bool mbLowSat;  //!< LS Flag
      bool mbNull;    //!< NULL Flag

      MosaicOptions mMosaicOptions; //!< Structure holding the tracking info
  };
};

#endif

