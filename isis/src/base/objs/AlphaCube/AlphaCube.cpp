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
#include "AlphaCube.h"

using namespace std;
namespace Isis {
  //! Constructs an AlphaCube object using a PVL object.
  AlphaCube::AlphaCube (Isis::Pvl &pvl) {
    Isis::PvlObject &isiscube = pvl.FindObject("IsisCube");
    if (isiscube.HasGroup("AlphaCube")) {
      Isis::PvlGroup &alpha = isiscube.FindGroup("AlphaCube");
      p_alphaSamples        = alpha["AlphaSamples"];
      p_alphaLines          = alpha["AlphaLines"];
      p_alphaStartingSample = alpha["AlphaStartingSample"];
      p_alphaStartingLine   = alpha["AlphaStartingLine"];
      p_alphaEndingSample   = alpha["AlphaEndingSample"];
      p_alphaEndingLine     = alpha["AlphaEndingLine"];
      p_betaSamples         = alpha["BetaSamples"];
      p_betaLines           = alpha["BetaLines"];
    }
    else {
    Isis::PvlGroup &dims = isiscube.FindGroup("Dimensions",Isis::Pvl::Traverse);
      p_alphaSamples        = dims["Samples"];
      p_alphaLines          = dims["Lines"];
      p_alphaStartingSample = 0.5;
      p_alphaStartingLine   = 0.5; 
      p_alphaEndingSample   = (double) p_alphaSamples + 0.5; 
      p_alphaEndingLine     = (double) p_alphaLines + 0.5;
      p_betaSamples         = p_alphaSamples;
      p_betaLines           = p_alphaLines;
    }
  
    ComputeSlope();
  }

  /** Constructs an AlphaCube object with a basic mapping from corner-to-corner, 
   * beta 0.5,0.5 maps to alpha 0.5,0.5 and beta ns+0.5,nl+0.5 maps to alpha 
   * ns+0.5, nl+0.5.
   */ 
  AlphaCube::AlphaCube (int alphaSamples, int alphaLines,
                                int betaSamples, int betaLines,
                                double alphaSs, double alphaSl, 
                                double alphaEs, double alphaEl) {
    p_alphaSamples        = alphaSamples;
    p_alphaLines          = alphaLines;
    p_alphaStartingSample = alphaSs;
    p_alphaStartingLine   = alphaSl; 
    p_alphaEndingSample   = alphaEs; 
    p_alphaEndingLine     = alphaEl;
  
    p_betaSamples = betaSamples;
    p_betaLines = betaLines;
  
    ComputeSlope();
  }

  /** Constructs an AlphaCube object given alphaSamples, alphaLines, 
   *  betaSamples and betaLines.
   */
  AlphaCube::AlphaCube (int alphaSamples, int alphaLines,
                                int betaSamples, int betaLines) {
    p_alphaSamples        = alphaSamples;
    p_alphaLines          = alphaLines;
    p_alphaStartingSample = 0.5;
    p_alphaStartingLine   = 0.5; 
    p_alphaEndingSample   = (double) alphaSamples + 0.5; 
    p_alphaEndingLine     = (double) alphaLines + 0.5;
  
    p_betaSamples = betaSamples;
    p_betaLines = betaLines;
  
    ComputeSlope();
  }

/**                                                   
 * Merges two AlphaCube objects. This facilities combinations of programs 
 * crop-enlarge, crop-crop, reduce-pad, etc.                                                    
 *                                                    
 * @param add The AlphaCube object to be merged.       
 */                                                     
  void AlphaCube::Rehash (AlphaCube &add) {
    double sl = AlphaLine(add.AlphaLine(0.5));
    double ss = AlphaSample(add.AlphaSample(0.5));
    double el = AlphaLine(add.AlphaLine(add.BetaLines()+0.5));
    double es = AlphaSample(add.AlphaSample(add.BetaSamples()+0.5));
  
    p_alphaStartingLine = sl;
    p_alphaStartingSample = ss;
    p_alphaEndingLine = el;
    p_alphaEndingSample = es;
    p_betaLines = add.BetaLines();
    p_betaSamples = add.BetaSamples();
  
    ComputeSlope();
  }

/**                                                   
 * Writes or update the Alpha keywords (AlphaLines, AlphaSamples, 
 * AlphaStartingSamples, etc) in the proper group in a PVL object. It chooses 
 * first to write and AlphaGroup if the Mapping group exists.                                                                                                                        
 * If not then is will write the keywords in the Instrument group if it exists. 
 * Otherwise it will write to the AlphaCube group. 
 * 
 * @param &pvl The PVL object to write to.
 *                                                    
 */                                                 
  void AlphaCube::UpdateGroup (Isis::Pvl &pvl) {
    // If we have a mapping group we do not want to update the alpha cube
    // group as it represents the dimensions and sub-area of the raw instrument
    // cube
    Isis::PvlObject &cube = pvl.FindObject("IsisCube");
    if (cube.HasGroup("Mapping")) return;
    
    // Add the labels to the pvl
    if (cube.HasGroup("AlphaCube")) {
      AlphaCube temp(pvl);
      temp.Rehash(*this);
      *this = temp;
  
      Isis::PvlGroup &alpha = cube.FindGroup("AlphaCube");
      alpha["AlphaSamples"] = p_alphaSamples;
      alpha["AlphaLines"] = p_alphaLines;
      alpha["AlphaStartingSample"] = p_alphaStartingSample;
      alpha["AlphaStartingLine"] = p_alphaStartingLine;
      alpha["AlphaEndingSample"] = p_alphaEndingSample;
      alpha["AlphaEndingLine"] = p_alphaEndingLine;
      alpha["BetaSamples"] = p_betaSamples;
      alpha["BetaLines"] = p_betaLines;
    }
    else {
      Isis::PvlGroup alpha("AlphaCube");
      alpha += Isis::PvlKeyword("AlphaSamples",p_alphaSamples);
      alpha += Isis::PvlKeyword("AlphaLines",p_alphaLines);
      alpha += Isis::PvlKeyword("AlphaStartingSample",p_alphaStartingSample);
      alpha += Isis::PvlKeyword("AlphaStartingLine",p_alphaStartingLine);
      alpha += Isis::PvlKeyword("AlphaEndingSample",p_alphaEndingSample);
      alpha += Isis::PvlKeyword("AlphaEndingLine",p_alphaEndingLine);
      alpha += Isis::PvlKeyword("BetaSamples",p_betaSamples);
      alpha += Isis::PvlKeyword("BetaLines",p_betaLines);
      cube.AddGroup(alpha);
    }
  }

  /** 
   * Computes the line and sample slopes. (Starting and ending 
   * alpha lines/samples divided by the number of beta lines/samples).
   * 
   * @internal
   * @todo Why the +0.5 and -0.5?
   */
  void AlphaCube::ComputeSlope () {
    p_lineSlope = double (p_alphaEndingLine - p_alphaStartingLine) /
                  double ((p_betaLines + 0.5)       - 0.5);
    p_sampSlope = double (p_alphaEndingSample - p_alphaStartingSample) /
                  double ((p_betaSamples + 0.5)       - 0.5);
  }
}
