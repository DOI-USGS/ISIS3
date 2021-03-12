#ifndef AbstractTreeItem_H
#define AbstractTreeItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QObject>

class QFontMetrics;
template< typename T > class QList;
class QString;
class QVariant;


namespace Isis {
  class ControlPoint;
  class ControlMeasure;

  /**
   * @brief Base class for an item in the tree
   *
   * This class represents an arbitrary item in the tree. Some of the data
   * access methods are provided for compatibility with the table models (i.e.
   * by column).
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode usages to
   *                           image serial number. References #5434.
   */
  class AbstractTreeItem : public QObject {

      Q_OBJECT

    public:
      enum InternalPointerType {
        None,
        Point,
        Measure,
        ImageAndNet,
      };


    public:
      AbstractTreeItem(AbstractTreeItem *parent = 0);
      virtual ~AbstractTreeItem();

      AbstractTreeItem *parent() const;
      void setParent(AbstractTreeItem *newParent);

      int row() const;

      // These methods are designed to be implemented either by
      // AbstractParentItem or AbstractLeafItem.
      virtual void addChild(AbstractTreeItem *child) = 0;
      virtual QList< AbstractTreeItem * > getChildren() const = 0;
      virtual AbstractTreeItem *childAt(int row) const = 0;
      virtual int childCount() const = 0;
      virtual AbstractTreeItem *getFirstVisibleChild() const = 0;
      virtual AbstractTreeItem *getLastVisibleChild() const = 0;
      virtual int indexOf(AbstractTreeItem *child) const = 0;
      virtual void setFirstVisibleChild(AbstractTreeItem *child) = 0;
      virtual void setLastVisibleChild(AbstractTreeItem *child) = 0;

      // These methods are designed to be implemented either by
      // AbstractPointItem, AbstractMeasureItem, AbstractSerialItem,
      // or AbstractNullDataItem.
      virtual QVariant getData() const = 0;
      virtual QVariant getData(QString columnTitle) const = 0;
      virtual void setData(QString const &columnTitle,
          QString const &newData) = 0;
      virtual bool isDataEditable(QString columnTitle) const = 0;
      virtual void deleteSource() = 0;
      virtual InternalPointerType getPointerType() const = 0;
      virtual void *getPointer() const = 0;

      // There are things that every AbstractTreeItem can do.
      virtual QString getFormattedData() const;
      virtual QString getFormattedData(QString columnTitle) const;

      virtual bool hasMeasure(ControlMeasure *) const;
      virtual bool hasImage(QString imageSerial) const;
      virtual bool hasPoint(ControlPoint *) const;

      virtual AbstractTreeItem *getNextVisiblePeer() const;
      virtual void setNextVisiblePeer(AbstractTreeItem *next);

      virtual void setExpanded(bool newState);
      virtual bool isExpanded() const;

      virtual void setSelected(bool newState);
      virtual void setSelectable(bool newSelectable);
      virtual bool isSelected() const;
      virtual bool isSelectable() const;

      virtual void setVisible(bool newState);
      virtual bool isVisible() const;

      virtual int getDataWidth() const;
      virtual int getDepth() const;

      virtual void setLastVisibleFilteredItem(AbstractTreeItem *item);
      virtual const AbstractTreeItem *getLastVisibleFilteredItem() const;

    protected slots:
      virtual void sourceDeleted() = 0;

    protected:
      virtual void calcDataWidth(int avgCharWidth);
      static double catchNull(QString);
      static QString catchNull(QVariant);


      // disable copying of this class
    private:
      AbstractTreeItem(AbstractTreeItem const &);
      AbstractTreeItem &operator=(AbstractTreeItem const &);


    private:
      AbstractTreeItem *m_nextVisibleItem;
      AbstractTreeItem *m_parentItem;
      bool m_expanded;
      bool m_selectable;
      bool m_selected;
      bool m_visible;
      int m_dataWidth;
  };
}

#endif
