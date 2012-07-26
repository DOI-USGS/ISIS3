#ifndef DemShape_h
#define DemShape_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
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
#include <vector>

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

#include "ShapeModel.h"
#include "SurfacePoint.h"
#include "Pvl.h"

namespace Isis {
  class Cube;
  class Interpolator;
  class Portal;
  class Projection;

  /**
   * @brief Define shapes and provide utilities for Isis3 targets
   *
   * This class will define shapes of Isis3 target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @ingroup 
   *
   * @author 2010-07-30 Debbie A. Cook
   *
   * @internal
   *  @history
   */
  class DemShape : public ShapeModel {
    public:
      //! Constructor
      DemShape(Isis::Pvl &pvl);

      //! Destructor
      ~DemShape();

      //! Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection,
                            std::vector<double> targetPosition);

      Distance localRadius(const Latitude &lat, const Longitude &lon);

      //! Return dem scale in pixels/degree
      double demScale();
    protected:
      Cube *m_demCube;        //!< The cube containing the model  NOTE::  TODO maybe make this a method that returns ptr to cube
      std::string m_demCubeFile;   //!< The file specification of the dem file
      Pvl *m_demLabel;        //!< The label of the dem cube

    private:
      double m_pixPerDegree;  //!< Scale of DEM file in pixels per degree
      Projection *m_demProj;  //!< The projection of the model
      Portal *m_portal;       //!< Buffer used to read from the model
      Interpolator *m_interp; //!< Use bilinear interpolation from dem

      // Aren't these only needed for Isis3EquatorialCylindricalShape model? Are they needed for 
      //  photometric computations?
      std::vector<double> m_samples; //!< Shape model sample values of 4 closest
                                     //   points to intersection point
      std::vector<double> m_lines;   //!< Shape model line values of 4 closest 
                                     //   points to intersection point

      // From Sensor.h
      /* double EmissionAngle(const std::vector<double> & sB) const; */
      /* void CommonInitialize(const std::string &demCube); */
      /* bool p_newLookB;      //!< flag to indicate we need to recompute ra/dec */
      /* SpiceDouble p_ra;     //!< Right ascension (sky longitude) */
      /* SpiceDouble p_dec;    //!< Decliation (sky latitude) */
      /* void computeRaDec();  //!< Computes the ra/dec from the look direction */

      bool m_hasElevationModel;     //!< Does sensor use an elevation model

      /* bool SetGroundLocal(bool backCheck);   //!<Computes look vector */
  };
};

#endif

