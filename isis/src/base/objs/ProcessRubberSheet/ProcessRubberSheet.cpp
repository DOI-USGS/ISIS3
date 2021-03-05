/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <iomanip>

#include <QVector>

#include "Affine.h"
#include "BasisFunction.h"
#include "BoxcarCachingAlgorithm.h"
#include "Brick.h"
#include "Interpolator.h"
#include "LeastSquares.h"
#include "Portal.h"
#include "ProcessRubberSheet.h"
#include "TileManager.h"
#include "Transform.h"
#include "UniqueIOCachingAlgorithm.h"

using namespace std;


namespace Isis {
  /**
   * Constructs a ProcessRubberSheet class with the default tile size range
   *
   * @param startSize Beginning size of output tiles for reverse driven geom
   * @param endSize Minimum size of output tiles for reverse driven geom
   */
  ProcessRubberSheet::ProcessRubberSheet(int startSize, int endSize) {

    p_bandChangeFunct = NULL;

    // Information used only in the tile transform method (StartProcess)
    p_forceSamp = Null;
    p_forceLine = Null;
    p_startQuadSize = startSize;
    p_endQuadSize = endSize;

    // Patch parameters used only in the patch transform (processPatchTransform)
    m_patchStartSample = 1;
    m_patchStartLine = 1;
    m_patchSamples = 5;
    m_patchLines = 5;
    m_patchSampleIncrement = 4;
    m_patchLineIncrement = 4;
  };


