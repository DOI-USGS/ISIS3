#ifndef QnetPointRangeFilter_h
#define QnetPointRangeFilter_h

#include "QnetFilter.h"

class QLineEdit;

namespace Isis {
  /**
   * Defines the Range filter for the QnetNavTool's Points
   * section.  The user must enter values for Minimum Latitude,
   * Maximum Latitude, Minimum Longitude, and Maximum Longitude.
   * This class is designed to remove points from the current
   * filtered list that lie outside of the given range.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.  Removed
   *                          "std::" in .cpp file.
   *
   */
  class QnetPointRangeFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointRangeFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    private:
      QLineEdit *m_minlat;
      QLineEdit *m_maxlat;
      QLineEdit *m_minlon;
      QLineEdit *m_maxlon;
  };
};

#endif
