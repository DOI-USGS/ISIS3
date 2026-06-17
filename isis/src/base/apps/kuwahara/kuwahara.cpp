/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "kuwahara.h"

#include "CubeAttribute.h"
#include "ProcessByBoxcar.h"
#include "SpecialPixel.h"
#include "Statistics.h"

using namespace std;

namespace Isis {

  // Filter size, shared with the boxcar callback below
  static int g_samples;
  static int g_lines;

  static void kuwaharaFilter(Buffer &in, double &result);

  /**
   * Kuwahara edge-preserving smoothing filter. Opens the input cube named by
   * the FROM parameter and forwards to the cube-based overload.
   *
   * @param ui User interface with application parameters
   */
  void kuwahara(UserInterface &ui) {
    Cube icube;
    icube.open(ui.GetCubeName("FROM"));
    kuwahara(&icube, ui);
  }

  /**
   * Kuwahara edge-preserving smoothing filter. Moves a boxcar of the
   * user-specified size across the input cube. For each pixel the four
   * overlapping quadrants of the boxcar are examined and the average of the
   * quadrant with the smallest variance is written to the output.
   *
   * @param icube Input cube
   * @param ui User interface with application parameters
   */
  void kuwahara(Cube *icube, UserInterface &ui) {
    ProcessByBoxcar p;
    p.SetInputCube(icube);
    CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
    p.SetOutputCube(ui.GetCubeName("TO"), att,
                    icube->sampleCount(), icube->lineCount(), icube->bandCount());

    // Get user input boxcar size
    g_samples = ui.GetInteger("SAMPLES");
    g_lines = ui.GetInteger("LINES");
    p.SetBoxcarSize(g_samples, g_lines);

    p.StartProcess(kuwaharaFilter);
    p.EndProcess();
  }

  /**
   * Boxcar callback. Computes the four overlapping quadrant statistics and
   * returns the average of the quadrant with the smallest variance. Where two
   * or more variances are equal, the first quadrant encountered wins.
   *
   * @param in Boxcar buffer centered on the current pixel
   * @param result Output pixel value
   */
  static void kuwaharaFilter(Buffer &in, double &result) {

    // Find subunit size
    int subSamp = 1 + (int)(g_samples / 2);
    int subLine = 1 + (int)(g_lines / 2);
    const int numStats = 4;
    Statistics stats[ numStats ];

    // Load up statistics from each subunit grouping
    int indexTop = 0; // indexTop leads the upper two quadrants
    int indexBottom = (g_samples * (subLine - 1)); // indexBottom leads the bottom two quadrants
    for(int i = 0 ; i < subLine ; i ++) {
      stats[0].AddData(&in[ indexTop ], subSamp);                       // Upper left
      stats[1].AddData(&in[(indexTop + (subSamp - 1)) ], subSamp);      // Upper right
      stats[2].AddData(&in[ indexBottom ], subSamp);                    // Lower left
      stats[3].AddData(&in[(indexBottom + (subSamp - 1)) ], subSamp);   // Lower right
      indexTop += g_samples;
      indexBottom += g_samples;
    }

    // Set first information to results by defualt
    double minimum = stats[0].Variance();
    result = stats[0].Average();

    // Find subgroup with smallest variance, set its mean to result
    for(int i = 1; i < numStats; i ++) {
      if(IsSpecial(minimum)) {
        minimum = stats[i].Variance();
        result = stats[i].Average();
      }
      else {
        if(IsValidPixel(stats[i].Variance())) {

          // Computers don't compute variance precisely every time
          bool isEqual = fabs(stats[i].Variance() - minimum) < 1e-13;
          if(stats[i].Variance() < minimum && !isEqual) {
            minimum = stats[i].Variance();
            result = stats[i].Average();
          }
        }
      }
    }

    // If the program is through without a valid output, its probabaly null,
    // make sure its null
    if(IsSpecial(minimum)) {
      result = Isis::Null;
    }
  }
}