  /**
   * This method allows the programmer to override the default values for patch
   * parameters used in the patch transform method (processPatchTransform)
   *
   * @author janderson (3/22/2012)
   *
   * @param startSample The starting sample in the input cube to process the
   *                    default is 1.
   *
   * @param startLine The starting line in the input cube to process the default
   *                  is 1.  It would unusual to use something other than 1,
   *                  however, for pushframe cameras it would make sense to use
   *                  the framelet size + 1 for even cubes and 1 for odd cubes.
   *
   * @param samples The number of samples in each input patch.  The default is
   *                five.  Larger values can make the patch algorithm run faster
   *                at the risk of transforming improperly because an affine
   *                transform is not necessarily equal to the geometric
   *                transform defined by the Transform object given to the
   *                processPatchTransform method.  This is especially important
   *                to consider during image orthorectification if the DEM is
   *                high resolution.
   *
   * @param lines   The number of lines in each input patch.  The default is
   *                five. Larger values can make the patch algorithm run faster
   *                at the risk of transforming improperly because an affine
   *                transform is not necessarily equal to the geometric
   *                transform defined by the Transform object given to the
   *                processPatchTransform method.  This is especially important
   *                to consider during image orthorectification if the DEM is
   *                high resolution. Also for pushframe cameras the line size
   *                for patches should divide nicely into the framelet size.
   *
   * @param sampleIncrement The number of input samples to increment for the
   *                        next patch.  The default is 4 which is one less than
   *                        the default patch size.  This guarantees overlap so
   *                        that there is not gaps in the output cube.
   *
   * @param sampleIncrement The number of input lines to increment for the next
   *                        patch.  The default is 4 which is one less than the
   *                        default patch size.  This guarantees overlap so that
   *                        there is not gaps in the output cube.  In rare
   *                        instances (pushframe cameras) the line increment
   *                        should be twice the framelet height which will
   *                        prevent processing of NULL framelets.
   *
   */
  void ProcessRubberSheet::setPatchParameters(int startSample, int startLine,
                             int samples, int lines,
                             int sampleIncrement, int lineIncrement) {

    m_patchStartSample = startSample;
    m_patchStartLine = startLine;
    m_patchSamples = samples;
    m_patchLines = lines;
    m_patchSampleIncrement = sampleIncrement;
    m_patchLineIncrement = lineIncrement;
  }


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
   *               member of this object is used to calculate output pixel
   *               values.
   *
   * @throws IException::Message
   */
  void ProcessRubberSheet::StartProcess(Transform &trans,
                                        Interpolator &interp) {

    // Error checks ... there must be one input and one output
    if (InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // allocate the sampMap/lineMap vectors
    p_lineMap.resize(p_startQuadSize);
    p_sampMap.resize(p_startQuadSize);

    for (unsigned int pos = 0; pos < p_lineMap.size(); pos++) {
      p_lineMap[pos].resize(p_startQuadSize);
      p_sampMap[pos].resize(p_startQuadSize);
    }

    // Create a tile manager for the output file
    TileManager otile(*OutputCubes[0], p_startQuadSize, p_startQuadSize);

    // Create a portal buffer for the input file
    Portal iportal(interp.Samples(), interp.Lines(),
                         InputCubes[0]->pixelType() ,
                         interp.HotSample(), interp.HotLine());

    // Start the progress meter
    p_progress->SetMaximumSteps(otile.Tiles());
    p_progress->CheckStatus();

    if (p_bandChangeFunct == NULL) {
      // A portal could read up to four chunks so we need to cache four times the number of bands to
      // minimize I/O thrashing
      InputCubes[0]->addCachingAlgorithm(new UniqueIOCachingAlgorithm(2 * InputCubes[0]->bandCount()));
      OutputCubes[0]->addCachingAlgorithm(new BoxcarCachingAlgorithm());

      long long int tilesPerBand = otile.Tiles() / OutputCubes[0]->bandCount();

      for (long long int tile = 1; tile <= tilesPerBand; tile++) {
        bool useLastTileMap = false;
        for (int band = 1; band <= OutputCubes[0]->bandCount(); band++) {
          otile.SetTile(tile, band);

          if (p_startQuadSize <= 2) {
            SlowGeom(otile, iportal, trans, interp);
          }
          else {
            QuadTree(otile, iportal, trans, interp, useLastTileMap);
          }

          useLastTileMap = true;

          OutputCubes[0]->write(otile);
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
          p_bandChangeFunct(lastOutputBand);
        }

        if (p_startQuadSize <= 2) {
          SlowGeom(otile, iportal, trans, interp);
        }
        else {
          QuadTree(otile, iportal, trans, interp, false);
        }

        OutputCubes[0]->write(otile);
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
  void ProcessRubberSheet::BandChange(void (*funct)(const int band)) {

    p_bandChangeFunct = funct;
  }


  void ProcessRubberSheet::SlowGeom(TileManager &otile, Portal &iportal,
                                    Transform &trans, Interpolator &interp) {

    double outputSamp, outputLine;
    double inputSamp, inputLine;
    int outputBand = otile.Band();

    for (int i = 0; i < otile.size(); i++) {
      outputSamp = otile.Sample(i);
      outputLine = otile.Line(i);
      // Use the defined transform to find out what input pixel the output
      // pixel came from
      if (trans.Xform(inputSamp, inputLine, outputSamp, outputLine)) {
        if ((inputSamp < 0.5) || (inputLine < 0.5) ||
            (inputLine > InputCubes[0]->lineCount() + 0.5) ||
            (inputSamp > InputCubes[0]->sampleCount() + 0.5)) {
          otile[i] = NULL8;
        }
        else {
          // Set the position of the portal in the input cube
          iportal.SetPosition(inputSamp, inputLine, outputBand);
          InputCubes[0]->read(iportal);
          otile[i] = interp.Interpolate(inputSamp, inputLine,
                                        iportal.DoubleBuffer());
        }
      }
      else {
        otile[i] = NULL8;
      }
    }
  }


  void ProcessRubberSheet::QuadTree(TileManager &otile, Portal &iportal,
                                    Transform &trans, Interpolator &interp,
                                    bool useLastTileMap) {

    // Initializations
    vector<Quad *> quadTree;

    if (!useLastTileMap) {
      // Set up the boundaries of the full tile
      Quad *quad = new Quad;
      quad->sline = otile.Line();
      quad->ssamp = otile.Sample();

      quad->eline = otile.Line(otile.size() - 1);
      quad->esamp = otile.Sample(otile.size() - 1);
      quad->slineTile = otile.Line();
      quad->ssampTile = otile.Sample();

      quadTree.push_back(quad);

      // Loop and compute the input coordinates filling the maps
      // until the quad tree is empty
      while (quadTree.size() > 0) {
        ProcessQuad(quadTree, trans, p_lineMap, p_sampMap);
      }
    }

    // Apply the map to the output tile
    int outputBand = otile.Band();
    for (int i = 0, line = 0; line < p_startQuadSize; line++) {
      for (int samp = 0; samp < p_startQuadSize; samp++, i++) {
        double inputLine = p_lineMap[line][samp];
        double inputSamp = p_sampMap[line][samp];
        if (inputLine != NULL8) {
          iportal.SetPosition(inputSamp, inputLine, outputBand);
          InputCubes[0]->read(iportal);
          otile[i] = interp.Interpolate(inputSamp, inputLine,
                                        iportal.DoubleBuffer());
        }
        else {
          otile[i] = NULL8;
        }
      }
    }
  }


  /**
   * This function walks a line (or rectangle) and tests a point every increment
   * pixels. If any of these points can transform, then this method will return
   * true. Otherwise, this returns false.
   *
   * @param trans The Transform object to test on
   * @param ssamp Starting Sample
   * @param esamp Ending Sample
   * @param sline Starting Line
   * @param eline Ending Line
   * @param increment The increment to step by while walking this line/rectangle
   */
  bool ProcessRubberSheet::TestLine(Transform &trans, int ssamp, int esamp,
                                    int sline, int eline, int increment) {

    for (int line = sline; line <= eline; line += increment) {
      for (int sample = ssamp; sample <= esamp; sample += increment) {
        double sjunk = 0.0;
        double ljunk = 0.0;

        if (trans.Xform(sjunk, ljunk, sample, line)) {
          return true;
        }
      }
    }

    return false;
  }


  // Process a quad trying to find input positions for output positions
  void ProcessRubberSheet::ProcessQuad(std::vector<Quad *> &quadTree,
                           Transform &trans,
                           std::vector< std::vector<double> > &lineMap,
                           std::vector< std::vector<double> > &sampMap) {

    Quad *quad = quadTree[0];
    double oline[4], osamp[4];
    double iline[4], isamp[4];

    // Try to convert the upper left corner to input coordinates
    int badCorner = 0;
    oline[0] = quad->sline;
    osamp[0] = quad->ssamp;
    if (!trans.Xform(isamp[0], iline[0], osamp[0], oline[0])) {
      badCorner++;
    }

    // Now try the upper right corner
    oline[1] = quad->sline;
    osamp[1] = quad->esamp;
    if (!trans.Xform(isamp[1], iline[1], osamp[1], oline[1])) {
      badCorner++;
    }

    // Now try the lower left corner
    oline[2] = quad->eline;
    osamp[2] = quad->ssamp;
    if (!trans.Xform(isamp[2], iline[2], osamp[2], oline[2])) {
      badCorner++;
    }

    // Now try the lower right corner
    oline[3] = quad->eline;
    osamp[3] = quad->esamp;
    if (!trans.Xform(isamp[3], iline[3], osamp[3], oline[3])) {
      badCorner++;
    }

    // If all four corners are bad then walk the edges. If any points
    // on the edges transform we will split the quad or
    // if the quad is already small just transform everything
    if (badCorner == 4) {
      if ((quad->eline - quad->sline) < p_endQuadSize) {
        SlowQuad(quadTree, trans, lineMap, sampMap);
      }
      else {
        if (p_forceSamp != Null && p_forceLine != Null) {
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
        // If we find data around the quad by walking around a 2x2 grid in the
        // box, then we need to split the quad. Check outside the box and
        // interior crosshair.
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
        if (TestLine(trans, quad->ssamp + 1, quad->esamp - 1,
                    quad->sline, quad->sline, 4) ||
            // Bottom Edge
            TestLine(trans, quad->ssamp + 1, quad->esamp - 1,
                     quad->eline, quad->eline, 4) ||
            // Left Edge
            TestLine(trans, quad->ssamp, quad->ssamp,
                     quad->sline + 1, quad->eline - 1, 4) ||
            // Right Edge
            TestLine(trans, quad->esamp, quad->esamp,
                     quad->sline + 1, quad->eline - 1, 4) ||
            // Center Column
            TestLine(trans, centerSample, centerSample,
                     quad->sline + 1, quad->eline - 1, 4) ||
            // Center Row
            TestLine(trans, quad->ssamp + 1, quad->esamp - 1,
                     centerLine, centerLine, 4)) {


          SplitQuad(quadTree);
          return;
        }

        //  Nothing in quad, fill with nulls
        for (int i = quad->sline; i <= quad->eline; i++) {
          for (int j = quad->ssamp; j <= quad->esamp; j++) {
            lineMap[i-quad->slineTile][j-quad->ssampTile] = NULL8;
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
    //        lineMap[i-quad->slineTile][j-quad->ssampTile] = NULL8;
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
        SlowQuad(quadTree, trans, lineMap, sampMap);
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
    for (int i = 0; i < 4; i++) {
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
        SlowQuad(quadTree, trans, lineMap, sampMap);
      }
      else {
        SplitQuad(quadTree);
      }
      return;
    }

    // Substitute our desired answers into B to get the coefficients for the
    // line dimension (Cramers Rule!!)
    double B[4][4];
    double lineCoef[4];
    for (int j = 0; j < 4; j++) {
      memmove(B, A, 16 * sizeof(double));

      for (int i = 0; i < 4; i++) {
        B[i][j] = iline[i];
      }

      lineCoef[j] = Det4x4(B) / detA;
    }

    // Do it again to get the sample coefficients
    double sampCoef[4];
    for (int j = 0; j < 4; j++) {
      memmove(B, A, 16 * sizeof(double));

      for (int i = 0; i < 4; i++) {
        B[i][j] = isamp[i];
      }

      sampCoef[j] = Det4x4(B) / detA;
    }

    // Test the middle point to see if the equations are good
    double quadMidLine = (quad->sline + quad->eline) / 2.0;
    double quadMidSamp = (quad->ssamp + quad->esamp) / 2.0;
    double midLine, midSamp;

    if (!trans.Xform(midSamp, midLine, quadMidSamp, quadMidLine)) {
      if ((quad->eline - quad->sline) < p_endQuadSize) {
        SlowQuad(quadTree, trans, lineMap, sampMap);
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
        SlowQuad(quadTree, trans, lineMap, sampMap);
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

    for (int ol = quad->sline; ol <= quad->eline; ol++) {
      // Now Compute the derivates of the equations with respect to the
      // output sample at the current line
      double lineChangeWrSamp = lineCoef[1] + lineCoef[2] * (double) ol;
      double sampChangeWrSamp = sampCoef[1] + sampCoef[2] * (double) ol;

      // Set first computed line to the left-edge position
      double cline = ulLine;
      double csamp = ulSamp;

      // Get pointers to speed processing
      int startSamp = quad->ssamp - quad->ssampTile;
      std::vector<double> &lineVect = lineMap[ol-quad->slineTile];
      std::vector<double> &sampleVect = sampMap[ol-quad->slineTile];

      // Loop computing input positions for respective output positions
      for (int os = quad->ssamp; os <= quad->esamp; os++) {
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
  void ProcessRubberSheet::SplitQuad(std::vector<Quad *> &quadTree) {

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
  void ProcessRubberSheet::SlowQuad(std::vector<Quad *> &quadTree,
                                    Transform &trans,
                                    std::vector< std::vector<double> > &lineMap,
                                    std::vector< std::vector<double> > &sampMap) {

    // Get the quad
    Quad *quad = quadTree[0];
    double iline, isamp;

    // Loop and do the slow computation of input position from output position
    for (int oline = quad->sline; oline <= quad->eline; oline++) {
      int lineIndex = oline - quad->slineTile;
      for (int osamp = quad->ssamp; osamp <= quad->esamp; osamp++) {
        int sampIndex = osamp - quad->ssampTile;
        lineMap[lineIndex][sampIndex] = NULL8;
        if (trans.Xform(isamp, iline, (double) osamp, (double) oline)) {
          if ((isamp >= 0.5) ||
              (iline >= 0.5) ||
              (iline <= InputCubes[0]->lineCount() + 0.5) ||
              (isamp <= InputCubes[0]->sampleCount() + 0.5)) {
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
  double ProcessRubberSheet::Det4x4(double m[4][4]) {

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
    double det = m[0][0] * Det3x3(cofact);

    cofact [0][0] = m[1][0];
    cofact [0][1] = m[1][2];
    cofact [0][2] = m[1][3];
    cofact [1][0] = m[2][0];
    cofact [1][1] = m[2][2];
    cofact [1][2] = m[2][3];
    cofact [2][0] = m[3][0];
    cofact [2][1] = m[3][2];
    cofact [2][2] = m[3][3];
    det -= m[0][1] * Det3x3(cofact);

    cofact [0][0] = m[1][0];
    cofact [0][1] = m[1][1];
    cofact [0][2] = m[1][3];
    cofact [1][0] = m[2][0];
    cofact [1][1] = m[2][1];
    cofact [1][2] = m[2][3];
    cofact [2][0] = m[3][0];
    cofact [2][1] = m[3][1];
    cofact [2][2] = m[3][3];
    det += m[0][2] * Det3x3(cofact);

    cofact [0][0] = m[1][0];
    cofact [0][1] = m[1][1];
    cofact [0][2] = m[1][2];
    cofact [1][0] = m[2][0];
    cofact [1][1] = m[2][1];
    cofact [1][2] = m[2][2];
    cofact [2][0] = m[3][0];
    cofact [2][1] = m[3][1];
    cofact [2][2] = m[3][2];
    det -= m[0][3] * Det3x3(cofact);

    return det;
  }


  // Determinate for 3x3 matrix
  double ProcessRubberSheet::Det3x3(double m[3][3]) {

    return m[0][0] * m[1][1] * m[2][2] -
           m[0][0] * m[1][2] * m[2][1] -
           m[0][1] * m[1][0] * m[2][2] +
           m[0][1] * m[1][2] * m[2][0] +
           m[0][2] * m[1][0] * m[2][1] -
           m[0][2] * m[1][1] * m[2][0];
  }


 /**
 * Applies a Transform and an Interpolator to small patches. The transform
 * should define a mapping from input pixel coordinates to output pixel
 * coordinates.  The input image will be broken into many small patches (default
 * 5x5).  The four corners of each input patch will be mapped into the output
 * cube using the transform.  This will provide four corresponding output patch
 * coordinates. These four coordinates (input and output) will be used to
 * compute an affine transform from output coordinate to input coordinates. The
 * affine transform is then used to quickly compute input coordinates
 * (fractional). This input coordinate is then used in the interp object in
 * order to geometrically move input pixels to output pixels.
 *
 * @param trans A fully initialized Transform object. The Transform member of
 *              this object is used to calculate an output pixel location given
 *              an input pixel location.
 *
 * @param interp A fully initialized Interpolator object. The Interpolate
 *               member of this object is used to calculate output pixel
 *               values.
 *
 * @throws iException::Message
 */
  void ProcessRubberSheet::processPatchTransform(Transform &trans,
                                                 Interpolator &interp) {

    // Error checks ... there must be one input and one output
    if (InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // Create a portal buffer for reading from the input file
    Portal iportal(interp.Samples(), interp.Lines(),
                   InputCubes[0]->pixelType() ,
                   interp.HotSample(), interp.HotLine());

    // Setup the progress meter
    int patchesPerBand = 0;
    for (int line = m_patchStartLine; line <= InputCubes[0]->lineCount();
          line += m_patchLineIncrement) {
      for (int samp = m_patchStartSample;
            samp <= InputCubes[0]->sampleCount();
            samp += m_patchSampleIncrement) {
        patchesPerBand++;
      }
    }

    int patchCount = InputCubes[0]->bandCount() * patchesPerBand;
    p_progress->SetMaximumSteps(patchCount);
    p_progress->CheckStatus();

    // For each band we will loop through the input file and work on small
    // spatial patches (nxm).  The objective is to determine where each patch
    // falls in the output file and transform that output patch.  As we loop
    // we want some overlap in the input patches which guarantees there will
    // be no gaps in the output cube.
    //
    // We use the variables m_patchLines and m_patchSamples to define
    // the patch size
    //
    // We use the variables m_patchStartLine and m_patchStartSample to define
    // where to start in the cube.  In almost all cases these should be (1,1).
    // An expection would be for PushFrameCameras which need to have
    // a different starting line for the even framed cubes
    //
    // We use the variables m_patchLineIncrement and m_patchSampleIncrement
    // to skip to the next patch.  Typically these are one less than the
    // patch size which guarantees a pixel of overlap in the patches.  An
    // exception would be for PushFrameCameras which should increment lines by
    // twice the framelet height.

    for (int band=1; band <= InputCubes[0]->bandCount(); band++) {
      if (p_bandChangeFunct != NULL) p_bandChangeFunct(band);
      iportal.SetPosition(1,1,band);

      for (int line = m_patchStartLine;
            line <= InputCubes[0]->lineCount();
            line += m_patchLineIncrement) {
        for (int samp = m_patchStartSample;
              samp <= InputCubes[0]->sampleCount();
              samp += m_patchSampleIncrement, p_progress->CheckStatus()) {
          transformPatch((double)samp, (double)(samp + m_patchSamples - 1),
                         (double)line, (double)(line + m_patchLines - 1),
                         iportal, trans, interp);
        }
      }
    }
  }


  // Private method to process a small patch of the input cube
  void ProcessRubberSheet::transformPatch(double ssamp, double esamp,
                                          double sline, double eline,
                                          Portal &iportal,
                                          Transform &trans,
                                          Interpolator &interp) {
    // Let's make sure our patch is contained in the input file
    // TODO:  Think about the image edges should I be adding 0.5
    if (esamp > InputCubes[0]->sampleCount()) {
      esamp = InputCubes[0]->sampleCount();
      if (ssamp == esamp) ssamp = esamp - 1;
    }
    if (eline > InputCubes[0]->lineCount()) {
      eline = InputCubes[0]->lineCount();
      if (sline == eline) sline = eline - 1;
    }

    // Use these to build a list of four corner control points
    // If any corner doesn't trasform, split the patch into for smaller patches
    QVector<double> isamps;
    QVector<double> ilines;
    QVector<double> osamps;
    QVector<double> olines;

    // Upper left control point
    double tline, tsamp;
    if (trans.Xform(tsamp,tline,ssamp,sline)) {
      isamps.push_back(ssamp);
      ilines.push_back(sline);
      osamps.push_back(tsamp);
      olines.push_back(tline);
    }

    // Upper right control point
    if (trans.Xform(tsamp,tline,esamp,sline)) {
      isamps.push_back(esamp);
      ilines.push_back(sline);
      osamps.push_back(tsamp);
      olines.push_back(tline);
    }

    // Lower left control point
    if (trans.Xform(tsamp,tline,ssamp,eline)) {
      isamps.push_back(ssamp);
      ilines.push_back(eline);
      osamps.push_back(tsamp);
      olines.push_back(tline);
    }

    // Lower right control point
    if (trans.Xform(tsamp,tline,esamp,eline)) {
      isamps.push_back(esamp);
      ilines.push_back(eline);
      osamps.push_back(tsamp);
      olines.push_back(tline);
    }

    // If none of the 4 input tile corners transformed inside the output cube, give up on this tile
    // NOTE: Probably should split if the tile is large
    if (isamps.size() == 0) {
      return;
    }

    // If at least one of the 4 input tile corners did NOT transform, split it
    if (isamps.size() < 4) {
      splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
      return;
    }

    // Get the min/max output line/sample patch (bounding box of the transformed output samp,line)
    int osampMin = (int) osamps[0];
    int osampMax = (int) (osamps[0] + 0.5);
    int olineMin = (int) olines[0];
    int olineMax = (int) (olines[0] + 0.5);
    for (int i = 1; i < osamps.size(); i++) {
      if (osamps[i] < osampMin) osampMin = (int) osamps[i];
      if (osamps[i] > osampMax) osampMax = (int) (osamps[i] + 0.5);
      if (olines[i] < olineMin) olineMin = (int) olines[i];
      if (olines[i] > olineMax) olineMax = (int) (olines[i] + 0.5);
    }

    // Check to see if the output patch is completely outside the image.  No
    // sense in computing the affine if we don't have to.
    if (osampMax < 1) return;
    if (olineMax < 1) return;
    if (osampMin > OutputCubes[0]->sampleCount()) return;
    if (olineMin > OutputCubes[0]->lineCount()) return;

    // Adjust our output patch if it extends outside the output cube (overlaps cube boundry)
    if (osampMin < 1) osampMin = 1;
    if (olineMin < 1) olineMin = 1;
    if (osampMax > OutputCubes[0]->sampleCount()) {
      osampMax = OutputCubes[0]->sampleCount();
    }
    if (olineMax > OutputCubes[0]->lineCount()) {
      olineMax = OutputCubes[0]->lineCount();
    }

    /* A small input patch should create a small output patch.  If we had
     * the 0-360 seam (or -180/180) in our patch it could be split across
     * a cylindrical projection (e.g., equirectangular, simple, etc).  So
     * If the output patch looks like it will span the full output image,
     * either lines or sample then resplit the input patch. When the patch
     * spans more than 50% (was 99% but there were problems with double
     * rounding error on different machines) of the image it is split.
     */

    if (osampMax - osampMin + 1.0 > OutputCubes[0]->sampleCount() * 0.50) {
      splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
      return;
    }
    if (olineMax - olineMin + 1.0 > OutputCubes[0]->lineCount() * 0.50) {
      splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
      return;
    }

    // Can we create an affine transform from output to input coordinates
    BasisFunction isampFunc("Ax+By+C",3,3);
    LeastSquares isampLSQ(isampFunc);

    BasisFunction ilineFunc("Dx+Ey+F",3,3);
    LeastSquares ilineLSQ(ilineFunc);

    try {
      for (int i=0; i < isamps.size(); i++) {
        std::vector<double> vars;
        vars.push_back(osamps[i]);
        vars.push_back(olines[i]);
        vars.push_back(1.0);

        isampLSQ.AddKnown(vars,isamps[i]);
        ilineLSQ.AddKnown(vars,ilines[i]);
      }

      isampLSQ.Solve(LeastSquares::QRD);
      ilineLSQ.Solve(LeastSquares::QRD);
    }
    catch (IException &e) {
      splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
      return;
    }

    // If the fit at any corner isn't good enough break it down
    for (int i=0; i<isamps.size(); i++) {
      if (fabs(isampLSQ.Residual(i)) > 0.5) {
        splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
        return;
      }
      if (fabs(ilineLSQ.Residual(i)) > 0.5) {
        splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
        return;
      }
    }

#if 0
    // What about the center point
    // TODO:  The patches are so small so maybe we don't care if the
    //        corners worked
    double csamp = (ssamp + esamp) / 2.0;
    double cline = (sline + eline) / 2.0;
    if (trans.Xform(tsamp,tline,csamp,cline)) {
      std::vector<double> vars;
      vars.push_back(tsamp);
      vars.push_back(tline);
      vars.push_back(1.0);

      double isamp = isampFunc.Evaluate(vars);
      double iline = ilineFunc.Evaluate(vars);

      double err = (csamp - isamp) * (csamp - isamp) +
                   (cline - iline) * (cline - iline);
      if (err > 0.25) {
        splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
        return;
      }
    }
    else {
      splitPatch(ssamp, esamp, sline, eline, iportal, trans, interp);
      return;
    }
#endif

    double A = isampFunc.Coefficient(0);
    double B = isampFunc.Coefficient(1);
    double C = isampFunc.Coefficient(2);

    double D = ilineFunc.Coefficient(0);
    double E = ilineFunc.Coefficient(1);
    double F = ilineFunc.Coefficient(2);

    // Now we can do our typical backwards geom. Loop over the output cube
    // coordinates and compute input cube coordinates for the corners of the current
    // buffer. The buffer is the same size as the current patch size.
    Brick oBrick(*OutputCubes[0], osampMax-osampMin+1, olineMax-olineMin+1, 1);
    oBrick.SetBasePosition(osampMin, olineMin, iportal.Band());

    int brickIndex = 0;
    bool foundNull = false;
    for (int oline = olineMin; oline <= olineMax; oline++) {
      double isamp = A * osampMin + B * oline + C;
      double iline = D * osampMin + E * oline + F;
      double isampChangeWRTosamp = A;
      double ilineChangeWRTosamp = D;
      for (int osamp = osampMin; osamp <= osampMax;
            osamp++, isamp += isampChangeWRTosamp, iline += ilineChangeWRTosamp) {
        // Now read the data around the input coordinate and interpolate a DN
        iportal.SetPosition(isamp, iline, iportal.Band());
        InputCubes[0]->read(iportal);
        double dn = interp.Interpolate(isamp, iline, iportal.DoubleBuffer());
        oBrick[brickIndex] = dn;
        if (dn == Null) foundNull = true;
        brickIndex++;
      }
    }

    // If there are any special pixel Null values in this output brick, we may be
    // up against an edge of the input image where the interpolaters get Nulls from
    // outside the image. Since the patches have some overlap due to finding the
    // rectangular area (bounding box, min/max line/samp) of the four points input points
    // projected into the output space, this causes valid DNs
    // from a previously processed patch to be replaced with Null DNs from this patch.
    // NOTE: A different method of accomplishing this fixing of Nulls was tested. We read
    // the buffer from the output and tested each pixel values before overwriting it
    // This resulted in a slighly slower run for the test. A concern was found in the
    // asynchronous write of buffers to the cube, where a race condition may have generated
    // different dns, not bad, but making testing more difficult.
    if (foundNull) {
      Brick readBrick(*OutputCubes[0], osampMax-osampMin+1, olineMax-olineMin+1, 1);
      readBrick.SetBasePosition(osampMin, olineMin, iportal.Band());
      OutputCubes[0]->read(readBrick);
      for (brickIndex = 0; brickIndex<(osampMax-osampMin+1)*(olineMax-olineMin+1); brickIndex++) {
        if (readBrick[brickIndex] != Null) {
          oBrick[brickIndex] = readBrick[brickIndex];
        }
      }
    }

    // Write filled buffer to cube
    OutputCubes[0]->write(oBrick);
  }


  // Private method used to split up input patches if the patch is too big to
  // process
  void ProcessRubberSheet::splitPatch(double ssamp, double esamp,
                                       double sline, double eline, Portal &iportal,
                                       Transform &trans, Interpolator &interp) {

    // Is the input patch too small to even worry about transforming?
    if ((esamp - ssamp < 0.1) && (eline - sline < 0.1)) return;

    // It's big enough so break it into four pieces
    double midSamp = (esamp + ssamp) / 2.0;
    double midLine = (eline + sline) / 2.0;

    transformPatch(ssamp, midSamp,
                   sline, midLine,
                   iportal, trans, interp);
    transformPatch(midSamp, esamp,
                   sline, midLine,
                   iportal, trans, interp);
    transformPatch(ssamp, midSamp,
                   midLine, eline,
                   iportal, trans, interp);
    transformPatch(midSamp, esamp,
                   midLine, eline,
                   iportal, trans, interp);

    return;
  }


} // end namespace isis
