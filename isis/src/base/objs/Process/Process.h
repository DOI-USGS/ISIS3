#ifndef Process_h
#define Process_h
/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/10/31 00:19:38 $
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

#include "Cube.h"
#include "Progress.h"
#include "CubeAttribute.h"
#include "Statistics.h"
  
namespace Isis {
  const int SizeMatch = 1;
  const int SpatialMatch = 2;
  const int OneBand = 16;
  const int BandMatchOrOne = 32;
  const int ReadWrite = 64;
  const int AllMatchOrOne = 128;
};

namespace Isis {
  class UserInterface;

/**
 * @brief Base class for all cube processing derivatives
 *
 * This is the core of the Isis system.  Process, a very important base class is
 * often used to derive new classes which process cubes in a systematic manner.
 * Some of these derived classes include ProcessByLine, FilterProcess, 
 * RubberSheet, and Export.  For history buffs, the Process class equates to 
 * DOCUBE in Isis 2.0 and DOIO in PICS. Essentially, this class manages much of 
 * the tedious programming work for cube i/o, user interaction, history, etc.
 *                                                             
 * If you would like to see Process being used in implementation, see stats.cpp.  
 * For classes that inherit from Process, see ProcessByLine, FilterProcess, 
 * ProcessByBoxcar, RubberSheet, Export, or Import
 * 
 * A working application example of Process can be found in the @link stats.cpp 
 * stats class.
 * @endlink
 *
 * @ingroup HighLevelCubeIO
 *
 * @author 2002-06-21 Jeff Anderson 
 * 
 * @internal
 *  @history 2002-06-24 Jeff Anderson - Added ic_base::OneBand requirement 
 *                                      option for SetInputCube methods
 *  @history 2002-07-15 Stuart Sides - Added capabilities for applications to 
 *                                     log information to standard out or the 
 *                                     GUI log window (ShowLog and Log).
 *  @history 2003-02-03 Jeff Anderson - Added propagation of labels from the 
 *                                      first input cube to each output cube
 *  @history 2003-02-04 Jeff Anderson - Added PropagateLabels method
 *  @history 2003-02-04 Jeff Anderson - Fixed bug in propagation of input labels
 *                                      to output cube
 *  @history 2003-02-07 Jeff Anderson - Integrated iString class into the Log 
 *                                      methods
 *  @history 2003-04-23 Jeff Anderson - Made accomodations for the updated 
 *                                      Preference class
 *  @history 2003-04-28 Jeff Anderson - Added method to Log Label objects
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...
 *  @history 2003-06-04 Jeff Anderson - Added Progress method
 *  @history 2003-06-30 Jeff Anderson - Added MissionData method
 *  @history 2003-09-02 Jeff Anderson - Added SetOutputWorkCube method
 *  @history 2003-10-06 Jeff Anderson - Added IsisPvl pointer to the SetOutput 
 *                                      methods so that the programmer can 
 *                                      control characteristics of the output 
 *                                      cube format.
 *  @history 2003-11-07 Stuart Sides - Added "ReadWrite" capability to the 
 *                                     requirements parameter on SetInputCube 
 *                                     and SetInputWorkCube.
 *  @history 2003-11-07 Jeff Anderson - Modified preference loading order so 
 *                                      that the Init method will load 
 *                                      $ISISROOT/testData/base/TestPreferences 
 *                                      when the application name contains the 
 *                                      word "unitTest".  This forces unit tests
 *                                      to use a standard set of system 
 *                                      preferences.
 *  @history 2003-12-01 Jeff Anderson - Added ProgagateLabel method to allow for
 *                                      propagation from secondary cubes.
 *  @history 2003-12-18 Jeff Anderson - Modifed preference loading to look for 
 *                                      the users preference file in the 
 *                                      directory $HOME/.Isis
 *  @history 2003-12-18 Jeff Anderson - Modified MissionData method to allow 
 *                                      searching for the highest version of
 *                                      a file
 *  @history 2004-02-02 Jeff Anderson - Modified SetInputCube and SetOutputCube 
 *                                      methods to accept CubeAttribute classes.
 *                                      Added the ClearInputCubes method.
 *  @history 2004-02-29 Jeff Anderson - Added ability to send LogResults to the 
 *                                      parent process
 *  @history 2004-03-01 Jeff Anderson - Made the Init method check to see if 
 *                                      .Isis and .Isis/history directories
 *                                      exist and if not then create them
 *  @history 2004-04-17 Stuart Sides - Fixed problem when creating .Isis/history
 *                                     directories on Solaris.
 *  @history 2004-06-30 Jeff Anderson - Added propagation of blobs 
 *  @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation
 *  @history 2006-09-19 Brendan George - Added WriteHistory function to
 *                                       independently write the history to the
 *                                       cube
 *  @history 2007-06-27 Steven Lambright - Added propagation of polygon blobs
 *  @history 2007-07-19 Steven Lambright - Fixed memory leak
 *  @history 2007-07-27 Steven Lambright - Updated AllMatchOrOne and BandMatchOrOne
 *                                         error messages
 *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
 *  @history 2009-10-29 Travis Addair - Added method calculating
 *           statistics on all bands of all cubes
 *  @history 2009-10-30 Travis Addair - Changed method
 *           calculating statistics to store off its results in
 *           both p_bandStats and p_cubeStats, and added methods
 *           to access those results
 * 
 *  @todo 2005-02-08 Jeff Anderson - add an example to the class documentation.
 */ 
  class Process {
    protected:
      Isis::Progress *p_progress;  //!< Pointer to a Progress object
      /**
       * Flag indicating if labels are be propagated to output cubes.
       */
      bool p_propagateLabels;
      /**
       * Flag indicating if tables are be propagated to output cubes.
       */
      bool p_propagateTables;
      /**
       * Flag indicating if blobs are be propagated to output cubes.
       */
      bool p_propagatePolygons;
      /**
       * Flag indicating if history is to be propagated to output cubes.
       */
      bool p_propagateHistory;
      /**
       * Flag indicating if original lable is to be propagated to output cubes.
       */
      bool p_propagateOriginalLabel;

