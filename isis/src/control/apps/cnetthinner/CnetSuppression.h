#ifndef CnetSuppression_h
#define CnetSuppression_h
/**
 * @file
 * $Revision: 6565 $
 * $Date: 2016-02-10 17:15:35 -0700 (Wed, 10 Feb 2016) $
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
#include <ostream>
#include <cfloat>
#include <cmath>

#include <QMap>
#include <QRectF>
#include <QSharedPointer>
#include <QSizeF>
#include <QString>
#include <QStringList>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "CnetManager.h"
#include "Progress.h"

#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array2d_utils.h"

namespace Isis {

/**
 * Container class for CnetManager
 *  
 * @author 2016-09-30 Kris Becker 
 *  
 * @internal 
 *   @history 2016-09-30 Kris Becker - Original Version
 *   @history 2016-12-28 Kristin Berry - Added documentation and tests for checkin
 *   @history 2017-08-09 Summer Stapleton - Added a try-catch in constructor to throw proper
 *                         error for invalid control net. Fixes #5068.
 * 
 */
  class CnetSuppression : public CnetManager {
    public:
      typedef CnetManager::IndexPoint  IndexPoint;
      typedef CnetManager::PointSet    PointSet;

      typedef TNT::Array2D<bool>       GridMask;
      typedef TNT::Array1D<bool>       BitMask;

      /** 
       * Results of a particular suppression
       *  
       * @author 2016-09-30 Kris Becker 
       *  
       * @internal 
       *   @history 2016-09-30 Kris Becker - Original Version
       *  
       */
      class Results {
        public:
          Results() : m_candidates(), m_selected(), m_points(), 
                      m_domain(), m_radius(0) { } 
          Results(const int n, const QRectF domain, const double &radius) : 
                  m_candidates(n, false), m_selected(n, false),
                  m_points(), m_domain(domain),m_radius(radius) {
          }


        /**
         * True if the Results are valid.
         * 
         * @return @b bool True if results are valid.
         */
          inline bool isValid() const {
            return (m_candidates.dim() > 0);
          }


       /**
         * The number of points in the Result. 
         * 
         * @return @b int Number of points in the Result. 
         */
          inline int size() const {
            return ( m_points.size() );
          }


        /**
         * Add a PointSet to the Results. 
         * 
         * @param points The points to add to the Result set. 
         */
          inline void add(const PointSet &points) {
            BOOST_FOREACH (const IndexPoint &p, points) { add(p); }
            return;
          }


        /**
         * Add a single point to the Result. 
         * 
         * @param point The point to add to the Result. 
         */
          inline void add(const IndexPoint &point) {
            m_points.append(point);
            int index = point.first;
            BOOST_ASSERT (index < m_selected.dim1() );
            m_selected[index] = true;
            return;
          }

          BitMask    m_candidates; //! 
          BitMask    m_selected; //! 
          PointSet   m_points; //! 
          QRectF     m_domain; //!
          double     m_radius; //! 
      };

      CnetSuppression();
      CnetSuppression(const QString &cnetfile, const double &weight = 0.0);
      CnetSuppression(const CnetManager &cman);

      virtual ~CnetSuppression();

      void setEarlyTermination(const bool &state = true);

      Results suppress(const int &minpts, const int &maxpts,
                       const double &min_radius = 1.5, 
                       const double &tolerance = 0.1,
                       const BitMask &bm = BitMask());

      Results suppress(const QString &serialno, const int minpts, 
                       const int &maxpts, const double &min_radius,
                       const double &tolerance, 
                       const BitMask &bm = BitMask());

      Results suppress(const PointSet &points, const int &minpts, 
                       const int &maxpts, const double &min_radius,
                       const double &tolerance, 
                       const BitMask &bm = BitMask());

   
      void write(const QString &onetfile, const Results &result,
                 const bool saveall = false, const QString &netid = "");

    protected:
      const ControlNet *net() const;

    private:
      QScopedPointer<ControlNet> m_cnet; //!
      QList<ControlPoint *>      m_points; //!
      BitMask                    m_saved; //!
      QVector<Results>           m_results; //!
      bool                       m_early_term; //! Will terminate early if true
      mutable QSizeF             m_area; //!

      int index(const IndexPoint &p) const;
      ControlMeasure *measure(const IndexPoint &p) const;
      BitMask  maskPoints(int nbits, const PointSet &p) const;
      PointSet contains(const BitMask &bm, const PointSet &pset) const;


      void cellIndex(const IndexPoint &p, const double &cell_size,
                     int &x_center, int &y_center) const;
      int nCovered(const GridMask &grid) const;
      int cover(GridMask &grid, const PointSet &points, 
                const double &cell_size) const;
      int cover(GridMask &grid, const int &x_center, const int &y_center, 
                const double &cell_size) const;

      PointSet merge(const PointSet &s1, const PointSet &s2) const;
      Results  merge(const Results &r1, const Results &r2) const;

      QRectF domain(const PointSet &pts) const;
      double getScale(const QSizeF &d) const;
      BitMask orMasks(const BitMask &b1, const BitMask &b2) const;
      QVector<double> linspace(const double dmin, const double dmax, 
                               const int num, const double &scale = 1.0) const;


      /**
       * @brief Descending order sort functor
       * 
       * This is a comparison class used to sort lists of objects, 
       * where the QPair.second datum is a number in descending order.
       * 
       * @author 2016-09-30 Kris Becker 
       *  
       * @internal 
       *   @history 2016-09-30 Kris Becker - Original Version
       *  
       */
      class SortSerialsByPntSize {
        public:
          SortSerialsByPntSize() { }
          ~SortSerialsByPntSize() { }
          inline bool operator()(const QPair<QString, int> &a,
                                 const QPair<QString, int> &b) const {
            return  ( a.second >  b.second );
          }
      };
  };


} // namespace Isis
#endif
