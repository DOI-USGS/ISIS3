#include "IsisDebug.h"

#include "CnetTreeView.h"

#include <QLabel>
#include <QVariant>
#include <QVBoxLayout>

#include "CnetTreeViewContent.h"
#include "CnetTreeViewHeader.h"
#include "TreeModel.h"


namespace Isis
{

  CnetTreeView::CnetTreeView(QWidget * parent) : QWidget(parent)
  {
    nullify();

    active = false;

    content = new CnetTreeViewContent(this);
    header = new CnetTreeViewHeader(content, this);
    connect(header, SIGNAL(activated()), this, SIGNAL(activated()));
    connect(content, SIGNAL(treeSelectionChanged()),
        this, SIGNAL(selectionChanged()));
//     content->setContextMenuPolicy(Qt::CustomContextMenu);
    
    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(header);
    layout->addWidget(content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
  }


  CnetTreeView::~CnetTreeView()
  {
  }


  QSize CnetTreeView::sizeHint()
  {
    if (parentWidget())
      return QSize(parentWidget()->width() / 4, parentWidget()->height() / 2);

    return QSize();
  }


  QFont CnetTreeView::getContentFont() const
  {
    return content->font();
  }


  void CnetTreeView::setModel(TreeModel * someModel)
  {
    ASSERT(content);
    content->setModel(someModel);

    connect(someModel, SIGNAL(filterProgressChanged(int)),
        header, SLOT(updateFilterProgress(int)));
    connect(someModel, SIGNAL(filterProgressRangeChanged(int, int)),
        header, SLOT(updateFilterProgressRange(int, int)));
    connect(someModel, SIGNAL(rebuildProgressChanged(int)),
        header, SLOT(updateRebuildProgress(int)));
    connect(someModel, SIGNAL(rebuildProgressRangeChanged(int, int)),
        header, SLOT(updateRebuildProgressRange(int, int)));
    connect(someModel, SIGNAL(filterCountsChanged(int, int)),
        header, SLOT(handleFilterCountsChanged(int, int)));

    connect(someModel, SIGNAL(modelModified()),
        this, SIGNAL(selectionChanged()));
  }


  TreeModel * CnetTreeView::getModel() const
  {
    return content->getModel();
  }


  bool CnetTreeView::isActive() const
  {
    return active;
  }


  QString CnetTreeView::getTitle() const
  {
    return header->getText();
  }


  void CnetTreeView::setTitle(QString someTitle)
  {
    header->setText(someTitle);
  }


  void CnetTreeView::deactivate()
  {
    active = false;

    ASSERT(header);
    if (header)
      header->setActive(false);
    update();
  }


  void CnetTreeView::activate()
  {
    active = true;

    ASSERT(header);
    if (header)
      header->setActive(true);
    update();
  }


  void CnetTreeView::onModelSelectionChanged()
  {
    content->refresh();
  }


  void CnetTreeView::nullify()
  {
    header = NULL;
    content = NULL;
  }
}

