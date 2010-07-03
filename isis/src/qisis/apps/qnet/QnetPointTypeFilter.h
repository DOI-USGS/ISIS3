#ifndef QnetPointTypeFilter_h
#define QnetPointTypeFilter_h

#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>
#include <QList>
#include "QnetFilter.h"


namespace Qisis {
  /**
   * Filter for control point type. The user must choose Ground,
   * Ignore, or Hold.  This class is designed to remove points
   * from the current filtered list that are not of the selected
   * type.
   *
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Fixed comment in
   *                          filter() method.
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-02 Jeannie Walldren - Modified filter()
   *                          method to treat a point as ignored
   *                          if all of its measures have
   *                          Ignore=True.
   *   @history 2010-06-03 Jeannie Walldren - Replaced "0" with
   *                          "Isis::ControlPoint::Ground" in
   *                          filter().  Initialized pointers to
   *                          null in constructor
   */
  class QnetPointTypeFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointTypeFilter(QWidget *parent = 0);
      virtual void filter();

    private:
      QRadioButton *p_ground;
      QRadioButton *p_ignore;
      QRadioButton *p_held;
  };
};

#endif
