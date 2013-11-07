#ifndef QnetPointGoodnessFilter_h
#define QnetPointGoodnessFilter_h

#include "QnetFilter.h"

class QCheckBox;
class QLineEdit;

namespace Isis {
  /**
   * Defines the Goodness of Fit filter for the QnetNavTool's 
   * Points section. The user may enter Less Than and Greater 
   * Than.  This class is designed to remove points from the 
   * current filtered list if all measures have goodness of fit 
   * values that lie outside the given range. 
   *  
   * @author 2008-11-26 Jeannie Walldren
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Original Version
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-02 Jeannie Walldren - Modified clearEdit()
   *                          method to disable the line edit if
   *                          the corresponding check box is not
   *                          checked.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.
   *   @history 2011-03-04 Tracie Sucharski - Updated for new version of
   *                          binary control networks.  The GoodnessOfFit
   *                          is now stored in the Log section of the control
   *                          net, not as a keyword within the ControlMeasure.
   */

  class QnetPointGoodnessFilter : public QnetFilter {
    Q_OBJECT

    public:
      QnetPointGoodnessFilter (QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    public slots:

    private:
      QCheckBox *m_lessThanCB; 
      QCheckBox *m_greaterThanCB;  
      QLineEdit *m_maxValueEdit;
      QLineEdit *m_minValueEdit;

    private slots:
      void clearEdit();
  };
};

#endif
