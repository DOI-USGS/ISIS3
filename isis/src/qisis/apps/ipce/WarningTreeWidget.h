#ifndef WarningTreeWidget_H
#define WarningTreeWidget_H

#include <QTreeWidget>

namespace Isis {

  /**
   * @brief Warning Widget for ipce
   *
   * @author 2012-05-29 Steven Lambright and Tracie Sucharski
   *
   * @internal
   *   @history 2012-07-31 Kimberly Oyama - Added comments to some of the methods.
   *   @history 2017-10-11 Ian Humphrey - Added a resize column call to showWarning() to ensure
   *                           the column size is fit to the width of the warning text. This
   *                           enables horizontal scrolling for the warning widget. Fixes #5164.
   */
  class WarningTreeWidget : public QTreeWidget {
      Q_OBJECT
    public:
      WarningTreeWidget(QWidget *parent = 0);
      virtual ~WarningTreeWidget();

      void showWarning(QString text);

    private:
      Q_DISABLE_COPY(WarningTreeWidget);
  };
}

#endif

