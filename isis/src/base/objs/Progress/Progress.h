#ifndef Progress_h
#define Progress_h
/**
 * @file
 * $Revision: 1.2 $
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

#include <string>

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
 *                                     isis.astrogeology...
 *  @history 2004-02-29 Jeff Anderson - Added ability to send progress status to 
 *                                      the parent process
 *  @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation
 *  @history 2005-10-03 Elizabeth Miller - Changed @ingroup tag
 *                                                                        
 *  @todo 2005-02-11 Jeff Anderson - add coded and implementation example to 
 *                                   class documentation                                                     
 */                                                                       

  class Progress {
    private:
      std::string p_text;     /**<Text string to output at the initial call to  
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
  
    public:
      // constructor
      Progress ();
      
      // destructor
      ~Progress ();
  
      // Change the text std::string
      void SetText (const std::string &text);
  
      // Get the text string
      std::string Text () const;
  
      // Change the maximum steps
      void SetMaximumSteps (const int steps);

      // Add steps before completion (for guessed initial step size)
      void AddSteps(const int steps);
  
      // Check and report status
      void CheckStatus ();
  };
};

#endif
