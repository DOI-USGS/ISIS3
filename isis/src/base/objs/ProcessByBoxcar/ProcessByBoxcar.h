#ifndef ProcessByBoxcar_h
#define ProcessByBoxcar_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Process.h"
#include "Buffer.h"

namespace Isis {
  /**
   * @brief Process cubes by boxcar
   *
   * This is the processing class used to move a boxcar through cube data. This
   * class allows only one input cube and one output cube.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2003-01-03 Tracie Sucharski
   *
   * @internal
   *   @history 2003-04-02 Tracie Sucharski - Added unitTest
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *   @history 2012-02-24 Steven Lambright - Added Finalize() and ProcessCube()
   *   @history 2015-01-15 Sasha Brownsberger - Added virtual keyword to several
   *                                           functions to ensure successful
   *                                           inheritance between Process and its
   *                                           child classes.  Also made destructor
   *                                           virtual.  References #2215.
   */

  class ProcessByBoxcar : public Isis::Process {

    private:
      bool p_boxsizeSet; //!< Indicates whether the boxcar size has been set
      int p_boxSamples;  //!< Number of samples in boxcar
      int p_boxLines;    //!< Number of lines in boxcar


    public:

      //! Constructs a ProcessByBoxcar object
      ProcessByBoxcar() {
        p_boxsizeSet = false;
      };

      //! Destroys the ProcessByBoxcar object.
      virtual ~ProcessByBoxcar() {};

      void SetBoxcarSize(const int ns, const int nl);

      using Isis::Process::StartProcess;  // make parent functions visable
      virtual void StartProcess(void funct(Isis::Buffer &in, double &out));
      void ProcessCube(void funct(Isis::Buffer &in, double &out)) {
        StartProcess(funct);
      }

      void EndProcess();
      void Finalize();
  };
};

#endif
