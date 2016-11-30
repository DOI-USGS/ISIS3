#ifndef TreeView_H
#define TreeView_H

#include <QWidget>


template< typename t > class QList;


namespace Isis {
  namespace CnetViz {
    class AbstractTreeItem;
    class TreeViewContent;
    class TreeViewHeader;
    class AbstractTreeModel;

    /**
     * @author ????-??-?? Unknown
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class TreeView : public QWidget {
        Q_OBJECT

      signals:
        void activated();
        void selectionChanged();


      public:
        TreeView(QWidget *parent = 0);
        virtual ~TreeView();

        QSize sizeHint() const;

        QFont getContentFont() const;
        void setModel(AbstractTreeModel *someModel);
        AbstractTreeModel *getModel() const;
        bool isActive() const;
        QString getTitle() const;
        void setTitle(QString someTitle);


      public slots:
        void deactivate();
        void activate();
        void handleModelSelectionChanged();


      private: // disable copying and assigning of this class
        TreeView(const TreeView &);
        TreeView &operator=(const TreeView &other);


      private: // methods
        void nullify();


      private: // data
        TreeViewHeader *m_header;
        TreeViewContent *m_content;
        bool m_active;
    };
  }
}

#endif
