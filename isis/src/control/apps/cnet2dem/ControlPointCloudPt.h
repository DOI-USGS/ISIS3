#ifndef ControlPointCloudPt_h
#define ControlPointCloudPt_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>

#include <QExplicitlySharedDataPointer>
#include <QSharedData>
#include <QtGlobal>
#include <QVector>

#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "SpecialPixel.h"


namespace Isis {

/**
 * @brief 3-D ControlPoint class for use in PointCloud datasets
 *
 *  The ControlPoint container is required to not change its content for the
 *  duration of use of the nanoflann kd-tree built from the points.
 *
 * @author 2015-10-11 Kris Becker
 *
 * @internal
 *   @history 2015-10-11 Kris Becker - Original Version
 */

class ControlPointCloudPt {
  public:
    enum CoordinateType { Image, Ground };
    enum Ownership { Shared,      // Indicates creator of ControlPoint retains
                                  // ownership. take() provides a unique clone
                                  // of ControlPoint.
                     Exclusive    // Creator gives ownership to ControlPointCloudPt.
                                  // Gives up pointer interally to first caller to
                                  // take() invalidating this object.
                   };

    ControlPointCloudPt() : m_xyz(), m_type(Image), m_serialno(),
                            m_data( new ControlPointData() ),
                            m_merged() {
      m_data->getImageCoordinates(m_xyz);
    }

    ControlPointCloudPt(ControlPoint *point, const CoordinateType &ptype,
                        const Ownership owner,
                        const QString &serialno = "",
                        const double &weight = 1.0) :
                        m_xyz( ), m_type(ptype), m_serialno(serialno),
                        m_data(new ControlPointData(point, owner, weight)),
                        m_merged() {
      if ( Image == ptype ) {
        m_data->setReference(serialno);
        if ( !selectImageCoordinates() ) {
          m_data->disable();
        }
      }
      else {  //  ( Ground == ptype )
        if ( !selectGroundCoordinates() ) {
          m_data->disable();
        }
      }
    }

    virtual ~ControlPointCloudPt() { }

    inline bool isValid() const {
      return ( !m_data->isDisabled() );
    }

    inline void disable() {
      return ( m_data->disable() );
    }

    inline int size() const {
      return ( m_data->size() );
    }

    Ownership owner() const {
      return ( m_data->m_owner );
    }

    ControlPoint *take() const {
       return ( m_data->take() );
    }

    inline bool selectGroundCoordinates() {
      m_type = Ground;
      return ( m_data->getGroundCoordinates(m_xyz) );
    }

    inline bool selectImageCoordinates() {
      m_type = Image;
      return ( m_data->getImageCoordinates(m_xyz) );
    }

    inline CoordinateType getCoordinateType() const {
      return ( m_type );
    }

    inline QString id() const {
      return ( m_data->m_point->GetId() );
    }


    inline bool getGroundCoordinates(double xyzw[4]) const {
      return ( m_data->getGroundCoordinates(xyzw) );
    }

    inline const ControlPoint *getPoint() const {
      return ( m_data->m_point );
    }

    inline ControlPoint &getPoint() {
      return ( *m_data->m_point );
    }

    inline QString getSerialNumber() const {
      return ( m_serialno );
    }

    inline ControlMeasure *getMeasure(const QString &serialno) const {
      if ( !isValid() ) { return (0); }
      if (m_data->m_point->HasSerialNumber(serialno) ) {
        return ( m_data->m_point->GetMeasure(serialno) );
      }

      return (0);
    }

    // Convenient retrieval coordinates
    inline double x() const { return ( m_xyz[0] ); }
    inline double y() const { return ( m_xyz[1] ); }
    inline double z() const { return ( m_xyz[2] ); }
    inline double w() const { return ( m_xyz[3] ); }
    inline const double *array() const { return ( m_xyz ); }

    /** Compute real vector length (radius) from ControlPoint
     *
     *  Radius is returned in meters
     */
    inline double radius() const {
      // compute magnitude
      double coords[4];
      if ( !getGroundCoordinates(coords) ) { return (Null); }
      return ( radius( coords[0], coords[1], coords[2]) );
    }


    /** Compute vector length (radius) - code based upon NAIF
     *
     *  Radius is returned in units of input parameters
     */
    inline double radius(const double x, const double y, const double z) const {
      double v1max = qMax(qAbs(x), qMax(qAbs(y), qAbs(z)));

      // We're done if its the zero vector
      if ( qFuzzyCompare(v1max+1.0, 1.0) ) { return ( 0.0 ); }

      // Compute magnitude of the vector
      double tmp0( x / v1max );
      double tmp1( y / v1max );
      double tmp2( z / v1max );
      double normsqr = tmp0*tmp0 + tmp1*tmp1 + tmp2*tmp2;
      return ( v1max * std::sqrt(normsqr) );
    }


