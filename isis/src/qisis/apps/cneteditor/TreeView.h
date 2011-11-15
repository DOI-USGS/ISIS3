#ifndef TreeView_H
#define TreeView_H

#include <QWidget>


template< typename t > class QList;


namespace Isis
{
  namespace CnetViz
  {
    class AbstractTreeItem;
    class TreeViewContent;
    class TreeViewHeader;
    class AbstractTreeModel;

    class TreeView : public QWidget
    {
        Q_OBJECT

      signals:
        void activated();
        void selectionChanged();


      public:
        TreeView(QWidget * parent = 0);
        virtual ~TreeView();
        QSize sizeHint();
        QFont getContentFont() const;
        void setModel(AbstractTreeModel * someModel);
        AbstractTreeModel * getModel() const;
        bool isActive() const;
        QString getTitle() const;
        void setTitle(QString someTitle);


      public slots:
        void deactivate();
        void activate();
        void handleModelSelectionChanged();


      private: // disable copying and assigning of this class
        TreeView(const TreeView &);
        TreeView & operator=(const TreeView & other);


      private: // methods
        void nullify();


      private: // data
        TreeViewHeader * header;
        TreeViewContent * content;
        bool active;
    };
  }
}

#endif
