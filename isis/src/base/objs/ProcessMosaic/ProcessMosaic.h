#ifndef ProcessMosaic_h
#define ProcessMosaic_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Process.h"

namespace Isis {
  class Portal;

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
   *   @history 2003-04-28 Stuart Sides  - Modified unitTest.cpp to do a better test
   *   @history 2003-09-04 Jeff Anderson - Added SetInputWorkCube method
   *   @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2006-09-01 Elizabeth Miller - Added BandBinMatch option to
   *                           propagate the bandbin group to the mosaic and make sure the input
   *                           cube bandbin groups match the mosaics bandbin group
   *   @history 2006-10-20 Stuart Sides - Fixed bug BandBin group did not get
   *                           copied to the output mosaic.
   *   @history 2008-10-03 Steven Lambright - Fixed problem where member variables
   *                           could be corrupted
   *   @history 2009-09-30 Sharmila Prasad - Added capability to track the pixel origin.
   *                           Priorities Top and Beneath can track origin for a single
   *                           band input image only. Band priority can track origin of a
   *                           multi-band input image based on a particular band. Also Band
   *                           placement is flexible and any bands from the input cube can
   *                           fit within the output mosaic. Also ability to allow HS, LS
   *                           or NULL pixels from input to the mosaic(output). Added table
   *                           for Origin Default values based on pixel type
   *   @history 2010-02-25 Sharmila Prasad - Changed stricmp to use IString function "Equal"
   *   @history 2010-10-21 Sharmila Prasad - The BandBin group must be carried thru to the mosaic
   *                           at creation time regardless of matchbandbin flag
   *   @history 2011-01-18 Sharmila Prasad - Added "Average" priority feature, to double
   *                           the number of mosaic bands to get Count info
   *   @history 2011-01-24 Sharmila Prasad - API to match DEM and also to add new group "mosaic"
   *                           to hold ShapeModel attributes for the mosaic if Flag is Enabled
   *   @history 2011-09-23 Steven Lambright - Fixed table resizing code to not
   *                           do nothing and eventually cause very, very bad
   *                           things to happen (writing out of array bounds).
   *                           Fixes #410.
   *   @history 2011-10-20 Sharmila Prasad - Fixes #0000462, allow Band Priority even if Tracking
   *                           is not enabled
   *   @history 2011-12-30 Sharmila Prasad - Fixed #00587, Disable Tracking for multiband mosaic for
   *                       ontop or beneath priority
   *   @history 2012-01-05 Sharmila Prasad - Fixed #00586 Allow Band Priority with no Tracking
   *   @history 2012-07-03 Steven Lambright and Stuart Sides - Updated to better follow good coding
   *                           practices and the new Isis coding standards. Removed #define's, moved
   *                           enums into the class, and stopped modifying member variables (makes
   *                           local copies instead) in StartProcess where possible. There were
   *                           flaws in the design that made it difficult to not carry over
   *                           erroneous state from past method calls and cause difficult to
   *                           reproduce problems. Fixes #967.
   *   @history 2012-08-16 Kimberly Oyama - Added a PvlObject, m_imagePositions, to store the image
   *                           information (file name, start sample, and start line). An accessor,
   *                           imagePositions(), for the PvlObject was also added. The image
   *                           information is used as log output by automos, handmos, and mapmos.
   *                           Fixes #976.
   *   @history 2013-03-27 Jeannie Backer - Added documentation and programmer comments.
   *                           References #1248.
   *   @history 2014-03-28 Kimberly Oyama - Added check for count band when priority=average. The
   *                           mosaic apps should fail when priority=average and there is no count
   *                           band. Fixes #746.
   *   @history 2014-04-07 Kimberly Oyama - Updated the GetBandIndex() method to return the same
   *                           number when you use the band number or band name. For example, if
   *                           band 3 is the emission angle, GetBandIndex() should return 3 whether
   *                           you enter 3 or emission angle. Modified BandPriorityWithNoTracking()
   *                           so valid data is always placed on top of the mosaic regardless of
   *                           the placement criteria (priority=band). The only way to place
   *                           special or null pixels is to check those options in mapmos or
   *                           automos. Fixes #1620. Fixes #1623.
   *   @history 2014-07-23 Janet Barrett - Fixed the StartProcess method to allow the overlay of
   *                           an input file on a pre-existing output file. Fixed #751.
   *   @history 2015-01-15 Sasha Brownsberger - Added virtual keyword to several functions to ensure
   *                           successful inheritance between Process and its child classes.
   *                           Made destructor virtual. References #2215.
   *   @history 2015-10-04 Jeannie Backer - Fixed SetMosaicOrigin() method to populate the input
   *                           images table properly. Fixes #1178
   *   @history 2017-05-19 Christopher Combs - Modified unitTest.cpp to truncate paths before date
   *                           directory. Allows test to pass when not using the default data area.
   *                           Fixes #4738.
   *   @history 2018-07-30 Jesse Mapel & Summer Stapleton - Refactoring of class to create a
   *                           separate tracking cube to keep track of input images for a mosaic
   *                           instead of storing this information within the mosaic cube itself.
   *                           The mosaic cube no longer contains a tracking band or a tracking
   *                           table; it now contains a tracking group containing the name of the
   *                           tracking file. The tracking file is named
   *                           <baseMosaicFileName>_tracking.cub. This tracking cube will contain
   *                           the tracking table as well as the tracking band; it will always be
   *                           of PixelType::UnsignedInteger regardless of the pixel type of the
   *                           mosaic cube or of the input images. References #971
   *   @history 2018-08-13 Summer Stapleton - Error now being thrown with appropriate message if
   *                           user attempts to add tracking capabilities to a mosaic that already
   *                           exists without tracking. Fixes #2052.
   */

