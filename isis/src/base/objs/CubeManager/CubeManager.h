#ifndef CubeManager_h
#define CubeManager_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IException.h"

#include <QMap>
#include <QQueue>
#include <QString>
#include <memory>

#include <iostream>

using namespace std;

/*
 *   Unless noted otherwise, the portions of Isis written by the
 *   USGS are public domain. See individual third-party library
 *   and package descriptions for intellectual property
 *   information,user agreements, and related information.
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
namespace Isis {
  class Cube;

  /**
   * @brief Class for quick re-accessing of cubes based on file name
   *
   * This class holds cubes in static memory for reading.
   * This is helpful to prevent reading of the same cube many times. Files will remain
   * opened for reading, this is not for use with a cube that will ever be written
   * to. You can either use the static methods of the class, in which case cubes
   * will be cleaned up after IsisMain(...) is done executing, or you can
   * instantiate the class for more control.
   *
   * @author 2008-05-20 Steven Lambright
   *
   * @internal
   *   @author 2008-06-19 Steven Lambright - Added CleanUp methods
   *   @history 2008-08-19 Steven Koechle - Removed Geos includes
   *   @author 2009-02-12 Steven Lambright - Made the class available as an
   *           instance and not just statically. Added optional cube open limits.
   *   @history 2009-11-03 Mackenzie Boyd - Modified to include
   *            cube attributes (input) when opening cubes.
   *   @history 2011-06-08 Steven Lambright - Better handles the case when a
   *                Cube fails to open, fixes #161.
   *   @history 2015-07-15 Ian Humphrey - Added private member variable to store the max number
   *                           files opened (60% of the system's limitations). Modified
   *                           SetNumOpenCubes to set the maximum number of open cubes to
   *                           60% of the system's file limits if the passed value exceeds this
   *                           60% limitations. Modified OpenCube to always clean excess cubes.
   *                           Updated unit test for better test coverage. Fixes #1951.
   */
  class CubeManager  {
    public:
      CubeManager();
      ~CubeManager();

      /**
       * This method calls the method OpenCube() on the static instance
       *
       * @see OpenCube
       *
       * @param cubeFileName FileName of the cube to be opened
       *
       * @return Cube* Pointer to the cube (guaranteed not null)
       */
      static Cube *Open(const QString &cubeFileName) {
        if (!p_instance) {
          p_instance = new CubeManager();
        }

        return p_instance->OpenCube(cubeFileName);
      }

      /**
       * This sets the maximum number of opened cubes for this instance of
       * CubeManager. The last "maxCubes" opened cubes are guaranteed to be
       * valid as long as one of the CleanCubes(...) are not called.
       * If the maximum number of open cubes specified exceeds 60% of system limitations,
       * the maximum number of opened cubes will be set to a 60% of the
       * system's open file limitation (this considers files used by the current process).
       *
       * @param numCubes Maximum number of open cubes
       */
      void SetNumOpenCubes(unsigned int numCubes) {

        // check to see if numCubes exceeds the number of open files limitations
        if (numCubes > p_maxOpenFiles) {
          p_currentLimit = p_maxOpenFiles;
        }
        p_currentLimit = numCubes;
      }


      /**
       * This method calls CleanCubes(const QString &cubeFileName)  on the static
       * instance
       *
       * @see CleanCubes(const QString &cubeFileName)
       *
       * @param cubeFileName The filename of the cube to destroy from memory
       */
      static void CleanUp(const QString &cubeFileName) {
        p_instance->CleanCubes(cubeFileName);
      }


      /**
       * This method calls CleanCubes() on the static instance
       *
       * @see CleanCubes
       */
      static void CleanUp() {
        delete p_instance;
        p_instance = 0;
      };

      void CleanCubes(const QString &cubeFileName);
      void CleanCubes();
      Cube *OpenCube(const QString &cubeFileName);

    protected:

      //! There is always at least one instance of CubeManager around
      static CubeManager *p_instance;

      //! This keeps track of the open cubes
      QMap<QString, Cube *> p_cubes;

      //! This keeps track of cubes that have been opened
      QQueue<QString> p_opened;

      //! The current limit regarding number of open files allowed
      unsigned int p_currentLimit;

      //! 60% of the maximum number of open files allowed by system resources
      unsigned int p_maxOpenFiles;
  };
}

#endif
