#ifndef TopoAtm_h
#define TopoAtm_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
 * $Date: 2009/05/11 22:11:31 $                                                                 
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
   * @brief
   * As in the case without an atmosphere, processing proceeds
   * in three steps, a pass 1 PHOTOM followed by a divide filter to is-
   * olate topography from albedo variations followed by a pass 2 PHOTOM.
   * The first pass is intended to isolate relative albedo from overall
   * photometric shading so that the filter will work as well as possible.
   * The second pass cleans up whatever funny scaling the first pass did
   * and scales the topographic modulation to desired standard conditions;
   * as before these had best not be normal incidence, or the topography
   * will vanish!
   *
   * Also as in the no-atmosphere case, the albedo mode (with atmosphere
   * this time) is used for the first pass.  The reference geometry for
   * this pass is normal incidence with no atmosphere, i.e.,
   * INC=EMA=PHASE=TAU=0
   * The second pass is going to assume implicitly that these reference
   * values were used the first time, freeing the TAE reference parameters
   * to define the conditions of finite incidence and maybe finite optical
   * depth to which the output will be normalized.
   *
   * Figuring out the scaling that got applied to the topographic modu-
   * lation in pass 1 turns out to be tricky because of the nonlinearity
   * of the equations and the fact that the original DN (which would be
   * recoverable from the atmosphere model immediately after pass 1) is
   * lost after the divide filter.  As an approximation, I will require
   * the user to input ALBEDO, which is the average DN in the image after
   * pass 1, a measure of the average value of RHO and, given the norm-
   * alization, the average of the normal albedo in the area.
   *
   * Rather than do the calculation analytically as I did in my hand-
   * written notes, I evaluate the contrast of unit slope numerically
   * at given albedo or RHO; the first-order dependence on RHO has been
   * divided out so this is inaccurate to second order same as the other
   * derivation but it is a whole lot easier to follow).  If it turns
   * out to be desirable, we can define two values of ALBEDO for this
   * program, one describing the input (i.e., giving the albedo for this
   * one image, at which slope normalization is calculated) and the other
   * describing the output (the albedo to be used in simulating the out-
   * put image).
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2007-08-15 Steven Lambright Refactored code
   *  @history 2008-06-18 Steven Lambright Fixed ifndef command
   *  @history 2008-11-05 Jeannie Walldren - Modified references
   *           to NumericalMethods class.
   *  @history 2009-05-11 Janet Barrett - Fixed so that the NormModelAlgorithm
   *           supporting DEM input is the empty function. DEM input is not yet
   *           supported.

   */
  class TopoAtm : public NormModel {
    public:
      TopoAtm (Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel); 
      virtual ~TopoAtm() {};

    protected:
      virtual void NormModelAlgorithm (double pha, double inc, double ema,
          double dn, double &albedo, double &mult, double &base);
      virtual void NormModelAlgorithm (double pha, double inc, double ema,
          double deminc, double demema, double dn, double &albedo,
	        double &mult, double &base) {};

    private:
      void SetNormIncref(const double incref);
      void SetNormAlbedo(const double albedo);

      double p_normIncref;
      double p_normAlbedo;
      double p_normAout;
      double p_normBout;
      double p_normRhobar;
  };
};

#endif