      /**
       * Holds the calculated statistics for each band separately of 
       * every input cubei after the CalculateStatistics method is 
       * called. 
       */
      std::vector< std::vector< Isis::Statistics* > > p_bandStats;

      /**
       * Holds the calculated statistics for every band together of 
       * every input cubei after the CalculateStatistics method is 
       * called. 
       */
      std::vector< Isis::Statistics* > p_cubeStats;

     /**
      * A vector of pointers to opened Cube objects. The pointers are 
      * established in the SetInputCube/SetInputWorkCube methods.
      */                                              
      std::vector<Isis::Cube *> InputCubes; 

     /**
      * A vector of pointers to allocated Cube objects. The pointers are 
      * established in the SetOutputCube method.
      */  
      std::vector<Isis::Cube *> OutputCubes;
  
    public:
      Process ();
      virtual ~Process ();

     /** 
      * In the base class, this method will invoked a user-specified function 
      * exactly one time. In derived classes such as ProcessByLine, the
      * StartProcess will invoke a user-specified function for every line in a 
      * cube. 
      * 
      * @param funct()  Name of your processing function
      */
      void StartProcess (void funct ()) { funct(); };
      virtual void EndProcess ();
  

      Isis::Cube* SetInputCube (const std::string &parameter,
                                    const int requirements=0);
      Isis::Cube* SetInputCube (const std::string &fname,
                                    const Isis::CubeAttributeInput &att,
                                    int requirements=0);
      void ClearInputCubes ();

      Isis::Cube* SetOutputCube (const std::string &parameter);
      Isis::Cube* SetOutputCube (const std::string &parameter, const int nsamps,
                                     const int nlines, const int nbands = 1);
      Isis::Cube* SetOutputCube (const std::string &fname,
                                     const Isis::CubeAttributeOutput &att,
                                     const int nsamps, const int nlines,
                                     const int nbands = 1);
  
      void PropagateLabels (const bool prop);
      void PropagateLabels (const std::string &cube);
      void PropagateTables (const bool prop);
      void PropagatePolygons (const bool prop);
      void PropagateHistory (const bool prop);
      void PropagateOriginalLabel (const bool prop);

     /**
      * This method returns a pointer to a Progress object
      * 
      * @return Progress*
      */
      Isis::Progress *Progress() { return p_progress; };
  
      std::string MissionData (const std::string &mission, const std::string &file,
                          bool highestVersion=false);

      void WriteHistory(Cube &cube);

      void CalculateStatistics();

      /**
       * Get the vector of Statistics objects for each band separately 
       * of a specified input cube. 
       *  
       * @param index The index of the input cube in InputCubes 
       * 
       * @return vector<Statistics*> A list of statistics ordered by 
       *         band
       */
      inline std::vector<Isis::Statistics*> BandStatistics (
        const unsigned index) { return p_bandStats[index]; }
       
      /**
       * Get the Statistics object for all bands of a specified input
       * cube. 
       *  
       * @param index The index of the input cube in InputCubes 
       * 
       * @return Statistics* Collections of statistics gathered on all
       *         bands
       */
      inline Isis::Statistics* CubeStatistics (
        const unsigned index) { return p_cubeStats[index]; }
  };
}

#endif
