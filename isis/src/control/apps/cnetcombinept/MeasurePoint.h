#ifndef MeasurePoint_h
#define MeasurePoint_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>

#include <QExplicitlySharedDataPointer>
#include <QSharedData>
#include <QVector>
#include <QtGlobal>

#include "ControlPoint.h"
#include "ControlMeasure.h"


namespace Isis {

/**
 * @brief ControlPoint class for use in PointCloud datasets
 *
 *  The ControlPoint container is required to not change its content for the
 *  duration of use of the nanoflann kd-tree built from the points.
 *
 * @author  2015-10-11 Kris Becker
 *
 * @internal
 *   @history 2015-10-11 Kris Becker - Original Version
 *   @history 2016-12-06 Jesse Mapel - Updated documentation.  References #4558.
 */
class MeasurePoint {
  public:

    /**
     * Constructs an empty MeasurePoint.
     */
    MeasurePoint() : m_data( new ControlMeasureData() ) { }


    /**
     * Constructs a MeasurePoint from a measure.
     *
     * @param measure  The measure that the MeasurePoint represents.
     * @param weight   The weight of the MeasurePoint in the
     */
    MeasurePoint(ControlMeasure *measure, const double &weight = 1.0) :
                 m_data(new ControlMeasureData(measure, weight) ) {
      m_data->m_isvalid = validMeasure();
    }


    /**
     * Destroys a MeasurePoint.
     */
    virtual ~MeasurePoint() { }


    /**
     * Determines if the MeasurePoint's parent control point is valid.
     *
     * @return @b bool If the measure's parent control point is valid.
     */
    inline bool validPoint() const {
      ControlMeasure *m = m_data->m_measure;
      BOOST_ASSERT ( m != 0 );
      ControlPoint *p = m->Parent();
      BOOST_ASSERT ( p != 0 );
      return (!(p->IsIgnored() || p->IsRejected() || p->IsEditLocked() || p->IsInvalid()) );
    }


    /**
     * If the MeasurePoint and parent control point are valid.
     *
     * @return @b bool If the MeasurePoint and parent control point are both valid.
     */
    inline bool isValid() const {
      return ( m_data->m_isvalid && validPoint() );
    }


    /**
     * Flags the MeasurePoint as invalid.
     */
    inline void disable() {
      m_data->m_isvalid = false;
      return;
    }


    /**
     * Returns the size of the MeasurePoint.
     *
     * @return @b int Always one.
     */
    inline int size() const {
      return ( 1 );
    }


    /**
     * Returns the measure's point ID
     *
     * @return @b QString The point ID of the measure's parent control point.
     */
    inline QString id() const {
      return ( m_data->m_measure->GetPointId() );
    }


    /**
     * Returns the MeasurePoint's measure.
     *
     * @return @b ControlMeasure* A pointer to the contained measure.
     */
    inline const ControlMeasure *data() const {
      return ( m_data->m_measure );
    }


    /**
     * Returns the parent control point.
     *
     * @return @b ControlPoint* A pointer to the parent control point.
     */
    inline ControlPoint *getPoint() const {
      return ( m_data->m_measure->Parent() );
    }


    /**
     * Returns the serial number of the cube the measure is on.
     *
     * @return @b QString The serial number of the cube the measure is on.
     */
    inline QString getSerialNumber() const {
      return ( m_data->m_measure->GetCubeSerialNumber() );
    }


    /**
     * Returns the MeasurePoint's x coordinate.
     *
     * @return @b double the MeasurePoint's x coordinate.
     */
    inline double x() const {
      return ( m_data->m_xyz[0] );
    }


    /**
     * Returns the MeasurePoint's y coordinate.
     *
     * @return @b double the MeasurePoint's y coordinate.
     */
    inline double y() const {
      return ( m_data->m_xyz[1] );
    }


    /**
     * Returns the MeasurePoint's z coordinate.
     *
     * @return @b double the MeasurePoint's z coordinate.
     */
    inline double z() const {
      return ( m_data->m_xyz[2] );
    }


