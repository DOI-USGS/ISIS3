#include <float.h>
#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "Filename.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

void minimumFilter (Buffer &in, double &v);
void maximumFilter (Buffer &in, double &v);

void IsisMain() {
  ProcessByBoxcar p;

  UserInterface &ui = Application::GetUserInterface();

  //  Open the input cube
  p.SetInputCube ("FROM");

  //  Allocate the output cube
  p.SetOutputCube ("TO");

  // Get dimensions of the boxcar
  int nSamples = ui.GetInteger ("SAMPLES");
  int nLines = ui.GetInteger ("LINES");
  
  //Set dimensions of the boxcar
  p.SetBoxcarSize (nSamples,nLines);
  
  // Set which filter is being used
  string filterType = ui.GetString ("FILTER");
  
  if (filterType == "MIN") {
    p.StartProcess (minimumFilter);
  }
  else if (filterType == "MAX") {
    p.StartProcess (maximumFilter);
  }
  p.EndProcess ();

}

//  Minimum DN filter
void minimumFilter (Buffer &in,double &v) {
	
	v = DBL_MAX;	/*initialize v to the BIGGEST DN possible for 
			Isis, ensuring that it will be replaced so
			long as there are valid pixels in the boxcar*/
  for (int i=0; i<in.size() ; i++) {
    if (!IsSpecial(in[i])){
      if (v >= in[i]){
	v = in[i];
      }
    }
  }

}


//   Maximum DN filter
void maximumFilter (Buffer &in,double &v) {
	
	v = -(DBL_MAX);	/*Initialize v to the SMALLEST DN possible for 
				Isis, ensuring that it will be replaced so 
				long as there are valid pixels in the boxcar*/
	
 for (int i=0; i<in.size() ; i++){
   if (!IsSpecial(in[i])){
     if (v <= in[i]){
       v = in[i];
     }
   }
 } 
 
}
