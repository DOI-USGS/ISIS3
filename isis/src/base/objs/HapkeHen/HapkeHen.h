#ifndef HapkeHen_h
#define HapkeHen_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2008/11/05 23:38:50 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include <string>
#include "HapkePhotoModel.h"

namespace Isis {
  class Pvl;

/**
 * @brief Hapke-Henyey-Greenstein photometric model.
 *  Derive model albedo using complete Hapke model with
 *  Henyey-Greenstein single-particle phase function
 *  whose coefficients are hg1 and hg2, plus single
 *  scattering albedo wh, opposition surge parameters
 *  hh and b0, and macroscopic roughness theta.
 *
 * @ingroup RadiometricAndPhotometricCorrection
 * @author 1989-08-02 Unknown
 * @history 2008-01-14 Janet Barret - Imported into Isis3 from 
 *          Isis2.
 * @history 2008-03-07 Janet Barrett - Moved variables and related
 *                     methods that pertain to Hapke specific parameters
 *                     to the PhotoModel class.
 * @history 2008-06-18 Stuart Sides - Fixed doc error 
 * @history 2008-10-17 Steven Lambright - Changed inheritance, moved 
 *              HapkeHen specific methods into this class from
 *              the base class.
 * @history 2008-11-05 Jeannie Walldren - Added documentation 
 *          from Isis2 files
 *
 */
  class HapkeHen : public HapkePhotoModel {
    public:
      HapkeHen (Pvl &pvl);
      virtual ~HapkeHen() {};
      
      void SetPhotoHg1(const double hg1);
      //! Return photometric Hg1 value
      inline double PhotoHg1 () const { return p_photoHg1; };

      void SetPhotoHg2(const double hg2);
      //! Return photometric Hg2 value
      inline double PhotoHg2 () const { return p_photoHg2; };

    protected:
      virtual double PhotoModelAlgorithm (double phase, double incidence,
            double emission);

    private:
      double p_photoHg1;
      double p_photoHg2;
      double p_photoCott;
      double p_photoCot2t;
      double p_photoTant;
      double p_photoSr;
      double p_photoOsr;
  };
};

#endif
