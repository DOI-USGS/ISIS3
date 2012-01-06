#ifndef QnetPointDistanceFilter_h
#define QnetPointDistanceFilter_h

#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include "QnetFilter.h"


namespace Isis {
  /**
   * Defines the Distance filter for the QnetNavTool's Points
   * section.  The user must enter a Minimum Distance value. This
   * class is designed to remove points from the current filtered
   * list whose distance from all other points in the control net
   * is greater than or equal to the given value.
   *
   * @author ????-??-?? Unknown
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Changed variable
   *                          names and labels for clarity.
   *                          Modified filter() to handle case in
   *                          which the lat/lon of the point is
   *                          Null. And to increase efficiency.
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to create new filtered list
   *                          from points in the existing filtered
   *                          list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.
   *
   */
  class QnetPointDistanceFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointDistanceFilter(QWidget *parent = 0);
      virtual void filter();

    private:
      QLineEdit *p_lineEdit;
  };
};

#endif