    /**
     * Returns the MeasurePoint's weight.
     *
     * @return @b double the MeasurePoint's weight.
     */
    inline double w() const {
      return ( m_data->m_xyz[3] );
    }


    /**
     * Returns the coordinates and weight of the MeasurePoint.
     *
     * @return @b double* A pointer to an array containing (in order)
     *                    the x, y, z, and weight of the MeasurePoint.
     */
    inline const double *array() const {
      return ( m_data->m_xyz );
    }


    /**
     * Determines if the measures contained by this MeasurePoint and
     * another MeasurePoint are the same.
     *
     * @param other The MeasurePoint to compare against.
     *
     * @return @b bool If the measures are the same.
     */
    inline bool operator==(const MeasurePoint &other) const {
      return ( m_data->m_measure == other.m_data->m_measure);
    }


    /**
     * Determines if the measures contained by this MeasurePoint and
     * another MeasurePoint are not the same.
     *
     * @param other The MeasurePoint to compare against.
     *
     * @return @b bool If the measures are not the same.
     */
    inline bool operator!=(const MeasurePoint &other) const {
      return ( m_data->m_measure != other.m_data->m_measure );
    }

  private:

    /**
     * Data wrapper class for ControlMeasures.
     *
     * @see QSharedData
     *
     * @author  2015-10-11 Kris Becker
     *
     * @internal
     *   @history 2015-10-11 Kris Becker - Original Version
     *   @history 2016-12-06 Jesse Mapel - Updated documentation.  References #4558.
     */
    class ControlMeasureData : public QSharedData {
      public:

        /**
         * Constructs and empty ControlMeasureData.
         */
        ControlMeasureData() : QSharedData(), m_measure(0),
                            m_weight(1.0), m_xyz(), m_isvalid(false)  {
          getImageCoordinates();
        }


        /**
         * Constructs a ControlMeasureData from a measure.
         *
         * @param measure The ControlMeasure to wrap.
         * @param weight The weight for the measure. Defaults to 1.0.
         */
        ControlMeasureData(ControlMeasure *measure, const double &weight = 1.0) :
                         QSharedData(), m_measure(measure), m_weight(weight),
                         m_xyz(), m_isvalid(false) {
          getImageCoordinates();
        }


        /**
         * Destroys a ControlMeasureData.
         */
        ~ControlMeasureData() { }


        /**
         * Retrives and stores the coordinates and weights from the wrapped measure.
         * If the measure has not been set, then all values are set to 0.0.
         */
        inline void getImageCoordinates()  {

          if ( !m_measure ) {
            m_xyz[0] = 0.0;
            m_xyz[1] = 0.0;
            m_xyz[2] = 0.0;
            m_xyz[3] = 0.0;
          }
          else {
            // ControlMeasure *refm = m_point->GetRefMeasure();
            m_xyz[0] = m_measure->GetSample();
            m_xyz[1] = m_measure->GetLine();
            m_xyz[2] = 0.0;
            m_xyz[3] = m_weight;
          }
          return;
        }

        ControlMeasure *m_measure;  //!< Wrapped ControlMeasure
        double          m_weight;   //!< The measure's weight.
        double          m_xyz[4];  /**!< The x, y, z, and weight for the measure.
                                         The x and y coordinates are the sample and
                                         line coordinates of the wrapped measure.*/
        bool            m_isvalid;  //!< If the data is valid.
    };

    QExplicitlySharedDataPointer<ControlMeasureData> m_data; /**!< The internal data containing
                                                                   a pointer to the measure along
                                                                   with its coordinates and
                                                                   weight.*/


    /**
     * Returns if the internal measure is valid.
     *
     * @return @b bool If the internal measure is valid.
     */
    inline bool validMeasure() const {
      ControlMeasure *m = m_data->m_measure;
      BOOST_ASSERT ( m != 0 );
      return (!(m->IsIgnored() || m->IsRejected()) );
    }

};

};  // namespace Isis
#endif
