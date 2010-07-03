#ifndef ProcessExportPds_h
#define ProcessExportPds_h
/*
 *   Unless noted otherwise, the portions of Isis written by the
 *   USGS are public domain. See individual third-party library
 *   and package descriptions for intellectual property
 *   information,user agreements, and related information.
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

#include "ProcessExport.h"

namespace Isis {
  class PvlFormatPds;  

  /**
   * @brief Process class for exporting cubes to PDS standards
   * 
   * This class extends the ProcessExport class to allow the user
   * to export cubes to PDS format.
   * 
   * @history 2006-09-05 Stuart Sides - Original version
   * @history 2006-12-14 Stuart Sides - Modified keword units to be PDS complient
   * @history 2008-05-20 Steven Lambright - Fixed documentation 
   * @history 2008-08-07 Christopher Austin - Added fixed label export capability 
   * @history 2008-10-02 Christopher Austin - Fixed LabelSize() and OutputLabel() 
   *          in accordace to the pds end of line sequence requirement
   * @history 2008-12-17 Steven Lambright - Added calculations for OFFSET and 
   *          SCALEFACTOR keywords
   * @history 2009-05-31 Kris Becker - Included the number of bands in the 
   *          computation of the number of FILE_RECORDS for fixed PDS type
   *          products.  It assumed only 1 band.
   * @history 2010-02-24 Janet Barrett - Added code to support JPEG2000.
   */
  class ProcessExportPds : public Isis::ProcessExport {
    public:
      ProcessExportPds();
      ~ProcessExportPds();
      enum PdsFileType   {Image, Qube, SpectralQube, JP2Image};
      enum PdsResolution {Meter, Kilometer};
      enum PdsExportType {Stream, Fixed};	 

      // Work with PDS labels for all data set types
      void StandardAllMapping(Pvl &mainPvl);

      // Work with PDS labels for data sets of type IMAGE
      void StreamImageRoot(Pvl &mainPvl);
      void FixedImageRoot(Pvl &mainPvl);
      void StreamJP2ImageRoot(Pvl &mainPvl);
      void FixedJP2ImageRoot(Pvl &mainPvl);
      void StandardImageImage(Pvl &mainPvl);
      void StandardJP2Image(Pvl &mainPvl);

      void SetPdsResolution( PdsResolution peType) { meResolution = peType;};
      void SetExportType( PdsExportType type ) { p_exportType = type; };	  	 

      virtual Pvl& StandardPdsLabel(PdsFileType type);
      void OutputLabel(std::ofstream &os);
			void OutputDetatchedLabel(void);

			void SetDetached(bool pbDetached, const std::string psLabelFile=""){ 
				p_detachedLabel = pbDetached;
				msLabelFile = psLabelFile;
	    }
	    inline bool GetDetached(void)      { return p_detachedLabel;}	

      inline void ForceBands(bool force) { p_forceBands = force; }
      inline void ForceBandName(bool force) { p_forceBandName = force; }
      inline void ForceCenterFilterWavelength(bool force) { p_forceCenterFilterWavelength = force; }
      inline void ForceBandwidth(bool force) { p_forceBandwidth = force; }
      inline void ForceBandStorageType(bool force) { p_forceBandStorageType = force; }
      inline void ForceOffset(bool force) { p_forceOffset = force; }
      inline void ForceScalingFactor(bool force) { p_forceScalingFactor = force; }
      inline void ForceSampleBits(bool force) { p_forceSampleBits = force; }
      inline void ForceSampleBitMask(bool force) { p_forceSampleBitMask = force; }
      inline void ForceSampleType(bool force) { p_forceSampleType = force; }
      inline void ForceCoreNull(bool force) { p_forceCoreNull = force; }
      inline void ForceCoreLrs(bool force) { p_forceCoreLrs = force; }
      inline void ForceCoreLis(bool force) { p_forceCoreLis = force; }
      inline void ForceCoreHrs(bool force) { p_forceCoreHrs = force; }
      inline void ForceCoreHis(bool force) { p_forceCoreHis = force; }

    protected:
      int LineBytes();
      int LabelSize();
      virtual void CreateImageLabel();
      void CreateQubeLabel();
      void CreateSpectralQubeLabel();

      std::string ProjectionName(Pvl &inputLabel);

	    PvlFormatPds *p_formatter;
      Pvl *p_label;
	    PdsExportType p_exportType;

    private:
      PdsResolution meResolution;      
      bool p_forceBands;
      bool p_forceBandName;
      bool p_forceCenterFilterWavelength;
      bool p_forceBandwidth;
      bool p_forceBandStorageType;
      bool p_forceOffset;
      bool p_forceScalingFactor;
      bool p_forceSampleBits;
      bool p_forceSampleBitMask;
      bool p_forceSampleType;
      bool p_forceCoreNull;
      bool p_forceCoreLrs;
      bool p_forceCoreLis;
      bool p_forceCoreHrs;
      bool p_forceCoreHis;
	    bool p_detachedLabel;
	    std::string msLabelFile;
      PdsFileType p_pdsFileType;
  };
}

#endif
