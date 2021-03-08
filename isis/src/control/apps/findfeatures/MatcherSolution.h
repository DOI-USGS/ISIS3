#ifndef MatcherSolution_h
#define MatcherSolution_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

// boost library
#include <boost/foreach.hpp>

#include "FeatureMatcherTypes.h"
#include "MatchPair.h"
#include "PvlFlatMap.h"
#include "QDebugLogger.h"
#include "RobustMatcher.h"
#include "Statistics.h"

namespace Isis {

// Forward define stategic pointers
class MatcherSolution;
typedef QSharedPointer<MatcherSolution> SharedMatcherSolution;
typedef QList<SharedMatcherSolution>    MatcherSolutionList;

/**
 * @brief Contains a feature-based match solution
 *
 *
 * @author 2015-08-10 Kris Becker
 * @internal
 *   @history 2015-08-10 Kris Becker - Original Version
 */

class MatcherSolution : public QLogger {
  public:
    typedef MatchPairQList::iterator       MatchPairIterator;
    typedef MatchPairQList::const_iterator MatchPairConstIterator;

    MatcherSolution() { }
    MatcherSolution(const SharedRobustMatcher &matcher,
                    const MatchPair &pair,
                    const QLogger &logger = QLogger()) : QLogger(logger),
                    m_matcher(matcher), m_pairs() {
      m_pairs.push_back(pair);
    }

    MatcherSolution(const SharedRobustMatcher &matcher,
                    const MatchPairQList &pairs,
                    const QLogger &logger = QLogger()) : QLogger(logger),
                    m_matcher(matcher), m_pairs(pairs) { }

    virtual ~MatcherSolution() { }

    /** Return number of image match pairs contained in the solution */
    inline int size() const {
      return ( m_pairs.size() );
    }

    /** Overly ridiculous scaveger hunt for the target. Needed to write control
     *  network */
    QString target(const QString &targdef = "") const {
      QString v_target;
      BOOST_FOREACH ( const MatchPair &mpair, m_pairs ) {
        v_target = mpair.target();
        if ( !v_target.isEmpty() ) { break; }
      }
      if ( v_target.isEmpty() ){  v_target = targdef;  }
      return ( v_target );
    }

    inline const SharedRobustMatcher &matcher() const {
      return ( m_matcher );
    }

    Statistics qualityStatistics() const {
      Statistics stats;
      for (int i = 0 ; i < size() ; i++) {
        stats.AddData( m_pairs[i].efficiency() );
      }
      return ( stats );
    }

    double quality() const {
      return ( qualityStatistics().Average() );
    }

    MatchPairIterator begin() {
      return ( m_pairs.begin() );
    }

    MatchPairIterator end() {
      return ( m_pairs.end() );
    }


    MatchPairConstIterator begin() const {
      return ( m_pairs.begin() );
    }

    MatchPairConstIterator end() const {
      return ( m_pairs.end() );
    }

    template <class T> int forEachPair( T &process ) {
        int npairs( 0 );
        BOOST_FOREACH ( MatchPair &mpair, m_pairs ) {
           process.apply(mpair, *m_matcher, *this);
           npairs++;
        }
        return ( npairs );
      }

    template <class T> int forEachPair( const T &process ) const {
        int npairs( 0 );
        BOOST_FOREACH ( const MatchPair &mpair, m_pairs ) {
           process.apply(mpair, *m_matcher, *this);
           npairs++;
        }
        return ( npairs );
      }


  /**
   * @brief Determine match with best solution
   *
   * @author Kris Becker 2015-08-29
   *
   * @param matches            List of match solutions with some unique elements
   *
   * @return MatcherSolution*  Pointer to best match. This SHOULD NOT be freed
   *                           as ownership is retained in this object
   */
  static const MatcherSolution *best( const MatcherSolutionList &matches ) {
    if ( matches.size()  <= 0 ) { return ( 0 );  }

    SharedMatcherSolution candidate(matches[0]);
    for (int i = 1 ; i < matches.size() ; i++) {
      if ( matches[i]->quality() < candidate->quality() ) {
        candidate = matches[i];
      }
    }
    return ( candidate.data() );
  }

  private:
    SharedRobustMatcher m_matcher;
    MatchPairQList      m_pairs;
    PvlFlatMap          m_parameters;
};


}  // namespace Isis
#endif
