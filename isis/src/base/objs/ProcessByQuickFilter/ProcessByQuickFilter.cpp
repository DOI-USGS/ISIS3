/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2006/12/15 15:58:39 $
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

#include "Preference.h"
#include "ProcessByQuickFilter.h"
#include "QuickFilter.h"
#include "LineManager.h"
#include "iException.h"
#include "Application.h"

using namespace std;
namespace Isis {

  //!  Constructs a FilterProcess object
  ProcessByQuickFilter::ProcessByQuickFilter() : Isis::Process () {
    SetFilterParameters(0,0);
    p_getParametersFromUser = true;
  }

 /** 
  * This method invokes the process on a line by line basis
  * 
  * @param funct (Isis::Buffer &in, Isis::Buffer &out, Isis::QuickFilter &filter) 
  *              Name of your processing function
  * 
  * @throws Isis::iException::Programmer
  */
  void ProcessByQuickFilter::StartProcess (void 
       funct(Isis::Buffer &in, Isis::Buffer &out, Isis::QuickFilter &filter)) {
    // Error checks ... there must be one input and output
    if (InputCubes.size() != 1) { 
      string m = "StartProcess only supports exactly one input file";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    if (OutputCubes.size() != 1) {
      string m = "StartProcess only supports exactly one output file";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    // The lines in the input and output must match
    if (InputCubes[0]->Lines() != OutputCubes[0]->Lines()) {
      string m = "The lines in the input and output cube must match";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    // The samples in the input and output must match
    if (InputCubes[0]->Samples() != OutputCubes[0]->Samples()) {
      string m = "The samples in the input and output cube must match";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    // The bands in the input and output must match
    if (InputCubes[0]->Bands() != OutputCubes[0]->Bands()) {
      string m = "The bands in the input and output cube must match";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    // Construct line buffer managers
    Isis::LineManager *topline = new Isis::LineManager(*InputCubes[0]);
    Isis::LineManager *iline = new Isis::LineManager(*InputCubes[0]);
    Isis::LineManager *botline = new Isis::LineManager(*InputCubes[0]);
    Isis::LineManager *oline = new Isis::LineManager(*OutputCubes[0]);
  
    int lines = InputCubes[0]->Lines();
    int samples = InputCubes[0]->Samples();
    int bands = InputCubes[0]->Bands();
    
    // See if we need to get parameters from the user
    if (p_getParametersFromUser) GetFilterParameters();
  
    // Make sure the boxcar widht and height aren't too big for the image
    if (lines*2-1 < p_boxcarLines) {
      string msg = "Boxcar height is too big for cube size";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
  
    if (samples*2-1 < p_boxcarSamples) {
      string msg = "Boxcar width is too big for cube size";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    
    // Create the filter object
    Isis::QuickFilter filter(samples,p_boxcarSamples,p_boxcarLines);
    filter.SetMinMax(p_low,p_high);
    filter.SetMinimumPixels(p_minimum);
  
    // Loop for each band
    p_progress->SetMaximumSteps(lines*bands);
    p_progress->CheckStatus();
    for (int band=1; band<=bands; band++) {
      // Preload the filter
      filter.Reset();
      int top = 1 - filter.HalfHeight();
      int bot;
      for (bot = top; bot<=(1+filter.HalfHeight()); bot++) {
        int iline = bot;
        if (bot <= 0) iline = (-1 * bot + 2);
        botline->SetLine(iline,band);
        InputCubes[0]->Read(*botline);
        filter.AddLine(botline->DoubleBuffer());
      }
      bot = 1 + filter.HalfHeight() + 1;
  
      // Loop for each line
      for (int line=1; line<=lines; line++) {
        // Process a line
        iline->SetLine(line,band);
        oline->SetLine(line,band);
        InputCubes[0]->Read(*iline);
        funct (*iline,*oline,filter);
        OutputCubes[0]->Write(*oline);
  
        // Remove the top line
        if (top >= 1) {
          topline->SetLine(top,band);
        }
        else {
          topline->SetLine(-1*top+2,band);
        }
        InputCubes[0]->Read(*topline);
        filter.RemoveLine(topline->DoubleBuffer());
        top++;
  
        // Add the next line
        p_progress->CheckStatus();
        if (line == lines) continue;

        if (bot <= InputCubes[0]->Lines()) {
          botline->SetLine(bot,band);
        }
        else {
          botline->SetLine(lines - (bot - lines),band);
        }
        InputCubes[0]->Read(*botline);
        filter.AddLine(botline->DoubleBuffer());
        bot++; 
  
        // Update the progress
      }
    }
  
    // Free buffers before returning
    delete topline;
    delete iline;
    delete botline;
    delete oline;
  }
  
 /** 
  * This method allows the programmer to set the filter parameters. If this 
  * method is not invoked then the parameters will be obtained from the user via 
  * the XML parameters indicate in the class description.
  * 
  * @param samples Number of samples in the boxcar. Must be odd.
  * 
  * @param lines Number of lines in the boxcar. Must be odd.
  * 
  * @param low Minimum valid pixel value to include in statistical computations
  *            of the boxcar. Defaults to -DBL_MAX
  *  
  * @param high Maximum valid pixel value to include in statistical computations 
  *             of the boxcar. Defaults to DBL_MAX
  * 
  * @param minimum Minimum number of valid pixels in the sample-by-line boxcar  
  *                in order for statistical computations to be valid. 
  *                Defaults to 0 
  */
  void ProcessByQuickFilter::SetFilterParameters(int samples, int lines, 
                                              double low, double high, int minimum) {
    p_getParametersFromUser = false;
    p_boxcarSamples = samples;
    p_boxcarLines = lines;
    p_low = low;
    p_high = high;
    p_minimum = minimum;
  }

  //! This method allows the user to input the filter parameters. 
  void ProcessByQuickFilter::GetFilterParameters() {
    // Get boxcar size
    p_boxcarSamples = Application::GetUserInterface().GetInteger("SAMPLES");
    p_boxcarLines = Application::GetUserInterface().GetInteger("LINES");
    
    // Get valid pixel range
    p_low = -DBL_MAX;
    p_high = DBL_MAX;
    if (Application::GetUserInterface().WasEntered("LOW")) {
      p_low = Application::GetUserInterface().GetDouble("LOW");
    }

    if (Application::GetUserInterface().WasEntered("HIGH")) {
      p_high = Application::GetUserInterface().GetDouble("HIGH");
    }
  
    // Get valid pixel count
    p_minimum = 0;
    if (Application::GetUserInterface().WasEntered("MINIMUM")) {
      p_minimum = Application::GetUserInterface().GetInteger("MINIMUM");
    }
  }
} // end namespace isis
