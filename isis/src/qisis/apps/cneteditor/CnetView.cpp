#include "IsisDebug.h"

#include "CnetView.h"

#include <QLabel>
#include <QVariant>
#include <QVBoxLayout>

#include "CnetViewContent.h"
#include "CnetViewHeader.h"
#include "TreeModel.h"


namespace Isis
{

  CnetView::CnetView(QWidget * parent) : QWidget(parent)
  {
    nullify();

    active = false;

    content = new CnetViewContent(this);
    header = new CnetViewHeader(content, this);
    connect(header, SIGNAL(activated()), this, SIGNAL(activated()));
    connect(content, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(header);
    layout->addWidget(content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
  }


  CnetView::~CnetView()
  {
  }


  QSize CnetView::sizeHint()
  {
    if (parentWidget())
      return QSize(parentWidget()->width() / 4, parentWidget()->height() / 2);

    return QSize();
  }


  QFont CnetView::getContentFont() const
  {
    return content->font();
  }


  void CnetView::setModel(TreeModel * someModel)
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


  TreeModel * CnetView::getModel() const
  {
    return content->getModel();
  }


  bool CnetView::isActive() const
  {
    return active;
  }


  QString CnetView::getTitle() const
  {
    return header->getText();
  }


  void CnetView::setTitle(QString someTitle)
  {
    header->setText(someTitle);
  }


  void CnetView::deactivate()
  {
    active = false;

    ASSERT(header);
    if (header)
      header->setActive(false);
    update();
  }


  void CnetView::activate()
  {
    active = true;

    ASSERT(header);
    if (header)
      header->setActive(true);
    update();
  }


  void CnetView::nullify()
  {
    header = NULL;
    content = NULL;
  }
}

