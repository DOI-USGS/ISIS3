/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:06 $                                                                 
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
#if !defined(AlphaCube_h)
#define AlphaCube_h

#include "Pvl.h"

namespace Isis {
/**                                                                       
 * @brief This class is used to rewrite the "alpha" keywords out of the 
 * AlphaCube group or Instrument group.                                                                  
 *                                                                         
 * This class allows programmers to map cube pixel positions back to the first 
 * cube they came from.This is critical for camera models or radiometric models 
 * in order to map input cube pixels to camera detector position.                                                                                                                                                                                           
 * The alpha keywords are automatically generated in programs like crop, pad, 
 * reduce, and enlarge.                           
 *                                                                        
 * @ingroup LowLevelCubeIO                                                 
 *                                                                        
 * @author 2004-02-13 Jeff Anderson                                         
 *                                                                        
 * @internal
 *   @history 2004-02-13 Jeff Anderson - Added another constructor and 
 *   refactored UpdateGroup method.     
 *   @history 2004-06-03 Jeff Anderson - Fixed bug in UpdateGroup which 
 *   occured when a cube label did not already have a AlphaCube group.
 *   @history 2005-02-14 Leah Dahmer - Modified file to support Doxygen
 *   documentation.
 * 
 *   @todo 2005-04-06 Add coded example.
 */
 class AlphaCube {
    public:
    AlphaCube (Isis::Pvl &pvl);
    AlphaCube (int alphaSamples, int alphaLines,
               int betaSamples, int betaLines);
    AlphaCube (int alphaSamples, int alphaLines,
               int betaSamples, int betaLines,
               double alphaSs, double alphaSl, 
               double alphaEs, double alphaEl);
    //! Destroys the AlphaCube object.
    ~AlphaCube () {};

    //! Returns the number of lines in the alpha cube.
      inline int AlphaLines () const { return p_alphaLines; };
    //! Returns the number of samples in the alpha cube.
      inline int AlphaSamples () const { return p_alphaSamples; };
    //! Returns the number of lines in the beta cube.
      inline int BetaLines () const { return p_betaLines; };
    //! Returns the number of samples in the beta cube.
      inline int BetaSamples () const { return p_betaSamples; };
    //! Returns an alpha line given a beta line.
      inline double AlphaLine(double betaLine) {
        return p_lineSlope * (betaLine - 0.5) + p_alphaStartingLine;
      }
    //! Returns an alpha sample given a beta sample.
      inline double AlphaSample(double betaSample) {
        return p_sampSlope * (betaSample - 0.5) + p_alphaStartingSample;
      }
    //! Returns a beta line given an alpha line.
      inline double BetaLine(double alphaLine) {
        return  (alphaLine - p_alphaStartingLine) / p_lineSlope + 0.5;
      }
    //! Returns a beta sample given an alpha sample.
      inline double BetaSample(double alphaSample) {
        return  (alphaSample - p_alphaStartingSample) / p_sampSlope + 0.5;
      }
  
      void UpdateGroup (Isis::Pvl &pvl);
  
      void Rehash (AlphaCube &alphaCube);
  
    private:
      void ComputeSlope();
      int p_alphaLines; /**< The number of alpha lines in the cube. */
      int p_alphaSamples; /**< The number of alpha samples in the cube.*/
      int p_betaLines; /**< The number of beta lines in the cube. */
      int p_betaSamples; /**< The number of beta samples in the cube. */
      double p_alphaStartingLine; /**< The starting alpha line. */
      double p_alphaStartingSample; /**< The starting alpha sample. */
      double p_alphaEndingLine; /**< The ending alpha line. */
      double p_alphaEndingSample; /**< The ending alpha sample. */
      double p_lineSlope; /**< The slope of the line. */
      double p_sampSlope; /**< The slope of the sample set. */
  };
};

#endif
