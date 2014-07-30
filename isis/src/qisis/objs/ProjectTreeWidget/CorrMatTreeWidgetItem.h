#ifndef CorrMatTreeWidgetItem_H
#define CorrMatTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>

namespace Isis {
  class CorrelationMatrix;

  /**
   * @brief A correlation matrix in the project tree widget
   *
   * This class visualizes a CorrelationMatrix * from the project in the project tree widget.
   *
   * @author 2012-09-26 Tracie Sucharski
   *
   * @internal
   */
  class CorrMatTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      CorrMatTreeWidgetItem(CorrelationMatrix *correlationMatrix, QTreeWidget *parent = 0);
      virtual ~CorrMatTreeWidgetItem();

      CorrelationMatrix correlationMatrix();
      void selectionChanged();

    private:
      CorrelationMatrix *m_correlationMatrix;
  };
}

#endif

