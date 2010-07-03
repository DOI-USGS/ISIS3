#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/12/12 17:17:52 $
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

#include "Progress.h"
#include "Application.h"
#include "Preference.h"

using namespace std;
namespace Isis {
 /** 
  * Constructs a Progress object.
  * 
  * @throws Isis::iException::User
  */
  Progress::Progress () {
    // Get user preferences
    int percent;
    bool printPercent;
    Isis::PvlGroup &group = Isis::Preference::Preferences().FindGroup("UserInterface");
    percent = group["ProgressBarPercent"];
    Isis::iString temp = (string) group["ProgressBar"];
    printPercent = temp.UpCase() == "ON";
  
    // Check for an error
    if ((percent != 1) && (percent != 2) && 
        (percent != 5) && (percent != 10)) {
      string m = "Invalid preference for [ProgressBarPercent] in ";
      m += "group [UserInterface] must be 1, 2, 5, or 10";
      throw Isis::iException::Message(Isis::iException::User,m,_FILEINFO_);
    }
  
    // Initialize private variables
    p_text = "Working";
    p_maximumSteps = 0;
    p_currentStep = 0;
    p_currentPercent = 0;
    p_percentIncrement = percent;
    p_printPercent = printPercent;
  }
  
  //! Destroys the Progress object
  Progress::~Progress () {
  }
  
 /** 
  * Changes the value of the text string reported just before 0% processed. This
  * text is only output under the following condition. After SetMaximumSteps is 
  * called, the first call to CheckStatus will immediately output the text. By 
  * default this is set to "Working".
  * 
  * @param text Text to output.
  */
  void Progress::SetText(const std::string &text) {
   p_text = text;
  }
  
 /** 
  * Returns the text to output. Generally, this in not needed except rare 
  * circumstances where an application has multiple steps and the text string 
  * needs to be saved and restored. For an example, see the Histogram method in
  * the CubeInfo class. 
  * 
  * @return string
  */
  string Progress::Text () const {
    return p_text;
  }
  
 /** 
  * This sets the maximum number of steps in the process. Whenever this is 
  * invoked it also resets the counters to their initial states. This allows for
  * programs which have multiple steps.
  * 
  * @param steps Maximum number of steps
  * 
  * @throws Isis::iException::Programmer Invalid value for step (must be >0)
  */
  void Progress::SetMaximumSteps(const int steps) {
    if (steps <= 0) {
      string m = "Value for [steps] must be greater than ";
      m += "zero in [Progress::SetMaximumSteps]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    p_maximumSteps = steps;
    p_currentStep = 0;
    p_currentPercent = 0;
  }
  
 /** 
  * Checks and updates the status. The first time this is invoked, it outputs 
  * the text from SetText and 0% processed. It should then be invoked for each 
  * step, for example, a step could be considered processing a line for NL lines
  * in the image. If you do not call this enough times you will not reached 100% 
  * processed.
  * 
  * @throws Isis::iException::Programmer Step exceeds maximumSteps
  */
  void Progress::CheckStatus () {
    // Error check
    //std::cout << p_currentStep << " / " << p_maximumSteps << std::endl;
    if (p_currentStep > p_maximumSteps) {
      string m = "Step exceeds maximumSteps in [Progress::CheckStatus]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    if (p_currentStep == 0) {
      if (Isis::iApp != NULL) {
        Isis::iApp->UpdateProgress(p_text,p_printPercent);
      }
      else {
        if (p_printPercent) {
          cout << p_text << endl;
        }
      }
    }
    
    // See if the percent processed needs to be updated
    while (100.0 * p_currentStep / p_maximumSteps >= p_currentPercent) {
      if (Isis::iApp != NULL) {
        Isis::iApp->UpdateProgress(p_currentPercent,p_printPercent);
      }
      else {
        if (p_printPercent) {
          if (p_currentPercent < 100) {
            cout << p_currentPercent << "% Processed\r" << flush;
          }
          else {
            cout << p_currentPercent << "% Processed" << endl;
          }
        }
      }
      p_currentPercent += p_percentIncrement;
    }

    if (Isis::iApp != NULL) {
      Isis::iApp->ProcessGuiEvents();
    }
  
    // Increment to the next step
    p_currentStep++;
  
    return;
  }

  /**
   * If the initial step size was a guess, it can be modified using this method. 
   * For example, if you SetMaximumSteps(11) then call AddSteps(1) then the new 
   * MaximumSteps is 12. The progress bar will not go backwards (it will not drop 
   * from 10% to 5%). "steps" can be negative to remove steps.
   * 
   * 
   * @param steps Amount to adjust the MaximumSteps by
   */
  void Progress::AddSteps(const int steps) {
    if (steps == 0) {
      string m = "Value for [steps] must not be zero in [Progress::AddSteps]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    p_maximumSteps += steps;

    if (p_currentStep > p_maximumSteps) {
      string m = "Step exceeds maximumSteps in [Progress::AddSteps]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    if (p_maximumSteps <= 0) {
      string m = "Maximum steps must be greater than zero in [Progress::AddSteps]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  }
} // end namespace isis
