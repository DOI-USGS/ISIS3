#ifndef ProcessMapMosaic_h
#define ProcessMapMosaic_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2010/06/21 18:38:51 $
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
   *  @history 2003-04-28 Stuart Sides - Modified unitTest.cpp to do a better test
   *  @history 2003-09-04 Jeff Anderson - Added SetInputWorkCube method
   *  @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2006-09-01 Elizabeth Miller - Added BandBinMatch option to
   *                                         propagate the bandbin group to the
   *                                         mosaic and make sure the input cube
   *                                         bandbin groups match the mosaics
   *                                         bandbin group
   *  @history 2006-10-20 Stuart Sides - Fixed bug BandBin group did not get
   *                                     copied to the output mosaic.
   *  @history 2008-10-03 Steven Lambright - Fixed problem where return values
   *                                     from SetUniversalGround were not checked
   *  @history 2008-11-18 Christopher Austin - Added the first cube's history to
   *           the mosaic's history along with the history object of the
   *           application which did the mosaic.
   *  @history 2008-12-08 Steven Lambright - Fixed one of the SetOutputCube(...)
   *           methods, a lat/lon range was specified but CreateFromCube was still
   *           being used (needed CreateFromCube because no cubes existed with the
   *           correct range).
   *  @history 2008-12-08 Steven Lambright - MinimumLatitude,MaximumLatitude,
   *           MinimumLongitude,MaximumLongitude keywords no longer required to
   *           exist if passed into SetOutputCube
   *  @history 2010-10-27 Sharmila Prasad - Read input file attributes
   *  @history 2011-01-18 Sharmila Prasad - Added "Average" priority feature, to double
   *           the number of mosaic bands to get Count info
   *  @history 2011-06-28 Jai Rideout and Steven Lambright - Now uses a
   *                          different caching algorithm.
   *  @history 2011-09-08 Sharmila Prasad - Fixed Bug #406, Additional boundary checks similar to
   *            ProcessMosaic so that there will be no interruption for applications like automos
   *
   *  @todo 2005-02-11 Stuart Sides - add coded example and implementation example
   *                                  to class documentation
   */

  class ProcessMapMosaic : public Isis::ProcessMosaic {

    public:

      //! Constructs a Mosaic object
      ProcessMapMosaic();

      //! Destructor
      ~ProcessMapMosaic() { };

      //! Set the output cube to specified file name and specified input images
      //! and output attributes
      Isis::Cube *SetOutputCube(FileList &propagationCubes, CubeAttributeOutput &oAtt,
                                const std::string &mosaicFile);

      //! Set the output cube to specified file name and specified input images
      //! and output attributes and lat,lons
      Isis::Cube *SetOutputCube(FileList &propagationCubes,
                                double slat, double elat, double slon, double elon,
                                CubeAttributeOutput &oAtt, const std::string &mosaicFile);

      //! Set the output cube to specified file name and specified input images
      //! and output attributes and lat,lons
      Isis::Cube *SetOutputCube(const std::string &inputFile,
                                double xmin, double xmax, double ymin, double ymax,
                                double slat, double elat, double slon, double elon, int nbands,
                                CubeAttributeOutput &oAtt, const std::string &mosaicFile);

      //! Set the output cube to specified file name and specified input images
      //! and output attributes and lat,lons
      Isis::Cube *SetOutputCube(const std::string &inputFile, PvlGroup mapping,
                                CubeAttributeOutput &oAtt, const std::string &mosaicFile);

      //! Mosaic output method for Mosaic Processing Method, this will use an existing mosaic
      Isis::Cube *SetOutputCube(const std::string &mosaicFile);

      //! Input cube cannot be set here
      Isis::Cube *SetInputCube() {
        throw IException(IException::Programmer,
                         "ProcessMapMosaic does not support the SetInputCube method",
                         _FILEINFO_);
      }

      //! Mosaic Processing method, returns false if the cube is not inside the mosaic
      bool StartProcess(std::string inputFile);

    private:
      //! Reset the buffer with NULL pixels
      static void FillNull(Buffer &data);

      //! Internal use; SetOutputMosaic (const std::string &) sets to false to
      //! not attempt creation when using SetOutputMosaic
      bool p_createMosaic;
  };
};

#endif

