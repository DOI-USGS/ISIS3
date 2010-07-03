#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Statistics.h"
#include "Stretch.h"
#include "Histogram.h"

using namespace std; 
using namespace Isis;

Stretch stretch;

void remap(vector<Buffer *> &in, vector<Buffer *> &out);

void IsisMain() {
    // Setup the input and output cubes along with histograms
    ProcessByLine p; 
    Cube *mcube = p.SetInputCube("MATCH", Isis::OneBand);
    Histogram *match = mcube->Histogram();
    p.ClearInputCubes();
    Cube *icube = p.SetInputCube("FROM", Isis::OneBand);
    Histogram* from = icube->Histogram();
    p.SetOutputCube ("TO");

    // Histogram specifications
    UserInterface &ui = Application::GetUserInterface();
    double minimum = ui.GetDouble("MINPER");
    double maximum = ui.GetDouble("MAXPER");

    stretch.ClearPairs();

    // CDF mode selected
    if (ui.GetString("STRETCH") == "CDF") {
        int increment = ui.GetInteger("INCREMENT");
				double lastPer = from->Percent(minimum);
        stretch.AddPair(lastPer, match->Percent(minimum));
        for (double i = increment+minimum; i < maximum; i += increment) {
						double curPer = from->Percent(i);
            if (lastPer < curPer && abs(lastPer - curPer) > DBL_EPSILON) {
                stretch.AddPair(curPer, match->Percent(i));
								lastPer = curPer;
						}
				}
				double curPer = from->Percent(maximum);
				if (lastPer < curPer && abs(lastPer - curPer) > DBL_EPSILON) {
					stretch.AddPair(curPer, match->Percent(maximum));
				}
    }

    // Modal mode is selected
    else {
        stretch.AddPair(from->Percent(minimum), match->Percent(minimum));
        stretch.AddPair(from->Mode(), match->Mode());
        stretch.AddPair(from->Percent(maximum), match->Percent(maximum));
    }

    // Start the processing
    p.StartProcess(remap);
    p.EndProcess();
}

// Adjust FROM histogram to resemble MATCH's histogram
void remap(vector<Buffer *> &in, vector<Buffer *> &out) {
		Buffer &from = *in[0];
		Buffer &to = *out[0];
		
    for (int i = 0; i < from.size(); i++) {
        to[i] = stretch.Map(from[i]);
    }
}   // end remap
