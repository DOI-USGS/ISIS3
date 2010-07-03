#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "Statistics.h"

using namespace std;
using namespace Isis;

void kuwahara (Buffer &in, double &v);

// Filter size
int g_samples;
int g_lines;

void IsisMain () {

  // Get parameters from user
  ProcessByBoxcar p;
  p.SetInputCube ("FROM");
  p.SetOutputCube ("TO");

  // Get user input boxcar size
  UserInterface &ui = Application::GetUserInterface();
  g_samples = ui.GetInteger("SAMPLES");
  g_lines = ui.GetInteger("LINES");
  p.SetBoxcarSize(g_samples, g_lines);
  
  // Start processing
  p.StartProcess(kuwahara);
  p.EndProcess();
}

void kuwahara (Buffer &in, double &result) {
  
  // Find subunit size
  int subSamp = 1 + (int) (g_samples / 2);
  int subLine = 1 + (int) (g_lines / 2);
  const int numStats = 4;
  Statistics stats[ numStats ];
    
  // Load up statistics from each subunit grouping
  int indexTop = 0; // indexTop leads the upper two quadrants
  int indexBottom = (g_samples * (subLine - 1)); // indexBottom leads the bottom two quadrants
  for (int i = 0 ; i < subLine ; i ++ ) {
    stats[0].AddData(&in[ indexTop ], subSamp);                       // Upper left
    stats[1].AddData(&in[ (indexTop + (subSamp - 1)) ], subSamp);     // Upper right
    stats[2].AddData(&in[ indexBottom ], subSamp);                    // Lower left
    stats[3].AddData(&in[ (indexBottom + (subSamp - 1)) ], subSamp);  // Lower right 
    indexTop += g_samples;
    indexBottom += g_samples;
  }

  // Set first information to results by defualt
  double minimum = stats[0].Variance(); 
  result = stats[0].Average();

  // Find subgroup with smallest variance, set its mean to result
  for (int i = 1; i < numStats; i ++) {
    if (IsSpecial(minimum)) {
      minimum = stats[i].Variance();
      result = stats[i].Average();
    } 
    else {
      if (IsValidPixel(stats[i].Variance())) {
           
        // Problems caused in this if between computers because of rounding errors. 
        // Cases where there are two equal variances will cause this and can be fixed
        // by checking equality of the doubles with a powerful methods along with && <
        if (stats[i].Variance() < minimum) {
            minimum = stats[i].Variance();
            result = stats[i].Average();
        }
      }
    }
  }
  
  // If the program is through without a valid output, its probabaly null, 
  // make sure its null
  if (IsSpecial(minimum)) {
    result = Isis::Null;
  }
}
