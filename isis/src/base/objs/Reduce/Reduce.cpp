#include "Reduce.h"
#include "iString.h"
#include "SpecialPixel.h"
#include "SubArea.h"

using namespace std;

namespace Isis {
  /**
   * Reduce Constructor
   * 
   * @author Sharmila Prasad (4/26/2011)
   * 
   * @param pInCube     - Input cube
   * @param psBands     - Bands list
   * @param sampleScale - Sample scale
   * @param lineScale   - Line scale
   */
  Reduce::Reduce(Isis::Cube *pInCube, vector<string>psBands, 
                 const double sampleScale, const double lineScale)
  {
    // Input Cube
    msBands = psBands;
    mInCube = pInCube;
    
    mdLine      = 1;
    miBandIndex = 0;
    // Set input image area to defaults
    mdStartSample = 1;
    mdEndSample   = mInCube->Samples();
    mdStartLine   = 1;
    mdEndLine     = mInCube->Lines();
  
    miInputSamples= mInCube->Samples();
    miInputLines  = mInCube->Lines();
    miOutputLines  = mInCube->Bands();
      
    // Save off the sample and mdLine magnification
    mdSampleScale = sampleScale;
    mdLineScale   = lineScale;
    
    mLineMgr = new LineManager(*mInCube);
    
    // Calculate output size based on the sample and line scales
    miOutputSamples = (int)ceil((double)mInCube->Samples() / mdSampleScale);
    miOutputLines   = (int)ceil((double)mInCube->Lines() / mdLineScale);
  }
  
  /**
   * Update the Mapping, Instrument, and AlphaCube groups in the output
   * cube label
   * 
   * @param pOutCube  - Resulting reduced output cube
   * @param pResultsGrp - This is the Results group that will go into the application
   *                 log file. This group must be created by the calling application.
   *                 Information will be added to it if the Mapping or Instrument
   *                 groups are deleted from the output image label
   */
  Isis::PvlGroup Reduce::UpdateOutputLabel(Isis::Cube *pOutCube)
  {
    // Construct a label with the results
    // This is the Results group that will go into the application
    // log file. This group must be created by the calling application.
    // Information will be added to it if the Mapping or Instrument
    // groups are deleted from the output image label
    PvlGroup resultsGrp("Results");
    resultsGrp += PvlKeyword("InputLines",     miInputLines);
    resultsGrp += PvlKeyword("InputSamples",   miInputSamples);
    resultsGrp += PvlKeyword("StartingLine",   (int)mdStartLine);
    resultsGrp += PvlKeyword("StartingSample", (int)mdStartSample);
    resultsGrp += PvlKeyword("EndingLine",     (int)mdEndLine);
    resultsGrp += PvlKeyword("EndingSample",   (int)mdEndSample);
    resultsGrp += PvlKeyword("LineIncrement",   mdLineScale);
    resultsGrp += PvlKeyword("SampleIncrement", mdSampleScale);
    resultsGrp += PvlKeyword("OutputLines",     miOutputSamples);
    resultsGrp += PvlKeyword("OutputSamples",   miOutputLines);
   
    Isis::SubArea subArea;
    subArea.SetSubArea(mInCube->Lines(), mInCube->Samples(), (int)mdStartLine, (int)mdStartSample, 
                       (int)mdEndLine, (int)mdEndSample, mdLineScale, mdSampleScale);
    subArea.UpdateLabel(mInCube, pOutCube, resultsGrp);
    
    return resultsGrp;
  }
  
  /**
   * Near Operator () overload, parameter for StartProcessInPlace 
   * refer ProcessByLine, ProcessByBrick 
   * 
   * @author Sharmila Prasad (4/26/2011)
   * 
   * @param out - output buffer
   */
  void Nearest::operator()(Isis::Buffer & out)
  {
    int readLine = (int)(mdLine + 0.5);
    mLineMgr->SetLine(readLine, (Isis::iString::ToInteger(msBands[miBandIndex])));
    mInCube->Read(*mLineMgr);
    
    // Scale down buffer
    for(int os = 0; os < miOutputSamples; os++) {
      out[os] = (*mLineMgr)[(int)((double) os * mdSampleScale)];
    }

    if(out.Line() == miOutputLines) {
      miBandIndex++;
      mdLine = 1;
    }
    else {
      mdLine += mdLineScale;
    }
  }
  