  class ProcessMosaic : public Process {

    public:
      static const char *TRACKING_TABLE_NAME;

      // see http://blog.stata.com/tag/binary/
      static const int FLOAT_STORE_INT_PRECISELY_MAX_VALUE = 16777216;
      static const int FLOAT_STORE_INT_PRECISELY_MIN_VALUE = -16777215;

      /**
       * Enumeration for different Mosaic priorities
       * (input, mosaic, band)
       */
      enum ImageOverlay {
        PlaceImagesOnTop,  //!< ontop priority
        PlaceImagesBeneath, //!< beneath priority
        UseBandPlacementCriteria,   //!< band priority
        AverageImageWithMosaic, //!< average priority
        NumImageOverlayOptions
      };

      // Constructs a Mosaic object
      ProcessMosaic();

      //  Destroys the Mosaic object. It will close all opened cubes.
      virtual ~ProcessMosaic();

      using Isis::Process::StartProcess; // make parent funtions visable

      // Line Processing method for one input and output cube
      virtual void StartProcess(const int &piOutSample, const int &piOutLine, const int &piOutBand);

      // Finish with tracking cube
      virtual void EndProcess();

      // Accessor for the placed images.
      PvlObject imagePositions();

      using Process::SetInputCube;

      // Set input cube to specified image name at the starting and count of
      // samples, lines, bands
      virtual Isis::Cube *SetInputCube(const QString &parameter,
                               const int ss, const int sl,
                               const int sb,
                               const int ns, const int nl,
                               const int nb);

      // Set input cube to specified image name with specified attributes at the
      // starting and count of samples, lines, bands
      virtual Isis::Cube *SetInputCube(const QString &fname,
                               Isis::CubeAttributeInput &att,
                               const int ss = 1, const int sl = 1,
                               const int sb = 1,
                               const int ns = -1, const int nl = -1,
                               const int nb = -1);

      // SetOutputCube() is not virtual in the Process class, so the following
      // definitions for this method are the only ones that are allowed for
      // ProcessMosaic objects and child objects, unless redifined in the
      // child class
      Isis::Cube *SetOutputCube(const QString &psParameter);
      Isis::Cube *SetOutputCube(const QString &psParameter, UserInterface &ui);

      void SetBandBinMatch(bool enforceBandBinMatch);

