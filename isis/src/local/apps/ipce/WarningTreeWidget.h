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
   *   @histroy 2012-07-31 Kimberly Oyama - Added comments to some of the methods.
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