  /**
   * Average Operator () overload, parameter for StartProcessInPlace 
   * refer ProcessByLine, ProcessByBrick 
   * 
   * @author Sharmila Prasad (4/26/2011)
   * 
   * @param out - output buffer
   */
  void Average::operator() (Isis::Buffer & out)
  {
    static double *sinctab;
    static double *sum;
    static double *npts;
    static double *sum2;
    static double *npts2;

    double rline = (double)out.Line() * mdLineScale;

    if(out.Line() == 1 && out.Band() == 1) {
      sinctab = new double[miOutputSamples];
      sum = new double[miOutputSamples];
      npts = new double[miOutputSamples];
      sum2 = new double[miOutputSamples];
      npts2 = new double[miOutputSamples];

      //  Fill sinctab and Initialize buffers for first band
      for(int osamp = 0; osamp < miOutputSamples; osamp++) {
        sinctab[osamp] = ((double)osamp + 1.) * mdSampleScale;
        sum[osamp] = 0.0;
        npts[osamp] = 0.0;
        sum2[osamp] = 0.0;
        npts2[osamp] = 0.0;
      }
      sinctab[miOutputSamples-1] = miInputSamples;
    }

    while(mdLine <= rline) {
      if((int)mdLine <= miInputLines) {
        mLineMgr->SetLine((int)mdLine, (iString::ToInteger(msBands[miBandIndex])));
        mInCube->Read(*mLineMgr);
      }
      int isamp = 1;
      for(int osamp = 0; osamp < out.size(); osamp++) {
        while((double)isamp <= sinctab[osamp]) {
          // If Pixel is valid add it to sum
          if(IsValidPixel((*mLineMgr)[isamp-1])) {
            sum[osamp] += (*mLineMgr)[isamp-1];
            npts[osamp] += 1.0;
          }
          isamp++;
        }

        double sdel = (double) isamp - sinctab[osamp];
        if(isamp > miInputSamples) continue;

        if(IsValidPixel((*mLineMgr)[isamp-1])) {
          sum[osamp] += (*mLineMgr)[isamp-1] * (1.0 - sdel);
          npts[osamp] += (1.0 - sdel);
          if(osamp + 1 < miOutputSamples) {
            sum[osamp+1] += (*mLineMgr)[isamp-1] * sdel;
            npts[osamp+1] += sdel;
          }
        }
        isamp++;
      }
      mdLine++;
    }

    if(mdLine <= miInputLines) {
      mLineMgr->SetLine((int)mdLine, (iString::ToInteger(msBands[miBandIndex])));
      mInCube->Read(*mLineMgr);
    }
    double ldel = (double)mdLine - rline;
    double ldel2 = 1.0 - ldel;
    int isamp = 1;
    for(int osamp = 0; osamp < miOutputSamples; osamp++) {
      while(isamp <= sinctab[osamp]) {
        if(IsValidPixel((*mLineMgr)[isamp-1])) {
          sum[osamp] += (*mLineMgr)[isamp-1] * ldel2;
          npts[osamp] += ldel2;
          sum2[osamp] += (*mLineMgr)[isamp-1] * ldel;
          npts2[osamp] += ldel;
        }
        isamp++;
      }

      double sdel = (double) isamp - sinctab[osamp];
      if(isamp > miInputSamples) continue;
      if(IsValidPixel((*mLineMgr)[isamp-1])) {
        sum[osamp] += (*mLineMgr)[isamp-1] * (1.0 - sdel) * ldel2;
        npts[osamp] += (1.0 - sdel) * ldel2;
        if(osamp + 1 < miOutputSamples) {
          sum[osamp+1] += (*mLineMgr)[isamp-1] * sdel * ldel2;
          npts[osamp+1] += sdel * ldel2;
        }
        sum2[osamp] += (*mLineMgr)[isamp-1] * (1.0 - sdel) * ldel;
        npts2[osamp] += (1.0 - sdel) * ldel;
        if(osamp + 1 < miOutputSamples) {
          sum2[osamp+1] += (*mLineMgr)[isamp-1] * sdel * ldel;
          npts2[osamp+1] += sdel * ldel;
        }
      }
      isamp++;
    }

    if(mdLine < miInputLines) mdLine++;

    double npix = mdSampleScale * mdLineScale;
    for(int osamp = 0; osamp < miOutputSamples; osamp++) {
      if(npts[osamp] > npix * mdValidPer) {
        out[osamp] = sum[osamp] / npts[osamp];
      }
      else {
        if(msReplaceMode == "NEAREST") {
          out[osamp] = (*mLineMgr)[(int)(sinctab[osamp] + 0.5) - 1];
        }
        else {
          out[osamp] = Isis::Null;
        }
      }
      sum[osamp] = sum2[osamp];
      npts[osamp] = npts2[osamp];
      sum2[osamp] = 0.0;
      npts2[osamp] = 0.0;
    }

    if(out.Line() == miOutputLines && out.Band() != miInputBands) {
      miBandIndex++;
      mdLine = 1;
      for(int osamp = 0; osamp < miOutputSamples; osamp++) {
        sum[osamp] = 0.0;
        npts[osamp] = 0.0;
        sum2[osamp] = 0.0;
        npts2[osamp] = 0.0;
      }
    }

    if(out.Line() == miOutputLines && out.Band() == miInputBands) {
      delete [] sinctab;
      delete [] sum;
      delete [] npts;
      delete [] sum2;
      delete [] npts2;
    }
  } 
}
