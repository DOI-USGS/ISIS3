#include "IsisDebug.h"

#include "TreeView.h"

#include <QLabel>
#include <QVariant>
#include <QVBoxLayout>

#include "TreeViewContent.h"
#include "TreeViewHeader.h"
#include "AbstractTreeModel.h"


namespace Isis {
  namespace CnetViz {
    TreeView::TreeView(QWidget *parent) : QWidget(parent) {
      nullify();

      m_active = false;

      m_content = new TreeViewContent(this);
      m_header = new TreeViewHeader(m_content, this);
      connect(m_header, SIGNAL(activated()), this, SIGNAL(activated()));
      connect(m_content, SIGNAL(treeSelectionChanged()),
          this, SIGNAL(selectionChanged()));
      //     m_content->setContextMenuPolicy(Qt::CustomContextMenu);

      QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(m_header);
      layout->addWidget(m_content);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(0);

      setLayout(layout);
    }


    TreeView::~TreeView() {
    }


    QSize TreeView::sizeHint() const {
      if (parentWidget())
        return QSize(parentWidget()->width() / 4, parentWidget()->height() / 2);

      return QSize();
    }


    QFont TreeView::getContentFont() const {
      return m_content->font();
    }


    void TreeView::setModel(AbstractTreeModel *someModel) {
      ASSERT(m_content);
      m_content->setModel(someModel);

      connect(someModel, SIGNAL(filterProgressChanged(int)),
          m_header, SLOT(updateFilterProgress(int)));
      connect(someModel, SIGNAL(filterProgressRangeChanged(int, int)),
          m_header, SLOT(updateFilterProgressRange(int, int)));
      connect(someModel, SIGNAL(rebuildProgressChanged(int)),
          m_header, SLOT(updateRebuildProgress(int)));
      connect(someModel, SIGNAL(rebuildProgressRangeChanged(int, int)),
          m_header, SLOT(updateRebuildProgressRange(int, int)));
      connect(someModel, SIGNAL(filterCountsChanged(int, int)),
          m_header, SLOT(handleFilterCountsChanged(int, int)));

      connect(someModel, SIGNAL(modelModified()),
          this, SIGNAL(selectionChanged()));
    }


    AbstractTreeModel *TreeView::getModel() const {
      return m_content->getModel();
    }


    bool TreeView::isActive() const {
      return m_active;
    }


    QString TreeView::getTitle() const {
      return m_header->getText();
    }


    void TreeView::setTitle(QString someTitle) {
      m_header->setText(someTitle);
    }


    void TreeView::deactivate() {
      m_active = false;

      ASSERT(m_header);
      if (m_header)
        m_header->setActive(false);
      update();
    }


    void TreeView::activate() {
      m_active = true;

      ASSERT(m_header);
      if (m_header)
        m_header->setActive(true);
      update();
    }


    void TreeView::handleModelSelectionChanged() {
      m_content->refresh();
    }


    void TreeView::nullify() {
      m_header = NULL;
      m_content = NULL;
    }
  }
}
