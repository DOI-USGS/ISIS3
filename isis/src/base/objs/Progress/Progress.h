#ifndef Progress_h
#define Progress_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QString>

namespace Isis {
  /**
   * @brief Program progress reporter
   *
   * This class is used to output the percent completion for programs in either
   * the command line mode or the graphical user interface. Generally, this object
   * is created within a Process derived class. Therefore you should only use this
   * object if you are developing such a class.
   *
   * @ingroup ApplicationInterface
   *
   * @author 2002-05-22 Jeff Anderson
   *
   * @internal
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                          isis.astrogeology...
   *  @history 2004-02-29 Jeff Anderson - Added ability to send progress status to
   *                          the parent process
   *  @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                          documentation
   *  @history 2005-10-03 Elizabeth Miller - Changed @ingroup tag
   *  @history 2012-03-13 Steven Lambright - Added DisableAutomaticGuiRedraws().
   *  @history 2012-09-11 Steven Lambright - Added accessors to the current progress value
   *                          and range. Renamed DisableAutomaticGuiRedraws() to
   *                          DisableAutomaticDisplay() in order to include command-line
   *                          progress printouts.
   *  @history 2012-10-04 Jeannie Backer - Removed documentation reference to 
   *                          the now non-existant CubeInfo class. No mantis
   *                          ticket related to this change.
   */

  class Progress {
    public:
      // constructor
      Progress();

      // destructor
      ~Progress();

      // Change the text QString
      void SetText(const QString &text);

      // Get the text string
      QString Text() const;

      // Change the maximum steps
      void SetMaximumSteps(const int steps);

      // Add steps before completion (for guessed initial step size)
      void AddSteps(const int steps);

      // Check and report status
      void CheckStatus();

      void DisableAutomaticDisplay();

      int MaximumSteps() const;

      int CurrentStep() const;

    private:
      QString p_text;     /**<Text string to output at the initial call to
                                  CheckStatus (0% processed)*/
      int p_maximumSteps;     /**<Number of steps in your processing sequence.
                                  For example, if there are 20 lines in an cube.
                                  This will be 20.*/
      int p_currentStep;      /**<The current step in the processing sequence.
                                  This is incremented by one everytime
                                  CheckStatus is called.*/
      int p_currentPercent;   /**<The current percent we are checking against.
                                  Once this percentage is exceeded we report
                                  that percentage is completed and increase
                                  this value by the increment.*/
      int p_percentIncrement; /**<How much to increment the currentPercent by.
                                  It should only be 1,2, 5, or 10.*/
      bool p_printPercent;

      bool p_autoDisplay;
  };
};

#endif
