#ifndef TreeView_H
#define TreeView_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QWidget>


template< typename t > class QList;


namespace Isis {
  class AbstractTreeItem;
  class AbstractTreeModel;
  class TreeViewContent;
  class TreeViewHeader;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
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

#endif
