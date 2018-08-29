#ifndef ControlPointV0001_h
#define ControlPointV0001_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
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

 #include <QSharedPointer>

 #include "ControlNetFileProtoV0001.pb.h"
 #include "ControlNetLogDataProtoV0001.pb.h"

namespace Isis {
  class PvlObject;
  class PvlContainer;

  /**
   * @brief A container for the information stored in a version 1 ControlPoint.
   *
   * A wrapper around the version 1 protobuf serialization of a ControlPoint. It allows for reading
   * ControlPoints serialized as both PvlObjects and protobuf messages.
   *
   * This class is designed to be compatible with all Pvl formats created prior
   * to the use of versioning. So, the PvlObject constructor has to work with
   * several different formats. Hence, several different keywords are checked
   * for the same value.
   *
   * Because this version supports several different formats, there is no
   * standardized set of keywords, but all version 1 Pvl control networks
   * have the same high level structure. Control points are represented by
   * objects contained in the ControlNetwork object. Control measures are
   * represented by groups contained in the control point objects.
   *
   * Once read in, the data is always stored in a protobuf message regardless
   * of the source. This is done to optimize reading binary control network
   * files. Because the protobuf format for version 1 control points is
   * identical to the version 2 format, control points read from a version 1
   * file are automatically "converted" to version 2. This also makes the
   * version 1 to 2 upgrade process as simple as passing a pointer.
   *
   * @ingroup ControlNetwork
   *
   * @author 2017-12-18 Jesse Mapel
   *
   * @internal
   *   @history 2017-12-18 Jesse Mapel - Original version.
   *   @history 2017-12-21 Adam Goins - Changed Pvl constructor to take PvlObject.
   *   @history 2017-12-21 Jesse Mapel - Improved documentation.
   *   @history 2017-01-27 Jesse Mapel - More documentation improvements.
   *   @history 2018-06-28 Debbie A Cook - Removed all calls to obsolete method
   *                                                   SurfacePoint::SetRadii.  References #5457.
   *   @history 2018-07-11 Debbie A Cook - Removed obsolete tests for failure
   *                                                   due to missing target radii.  SurfacePoint
   *                                                   now uses the local radius of the point to 
   *                                                   convert sigmas to target radii are no longer
   *                                                   used.  References #5457
   */
  class ControlPointV0001 {
    public:
      ControlPointV0001(PvlObject &pointObject, const QString targetName);
      ControlPointV0001(QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> pointData,
                        QSharedPointer<ControlNetLogDataProtoV0001_Point> logData);

      QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> pointData();
      QSharedPointer<ControlNetLogDataProtoV0001_Point> logData();

    private:
      /**
       * Default constructor. Intentionally un-implemented.
       */
      ControlPointV0001();
      /**
       * Copy constructor. Intentionally un-implemented.
       *
       * @param other The other ControlPointV0001 to copy from.
       */
      ControlPointV0001(const ControlPointV0001 &other);
      /**
       * Assignment operator. Intentionally un-implemented.
       *
       * @param other The other ControlPointV0001 to assign from.
       *
       * @return @b ControlPointV0001& A reference to this after assignment.
       */
      ControlPointV0001 &operator=(const ControlPointV0001 &other);

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
      /**< Protobuf container that holds information used to create a control point.*/
      QSharedPointer<ControlNetLogDataProtoV0001_Point> m_logData;
      /**< Protobuf container that holds log data for the control measures in the point.*/
  };
}

#endif
