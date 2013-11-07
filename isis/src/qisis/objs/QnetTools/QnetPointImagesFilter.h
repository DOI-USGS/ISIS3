#ifndef QnetPointImagesFilter_h
#define QnetPointImagesFilter_h

#include "QnetFilter.h"

class QLineEdit;
class QRadioButton;

namespace Isis {
  /**
   * Defines the Images filter for the QnetNavTool's Points
   * section, i.e. the number of measures in control point. The
   * user may enter values for Less Than and/or Greater Than. This
   * class is designed to remove points from the current filtered
   * list that lie outside of the given range.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor
   *
   */
  class QnetPointImagesFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointImagesFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    private:
      QRadioButton *m_lessThanRB;
      QRadioButton *m_greaterThanRB;
      QLineEdit *m_imageEdit;
  };
};

#endif
