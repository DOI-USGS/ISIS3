#include "IsisDebug.h"

#include "TreeView.h"

#include <QLabel>
#include <QVariant>
#include <QVBoxLayout>

#include "TreeViewContent.h"
#include "TreeViewHeader.h"
#include "AbstractTreeModel.h"


namespace Isis
{
  namespace CnetViz
  {
    TreeView::TreeView(QWidget * parent) : QWidget(parent)
    {
      nullify();

      active = false;

      content = new TreeViewContent(this);
      header = new TreeViewHeader(content, this);
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


    TreeView::~TreeView()
    {
    }


    QSize TreeView::sizeHint()
    {
      if (parentWidget())
        return QSize(parentWidget()->width() / 4, parentWidget()->height() / 2);

      return QSize();
    }


    QFont TreeView::getContentFont() const
    {
      return content->font();
    }


    void TreeView::setModel(AbstractTreeModel * someModel)
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


    AbstractTreeModel * TreeView::getModel() const
    {
      return content->getModel();
    }


    bool TreeView::isActive() const
    {
      return active;
    }


    QString TreeView::getTitle() const
    {
      return header->getText();
    }


    void TreeView::setTitle(QString someTitle)
    {
      header->setText(someTitle);
    }


    void TreeView::deactivate()
    {
      active = false;

      ASSERT(header);
      if (header)
        header->setActive(false);
      update();
    }


    void TreeView::activate()
    {
      active = true;

      ASSERT(header);
      if (header)
        header->setActive(true);
      update();
    }


    void TreeView::handleModelSelectionChanged()
    {
      content->refresh();
    }


    void TreeView::nullify()
    {
      header = NULL;
      content = NULL;
    }
  }
}
