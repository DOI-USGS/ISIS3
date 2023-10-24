/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IProj.h"

#include <proj.h>

#include "IException.h"
#include "TProjection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

using namespace std;

namespace Isis {
  IProj::IProj(Pvl &label, bool allowDefaults) :
      TProjection::TProjection(label) {
    PvlGroup &mapGroup = label.findGroup("Mapping", Pvl::Traverse);
    if (!mapGroup.hasKeyword("ProjectionType")) {
      QString message = "No ProjectionType keyword in mapping group, either add a ProjectionType or select a different projection method";
      throw IException(IException::User, message, _FILEINFO_);
    }
    m_C = proj_context_create();

    m_userOutputProjType = new QString(mapGroup["ProjectionType"]);
    std::string userOutputProjStr = "+proj=" + (*m_userOutputProjType).toStdString();
    userOutputProjStr += " +x_0=0 +y_0=0";
    addRadii(userOutputProjStr);

    std::string llaProjString = "+proj=latlong";
    addRadii(llaProjString);

    if (LongitudeDomainString() == "360") {
      llaProjString += " +lon_0=180";
    }

    if (LongitudeDirectionString() == "PositiveEast" || LongitudeDomainString() == "180") {
      llaProjString += " +axis=enu";
      userOutputProjStr += " +axis=enu";
    }
    else {
      llaProjString += " +axis=wnu";
      userOutputProjStr += " +axis=wnu";
    }

    // We will likely need to add more proj parameters to proj strings
    // to support individual proj projections in ISIS
    
    llaProjString += " +type=crs";
    userOutputProjStr += " +type=crs";
    m_userOutputProjStr = new QString(userOutputProjStr.c_str());

    m_llaProj = proj_create(m_C, llaProjString.c_str());

    if (0 == m_llaProj) {
      QString msg = "Unable to create projection from [" + QString(llaProjString.c_str()) + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    /* Get the geodetic CRS for that projection. */
    m_geocentricProj = proj_crs_get_geodetic_crs(m_C, m_llaProj);

    if (0 == m_geocentricProj) {
      QString msg = "Unable to create geocentric projection";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_geocentProj2llaProj = proj_create_crs_to_crs_from_pj(m_C, m_geocentricProj, m_llaProj, 0, 0);

    /* Create a projection. */
    std::string projString = m_userOutputProjStr->toStdString();
    m_outputProj = proj_create(m_C, projString.c_str());
    if (0 == m_outputProj) {
      QString msg = "Unable to create projection from [" + QString(projString.c_str()) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    m_llaProj2outputProj = proj_create_crs_to_crs_from_pj(m_C, m_llaProj, m_outputProj, 0, 0);
  }

  //! Destroys the IProj object
  IProj::~IProj() {
    delete m_userOutputProjType;
    delete m_userOutputProjStr;
    proj_destroy(m_llaProj);
    proj_destroy(m_outputProj);
    proj_destroy(m_geocentricProj);
    proj_destroy(m_llaProj2outputProj);
    proj_destroy(m_geocentProj2llaProj);
    proj_context_destroy(m_C);
  }

  /**
   * Returns the name of the map projection, "Proj"
   *
   * @return QString Name of projection, "Proj"
   */
  QString IProj::Name() const {
    return "Proj";
  }

  /**
   * This function returns the keywords that this projection uses.
   * 
   * We also include the generated PROJ string
   *
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup IProj::Mapping()  {
    PvlGroup mapping = TProjection::Mapping();

    mapping += PvlKeyword("ProjStr", *m_userOutputProjStr);
    mapping += PvlKeyword("ProjectionType", *m_userOutputProjType);

    return mapping;
  }

  /**
   * Returns the version of the map projection
   *
   *
   * @return QString Version number
   */
  QString IProj::Version() const {
    return "1.0";
  }

  bool IProj::SetGround(const double lat, const double lon) {
    m_longitude = lon;
    m_latitude = lat;

    PJ_COORD c_in;
    c_in.lpz.lam = m_longitude;
    c_in.lpz.phi = m_latitude;

    PJ_COORD c_out;

    if (LatitudeTypeString() == "Planetographic") {
      c_out = proj_trans(m_geocentProj2llaProj, PJ_FWD, c_in);

      c_in.lpz.lam = c_out.lpz.lam;
      c_in.lpz.phi = c_out.lpz.phi;
    }

    c_out = proj_trans(m_llaProj2outputProj, PJ_FWD, c_in);
    SetComputedXY(c_out.xy.x, c_out.xy.y);
    m_good = true;
    return m_good;
  }

  bool IProj::SetCoordinate(const double x, const double y) {
    SetXY(x, y);

    PJ_COORD c_in;
    c_in.xy.x = x;
    c_in.xy.y = y;

    PJ_COORD c_out = proj_trans(m_llaProj2outputProj, PJ_INV, c_in);

    if (LatitudeTypeString() == "Planetographic") {
      c_in.lpz.lam = c_out.lpz.lam;
      c_in.lpz.phi = c_out.lpz.phi;

      c_out = proj_trans(m_geocentProj2llaProj, PJ_INV, c_in);
    }

    m_longitude  = c_out.lpz.lam;
    m_latitude = c_out.lpz.phi;
    m_good = true;
    return m_good;
  }

    /**
   * This method is used to determine the x/y range which completely covers the
   * area of interest specified by the lat/lon range. The latitude/longitude
   * range may be obtained from the labels. The purpose of this method is to
   * return the x/y range so it can be used to compute how large a map may need
   * to be. For example, how big a piece of paper is needed or how large of an
   * image needs to be created. The method may fail as indicated by its return
   * value. This currently mimics the sinusoidal projection's XYRange check
   * and should be made more robust for use in proj. This will likely be a 
   * method that walks the boundary of the projection
   *
   * @param minX Minimum x projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   *
   * @param maxX Maximum x projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   *
   * @param minY Minimum y projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   *
   * @param maxY Maximum y projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   *
   * @return bool Indicates whether the method was successful.
   */
  bool IProj::XYRange(double &minX, double &maxX,
                      double &minY, double &maxY) {
    // Check the corners of the lat/lon range
    XYRangeCheck(m_minimumLatitude, m_minimumLongitude);
    XYRangeCheck(m_maximumLatitude, m_minimumLongitude);
    XYRangeCheck(m_minimumLatitude, m_maximumLongitude);
    XYRangeCheck(m_maximumLatitude, m_maximumLongitude);

    // If the latitude crosses the equator check there
    if ((m_minimumLatitude < 0.0) && (m_maximumLatitude > 0.0)) {
      XYRangeCheck(0.0, m_minimumLongitude);
      XYRangeCheck(0.0, m_maximumLongitude);
    }

    // Make sure everything is ordered
    if (m_minimumX >= m_maximumX) return false;
    if (m_minimumY >= m_maximumY) return false;

    // Return X/Y min/maxs
    minX = m_minimumX;
    maxX = m_maximumX;
    minY = m_minimumY;
    maxY = m_maximumY;
    return true;
  }

  void IProj::addRadii(std::string &projString) {
    std::string radiiStr = " +a=" +
                           toString(m_equatorialRadius, 16).toStdString() +
                           " +b=" +
                           toString(m_polarRadius, 16).toStdString() +
                           " +units=m";
    projString += radiiStr;
  }

  /**
   * This is the function that is called in order to instantiate a
   * Sinusoidal object.
   *
   * @param lab Cube labels with appropriate Mapping information.
   *
   * @param allowDefaults Indicates whether CenterLongitude are allowed to
   *                      be computed using the middle of the longitude
   *                      range specified in the labels.
   *
   * @return @b Isis::Projection* Pointer to a Sinusoidal projection object.
   */
  extern "C" Isis::TProjection *IProjPlugin(Isis::Pvl &lab,
                                                 bool allowDefaults)
  {
    return new Isis::IProj(lab, allowDefaults);
  }
}