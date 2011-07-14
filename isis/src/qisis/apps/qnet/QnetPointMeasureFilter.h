#ifndef QnetPointMeasureFilter_h
#define QnetPointMeasureFilter_h

#include "QnetFilter.h"

class QCheckBox;
class QGroupBox;
class QRadioButton;


namespace Qisis {
  /**
   * Defines the Measure Properties filter for the QnetNavTool's
   * Points section.  This class filters points whose
   * measures have at least one measure with the selected
   * properties. If the user chooses more than one measure type,
   * the list will contain points with at least one measure with
   * either of these types. If the user chooses to filter by
   * measure type and ignore status, the list will contain points
   * with at least one measure that contains both of these
   * properties. This class is designed to remove points from the
   * current filtered list that do not contain any measures of the
   * selected type.
   *  
   * @todo 
   *   Add ability to filter points without a Reference.
   *  
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-02 Jeannie Walldren - Created MeasureTypeMatched()
   *                          method.  Modified filter() to add
   *                          the Ignore Status filter
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.
   *   @history 2010-07-16 Tracie Sucharski - Implemented binary
   *                          control networks.
   *   @history 2011-05-20 Tracie Sucharski - Remove reference as a measure
   *                          type.  Need to add ability to search for
   *                          points without a reference?
   *   @history 2011-07-12 Tracie Sucharski - Use Group boxes instead of
   *                          check boxes and reimplement filter to add
   *                          the edit Lock status.
   *
   */
  class QnetPointMeasureFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointMeasureFilter(QWidget *parent = 0);
      virtual void filter();

    private:
      bool MeasureTypeMatched(int cmType);

      QGroupBox *p_measureType;
      QCheckBox *p_candidate;
      QCheckBox *p_manual;
      QCheckBox *p_registeredPixel;
      QCheckBox *p_registeredSubPixel;

      QGroupBox *p_ignoreStatus;
      QRadioButton *p_ignored;
      QRadioButton *p_notIgnored;

      QGroupBox *p_editLockStatus;
      QRadioButton *p_editLocked;
      QRadioButton *p_notEditLocked;
  };
};

#endif
