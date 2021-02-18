#ifndef MatchMaker_h
#define MatchMaker_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>
#include <QScopedPointer>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

#include "ControlNet.h"
#include "FeatureMatcherTypes.h"
#include "ID.h"
#include "MatchImage.h"
#include "PvlFlatMap.h"
#include "QDebugLogger.h"
#include "MatcherSolution.h"

namespace Isis {


class ControlNet;
class ControlPoint;
class ControlMeasure;

/**
 * @brief Container for a feature match pair/set of data sources
 *
 *
 * @author 2015-08-18 Kris Becker
 * @internal
 *   @history 2015-08-18 Kris Becker - Original Version
 *   @history 2015-09-29 Kris Becker - Had line/sample transposed when computing
 *                           apriori lat/lon
 */

class MatchMaker : public QLogger {
  public:
    enum GeometrySourceFlag { None, Query, Train };
    MatchMaker();
    MatchMaker(const QString &name,
               const PvlFlatMap &parameters = PvlFlatMap(),
               const QLogger &logger = QLogger());

    virtual ~MatchMaker() { }

    QString name() const;
    int size() const;

    void setParameters(const PvlFlatMap &parameters);

    void setQueryImage(const MatchImage &query);
    void addTrainImage(const MatchImage &train);

    const MatchImage &query() const;
    MatchImage &query();

    const MatchImage &train(const int &index = 0) const;
    MatchImage &train(const int &index = 0);

    template <class T> int foreachPair( T &process ) {
        int npairs( 0 );
        BOOST_FOREACH ( MatchImage &train, m_trainers ) {
           process.apply(m_query, train);
           npairs++;
        }
        return ( npairs );
      }

    template <class T> int foreachPair( const T &process ) {
        int npairs( 0 );
        BOOST_FOREACH ( MatchImage &train, m_trainers ) {
           process.apply(m_query, train);
           npairs++;
        }
        return ( npairs );
      }

    void setGeometrySourceFlag(const GeometrySourceFlag &source);
    GeometrySourceFlag getGeometrySourceFlag() const;
    MatchImage getGeometrySource() const;

    MatcherSolution *match(const SharedRobustMatcher &algorithms);
    MatcherSolutionList match(const RobustMatcherList &algorithms);

    PvlGroup network(ControlNet &cnet, const MatcherSolution &solution,
                     ID &pointMaker) const;

  private:
    typedef  QScopedPointer<ControlPoint> ScopedControlPoint;

    QString             m_name;
    PvlFlatMap          m_parameters;
    MatchImage          m_query;
    MatchImageQList     m_trainers;
    GeometrySourceFlag  m_geomFlag;

    double getParameter(const QString &name, const PvlFlatMap &parameters,
                        const double &defaultParm) const;

    int addMeasure(ControlPoint **cpt, const MatchPair &mpair,
                   const cv::DMatch &point, const MatcherSolution &solution)
                   const;

    ControlMeasure *makeMeasure(const MatchImage &image,
                                const int &keyindex,
                                const QString &name = "ControlMeasure") const;
    bool setAprioriLatLon(ControlPoint &point, const ControlMeasure &measure,
                          const MatchImage &image) const;

    double goodnessOfFit(const cv::KeyPoint &query, const cv::KeyPoint &train)
                         const;
};

///!<   Shared FeatureAlgorithm pointer that everyone can use
typedef QSharedPointer<MatchMaker> SharedMatchMaker;
typedef QList<SharedMatchMaker>    MatchMakerQList;

}  // namespace Isis
#endif
