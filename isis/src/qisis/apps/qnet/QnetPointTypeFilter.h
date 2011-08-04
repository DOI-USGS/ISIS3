#ifndef QnetPointTypeFilter_h
#define QnetPointTypeFilter_h

#include "QnetFilter.h"

class QCheckBox;
class QGroupBox;
class QRadioButton;


namespace Isis {
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
   *                          "ControlPoint::Ground" in
   *                          filter().  Initialized pointers to
   *                          null in constructor
   *   @history 2011-06-08 Tracie Sucharski - Change for new point types and
   *                          held points no longer exist, but editLocked
   *                          points do.
   */
  class QnetPointTypeFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointTypeFilter(QWidget *parent = 0);
      virtual void filter();

    private:
      bool PointTypeMatched(int pointType);

      QGroupBox    *p_pointType;
      QCheckBox    *p_free;
      QCheckBox    *p_constrained;
      QCheckBox    *p_fixed;

      QGroupBox    *p_ignoreStatus;
      QRadioButton *p_ignored;
      QRadioButton *p_notIgnored;

      QGroupBox    *p_editLockStatus;
      QRadioButton *p_editLocked;
      QRadioButton *p_notEditLocked;
  };
};

#endif
