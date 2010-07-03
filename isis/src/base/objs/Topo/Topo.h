#ifndef Topo_h
#define Topo_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2008/07/08 18:54:49 $                                                                 
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

#include "NormModel.h"

namespace Isis {
  class Pvl;

/**
 * @brief Topographic derivative of an arbitrary photometric function
 *
 * @author 1999-01-08 Randy Kirk
 *
 * @internal
 *  @history 2007-08-15 Steven Lambright Refactored and fixed unit test
 *  @history 2008-06-18 Steven Lambright Fixed ifndef, removed endlink doxygen
 *           command
 */
  class Topo : public NormModel {
    public:
      Topo (Pvl &pvl, PhotoModel &pmodel);
      virtual ~Topo() {};

    protected:
      virtual void NormModelAlgorithm (double pha, double inc, double ema,
          double dn, double &albedo, double &mult, double &base);
      virtual void NormModelAlgorithm (double pha, double inc, double ema,
          double deminc, double demema, double dn, double &albedo,
          double &mult, double &base) {};

    private:

      void SetNormIncref(const double incref);
      void SetNormThresh(const double thresh);
      void SetNormAlbedo(const double albedo);

      double p_normIncref;
      double p_normThresh;
      double p_normAlbedo;

  };
};

#endif
