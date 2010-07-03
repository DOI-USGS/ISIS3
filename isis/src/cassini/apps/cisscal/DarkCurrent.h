#ifndef DARKCURRENT_H
#define DARKCURRENT_H
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/05/27 21:26:15 $
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
#include <vector>
#include "Filename.h"

using namespace std;

namespace Isis {
  class CissLabels;
/**                                                                       
 * @brief Compute Cassini ISS dark current subtraction
 *  
 * This class was created in order to perform necessary 
 * calculations for computing the two-dimensional dark current 
 * array to be subtracted from Cassini ISS images during 
 * calibration using the <B>Isis cisscal</B> application.
 *                                                                        
 * @ingroup Cassini
 * @author 2008-11-05 Jeannie Walldren 
 * @internal 
 *  @history 2008-11-05 Jeannie Walldren - Original Version
  * @history 2009-01-26 Jeannie Walldren - Changed declarations
  *            of 2 dimensional vectors
 *  @history 2009-05-27 Jeannie Walldren - Added
 *           p_flightSoftware variable.  Updated
 *           ComputeLineTime() code with algorithm from the new
 *           version of linetime.pro in idl cisscal 3.6.  Fixed
 *           instrument data rate value in the constructor.
 */                                                                       
  class DarkCurrent {
    public:
      // implied open file
      DarkCurrent (CissLabels &cissLab);
      ~DarkCurrent (){};  //!< Empty Destructor

      vector <vector <double> > ComputeDarkDN();
      Filename BiasDistortionTable(){return p_bdpath;};      //!<Retrieves the name of the bias distortion table
      Filename DarkParameterFile()  {return p_dparamfile;};  //!<Retrieves the name of the dark parameters file
      

  private:
    double ComputeLineTime(int lline);
    void   FindDarkFiles();
    void   ComputeTimeArrays();
    vector <vector <double> > MakeDarkArray();
    vector <vector <double> > MakeManyLineDark(Brick &darkBrick);

  
    int p_lines;            //!< Number of lines in the image.
    int p_samples;          //!< Number of samples in the image.
    Filename p_bdpath;      //!< Bias distortion table for the image.  Only exists for narrow camera images.
    Filename p_dparamfile;  //!< Dark parameters file for the image.

    //LABEL VARIABLES
    int p_btsm;                   //!< Value dependent upon <b>PvlKeyword</b> DelayedReadoutFlag. Valid values are: "No"=0, "Yes"=1, "Unknown"=-1.  Called "botsim" or "btsm" in IDL code.
    double p_compRatio;           //!< Value of <b>PvlKeyword</b> CompressionRatio from the labels of the image.  Called "ratio" in IDL code.
    string p_compType;            //!< Value of <b>PvlKeyword</b> CompressionType from the labels of the image.  Called "comp" in IDL code.
    string p_dataConvType;        //!< Value of <b>PvlKeyword</b> DataConversionType from the labels of the image.  Called "conv" in IDL code.
    double p_expDur;              //!< Value of <b>PvlKeyword</b> ExposureDuration from the labels of the image.  Called "exposure" or "time" in IDL code.
    iString p_flightSoftware;     //!< Value of <b>PvlKeyword</b> FlightSoftwareVersion from the labels of the image.  Called "fsw" in IDL code.
    int p_gainMode;               //!< Value of <b>PvlKeyword</b> GainModeId from the labels of the image.
    double p_instDataRate;        //!< Value of <b>PvlKeyword</b> InstrumentDataRate from the labels of the image.
    bool p_narrow;                //!< Indicates whether the image is from a narrow-angle camera
    int p_readoutIndex;           //!< Value of <b>PvlKeyword</b> InstrumentDataRate from the labels of the image.  Called "rdind" or "roindex" in IDL code.
    int p_readoutOrder;           //!< Value of <b>PvlKeyword</b> ReadoutOrder from the labels of the image. Valid values are: NAC first = 0, WAC first = 1.  Called "roo" in IDL code.
    iString p_sum;                //!< Summing mode, as found in the labels of the image.  This integer is created as an iString so that it may be added to a string.  Called "sum" in IDL code.
    int p_telemetryRate;          //!< Telemetry rate of the image in packets per second.  This is dependent on the range of the instrument data rate.  Called "cdsr" in IDL code.

    vector <vector <double> > p_startTime;    //!< Array of start times for each pixel of the image.
    vector <vector <double> > p_endTime;      //!< Array of end times for each pixel of the image.
    vector <vector <double> > p_duration;     //!< Array of durations for each pixel of the image.
  };
};
#endif

