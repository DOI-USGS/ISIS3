#ifndef ProcessPolygons_h
#define ProcessPolygons_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <geos/geom/Coordinate.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/Polygon.h>

#include "Brick.h"
#include "FileName.h"
#include "Process.h"
#include "ProjectionFactory.h"

namespace Isis {

  /**
   *
   * @author 2007-12-07 Stacy Alley
   *
   * @ingroup HighLevelCubeIO
   *  
   * @internal
   *   @history 2010-02-26 Steven Lambright - Now using a geometry snapper on
   *             points before testing if they are inside a polygon inside
   *             DoWork(...).
   *   @history 2012-02-24 Steven Lambright - Added Finalize()
   *   @history 2013-03-27 Jeannie Backer - Added programmer comments.
   *                           References #1248.
   *   @history 2013-02-06 Tracie Sucharski - Improved speed by adding BoxcarCaching Algorithm
   *                           for I/O, using prepared polygons for the intersections and read/write
   *                           the entire spectra for the input pixel rather than one band at a
   *                           time.  References #1289.
   *   @history 2013-09-10 Stuart Sides, Tracie Sucharski - Cleaned up code, added additional error
   *                           checks for valid polygons, added option to check for either input
   *                           pixel intersecting the center of the output pixel or any part of the
   *                           output pixel.  References #1604.
   *   @history 2015-01-19 Sasha Brownsberger - Changed name of SetOutputCube function to SetStatCubes 
   *                                            function to better reflect the command functionality 
   *                                            and to avoid conflicts with Process::SetOutputCube 
   *                                            virtual function.  References #2215.
   *                          
   */
  class ProcessPolygons : public Isis::Process {

    public:
      ProcessPolygons();

      void SetStatCubes(const QString &parameter, const int nsamps,
                         const int nlines, int nbands = 1);

      void SetStatCubes(const QString &avgFileName, const QString
                         &countFileName, Isis::CubeAttributeOutput &atts,
                         const int nsamps, const int nlines, int nbands = 1);
      void SetIntersectAlgorithm(const bool useCenter);

      Isis::Cube *AppendOutputCube(const QString &avgFileName,
                                   const QString &countFileName = "");

      void Rasterize(std::vector<double> &samples,
                     std::vector<double> &lines,
                     std::vector<double> &values);

      void Rasterize(std::vector<double> &samples,
                     std::vector<double> &lines,
                     int &band, double &value);

      void EndProcess();
      void Finalize();

    private:
      void FillPolygon(int Flag);
      void GetPolygonCoords();

      bool m_useCenter;
      std::vector<double> m_sampleVertices, m_lineVertices, m_dns;
      int m_band;
      Brick *m_average;
      Brick *m_count;
      geos::geom::Polygon *m_imagePoly;

  };

};

#endif
