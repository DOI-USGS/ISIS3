#ifndef CalParameters_h
#define CalParameters_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/01/30 22:12:24 $                                                                 
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

/**                                                                       
 * @brief Sets up calibration parameters for Viking images                
 *                                                                        
 * This class sets up necessary calibration parameters for a given Viking input
 * image.  It is a helper class for the vikcal application.
 * 
 * If you would like to see CalParameters
 * being used in implementation, see vikcal.cpp                                      
 *                                                                        
 * @ingroup Viking                                                  
 *                                                                        
 * @author 2005-05-18 Elizabeth Ribelin    
 * 
 * @internal
 *   @history 2005-11-15 Elizabeth Miller - Fixed problems caused by the split
 *                                          of the viking data area
 *                                         
 */                                                                       
namespace Isis {
  class CalParameters {
    public:
      // Constructor
      CalParameters(const std::string &fname);

     /**
      * Calculates and returns time based offset at specified line and sample
      * location
      * 
      * @return double iTime Based Offset
      */
      inline double TimeBasedOffset(int l, int s) {
        double off = p_A * l + p_B * l * l + p_C * s + p_D * l * s + p_E;
        off += p_off_off;
        return off;
      }

     /**
      * Returns the w0 value found in the vikcal.sav file
      * 
      * @return double w0
      */
      inline double Omega0() { return p_w0; }

     /**
      * Returns distance value found in the vikcal.sav file
      * 
      * @return double Approximate distance from the sun
      */
      inline double Distance() { return p_dist; }

     /**
      * Returns the gain value found in the vikcal.sav file
      * 
      * @return double Gain
      */
      inline double Gain() { return p_gain; }

     /**
      * Returns the offset value found in the vikcal.sav file
      * 
      * @return double Offset
      */
      inline double Offset() { return p_off_off; }

     /**
      * Returns the exposure value found in the vikcal.sav file added to the 
      * exposure value found in the input files label
      * 
      * @return double Exposure
      */
      inline double Exposure() { return p_exp + p_labexp; }

     /**
      * Returns the gain file found in the vikcal.sav file
      * 
      * @return string Gain file
      */
      inline std::string GainFile() { return p_gainFile; }

     /**
      * Returns the offset file found in the vikcal.sav file
      * 
      * @return string Offset file
      */
      inline std::string OffsetFile() { return p_offsetFile; }

     /**
      * Returns the linearity correction value found in the viklin.sav file
      * 
      * @return double Linearity Correction (sometimes refered to as B)
      */
      inline double LinearityCorrection() { return p_b; }

     /**
      * Returns the linearity power value found in the viklin.sav file
      * 
      * @return double Linearity Power (sometimes refered to as K)
      */
      inline int LinearityPower() { return p_k; }

     /**
      * Returns the normalizing power value found in the viklin.sav file 
      * (usually 128)
      * 
      * @return double Normalizing Power 
      */
      inline double NormalizingPower() { return p_normpow; }

     /**
      * Returns the A coefficient value calculated from data found in the 
      * vikoff.sav file
      * 
      * @return double A Coefficient
      */
      inline double Acoeff() { return p_A; }

     /**
      * Returns the B coefficient value calculated from data found in the 
      * vikoff.sav file
      * 
      * @return double B Coefficient
      */
      inline double Bcoeff() { return p_B; }

     /**
      * Returns the C coefficient value calculated from data found in the 
      * vikoff.sav file
      * 
      * @return double C Coefficient
      */
      inline double Ccoeff() { return p_C; }

     /**
      * Returns the D coefficient value calculated from data found in the 
      * vikoff.sav file
      * 
      * @return double D Coefficient
      */
      inline double Dcoeff() { return p_D; } 

     /**
      * Returns the E coefficient value calculated from data found in the 
      * vikoff.sav file
      * 
      * @return double E Coefficient
      */
      inline double Ecoeff() { return p_E; }

     /**
      * Calculates and returns Omega1 from the estimated and calculated 
      * distances from the sun
      * 
      * @return double w1
      */
      inline double Omega1() { 
        double w1 = p_w0 * ((p_dist * p_dist)/(p_dist1 * p_dist1));
        return w1;
      }

     /**
      * Returns the calculated distance of the planet from the sun
      * 
      * @return double Distance from the planet to the Sun
      */
      inline double Dist1() { return p_dist1; }

    private:
      void vikcalSetup(std::string mission, int spn, std::string target, 
                     int cam, std::string wav, int cs1, int cs2, int cs3, int cs4);
      void viklinSetup(std::string mission, int spn, std::string target, 
                     int cam, std::string wav, int cs1, int cs2, int cs3, int cs4);
      void vikoffSetup(std::string mission, int spn, std::string target, 
                     int cam, double clock, int cs3);
      void CalcSunDist(std::string t);

      double p_labexp;           //!<Exposure Duration from cube label
      double p_w0;               //!<Omega0 from vikcal.sav file
      double p_dist;             //!<Distance from the sun from vikcal.sav file
      double p_gain;             //!<Gain from vikcal.sav file
      double p_offset;           //!<Offset from vikcal.sav file
      double p_exp;              //!<Exposure from vikcal.sav file
      std::string p_gainFile;    //!<Gain file from vikcal.sav file
      std::string p_offsetFile;  //!<Offset file from vikcal.sav file
      double p_b;                //!<Linearity correction from viklin.sav file
      int p_k;                   //!<Linearity power from viklin.sav file
      double p_normpow;          //!<Normalizing power from viklin.sav file
      double p_A;                //!<A Coefficient of iTime Based Offset Equation
      double p_B;                //!<B Coefficient of iTime Based Offset Equation
      double p_C;                //!<C Coefficient of iTime Based Offset Equation
      double p_D;                //!<D Coefficient of iTime Based Offset Equation
      double p_E;                //!<E Coefficient of iTime Based Offset Equation
      double p_dist1;            //!<Calculated distance from the sun
      int p_constOff;            //!<Flag indicating constant offset
      double p_off_off;          //!<Constant offset
  };
};
#endif   
