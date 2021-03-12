#ifndef ProcessMapMosaic_h
#define ProcessMapMosaic_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ProcessMosaic.h"
#include "Buffer.h"
#include "FileList.h"

namespace Isis {
  /**
   * @brief Mosaic two cubs together
   *
   * This class allows a programmer to develop a program which merges two cubes
   * together. The application sets the position where input (child) cube will be
   * placed in the mosaic (parent) cube and priority. The the Mosaic object will
   * merge the overlapping area.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2003-04-28 Stuart Sides
   *
   * @internal
   *   @history 2003-04-28 Stuart Sides - Modified unitTest.cpp to do a better
   *                           test
   *   @history 2003-09-04 Jeff Anderson - Added SetInputWorkCube method
   *   @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2006-09-01 Elizabeth Miller - Added BandBinMatch option to
   *                           propagate the bandbin group to the mosaic and make
   *                           sure the input cube bandbin groups match the
   *                           mosaics bandbin group
   *   @history 2006-10-20 Stuart Sides - Fixed bug BandBin group did not get
   *                           copied to the output mosaic.
   *   @history 2008-10-03 Steven Lambright - Fixed problem where return values
   *                           from SetUniversalGround were not checked
   *   @history 2008-11-18 Christopher Austin - Added the first cube's history to
   *                           the mosaic's history along with the history object
   *                           of the application which did the mosaic.
   *   @history 2008-12-08 Steven Lambright - Fixed one of the SetOutputCube(...)
   *                           methods, a lat/lon range was specified but
   *                           CreateFromCube was still being used (needed
   *                           CreateFromCube because no cubes existed with the
   *                           correct range).
   *   @history 2008-12-08 Steven Lambright - MinimumLatitude,MaximumLatitude,
   *                           MinimumLongitude,MaximumLongitude keywords no
   *                           longer required to exist if passed into
   *                           SetOutputCube
   *   @history 2010-10-27 Sharmila Prasad - Read input file attributes
   *   @history 2011-01-18 Sharmila Prasad - Added "Average" priority feature,
   *                           to double the number of mosaic bands to get Count
   *                           info
   *   @history 2011-06-28 Jai Rideout and Steven Lambright - Now uses a
   *                           different caching algorithm.
   *   @history 2011-09-08 Sharmila Prasad - Fixed Bug #406, Additional boundary
   *                           checks similar to ProcessMosaic so that there will
   *                           be no interruption for applications like automos
   *   @history 2012-10-05 Jeannie Backer - Fixed indentation of history entries.
   *                           Added caught exception to new thrown exception.
   *                           Moved method implementation to cpp. References
   *                           #1169.
   *   @history 2013-03-27 Jeannie Backer - Added programmer comments.
   *                           References #1248.
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   *   @history 2013-11-14 Kimberly Oyama - Modified StartProcess() to account for a negative x
   *                           or y from the projection. These x and y values are used to
   *                           calculate the outline and outsample. Fixes #1736.
   *   @history 2013-12-27 Janet Barrett - Added additional latlonflag parameter to the
   *                           SetOutputCube method so that valid lat/lon values are checked
   *                           for in the input files that make up a mosaic. This change was
   *                           needed to handle input files that are missing the min/max lat/lon
   *                           keywords (such as those that were cropped). Fixes #1500.
   *   @history 2015-01-15 Sasha Brownsberger - Added virtual keyword to several
   *                                            functions to ensure successful
   *                                            inheritance between Process and its
   *                                            child classes. Made destructor virtual.
   *                                            References #2215.
   *
   *  @todo 2005-02-11 Stuart Sides - add coded example and implementation
   *                          example to class documentation
   * @history 2016-08-28 Kelvin Rodriguez - Changed SetOutputCube default parameters to
   *                         avoid hidden virtual function warnings in clang and the abiguous called
   *                         errors. Part of porting to OS X 10.11.
   * @history 2018-08-13 Summer Stapleton - Removed code that added tracking band to mosaic cube as
   *                         tracking is now being stored in a separate tracking cube. This was part
   *                         of a larger refactoring project to turn the tracking band of a mosaic
   *                         cube into its own tracking cube containing the InputImages table to
   *                         relate DN values to input images. Mosaic cube no longer holds tracking
   *                         band or InputImages table, but instead has a Tracking group that points
   *                         to external tracking cube.
   */

  class ProcessMapMosaic : public Isis::ProcessMosaic {

    public:

      ProcessMapMosaic();

      virtual ~ProcessMapMosaic();

      // SetOutputCube() is not virtual in the Process class nor in the
      // ProcessMosaic class, so the following definitions for this method are
      // the only ones that are allowed for ProcessMapMosaic objects and child
      // objects
      virtual Isis::Cube *SetOutputCube(FileList &propagationCubes, CubeAttributeOutput &oAtt,
                                const QString &mosaicFile);

      Isis::Cube *RingsSetOutputCube(FileList &propagationCubes, CubeAttributeOutput &oAtt,
                                const QString &mosaicFile);

      virtual Isis::Cube *SetOutputCube(FileList &propagationCubes,
                                double slat, double elat, double slon, double elon,
                                CubeAttributeOutput &oAtt, const QString &mosaicFile);

      Isis::Cube *RingsSetOutputCube(FileList &propagationCubes,
                                double srad, double erad, double saz, double eaz,
                                CubeAttributeOutput &oAtt, const QString &mosaicFile);

      virtual Isis::Cube *SetOutputCube(const QString &inputFile,
                                double xmin, double xmax, double ymin, double ymax,
                                double slat, double elat, double slon, double elon, int nbands,
                                CubeAttributeOutput &oAtt, const QString &mosaicFile,
                                bool latlonflag=true);

      Isis::Cube *RingsSetOutputCube(const QString &inputFile,
                                double xmin, double xmax, double ymin, double ymax,
                                double srad, double erad, double saz, double eaz, int nbands,
                                CubeAttributeOutput &oAtt, const QString &mosaicFile);

      virtual Isis::Cube *SetOutputCube(const QString &inputFile, PvlGroup mapping,
                                CubeAttributeOutput &oAtt, const QString &mosaicFile);

      Isis::Cube *RingsSetOutputCube(const QString &inputFile, PvlGroup mapping,
                                CubeAttributeOutput &oAtt, const QString &mosaicFile);

      Isis::Cube *SetOutputCube(const QString &mosaicFile);

      Isis::Cube *RingsSetOutputCube(const QString &mosaicFile);

      using Isis::ProcessMosaic::SetInputCube;
      virtual Isis::Cube *SetInputCube();

      using Isis::ProcessMosaic::StartProcess;
      virtual bool StartProcess(QString inputFile);

    private:
      static void FillNull(Buffer &data);

     /**
      * Internal use; SetOutputMosaic (const QString &) sets to false to
      * not attempt creation when using SetOutputMosaic
      */
      bool p_createMosaic;
  };
};

#endif
