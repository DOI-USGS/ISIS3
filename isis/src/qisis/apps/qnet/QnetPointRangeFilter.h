#ifndef QnetPointRangeFilter_h
#define QnetPointRangeFilter_h

#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include "QnetFilter.h"


namespace Qisis {
  /**
   * Defines the Range filter for the QnetNavTool's Points 
   * section.  The user must enter values for Minimum Latitude, 
   * Maximum Latitude, Minimum Longitude, and Maximum Longitude. 
   * This class is designed to remove points from the current 
   * filtered list that lie outside of the given range. 
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
      QnetPointRangeFilter (QWidget *parent=0);
      virtual void filter();

    private:
      QLineEdit *p_minlat;
      QLineEdit *p_maxlat;
      QLineEdit *p_minlon;
      QLineEdit *p_maxlon;
  };
};

#endif