    inline bool operator==(const ControlPointCloudPt &other) const {
      return ( m_data->m_point == other.m_data->m_point );
    }

    inline bool operator!=(const ControlPointCloudPt &other) const {
      return ( m_data->m_point != other.m_data->m_point );
    }

  private:
    /**
     * @brief Shared ControlPoint data pointer
     *
     * @author 2015-10-11 Kris Becker
     *
     * @internal
     *   @history 2015-10-11 Kris Becker - Original Version
     */
    class ControlPointData : public QSharedData {
      typedef ControlPointCloudPt::Ownership Ownership;
      public:
        ControlPointData() : QSharedData(), m_point(0), m_reference(0),
                             m_weight(1.0), m_initial(0),
                             m_owner(Exclusive){ }
        ControlPointData(ControlPoint *point, const Ownership owner,
                         const double weight = 1.0) :
                         QSharedData(), m_point(point),
                         m_reference(0),
                         m_weight(weight),
                         m_initial(point->GetNumValidMeasures()),
                         m_owner(owner) {
          if ( m_point->GetNumMeasures() > 0) {
            m_reference = m_point->GetRefMeasure();
          }
        }
        ControlPointData(const ControlPointData &other) : QSharedData(other),
                         m_point(other.m_point),
                         m_reference(other.m_reference),
                         m_weight(other.m_weight),
                         m_initial(other.m_initial),
                         m_owner(Shared) { }
        ~ControlPointData() {
          if ( Exclusive == m_owner ) {
             delete (m_point);
             m_point = 0;
          }
        }

        inline int size() const {
          if ( 0 == m_point ) { return (0); }
          return ( m_point->GetNumValidMeasures() );
        }

        inline bool setReference(const QString &serialno) {
          if ( 0 == m_point ) { return ( false); }
          m_reference = 0;

          if ( serialno.isEmpty() ) {
            m_reference = m_point->GetRefMeasure();
          }
          else {
            if ( m_point->HasSerialNumber(serialno) ) {
              m_reference = m_point->GetMeasure(serialno);
            }
          }

          return ( 0 != m_reference );
        }

        /** This approach allows use of 2 and 3 dimensional Euclidean
         *  distances */
        inline bool getImageCoordinates(double coords[4]) const {
          (void) getNoPointData(coords);
          if ( 0 == m_reference ) { return (false); }
          if ( 0 == m_point ) { return ( false ); }

          // ControlMeasure *refm = m_point->GetRefMeasure();
          coords[0] = m_reference->GetSample();
          coords[1] = m_reference->GetLine();
          coords[2] = 0.0;
          coords[3] = m_weight;
          return ( true );
        }


        /** This approach assumes 3 dimensional Euclidean distances */
        inline bool getGroundCoordinates(double coords[4]) const {
          (void) getNoPointData(coords);
          if ( 0 == m_point ) { return ( false ); }

          // Always get the best surface point (2015-10-27)
          SurfacePoint surfpt = m_point->GetBestSurfacePoint();
          if ( !surfpt.Valid() ) { return (false); }

          // Get the location and convert to meters!
          surfpt.ToNaifArray(&coords[0]);
          coords[0] *= 1000.0;
          coords[1] *= 1000.0;
          coords[2] *= 1000.0;
          coords[3] = m_weight;
          return (true);
        }

        inline bool isDisabled() const {
          if ( 0 == m_point ) return ( true );
          return (
                   m_point->IsInvalid()  ||
                   m_point->IsIgnored() ||
                   m_point->IsRejected() ||
                   m_point->IsEditLocked()  ||
                   ( 0 == m_reference )
                   );
        }

        inline void disable() {
          if ( !isDisabled() ) {
            m_point->SetIgnored( true );
          }
          return;
        }

        // The first to call this method will take ownership of the
        // ControlPoint if Exclusive. Cloned otherwise.
        ControlPoint *take() {
          ControlPoint *p(m_point);
          if ( 0 == m_point ) {  return (p); }

          // If someone else owns the point, clone it
          if ( Shared == m_owner ) {
            p = new ControlPoint(*m_point);
          }
          else {
            // Exclusive == Ownership
            // Relinquish ownership
            m_point = 0;
          }

          return ( p );
        }

        // Data....
        ControlPoint          *m_point;
        ControlMeasure        *m_reference;
        double                m_weight;
        int                   m_initial;
        Ownership             m_owner;


      private:
        inline bool getNoPointData(double coords[4]) const {
          coords[0] = coords[1] = coords[2] = 0.0;
          coords[3] = m_weight;
          return ( false );
        }
    };

    // Variables...
    double                                  m_xyz[4];
    CoordinateType                          m_type;
    QString                                 m_serialno;
    QExplicitlySharedDataPointer<ControlPointData> m_data;
    QList<ControlPointCloudPt>              m_merged;
};

};  // namespace Isis
#endif
