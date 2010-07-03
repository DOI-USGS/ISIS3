#ifndef MocLabels_h
#define MocLabels_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.8 $                                                             
 * $Date: 2010/01/05 20:56:07 $                                                                 
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

#include <map>
#include "Filename.h"


namespace Isis {
  class Pvl;
  namespace Mgs {
    /** 
     * @brief Read values from MOC labels 
     *  
     * @ingroup MarsGlobalSurveyor
     * @author 2007-01-30 Author Unknown
     *                                                                        
     * @internal                                                              
     *  @history 2008-04-30  Steven Lambright corrected infinite loop
     *  @history 2008-05-29  Steven Lambright Fixed binary search indexing,
     *          bad calls to std::string::_cstr() references
     *  @history 2008-06-18  Steven Koechle - Fixed Documentation Errors
     *   @history 2008-08-11 Steven Lambright - Fixed definition of WAGO,
     *            problem pointed out by "novas0x2a" (Support Board Member)
     *   @history 2008-11-05 Jeannie Walldren - Changed
     *            IsNarrowAngle(), IsWideAngle(), IsWideAngleBlue(),
     *            and IsWideAngleRed() to NarrowAngle(),
     *            WideAngle(), WideAngleBlue(), and WideAngleRed(),
     *            respectively.  Added documentation.
     *   @history 2008-11-07 Jeannie Walldren - Fixed documentation
     *   @history 2010-01-05 Jeannie Walldren - Fixed bug in
     *                                          InitWago() method.
     */  
    class MocLabels {
      public:
        MocLabels (Pvl &lab);
        MocLabels (const std::string &file);
        //! Empty destructor.
        ~MocLabels () {};
        
        /**
         * Indicates whether the camera was narrow angle.
         * @return @b bool True if the instrument ID is MOC-NA.
         */
        inline bool NarrowAngle () const { return p_mocNA; };
        /**
         * Indicates whether the camera was wide angle.
         * @return @b bool True if the instrument ID is MOC-WA.
         */
        inline bool WideAngle () const { return !p_mocNA; };
        /**
         * Indicates whether the camera was red wide angle.
         * @return @b bool True if the instrument ID is MOC-WA and 
         *         filter name is RED.
         */
        inline bool WideAngleRed () const { return p_mocRedWA; };
        /**
         * Indicates whether the camera was blue wide angle.
         * @return @b bool True if the instrument ID is MOC-WA and 
         *         filter name is BLUE.
         */
        inline bool WideAngleBlue () const { return p_mocBlueWA; };
        /**
         * Returns value for CrosstrackSumming from the instrument
         * group.
         * @return @b int Crosstrack summing
         */
        inline int CrosstrackSumming () const { return p_crosstrackSumming; };
        /**
         * Returns value for DowntrackSumming from the instrument group.
         * @return @b int Downtrack summing
         */
        inline int DowntrackSumming () const { return p_downtrackSumming; };
        /**
         * Returns value for FirstLineSample from the instrument group.
         * @return @b int First line sample
         */
        inline int FirstLineSample () const { return p_startingSample; };
        /**
         * Returns value for FocalPlaneTemperature from the instrument 
         * group. 
         * @return @b double Focal plane temperature
         */
        inline double FocalPlaneTemperature () const { return p_focalPlaneTemp; };
        /**
         * Returns the value for the true line rate.  This is calculated 
         * by dividing the product of LineExposureDuration and the 
         * DowntrackSumming by 1000.
         * @return @b double Value for the true line rate
         */
        inline double LineRate () const { return p_trueLineRate; };
        /**
         * Returns the value for LineExposureDuration from the 
         * instrument group.
         * @return @b double Line exposure duration
         */
        inline double ExposureDuration() const { return p_exposureDuration; };
        /**
         * Returns the value for StartTime from the instrument group.
         * @return @b std::string Start time
         */
        inline std::string StartTime() const { return p_startTime; };  
        /**
         * Returns 2048 if narrow angle and 3456 if wide angle.
         * @return @b int Value of detectors.
         */
        inline int Detectors() const { return p_mocNA ? 2048 : 3456; };
        int StartDetector(int sample) const;
        int EndDetector(int sample) const;
        double Sample(int detector) const;
        double EphemerisTime(double line) const;
        double Gain(int line=1);
        double Offset(int line=1);
    
      private:
        void Init (Pvl &lab);
        void ReadLabels (Pvl &lab);
        void ValidateLabels ();
        void Compute();
    
        int p_crosstrackSumming;
        int p_downtrackSumming;
        int p_startingSample;
        int p_orbitNumber;
        double p_exposureDuration;
        double p_trueLineRate;
        double p_focalPlaneTemp;
        bool p_mocNA;
        bool p_mocRedWA;
        bool p_mocBlueWA;
        std::string p_instrumentId;
        std::string p_filter;
        std::string p_clockCount;
        std::string p_gainModeId;
        int p_offsetModeId;
        std::string p_startTime;
        std::string p_dataQuality;
        double p_etStart;
        double p_etEnd;
    
        void InitGainMaps();
        std::map<std::string,double> p_gainMapNA;
        std::map<std::string,double> p_gainMapWA;
        double p_gain;
        double p_offset;
    
        int p_nl;
        int p_ns;
        int p_startDetector[3456];
        int p_endDetector[3456];
        double p_sample[3456];
        void InitDetectorMaps();
    
        struct WAGO {
          double et;
          double gain;
          double offset;
          inline bool operator<(const WAGO &w) const { return (et < w.et); };
          inline bool operator==(const WAGO &w) const { return (et == w.et); };
        };
        std::vector<WAGO> p_wagos;
        void InitWago();
    
        Filename p_lsk;
        Filename p_sclk;
    };
  };
};

#endif
