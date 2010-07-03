/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/06/04 23:51:27 $
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

#ifndef ControlPoint_h
#define ControlPoint_h

#include <vector>
#include <string>
#include "ControlMeasure.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief A single control point
   *
   * A control point is one or more measurements that identify the same feature
   * or location in different images.
   *
   * @ingroup ControlNetwork
   *
   * @author 2005-07-29 Jeff Anderson
   *
   * @see ControlMeasure ControlNet
   *
   * @internal
   *   @history 2005-07-29 Jeff Anderson Original version
   *   @history 2006-01-11 Jacob Danton Added ReferenceIndex method and updated unitTest
   *   @history 2006-06-28 Tracie Sucharski, Added method to return measure
   *            for given serial number.
   *   @history 2006-10-31 Tracie Sucharski, Added HasReference method,
   *            changed ReferenceIndex method to throw error if there is no
   *            Reference ControlMeasure.
   *   @history 2007-01-25 Debbie A. Cook, Removed return statement in SetApriori method
   *            for GroundPoint case so that FocalPlaneMeasures will get set.  The method
   *            already has a later return statement to avoid changing the lat/lon values.
   *   @history 2007-10-19 Debbie A. Cook, Wrapped longitudes when calculating apriori longitude
   *            for points with a difference of more than 180 degrees of longitude between measures.
   *   @history 2008-01-14 Debbie A. Cook, Changed call to Camera->SetUniversalGround in ComputeErrors
   *            to include the radius as an argument since the method has been overloaded to include
   *            radius.
   *   @history 2008-09-12 Tracie Sucharski, Add method to return true/false
   *                   for existence of Serial Number.
   *   @history 2009-03-07 Debbie A. Cook Fixed ComputeErrors method to set focal plane coordinates
   *                   without changing time and improved error messages.
   *   @history 2009-06-03 Christopher Austin, Added the p_invalid functionality
   *            along with forceBuild, fixed documentation errors.
   *   @history 2009-06-22 Jeff Anderson, Modified ComputeAprior
   *            method to correctly handle ground and held points.
   *            Previosuly it would throw an error if the lat/lon
   *            of a measure could not be computed.  Also, modify
   *            the ComputeErrors method to not abort any longer
   *            if a control point lat/lon could not be projected
   *            back to a image line/sample.
   *   @history 2009-08-13 Debbie A. Cook Corrected calculation of
   *            scale used to get the undistorted focal plane
   *            coordinates to use the signed focal length (Z) from
   *            the CameraDistortionMap,
   *   @history 2009-08-21 Christopher Austin, Put the default return of
   *            ReferenceIndex() back as the first Measured measure.
   *   @history 2009-09-01 Eric Hyer, fixed some include issues.
   *   @history 2009-09-08 Eric Hyer, Added PointTypeToString method.
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *            error messages.
   *   @history 2010-03-19 Debbie A. Cook Replaced code in method ComputeErrors with call to
   *  					 CameraGroundMap->GetXY
   *   @history 2010-05-11 Sharmila Prasad Added API's Copy Constructor to copy one point to another and
   *  					ReferenceIndexNoException not to throw Exception if there are no reference point or
   *   			    no measures in a Control Point.
   *   @history 2010-06-04 Eric Hyer - removed parametor for PointTypeToString()
   */
  class ControlPoint {
    public:
      ControlPoint();
      ControlPoint(const std::string &id);

      //! Destroy a control point
      ~ControlPoint() {};

      void Load(PvlObject &p, bool forceBuild = false);

      PvlObject CreatePvlObject();

      /**
       * Sets the Id of the control point
       *
       * @param id Control Point Id
       */
      void SetId(const std::string &id) {
        p_id = id;
      };

      /**
       * Return the Id of the control point
       *
       * @return Control Point Id
       */
      std::string Id() const {
        return p_id;
      };

      void Add(const ControlMeasure &measure, bool forceBuild = false);
      void Delete(int index);

      /**
       * Return the ith measurement of the control point
       *
       * @param index Control Measure index
       *
       * @return The Control Measure at the provided index
       */
      ControlMeasure &operator[](int index) {
        return p_measures[index];
      };

      /**
       * Return the ith measurement of the control point
       *
       * @param index Control Measure index
       *
       * @return The Control Measure at the provided index
       */
      const ControlMeasure &operator[](int index) const {
        return p_measures[index];
      };

      //! Return the measurement for the given serial number
      ControlMeasure &operator[](const std::string &serialNumber);

      //! Return the measurement for the given serial number
      const ControlMeasure &operator[](const std::string &serialNumber) const;

      //! Does Serial Number exist in point
      bool HasSerialNumber(std::string &serialNumber);

      //! Return the number of measurements in the control point
      int Size() const {
        return p_measures.size();
      };
      int NumValidMeasures();

      /**
       * Set whether to ignore or use control point
       *
       * @param ignore True to ignore this Control Point, False to un-ignore
       */
      void SetIgnore(bool ignore) {
        p_ignore = ignore;
      };

      //! Return if the control point should be ignored
      bool Ignore() const {
        return p_ignore;
      };

      //! Return if the control point is invalid
      bool Invalid() const {
        return p_invalid;
      }

      /**
       * Set the control point as held to its lat/lon
       *
       * @param held True to hold this Control Point, False to release
       */
      void SetHeld(bool held) {
        p_held = held;
      };

      //! Is the control point lat/lon held?
      bool Held() const {
        return p_held;
      };

      /**
       * A control point can have one of two types, either Ground or Tie.
       */
      enum PointType {
        /**
         * A Ground point is a Control Point whose lat/lon is well established
         * and should not be changed. Some people will refer to this as a
         * truth (i.e., ground truth).  Holding a point is equivalent to making
         * it a ground point.  A ground point can be identifed in one or more
         * cubes.
         */
        Ground,
        /**
         * A Tie point is a Control Point that identifies common measurements
         * between two or more cubes. While it could have a lat/lon, it is not
         * necessarily correct and is subject to change.  This is the most
         * common type of control point.
         */
        Tie
      };

      /**
       * Change the type of the control point
       *
       * @param type The type for this Control Point
       */
      void SetType(PointType type) {
        p_type = type;
      };

      //! Return the type of the point
      PointType Type() const {
        return p_type;
      };

      const std::string PointTypeToString() const;

      void SetUniversalGround(double lat, double lon, double radius);

      //! Return the planetocentric latitude of the point
      double UniversalLatitude() const {
        return p_latitude;
      };

      //! Return the planetocentric longitude of the point
      double UniversalLongitude() const {
        return p_longitude;
      };

      //! Return the radius of the point in meters
      double Radius() const {
        return p_radius;
      };

      double AverageError() const;

      // std::string Thumbnail() const;
      // std::string FeatureName() const;

      bool HasReference();

      int ReferenceIndex();

      int ReferenceIndexNoException();

      void ComputeApriori();

      void ComputeErrors();

      double MaximumError() const;

      double WrapLongitude(double lon, double baselon);

      bool operator == (const Isis::ControlPoint &pPoint) const;
      bool operator != (const Isis::ControlPoint &pPoint) const;
      ControlPoint &operator = (const Isis::ControlPoint &pPoint);


    private:
      std::string p_id; //!< Point Id
      std::vector<Isis::ControlMeasure> p_measures; //!< List of Control Measures
      PointType p_type; //!< This Control Point's Type
      bool p_ignore;    //!< If this Control Point is ignored
      bool p_held;      //!< If this Control Point is held
      double p_latitude;  //!< The Latitude of this Control Point
      double p_longitude; //!< The Longtude of this Control Point
      double p_radius;    //!< The raduis of this Control Point

      bool p_invalid;  //!< If this Control Point is invalid
  };
};

#endif

