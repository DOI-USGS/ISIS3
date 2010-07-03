#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Statistics.h"
#include "Stretch.h"
#include "Histogram.h"

using namespace std; 
using namespace Isis;

Stretch stretch;
void remap(Buffer &in, Buffer &out);

void IsisMain() {

    // Setup the input and output cubes
    ProcessByLine p;  // used for getting histograms from input cubes
    Cube *icube = p.SetInputCube("FROM", Isis::OneBand);
    p.SetOutputCube ("TO");

    // Histogram parameters
    UserInterface &ui = Application::GetUserInterface(); 
    double minimum = ui.GetDouble("MINPER");
    double maximum = ui.GetDouble("MAXPER");
    int increment = ui.GetInteger("INCREMENT");

    // Histograms from input cubes
    Histogram *from = icube->Histogram();
    Histogram *match = icube->Histogram();

    double fromMin = from->Percent(minimum);
    double fromMax = from->Percent(maximum);
    int fromBins = from->Bins();
    double data[fromBins];
    double slope = (fromMax - fromMin) / (fromBins - 1);

    // Set "match" to have the same data range and number of bins as "to"
    match->SetBins(fromBins);
    match->SetValidRange(fromMin, fromMax);
    for (int i = 0; i < fromBins; i++) {
        data[i] = fromMin + (slope * i);
    }
    match->AddData(data, fromBins);

		stretch.ClearPairs();
		double lastPer = from->Percent(minimum);
    stretch.AddPair(lastPer, match->Percent(minimum));
    for (double i = increment+minimum; i < maximum; i += increment) {
				double curPer = from->Percent(i);
        if (lastPer < curPer) {
						if(abs(lastPer - curPer) > DBL_EPSILON) {
  	          stretch.AddPair(curPer, match->Percent(i));
							lastPer = curPer;
						}
				}
    }
		double curPer = from->Percent(maximum);
		if (lastPer < curPer && abs(lastPer - curPer) > DBL_EPSILON) {
			stretch.AddPair(curPer, match->Percent(maximum));
		}

    // Start the processing
    p.StartProcess(remap);
    p.EndProcess();
}

// Adjust FROM cumulative distribution to be flatter
void remap(Buffer &in, Buffer &out) {
    for (int i = 0; i < in.size(); i++) {
        out[i] = stretch.Map(in[i]);
    }
}   // end remap .