      void SetBandKeyword(QString bandPriorityKeyName, QString bandPriorityKeyValue);
      void SetBandNumber(int bandPriorityBandNumber);
      void SetBandUseMaxValue(bool useMax);
      void SetCreateFlag(bool createOutputMosaic);
      void SetHighSaturationFlag(bool placeHighSatPixels);
      void SetImageOverlay(ImageOverlay placement);
      void SetLowSaturationFlag(bool placeLowSatPixels);
      void SetMatchDEM(bool matchDEM);
      void SetNullFlag(bool placeNullPixels);
      void SetTrackFlag(bool trackingEnabled);

      bool GetHighSaturationFlag() const;
      ImageOverlay GetImageOverlay() const;
      bool GetLowSaturationFlag() const;
      bool GetNullFlag() const;
      bool GetTrackFlag() const;

      int GetInputStartLineInMosaic() const;
      int GetInputStartSampleInMosaic() const;
      int GetInputStartBandInMosaic() const;

      static QString OverlayToString(ImageOverlay);
      static ImageOverlay StringToOverlay(QString);

    private:

      //Compare the input and mosaic for the specified band based on the criteria and update the
      //  mosaic origin band.
      void BandComparison(int iss, int isl, int ins, int inl,
                          int bandPriorityInputBandNumber, int bandPriorityOutputBandNumber,
                          int index);

      // Mosaicking for Band Priority with no Tracking
      void BandPriorityWithNoTracking(int iss, int isl, int isb,
                                      int ins, int inl, int inb,
                                      int bandPriorityInputBandNumber,
                                      int bandPriorityOutputBandNumber);

      // Get the default origin value based on pixel type for the origin band
      int GetOriginDefaultByPixelType();

      // Get the Band Index in an image of type (input/output)
      int GetBandIndex(bool inputFile);

      // Checks for the table with name "InputImage"
      bool GetTrackStatus();

      // New mosaic, add the Band Bin group specific to the mosaic
      void AddBandBinGroup(int origIsb);

      // Default BandBin group if Match BandBin is set to False
      void AddDefaultBandBinGroup();

      // Mosaic exists, match the band with the input image
      void MatchBandBinGroup(int origIsb, int &inb);

      bool ProcessAveragePriority(int piPixel, Portal& pInPortal, Portal& pOutPortal,
                                  Portal& pOrigPortal);

      void ResetCountBands();

      // Match DEM between Input & Mosaic if MatchDEM Flag is enabled
      void MatchDEMShapeModel();

      bool m_trackingEnabled;         //!<
      Cube *m_trackingCube;           //!< Output tracking cube. NULL unless tracking is enabled.
      bool m_createOutputMosaic;      //!<
      int  m_bandPriorityBandNumber;  //!<
      QString m_bandPriorityKeyName;  //!<
      QString m_bandPriorityKeyValue; //!<
      bool m_bandPriorityUseMaxValue; //!<


      int m_iss; //!< The starting sample within the input cube
      int m_isl; //!< The starting line within the input cube
      int m_isb; //!< The starting band within the input cube
      int m_ins; //!< The number of samples from the input cube
      int m_inl; //!< The number of lines from the input cube
      int m_inb; //!< The number of bands from the input cube

      int m_oss; //!< The starting sample within the output cube
      int m_osl; //!< The starting line within the output cube
      int m_osb; //!< The starting band within the output cube
      int m_onb; //!< The number of bands in the output cube

      bool m_enforceBandBinMatch; /**< True/False value to determine whether to
                                       enforce the input cube bandbin matches
                                       the mosaic bandbin group*/

      bool m_enforceMatchDEM; //!< DEM of the input and mosaic should match

      ImageOverlay m_imageOverlay; //!<

      PvlObject m_imagePositions; //!< List of images placed on the mosaic.

      /*
       * Set the Special Pixels Flags to True/False.
       * True- allow the special pixel to be passed onto the mosaic.
       * Holds good for input and band priority
       */
      bool m_placeHighSatPixels; //!<
      bool m_placeLowSatPixels;  //!<
      bool m_placeNullPixels;    //!<
  };
};

#endif
