#ifndef QnetPointIdFilter_h
#define QnetPointIdFilter_h

#include "QnetFilter.h"

class QLineEdit;

namespace Isis {
  /**
   * Defines the Point ID filter for the QnetNavTool's Points
   * section.  The user must enter a string. This class is
   * designed to remove points from the current filtered list
   * whose PointId keyword values do not contain the string.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointer
   *                          to null in constructor
   *
   *
   */

  class QnetPointIdFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointIdFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    private:
      QLineEdit *m_pointIdEdit;
  };
};

#endif
