#include "CorrMatTreeWidgetItem.h"

#include <QDebug>

#include "CorrelationMatrix.h"


namespace Isis {

  /**
   * CorrMatTreeWidgetItem constructor.
   * CorrMatTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  CorrMatTreeWidgetItem::CorrMatTreeWidgetItem(CorrelationMatrix correlationMatrix,
                                               QTreeWidget *parent) :
      QTreeWidgetItem(parent, UserType) {

    m_correlationMatrix = new CorrelationMatrix(correlationMatrix);

    setText( 0, "Correlation Matrix");
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setIcon( 0, QIcon(":pointReg") );
  }


  CorrMatTreeWidgetItem::~CorrMatTreeWidgetItem() {
    delete m_correlationMatrix;
    m_correlationMatrix = NULL;
  }


  CorrelationMatrix *CorrMatTreeWidgetItem::correlationMatrix() {
    return m_correlationMatrix;
  }


  void CorrMatTreeWidgetItem::selectionChanged() {
//     m_correlationMatrix->displayProperties()->setSelected( isSelected() );
  }
}
