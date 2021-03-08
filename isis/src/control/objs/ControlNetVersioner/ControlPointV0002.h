#ifndef ControlPointV0002_h
#define ControlPointV0002_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

 #include <QSharedPointer>

 #include "ControlNetFileProtoV0001.pb.h"
 #include "ControlNetLogDataProtoV0001.pb.h"

namespace Isis {
  class ControlPointV0001;
  class PvlObject;
  class PvlContainer;

  /**
   * @brief A container for the information stored in a version 2 ControlPoint.
   *
   * A wrapper around the version 2 protobuf serialization of a ControlPoint. It allows for reading
   * ControlPoints serialized as both PvlObjects and protobuf messages.
   *
   * The version 1 and 2 control points use the same internal protobuf message format.
   * Thus the "upgrade" process simply copies the shared pointer to the protobuf message.
   *
   * Version 2 was the first version to have a standardized Pvl format. In the
   * Pvl format, control points are represented by objects contained in the
   * ControlNetwork object. Control measures are represented by groups
   * contained in the control point objects.
   *
   * <b>Valid Control Point Keywords</b>
   *
   * <ul>
   *   <li><em>PointId:</em> The point ID string</li>
   *   <li><em>ChooserName:</em> The name of the application or user that last
   *       modified the point</li>
   *   <li><em>DateTime:</em> The date and time of the last modification to the
   *       point</li>
   *   <li><em>AprioriXYZSource:</em> What type of source the apriori ground
   *       point was calculated from. Options:
   *       <ul>
   *           <li>None</li>
   *           <li>User</li>
   *           <li>AverageOfMeasures</li>
   *           <li>Reference</li>
   *           <li>Basemap</li>
   *           <li>BundleSolution</li>
   *       </ul></li>
   *   <li><em>AprioriXYZSourceFile:</em> The name of the file that the apriori
   *       ground point was calculated from</li>
   *   <li><em>AprioriRadiusSource:</em> What type of source the apriori point
   *       radius was calculated from. Options:
   *       <ul>
   *           <li>None</li>
   *           <li>User</li>
   *           <li>AverageOfMeasures</li>
   *           <li>Ellipsoid</li>
   *           <li>DEM</li>
   *           <li>BundleSolution</li>
   *       </ul></li>
   *   <li><em>AprioriRadiusSourceFile:</em> The name of the file that the
   *       apriori point radius was calculated from</li>
   *   <li><em>JigsawRejected:</em> If the point was rejected by a bundle
   *       adjustment</li>
   *   <li><em>EditLock:</em> If the point is locked out of editing</li>
   *   <li><em>Ignore:</em> If the point will be ignored</li>
   *   <li><em>AprioriX:</em> The body fixed X coordinate of the a priori
   *       ground point in meters</li>
   *   <li><em>AprioriY:</em> The body fixed Y coordinate of the a priori
   *       ground point in meters</li>
   *   <li><em>AprioriZ:</em> The body fixed Z coordinate of the a priori
   *       ground point in meters</li>
   *   <li><em>AdjustedX:</em> The body fixed X coordinate of the adjusted
   *       ground point in meters</li>
   *   <li><em>AdjustedY:</em> The body fixed Y coordinate of the adjusted
   *       ground point in meters</li>
   *   <li><em>AdjustedZ:</em> The body fixed Z coordinate of the adjusted
   *       ground point in meters</li>
   *   <li><em>LatitudeConstrained:</em> If the latitude of the ground point
   *       is constrained</li>
   *   <li><em>LongitudeConstrained:</em> If the longitude of the ground point
   *       is constrained</li>
   *   <li><em>RadiusConstrained:</em> If the radius of the ground point
   *       is constrained</li>
   *   <li><em>PointType:</em> What type of point it is. Options:
   *       <ul>
   *           <li>Ground</li>
   *           <li>Tie</li>
   *       </ul></li>
   *   <li><em>AprioriCovarianceMatrix:</em> A six element vector corresponding
   *       to the upper triangle; elements (0,0), (0,1), (0,2), (1,1), (1,2),
   *       and (2,2); of the 3x3, symmetric covariance matrix for
   *       the rectangular, a priori ground point.</li>
   *   <li><em>AdjustedCovarianceMatrix:</em> A six element vector corresponding
   *       to the upper triangle; elements (0,0), (0,1), (0,2), (1,1), (1,2),
   *       and (2,2); of the 3x3, symmetric covariance matrix for
   *       the rectangular, adjusted ground point.</li>
   * </ul>
   *
   * <b>Valid Control Measure Keywords</b>
   *
   * <ul>
   *   <li><em>SerialNumber:</em> The serial number of the cube the measure
   *       is from</li>
   *   <li><em>ChooserName:</em> The name of the application or user who last
   *       modified the measure</li>
   *   <li><em>DateTime:</em> The date and time of the last modification</li>
   *   <li><em>Diameter:</em> If the measure was selected from a crater, this
   *       is the diameter of the crater in meters</li>
   *   <li><em>EditLock:</em> If the measure is locked out of editing</li>
   *   <li><em>Ignore:</em> If the measure will be ignored</li>
   *   <li><em>JigsawRejected:</em> If the measure was rejected during a
   *       bundle adjustment</li>
   *   <li><em>AprioriSample:</em> The a priori sample</li>
   *   <li><em>AprioriLine:</em> The a priori line</li>
   *   <li><em>SampleSigma:</em> The standard deviation of the sample
   *       measurement</li>
   *   <li><em>LineSigma:</em> The standard deviation of the line
   *       measurement</li>
   *   <li><em>Sample:</em> The adjusted sample</li>
   *   <li><em>Line:</em> The adjusted line</li>
   *   <li><em>SampleResidual:</em> The difference between the a priori and
   *       adjusted sample</li>
   *   <li><em>LineResidual:</em> The difference between the a priori and
   *       adjusted line</li>
   *   <li><em>Reference:</em> If the measure is the reference measure for
   *       its point</li>
   *   <li><em>MeasureType:</em> What type of measure it is. Options:
   *       <ul>
   *           <li>candidate</li>
   *           <li>manual</li>
   *           <li>registeredpixel</li>
   *           <li>registeredsubpixel</li>
   *       </ul></li>
   * </ul>
   *
   *
   * @ingroup ControlNetwork
   *
   * @author 2017-12-14 Jesse Mapel
   *
   * @internal
   *   @history 2017-12-14 Jesse Mapel - Original version.
   *   @history 2017-12-21 Jesse Mapel - Added support for measure log data.
   *   @history 2017-12-21 Adam Goins - Changed Pvl constructor to take PvlObject.
   *   @history 2018-01-03 Jesse Mapel - Improved documentation.
   *   @history 2017-01-27 Jesse Mapel - More documentation improvements.
   */
  class ControlPointV0002 {
    public:
      ControlPointV0002(PvlObject &pointObject);
      ControlPointV0002(QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> pointData,
                        QSharedPointer<ControlNetLogDataProtoV0001_Point> logData);
      ControlPointV0002(ControlPointV0001 &oldPoint);

      QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> pointData();
      QSharedPointer<ControlNetLogDataProtoV0001_Point> logData();

    private:
      /**
       * Default constructor. Intentionally un-implemented.
       */
      ControlPointV0002();
      /**
       * Copy constructor. Intentionally un-implemented.
       *
       * @param other The other ControlPointV0002 to copy from.
       */
      ControlPointV0002(const ControlPointV0002 &other);
      /**
       * Assignment operator. Intentionally un-implemented.
       *
       * @param other The other ControlPointV0002 to assign from.
       *
       * @return @b ControlPointV0002& A reference to this after assignment.
       */
      ControlPointV0002 &operator=(const ControlPointV0002 &other);

      // methods for converting from Pvl to protobuf
      void copy(PvlContainer &container,
                QString keyName,
                QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> point,
                void (ControlNetFileProtoV0001_PBControlPoint::*setter)(bool));
      void copy(PvlContainer &container,
                QString keyName,
                QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> point,
                void (ControlNetFileProtoV0001_PBControlPoint::*setter)(double));
      void copy(PvlContainer &container,
                QString keyName,
                QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> point,
                void (ControlNetFileProtoV0001_PBControlPoint::*setter)(const std::string&));
      void copy(PvlContainer &container,
                QString keyName,
                ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure &measure,
                void (ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure::*setter)(bool));
      void copy(PvlContainer &container,
                QString keyName,
                ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure &measure,
                void (ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure::*setter)(double));
      void copy(PvlContainer &container,
                QString keyName,
                ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure &measure,
                void (ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure::*setter)(
                            const std::string &));

      QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> m_pointData;
      /**< protobuf container that holds information used to create a control point.*/
      QSharedPointer<ControlNetLogDataProtoV0001_Point> m_logData;
      /**< Protobuf container that holds log data for the control measures in the point.*/
  };
}

#endif
