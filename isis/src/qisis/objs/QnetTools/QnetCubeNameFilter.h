#ifndef QnetCubeNameFilter_h
#define QnetCubeNameFilter_h

#include "QnetFilter.h"

class QLineEdit;

namespace Isis {
  /**
   * Defines the Name filter for the QnetNavTool's Cubes
   * section.  The user must enter a string. This class is
   * designed to remove cubes from the current filtered list
   * whose filename does not contain the string.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to create new filtered list
   *                          from images in the existing filtered
   *                          list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor
   *
   */
  class QnetCubeNameFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetCubeNameFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    private:
      QLineEdit *p_cubeNameEdit;
  };
};

#endif
