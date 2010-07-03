/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2009/06/05 16:17:13 $                                                                 
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

#include <iostream>
#include <iomanip>

#include "Portal.h"
#include "Transform.h"
#include "Interpolator.h"
#include "TileManager.h"
#include "ProcessRubberSheet.h"
#include "Portal.h"

using namespace std;
namespace Isis {
 /** 
  * Applies a Transform and an Interpolator to every pixel in the output cube.
  * The output cube is written using an Tile and the input cube is read using 
  * a Portal. The input cube and output cube must be initialized prior to 
  * calling this method. Output pixels which come from outside the input cube 
  * are set to NULL8.
  * 
  * @param trans A fully initialized Transform object. The Transform member of  
  *              this object is used to calculate what input pixel location 
  *              should be used to interpolate the output pixel value.
  * 
  * @param interp A fully initialized Interpolator object. The Interpolate  
  *               member of this object is used to calculate output pixel values.
  * 
  * @throws Isis::iException::Message 
  */
  void ProcessRubberSheet::StartProcess (Isis::Transform &trans,
                                      Isis::Interpolator &interp) {
    // Error checks ... there must be one input and one output
    if (InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    // allocate the sampMap/lineMap vectors
    p_lineMap.resize(p_startQuadSize);
    p_sampMap.resize(p_startQuadSize);

    for(unsigned int pos = 0; pos < p_lineMap.size(); pos++) {
      p_lineMap[pos].resize(p_startQuadSize);
      p_sampMap[pos].resize(p_startQuadSize);
    }

    // Create a tile manager for the output file
    Isis::TileManager otile (*OutputCubes[0], p_startQuadSize, p_startQuadSize);

    // Create a portal buffer for the input file
    Isis::Portal iportal (interp.Samples(), interp.Lines(),
                        InputCubes[0]->PixelType() ,
                        interp.HotSample(), interp.HotLine());

    // Start the progress meter
    p_progress->SetMaximumSteps (otile.Tiles());
    p_progress->CheckStatus();

    if (p_bandChangeFunct == NULL) {
      int tilesPerBand = otile.Tiles() / OutputCubes[0]->Bands();

      for (int tile=1; tile<=tilesPerBand; tile++) {
        bool useLastTileMap = false;
        for (int band=1; band<=OutputCubes[0]->Bands(); band++) {
          otile.SetTile(tile,band);

          if(p_startQuadSize == 2) {
            SlowGeom (otile,iportal,trans,interp);  
          }
          else {
            QuadTree (otile,iportal,trans,interp,useLastTileMap);
          }

          useLastTileMap = true;

          OutputCubes[0]->Write(otile);
          p_progress->CheckStatus();
        }
      }
    }
    else {
      int lastOutputBand = -1;

      for (otile.begin(); !otile.end(); otile++) {
        // Keep track of the current band
        if (lastOutputBand != otile.Band()) {
          lastOutputBand = otile.Band();
          // Call an application function if the band number changes
          p_bandChangeFunct (lastOutputBand);
        }

        if(p_startQuadSize == 2) {
          SlowGeom (otile,iportal,trans,interp);  
        }
        else {
          QuadTree (otile,iportal,trans,interp,false);
        }

        OutputCubes[0]->Write(otile);
        p_progress->CheckStatus();
      }
    }

    p_sampMap.clear();
    p_lineMap.clear();
  }
 /** 
  * Registers a function to be called when the current output cube band number 
  * changes. This includes the first time. If and application does NOT need to 
  * be notified when the processing is about to proceed to the next band there 
  * is no need to call this member. The application function will not be called.
  * 
  * @param funct (const int band) An application defined function which will be 
  *                              called every time the current band number
  *                              changes.
  */
  void ProcessRubberSheet::BandChange (void (*funct)(const int band)) {
    p_bandChangeFunct = funct;
  }
  
  void ProcessRubberSheet::SlowGeom(Isis::TileManager &otile, Isis::Portal &iportal, 
                                 Isis::Transform &trans, Isis::Interpolator &interp) {
    double outputSamp, outputLine;
    double inputSamp, inputLine;
    int outputBand = otile.Band();
  
    for (int i=0; i<otile.size(); i++) {
      outputSamp = otile.Sample(i);
      outputLine = otile.Line(i);
      // Use the defined transform to find out what input pixel the output
      // pixel came from
      if (trans.Xform (inputSamp, inputLine, outputSamp, outputLine)) {
        if ((inputSamp < 0.5) || (inputLine < 0.5) ||
            (inputLine > InputCubes[0]->Lines()+0.5) ||
            (inputSamp > InputCubes[0]->Samples()+0.5)) {
          otile[i] = Isis::NULL8;
        }
        else {
          // Set the position of the portal in the input cube
          iportal.SetPosition (inputSamp, inputLine, outputBand);
          InputCubes[0]->Read(iportal);
          otile[i] = interp.Interpolate (inputSamp, inputLine, iportal.DoubleBuffer());
        }
      }
      else {
        otile[i] = Isis::NULL8;
      }
    }
  }
  
  void ProcessRubberSheet::QuadTree(Isis::TileManager &otile, Isis::Portal &iportal, 
                                    Isis::Transform &trans, Isis::Interpolator &interp,
                                    bool useLastTileMap) {
    // Initializations
    vector<Quad *> quadTree;

    if (!useLastTileMap) {
      // Set up the boundaries of the full tile
      Quad *quad = new Quad;
      quad->sline = otile.Line();
      quad->ssamp = otile.Sample();
    
      quad->eline = otile.Line(otile.size()-1);
      quad->esamp = otile.Sample(otile.size()-1);
      quad->slineTile = otile.Line();
      quad->ssampTile = otile.Sample();
    
      quadTree.push_back(quad);
    
      // Loop and compute the input coordinates filling the maps
      // until the quad tree is empty
      while (quadTree.size() > 0) {
        ProcessQuad(quadTree,trans,p_lineMap,p_sampMap);
      }
    }
  
    // Apply the map to the output tile
    int outputBand = otile.Band();
    for (int i=0, line=0; line<p_startQuadSize; line++) {
      for (int samp=0; samp<p_startQuadSize; samp++, i++) {
        double inputLine = p_lineMap[line][samp];
        double inputSamp = p_sampMap[line][samp];
        if (inputLine != Isis::NULL8) {
          iportal.SetPosition (inputSamp, inputLine, outputBand);
          InputCubes[0]->Read(iportal);
          otile[i] = interp.Interpolate (inputSamp, inputLine, iportal.DoubleBuffer());
        }
        else {
          otile[i] = Isis::NULL8;
        }
      }
    }
  }
  
    
  /**
   * This function walks a line (or rectangle) and tests a point every increment pixels. If any of these
   *   points can transform, then this method will return true. Otherwise, this returns false.
   *
   * @param trans The Transform object to test on
   * @param ssamp Starting Sample
   * @param esamp Ending Sample
   * @param sline Starting Line
   * @param eline Ending Line
   * @param increment The increment to step by while walking this line/rectangle
   */
  bool ProcessRubberSheet::TestLine(Isis::Transform &trans, int ssamp, int esamp, int sline, int eline, int increment) {
    for(int line = sline; line <= eline; line += increment) {
      for(int sample = ssamp; sample <= esamp; sample += increment) {
        double sjunk = 0.0;
        double ljunk = 0.0;
        
        if(trans.Xform (sjunk, ljunk, sample, line)) {
          return true;
        }
      }
    }
    
    return false;
  }
  
  // Process a quad trying to find input positions for output positions
  void ProcessRubberSheet::ProcessQuad (std::vector<Quad *> &quadTree, Isis::Transform &trans,
                                        std::vector< std::vector<double> > &lineMap, 
                                        std::vector< std::vector<double> > &sampMap) {
    Quad *quad = quadTree[0];
    double oline[4],osamp[4];
    double iline[4],isamp[4];
    
    // Try to convert the upper left corner to input coordinates  
    int badCorner = 0;
    oline[0] = quad->sline;
    osamp[0] = quad->ssamp;
    if (!trans.Xform (isamp[0], iline[0], osamp[0], oline[0])) {
      badCorner++;
    }
  
    // Now try the upper right corner
    oline[1] = quad->sline;
    osamp[1] = quad->esamp;
    if (!trans.Xform (isamp[1], iline[1], osamp[1], oline[1])) {
      badCorner++;
    }
  
    // Now try the lower left corner
    oline[2] = quad->eline;
    osamp[2] = quad->ssamp;
    if (!trans.Xform (isamp[2], iline[2], osamp[2], oline[2])) {
      badCorner++;
    }
  
    // Now try the lower right corner
    oline[3] = quad->eline;
    osamp[3] = quad->esamp;
    if (!trans.Xform (isamp[3], iline[3], osamp[3], oline[3])) {
      badCorner++;
    }
  
    // If all four corners are bad then walk the edges. If any points 
    // on the edges transform we will split the quad or
    // if the quad is already small just transform everything
    if (badCorner == 4) {
      if ((quad->eline - quad->sline) < p_endQuadSize) {
        SlowQuad(quadTree,trans,lineMap,sampMap);
      }
      else {
        if (p_forceSamp != Isis::Null && p_forceLine != Isis::Null) {
          if (p_forceSamp >= quad->ssamp && p_forceSamp <= quad->esamp &&
              p_forceLine >= quad->sline && p_forceLine <= quad->eline) {
            SplitQuad(quadTree);
            return;
          }
        }
        
        int centerSample = (quad->ssamp + quad->esamp) / 2;
        int centerLine   = (quad->sline + quad->eline) / 2;
        
        // All 4 corner points have failed tests.
        //
        // If we find data around the quad by walking around a 2x2 grid in the box, then
        //   we need to split the quad. Check outside the box and interior crosshair.
        //
        //   This is what we're walking:
        //                       -----------
        //                       |    |    |
        //                       |    |    |
        //                       |----|----|
        //                       |    |    |
        //                       |    |    |
        //                       -----------
          // Top Edge
        if(TestLine(trans, quad->ssamp+1, quad->esamp-1, quad->sline, quad->sline, 4) ||
          // Bottom Edge
           TestLine(trans, quad->ssamp+1, quad->esamp-1, quad->eline, quad->eline, 4) ||
           // Left Edge
           TestLine(trans, quad->ssamp, quad->ssamp, quad->sline+1, quad->eline-1, 4) ||
           // Right Edge
           TestLine(trans, quad->esamp, quad->esamp, quad->sline+1, quad->eline-1, 4) ||
           // Center Column
           TestLine(trans, centerSample, centerSample, quad->sline+1, quad->eline-1, 4) ||
           // Center Row
           TestLine(trans, quad->ssamp+1, quad->esamp-1, centerLine, centerLine, 4)) {
           
           
           SplitQuad(quadTree);
           return;
        }
      
        //  Nothing in quad, fill with nulls
        for (int i=quad->sline; i<=quad->eline; i++) {
          for (int j=quad->ssamp; j<=quad->esamp; j++) {
            lineMap[i-quad->slineTile][j-quad->ssampTile] = Isis::NULL8;
          }
        }
        delete quad;
        quadTree.erase(quadTree.begin());
      }
      return;  
    }
  
    // If all four corners are bad then assume the whole tile is bad
    // Load the maps with nulls and delete the quad from the list
    // Free memory too
  //  if (badCorner == 4) {
  //    for (int i=quad->sline; i<=quad->eline; i++) {
  //      for (int j=quad->ssamp; j<=quad->esamp; j++) {
  //        lineMap[i-quad->slineTile][j-quad->ssampTile] = Isis::NULL8;
  //      }
  //    }
  //    delete quad;
  //    quadTree.erase(quadTree.begin());
  //    return;
  //  }
  
    // See if any other corners are bad in which case we will need to
    // split the quad into finer pieces. But lets not get ridiculous.
    // If the split distance is small we might as well compute at every
    // point
    if (badCorner > 0) {
       if ((quad->eline - quad->sline) < p_endQuadSize) {
        SlowQuad(quadTree,trans,lineMap,sampMap);
      }
      else {
        SplitQuad(quadTree);
      }
      return;
    }
  
    // We have good corners ... create two equations using them
    //   iline  =  a*oline + b*osamp + c*oline*osamp + d
    //   isamp  =  e*oline + f*osamp + g*oline*osamp + h
    // Start by setting up a 4x4 matrix
    double A[4][4];
    for (int i=0; i<4; i++) {
      A[i][0] = oline[i];
      A[i][1] = osamp[i];
      A[i][2] = oline[i] * osamp[i];
      A[i][3] = 1.0;
    }
  
    // Make sure the determinate is non-zero, otherwise split it up again
    // and hope for the best.  If this happens it probably is because the 
    // transform is lame (bugged)
    double detA;
    if ((detA = Det4x4(A)) == 0.0) {
      if ((quad->eline - quad->sline) < p_endQuadSize) {
        SlowQuad(quadTree,trans,lineMap,sampMap);
      }
      else {
        SplitQuad(quadTree);
      }    
      return;
    }
  
    // Substitute our desired answers into B to get the coefficients for the line
    // dimension (Cramers Rule!!)
    double B[4][4];
    double lineCoef[4];
    for (int j=0; j<4; j++) {
      memmove (B,A,16*sizeof(double));
  
      for (int i=0; i<4; i++) {
        B[i][j] = iline[i];
      }
  
      lineCoef[j] = Det4x4(B) / detA;
    }
  
    // Do it again to get the sample coefficients
    double sampCoef[4];
    for (int j=0; j<4; j++) {
      memmove (B,A,16*sizeof(double));
  
      for (int i=0; i<4; i++) {
        B[i][j] = isamp[i];
      }
  
      sampCoef[j] = Det4x4(B) / detA;
    }
  
    // Test the middle point to see if the equations are good
    double quadMidLine = (quad->sline + quad->eline) / 2.0;
    double quadMidSamp = (quad->ssamp + quad->esamp) / 2.0;
    double midLine,midSamp;
  
    if (!trans.Xform (midSamp, midLine, quadMidSamp, quadMidLine)) {
      if ((quad->eline - quad->sline) < p_endQuadSize) {
        SlowQuad(quadTree,trans,lineMap,sampMap);
      }
      else {
        SplitQuad(quadTree);
      }
      return;
    }
  
    double cmidLine = lineCoef[0] * quadMidLine + 
                      lineCoef[1] * quadMidSamp +
                      lineCoef[2] * quadMidLine * quadMidSamp + 
                      lineCoef[3];
  
    double cmidSamp = sampCoef[0] * quadMidLine + 
                      sampCoef[1] * quadMidSamp +
                      sampCoef[2] * quadMidLine * quadMidSamp + 
                      sampCoef[3];
  
    if ((abs(cmidSamp - midSamp) > 0.5) || (abs(cmidLine - midLine) > 0.5)) {
      if ((quad->eline - quad->sline) < p_endQuadSize) {
        SlowQuad(quadTree,trans,lineMap,sampMap);
      }
      else {
        SplitQuad(quadTree);
      }
      return;
    }
  
    // Equations are suitably accurate.  
    // First compute input at the top corner of the output quad
    double ulLine = lineCoef[0] * (double) quad->sline + 
                    lineCoef[1] * (double) quad->ssamp +
                    lineCoef[2] * (double) quad->sline * (double) quad->ssamp + 
                    lineCoef[3];
    
    double ulSamp = sampCoef[0] * (double) quad->sline + 
                    sampCoef[1] * (double) quad->ssamp +
                    sampCoef[2] * (double) quad->sline * (double) quad->ssamp + 
                    sampCoef[3];
    
    // Compute the derivate of the equations with respect to the
    // output line as we will be changing the output line in a loop
    double lineChangeWrLine = lineCoef[0] + lineCoef[2] * (double) quad->ssamp;
    double sampChangeWrLine = sampCoef[0] + sampCoef[2] * (double) quad->ssamp;
  
    for (int ol=quad->sline; ol<=quad->eline; ol++) {
      // Now Compute the derivates of the equations with respect to the
      // output sample at the current line
      double lineChangeWrSamp = lineCoef[1] + lineCoef[2] * (double) ol;
      double sampChangeWrSamp = sampCoef[1] + sampCoef[2] * (double) ol;
  
      // Set first computed line to the left-edge position
      double cline = ulLine;
      double csamp = ulSamp;
  
      // Get pointers to speed processing
      int startSamp = quad->ssamp-quad->ssampTile;
      std::vector<double> &lineVect = lineMap[ol-quad->slineTile];
      std::vector<double> &sampleVect = sampMap[ol-quad->slineTile];
  
      // Loop computing input positions for respective output positions
      for (int os=quad->ssamp; os<=quad->esamp; os++) {
        lineVect[startSamp] = cline;
        sampleVect[startSamp] = csamp;

        startSamp ++;

        cline += lineChangeWrSamp;
        csamp += sampChangeWrSamp;
      }
  
      // Reposition at the left edge of the tile for the next line
      ulLine += lineChangeWrLine;
      ulSamp += sampChangeWrLine;
    }
  
    // All done so remove the quad
    delete quad;
    quadTree.erase(quadTree.begin());
  }
  
  // Break input quad into four pieces
  void ProcessRubberSheet::SplitQuad (std::vector<Quad *> &quadTree) {
    // Get the quad to split
    Quad *quad = quadTree[0];
    int n = (quad->eline - quad->sline + 1) / 2;
  
    // New upper left quad
    Quad *q1 = new Quad;
    *q1 = *quad;
    q1->eline = quad->sline + n - 1;
    q1->esamp = quad->ssamp + n - 1; 
    quadTree.push_back(q1);
  
    // New upper right quad
    Quad *q2 = new Quad;
    *q2 = *quad;
    q2->eline = quad->sline + n - 1;
    q2->ssamp = quad->ssamp + n;
    quadTree.push_back(q2);
  
    // New lower left quad
    Quad *q3 = new Quad;
    *q3 = *quad;
    q3->sline = quad->sline + n; 
    q3->esamp = quad->ssamp + n - 1; 
    quadTree.push_back(q3);
  
    // New lower right quad
    Quad *q4 = new Quad;
    *q4 = *quad;
    q4->sline = quad->sline + n; 
    q4->ssamp = quad->ssamp + n;
    quadTree.push_back(q4);
  
    // Remove the old quad since it has been split up
    delete quad;
    quadTree.erase(quadTree.begin());
  }
  
  
  // Slow quad computation for every output pixel
  void ProcessRubberSheet::SlowQuad (std::vector<Quad *> &quadTree, Isis::Transform &trans,
                                     std::vector< std::vector<double> > &lineMap, 
                                     std::vector< std::vector<double> > &sampMap) {
    // Get the quad
    Quad *quad = quadTree[0];
    double iline,isamp;
  
    // Loop and do the slow computation of input position from output position
    for (int oline=quad->sline; oline<=quad->eline; oline++) {
      int lineIndex = oline - quad->slineTile;
      for (int osamp=quad->ssamp; osamp<=quad->esamp; osamp++) {
        int sampIndex = osamp - quad->ssampTile;
        lineMap[lineIndex][sampIndex] = Isis::NULL8;
        if (trans.Xform (isamp, iline, (double) osamp, (double) oline)) {
          if ((isamp >= 0.5) || 
              (iline >= 0.5) ||
              (iline <= InputCubes[0]->Lines()+0.5) ||
              (isamp <= InputCubes[0]->Samples()+0.5)) {
            lineMap[lineIndex][sampIndex] = iline;
            sampMap[lineIndex][sampIndex] = isamp;
          }
        }
      }
    }
  
    // All done with the quad
    delete quad;
    quadTree.erase(quadTree.begin());
  }
  
  // Determinate method for 4x4 matrix using cofactor expansion
  double ProcessRubberSheet::Det4x4 (double m[4][4]) {
    double cofact[3][3];
    
    cofact [0][0] = m[1][1];
    cofact [0][1] = m[1][2];
    cofact [0][2] = m[1][3];
    cofact [1][0] = m[2][1];
    cofact [1][1] = m[2][2];
    cofact [1][2] = m[2][3];
    cofact [2][0] = m[3][1];
    cofact [2][1] = m[3][2];
    cofact [2][2] = m[3][3];
    double det = m[0][0] * Det3x3 (cofact);
  
    cofact [0][0] = m[1][0];
    cofact [0][1] = m[1][2];
    cofact [0][2] = m[1][3];
    cofact [1][0] = m[2][0];
    cofact [1][1] = m[2][2];
    cofact [1][2] = m[2][3];
    cofact [2][0] = m[3][0];
    cofact [2][1] = m[3][2];
    cofact [2][2] = m[3][3];
    det -= m[0][1] * Det3x3 (cofact);
  
    cofact [0][0] = m[1][0];
    cofact [0][1] = m[1][1];
    cofact [0][2] = m[1][3];
    cofact [1][0] = m[2][0];
    cofact [1][1] = m[2][1];
    cofact [1][2] = m[2][3];
    cofact [2][0] = m[3][0];
    cofact [2][1] = m[3][1];
    cofact [2][2] = m[3][3];
    det += m[0][2] * Det3x3 (cofact);
  
    cofact [0][0] = m[1][0];
    cofact [0][1] = m[1][1];
    cofact [0][2] = m[1][2];
    cofact [1][0] = m[2][0];
    cofact [1][1] = m[2][1];
    cofact [1][2] = m[2][2];
    cofact [2][0] = m[3][0];
    cofact [2][1] = m[3][1];
    cofact [2][2] = m[3][2];
    det -= m[0][3] * Det3x3 (cofact);
  
    return det;
  }
  
  // Determinate for 3x3 matrix
  double ProcessRubberSheet::Det3x3 (double m[3][3]) {
    return m[0][0] * m[1][1] * m[2][2] -
           m[0][0] * m[1][2] * m[2][1] -
           m[0][1] * m[1][0] * m[2][2] +
           m[0][1] * m[1][2] * m[2][0] +
           m[0][2] * m[1][0] * m[2][1] -
           m[0][2] * m[1][1] * m[2][0];
  }
} // end namespace isis
