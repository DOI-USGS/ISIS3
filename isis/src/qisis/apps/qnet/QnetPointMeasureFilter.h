#ifndef QnetPointMeasureFilter_h
#define QnetPointMeasureFilter_h

#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QList>
#include <QRadioButton>
#include "QnetFilter.h"


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
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-02 Jeannie Walldren - Created MeasureTypeMatched()
   *                          method.  Modified filter() to add
   *                          the Ignore Status filter
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.
   *
   */
  class QnetPointMeasureFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointMeasureFilter(QWidget *parent = 0);
      virtual void filter();
      bool MeasureTypeMatched(int cmType);

    private slots:
      void enableIgnoreFilter();
      void enableTypeFilter();

    private:
      QCheckBox *p_measureType;
      QCheckBox *p_unmeasured;
      QCheckBox *p_manual;
      QCheckBox *p_estimated;
      QCheckBox *p_automatic;
      QCheckBox *p_validatedManual;
      QCheckBox *p_validatedAutomatic;
      QCheckBox *p_ignoreStatus;
      QRadioButton *p_ignored;
      QRadioButton *p_notIgnored;
  };
};

#endif
