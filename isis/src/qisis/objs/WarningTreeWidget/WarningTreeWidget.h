#ifndef WarningTreeWidget_H
#define WarningTreeWidget_H

#include <QTreeWidget>

namespace Isis {

  /**
   * @brief Warning Widget for cnetsuite
   *
   * @author 2012-05-29 Steven Lambright and Tracie Sucharski
   *
   * @internal
   */
  class WarningTreeWidget : public QTreeWidget {
      Q_OBJECT
    public:
      WarningTreeWidget(QWidget *parent = 0);
      virtual ~WarningTreeWidget();

      void showWarning(QString);

    private:
      Q_DISABLE_COPY(WarningTreeWidget);
  };
}

#endif

