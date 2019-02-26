/**
 * @file
 * $Revision$
 * $Date$
 * $Id$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "NaifDskPlateModel.h"

#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>

#include <QStringList>

#include "FileName.h"
#include "IException.h"
#include "Intercept.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifDskApi.h"
#include "NaifStatus.h"
#include "SurfacePoint.h"
#include "TriangularPlate.h"

using namespace std;

namespace Isis {

  /** Default empty constructor */
  NaifDskPlateModel::NaifDskPlateModel() { }



  /** Construct given a file name - the only way to create with a DSK file */
  NaifDskPlateModel::NaifDskPlateModel(const QString &dskfile) {
    m_dsk = openDSK(dskfile);
    if ( !isValid() ) {
      QString mess = "Could not open DSK file " + dskfile;
      throw IException(IException::User, mess, _FILEINFO_);
    }
  }



  NaifDskPlateModel::~NaifDskPlateModel() { }



  /** Checks validity of the object */
  bool NaifDskPlateModel::isValid() const {
    return ( !m_dsk.isEmpty() );
  }



  /** Returns the nane of the NAIF DSK file */
  QString NaifDskPlateModel::filename() const {
    if ( isValid() ) {
      return m_dsk.front()->m_dskfile;
    }
    return ( QString() );
  }



  /** Returns the number of plates in the DSK file - mostly for conformity */
  int NaifDskPlateModel::size() const {
    return ( numberPlates() );
  }



  /** Returns the number of plates in the model */
  int NaifDskPlateModel::numberPlates() const {
    int totalPlates = 0;
    if ( isValid() ) {
      for(auto segment: m_dsk) {
        totalPlates += segment->m_plates;
      }
    }
    return totalPlates;
  }



  /** Returns the number of vertices in the plate model */
  int NaifDskPlateModel::numberVertices() const {
    int totalVertices = 0;
    if ( isValid() ) {
      for(auto segment: m_dsk) {
        totalVertices += segment->m_vertices;
      }
    }
    return totalVertices;
  }



  /**
   * @brief Get surface intersection for a lat/lon grid point
   *
   * This method will return a surface point intercept given a lat/lon coordinate.
   * Primary use of this method is to determine radius values at the grid point.
   *
   * @history 2013-12-05 Kris Becker Original Version
   *
   * @param lat Latitide of the grid coordinate point
   * @param lon Longitude of the grid coordinate point
   *
   * @return SurfacePoint*  Returns a pointer to a valid intercept point
   */
  SurfacePoint *NaifDskPlateModel::point(const Latitude &lat,
                                         const Longitude &lon) const {

    // Sanity check on input point
    verify ( lat.isValid(), "Latitude parameter invalid in NaifDskPlateMode::point()" );
    verify ( lon.isValid(), "Longitude parameter invalid in NaifDskPlateMode::point()" );

    // Ensure a DSK file is opened or exception is thrown
    verify( isValid(), "NAIF DSK file not opened/valid!");

    // Get the lon/lat point in radians
    SpiceDouble lonlat[2];
    lonlat[0] = lon.positiveEast(Angle::Radians);
    lonlat[1] = lat.planetocentric(Angle::Radians);
    SpiceInt npoints(1);
    NaifVertex spoint(3, 0.0);
    SpiceInt plateId(-1);
    QStringList errors;
    QVector<NaifVertex> intersections;

    for (auto segment: m_dsk) {
      try {
        llgrid_pl02( segment->m_handle, &segment->m_dladsc, npoints,
                     (ConstSpiceDouble (*)[2]) lonlat,
                     (SpiceDouble (*)[3]) &spoint[0], &plateId);
        NaifStatus::CheckErrors();
        // If the llgrid_pl02 method didn't error, then we found an intersection
        intersections.push_back(spoint);
      }
      catch(IException &e) {
        // Collect error messages, if there is no intersection for any segment
        // return all of them.
        errors.append(e.toString());
      }
    }

    if ( intersections.isEmpty() ) {
      QString msg = "Could not compute a ground point at latitude [" +
                    lat.toString() + "] and longitude [" + lon.toString() +
                    "] using dsk file [" + filename() + "].";
      msg += " NAIF intersection error(s):\n" + errors.join("\n");
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    // TODO sort based on something
    return  ( makePoint(intersections.front()) );
  }



  /**
   * @brief Determine a target body intercept point from an observer and look
   *        direction
   *
   * This method will compute an intercept point on the target body given a body
   * fixed vertex of an observer and a look direction.  This implements a true ray
   * intersection algortihm.
   *
   * If an intercept point cannot be found a null pointer is returned.
   *
   * @history 2013-12-05 Kris Becker Original Version
   *
   * @param vertex  Body fixed X/Y/Z coordinate of the observer
   * @param raydir  Vector that specifies a look direction
   *
   * @return Intercept* Pointer to a valid intercept point.
   */
  Intercept *NaifDskPlateModel::intercept(const NaifVertex &vertex,
                                          const NaifVector &raydir) const {
    // Sanity check on input parameters
    try {
      verify( validate(vertex), "Invalid/bad dimensions on intercept source point");
      verify( validate(raydir), "Invalid/bad dimensions on ray direction vector");
    }
    catch ( IException &ie ) {
      throw IException(ie, IException::Programmer,
                       "Invalid point source data to determine intercept",
                       _FILEINFO_);
    }

    // Ensure a DSK file is opened or exception is thrown
    verify( isValid(), "NAIF DSK file not opened/valid!");

    // Get the lon/lat point in radians
    SpiceInt plateid;
    NaifVertex xpt(3, 0.0);
    SpiceBoolean found;
    QStringList errors;
    QVector<Intercept> intersections;

    // Find the plate of intersection and intercept point
    for (auto segment: m_dsk) {
      try {
        dskx02_c( segment->m_handle, &segment->m_dladsc, &vertex[0], &raydir[0],
                  &plateid, &xpt[0], &found);
        // Check status
        NaifStatus::CheckErrors();
        if ( found ) {
          NaifTriangle triangle = plate(plateid, segment);
          intersections.push_back(Intercept(vertex, raydir, makePoint(xpt),
                                            new TriangularPlate(triangle, plateid)));
        }
      }
      catch(IException &e) {
        // Collect error messages, if there is no intersection for any segment
        // return all of them.
        errors.append(e.toString());
      }
    }

    if ( !errors.isEmpty() ) {
      QString msg = "NAIF errors occured while computing a ground point "
                    "intersection for ray starting at [" + toString(vertex[0]) +
                    ", " + toString(vertex[1]) + ", " + toString(vertex[2]) + "] "
                    "and pointing in the direction of [" + toString(raydir[0]) +
                    ", " + toString(raydir[1]) + ", " + toString(raydir[2]) + "] "
                    "using dsk file [" + filename() + "].";
      msg += " NAIF error(s):\n" + errors.join("\n");
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    if ( !intersections.isEmpty() ) {
      return (new Intercept(intersections.front()) );
    }
    return (0);
  }



  /**
   * @brief Retrieve the triangular plate identified by its ID
   *
   * This method can be used to retrieve a particular plate specified by its ID.
   *
   * @author 2014-02-05 Kris Becker
   *
   * @param plateid Valid id of the plate to retrieve
   * @param segment The information about the segment the plate belongs to
   *
   * @return NaifTriangle Triangle associated with the plate id
   */
  NaifTriangle NaifDskPlateModel::plate(SpiceInt plateid, SharedNaifDskDescriptor segment) const {

    // Ensure a DSK file is opened or exception is thrown
    verify( isValid(), "NAIF DSK file not opened/valid!");

    // Sanity check on plateid
    if ( (plateid < 1) || (plateid > segment->m_plates) ) {
      QString mess = "Plateid = " + QString::number(plateid) + " is invalid";
      throw IException(IException::Programmer, mess, _FILEINFO_);
    }

   // Get the plate
    SpiceInt nplates;
    SpiceInt iplate[3];

    NaifStatus::CheckErrors();
    dskp02_c(segment->m_handle, &segment->m_dladsc, plateid, 1, &nplates,
             ( SpiceInt(*)[3] )(iplate));
    NaifStatus::CheckErrors();

    // Get the verticies of the plates
    NaifTriangle plate(3, 3);
    SpiceInt n;
    for (int i = 0 ; i < 3 ; i++) {
    dskv02_c(segment->m_handle, &segment->m_dladsc, iplate[i], 1, &n,
             ( SpiceDouble(*)[3] )(plate[i]));
    }
    NaifStatus::CheckErrors();

    return plate;
  }



  /**
   * @brief Opens a valid NAIF DSK plate model file
   *
   * This method opens a NAIF DSK plate model file and extracts segment
   * information
   *
   * @history 2013-12-05 Kris Becker Original Version
   * @history 2019-02-26 Jesse Mapel - Changed to collect and return info for
   *                                   each segment.
   *
   * @param dskfile Name of NAIF DSK file to open.  These files typically end in a
   *                ".bds" extension
   *
   * @return QVector<SharedNaifDskDescriptor> Returns a vector of information
   *                                          about each segment.
   */
  QVector<NaifDskPlateModel::SharedNaifDskDescriptor> NaifDskPlateModel::openDSK(const QString &dskfile) {

    // Sanity check
    FileName dskFile(dskfile);
    if ( !dskFile.fileExists() ) {
      QString mess = "NAIF DSK file [" + dskfile + "] does not exist.";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Open the NAIF Digital Shape Kernel (DSK)
    SpiceInt dlaHandle;
    dasopr_c( dskFile.expanded().toLatin1().data(), &dlaHandle );
    NaifStatus::CheckErrors();

    // Search to the first DLA segment
    SpiceBoolean found;
    SpiceDLADescr currentDLADescriptor;
    dlabfs_c( dlaHandle, &currentDLADescriptor, &found );
    NaifStatus::CheckErrors();
    if ( !found ) {
      QString mess = "No segments found in DSK file " + dskfile ;
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Collect each segment
    QVector<SharedNaifDskDescriptor> segments;
    while (found) {
      SharedNaifDskDescriptor segment(new NaifDskDescriptor());
      segment->m_dskfile = dskfile;
      segment->m_handle = dlaHandle;
      segment->m_dladsc = currentDLADescriptor;

      // Get the DSK descriptor
      dskgd_c( dlaHandle, &currentDLADescriptor, &segment->m_dskdsc );
      NaifStatus::CheckErrors();

      // Get the Vertex and Plate counts
      dskz02_c( dlaHandle, &currentDLADescriptor,
                &segment->m_vertices, &segment->m_plates );
      NaifStatus::CheckErrors();

      segments.push_back(segment);

      // Move to the next segment
      SpiceDLADescr nextDLADescriptor;
      dlafns_c( dlaHandle, &currentDLADescriptor, &nextDLADescriptor, &found );
      NaifStatus::CheckErrors();
      if (found) {
        currentDLADescriptor = nextDLADescriptor;
      }
    }

    // TODO check that all segments are consistent

    return segments;
  }



  /** Convenience method for generalized error reporting */
  bool NaifDskPlateModel::verify(const bool &test, const QString &errmsg,
                                 const NaifDskPlateModel::ErrAction &action)
                                 const {
    if ( ( Throw == action ) && ( !test ) ) {
      throw IException(IException::Programmer, errmsg, _FILEINFO_);
    }

    // Looks good
    return ( test );
  }



  /** Construct and return a SurfacePoint pointer  */
  SurfacePoint *NaifDskPlateModel::makePoint(const NaifVertex &v) const {
    verify( validate(v), "Vertex/point invalid - not a 3 vector" );
    return (new SurfacePoint(Displacement(v[0], Displacement::Kilometers),
                             Displacement(v[1], Displacement::Kilometers),
                             Displacement(v[2], Displacement::Kilometers)));

  }


  NaifDskPlateModel::NaifDskDescriptor::NaifDskDescriptor() : m_dskfile(), m_handle(-1),
                                                              m_dladsc(), m_dskdsc(), m_plates(0),
                                                              m_vertices(0) {
  }


  NaifDskPlateModel::NaifDskDescriptor::~NaifDskDescriptor() {
    if ( -1 != m_handle ) {
      NaifStatus::CheckErrors();
      dascls_c ( m_handle );
      NaifStatus::CheckErrors();
    }
  }


}  // namespace Isis
