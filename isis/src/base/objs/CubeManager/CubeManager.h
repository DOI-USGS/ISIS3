#ifndef CubeManager_h
#define CubeManager_h

#include <QString>
#include <QMap>
#include <QQueue>

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
   *  
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
       * @param cubeFilename Filename of the cube to be opened
       * 
       * @return Cube* Pointer to the cube (guaranteed not null)
       */
      static Cube *Open(const std::string &cubeFilename) { return p_instance.OpenCube(cubeFilename); }

      /**
       * This method calls CleanCubes(const std::string &cubeFilename)  on the static 
       * instance 
       *  
       * @see CleanCubes(const std::string &cubeFilename)
       * 
       * @param cubeFilename The filename of the cube to destroy from memory 
       */
      static void CleanUp(const std::string &cubeFilename) { p_instance.CleanCubes(cubeFilename); }

      /**
       * This method calls CleanCubes() on the static instance
       *  
       * @see CleanCubes 
       */
      static void CleanUp() { p_instance.CleanCubes(); };

      void CleanCubes(const std::string &cubeFilename);
      void CleanCubes();

      Cube *OpenCube(const std::string &cubeFilename);

      /**
       * This sets the maximum number of opened cubes for this instance of 
       * CubeManager. The last "maxCubes" opened cubes are guaranteed to be 
       * valid as long as one of the CleanCubes(...) are not called.
       * 
       * @param numCubes Maximum number of open cubes 
       */
      void SetNumOpenCubes(unsigned int numCubes) { p_minimumCubes = numCubes; }

    protected:
      //! There is always at least one instance of CubeManager around
      static CubeManager p_instance;

      //! This keeps track of the open cubes
      QMap<QString, Cube *> p_cubes;
      //! This keeps track of cubes that have been opened
      QQueue<QString> p_opened;

      //! At least this many cubes must be allowed in memory, more can be cleaned up, 0 means no limit
      unsigned int p_minimumCubes;
  };
}

#endif
