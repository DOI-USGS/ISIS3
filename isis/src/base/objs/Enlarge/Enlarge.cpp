/**
 * @file
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
#include "Enlarge.h"

#include <cmath>

#include "Cube.h"
#include "iException.h"
#include "PvlGroup.h"
#include "SubArea.h"

using namespace std;

namespace Isis {

  /**
   * Constructs an Enlarge object.
   * 
   * @param pInCube       - Input cube to be enlarged
   * @param sampleScale   - Sample scale
   * @param lineScale     - Line scale
   */
  Enlarge::Enlarge(Cube *pInCube, const double sampleScale, 
                   const double lineScale) {
    // Input Cube
    mInCube = pInCube;
  
    // Set input image area to defaults
    mdStartSample = 1;
    mdEndSample   = mInCube->getSampleCount();
    mdStartLine   = 1;
    mdEndLine     = mInCube->getLineCount();
  
    // Save off the sample and line magnification
    mdSampleScale = sampleScale;
    mdLineScale   = lineScale;

    miOutputSamples = (int)ceil(mInCube->getSampleCount() * mdSampleScale);
    miOutputLines   = (int)ceil(mInCube->getLineCount() * mdLineScale);
  }

  /**
   * Implementations for parent's pure virtual members 
   * Convert the requested output samp/line to an input samp/line
   * 
   * @param inSample  - Calculated input sample corresponding to output sample 
   * @param inLine    - Calculated input line corresponding to output line
   * @param outSample - Output sample
   * @param outLine   - Output line
   * 
   * @return bool 
   */
  bool Enlarge::Xform(double &inSample, double &inLine,
                      const double outSample, const double outLine) {
    inSample = (outSample - 0.5) / mdSampleScale + 0.5 + (mdStartSample - 1);
    inLine   = (outLine - 0.5) / mdLineScale + 0.5 + (mdStartLine - 1);
    
    return true;
  }

  /**
   * Sets the sub area dimensions of the input image. 
   * Default is the entire image 
   * 
   * @author Sharmila Prasad (4/14/2011)
   * 
   * @param pdStartSample - Input start sample
   * @param pdEndSample   - Input end sample
   * @param pdStartLine   - Input start line
   * @param pdEndLine     - Input end line
   */
  void Enlarge::SetInputArea(double pdStartSample, double pdEndSample, 
                             double pdStartLine, double pdEndLine) {
    // Check for the right image dimensions
    if (pdStartSample > pdEndSample || pdStartLine > pdEndLine) {
      string sErrMsg = "Error in Input Area Dimesions";
      throw iException::Message(iException::Programmer, sErrMsg, _FILEINFO_); 
    }
    
    if (pdStartSample >= 1) {
      mdStartSample = pdStartSample;
    }
    if (pdEndSample <= mInCube->getSampleCount()) {
      mdEndSample = pdEndSample;
    }
    if (pdStartLine >= 1) {
      mdStartLine = pdStartLine;
    }
    if (pdEndLine <= mInCube->getLineCount()) {
      mdEndLine = pdEndLine;
    }
    
    miOutputSamples = (int)ceil((mdEndSample - mdStartSample + 1) * mdSampleScale);
    miOutputLines   = (int)ceil((mdEndLine - mdStartLine + 1) * mdLineScale);
  }
  
  /**
   * Update the Mapping, Instrument, and AlphaCube groups in the output
   * cube label
   * 
   * @param pOutCube  - Resulting enlarged output cube 
   *  
   * @return @b PvlGroup - This is the Results group that 
   *                 will go into the application log file. This group
   *                 must be created by the calling application.
   *                 Information will be added to it if the Mapping or
   *                 Instrument groups are deleted from the output
   *                 image label
   */
  PvlGroup Enlarge::UpdateOutputLabel(Cube *pOutCube) {
    int iNumSamples= mInCube->getSampleCount();
    int iNumLines  = mInCube->getLineCount();
    // Construct a label with the results
    // This is the Results group that will go into the application
    // log file. This group must be created by the calling application.
    // Information will be added to it if the Mapping or Instrument
    // groups are deleted from the output image label
    PvlGroup resultsGrp("Results");
    resultsGrp += PvlKeyword("InputLines",     iNumLines);
    resultsGrp += PvlKeyword("InputSamples",   iNumSamples);
    resultsGrp += PvlKeyword("StartingLine",   (int)mdStartLine);
    resultsGrp += PvlKeyword("StartingSample", (int)mdStartSample);
    resultsGrp += PvlKeyword("EndingLine",     (int)mdEndLine);
    resultsGrp += PvlKeyword("EndingSample",   (int)mdEndSample);
    resultsGrp += PvlKeyword("LineIncrement",   1. / mdLineScale);
    resultsGrp += PvlKeyword("SampleIncrement", 1. / mdSampleScale);
    resultsGrp += PvlKeyword("OutputLines",     miOutputSamples);
    resultsGrp += PvlKeyword("OutputSamples",   miOutputLines);
  
    SubArea subArea;
    subArea.SetSubArea(mInCube->getLineCount(), mInCube->getSampleCount(), (int)mdStartLine, (int)mdStartSample, 
                       (int)mdEndLine, (int)mdEndSample, 1.0 / mdLineScale, 1.0 / mdSampleScale);
    subArea.UpdateLabel(mInCube, pOutCube, resultsGrp);
    
    return resultsGrp;
  }
}


