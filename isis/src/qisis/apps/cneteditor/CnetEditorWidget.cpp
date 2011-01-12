#include "CnetEditorWidget.h"

#include <QApplication>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QTreeView>

#include "ControlNet.h"
#include "CubeModel.h"

namespace Qisis
{

  CnetEditorWidget::CnetEditorWidget(Isis::ControlNet * cNet) : controlNet(cNet)
  {
    nullify();

    QBoxLayout* mainLayout = createMainLayout();
    setLayout(mainLayout);
  }

  CnetEditorWidget::~CnetEditorWidget()
  {}

  QBoxLayout* CnetEditorWidget::createMainLayout()
  {
    cubeModel = new CubeModel(controlNet, qApp);
    cubeModel->printCubeStructure();
    
    cubeView = new QTreeView();
    cubeView->setModel(cubeModel);
    pointView = new QTableView();
    measureView = new QTableView();  

    QBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->addWidget(cubeView);
    mainLayout->addWidget(pointView);
    mainLayout->addWidget(measureView);

    return mainLayout;
  }

  void CnetEditorWidget::nullify() 
  {
    cubeView = NULL;
    pointView = NULL;
    measureView = NULL;
  }
}
