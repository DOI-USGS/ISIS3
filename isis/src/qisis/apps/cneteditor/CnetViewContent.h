#ifndef CnetViewContent_H
#define CnetViewContent_H

#include <QAbstractScrollArea>


class QEvent;
class QKeyEvent;
template< typename T > class QList;
class QMouseEvent;
class QPoint;
class QResizeEvent;


namespace Isis
{
  class AbstractTreeItem;
  class CnetView;
  class TreeModel;

  class CnetViewContent : public QAbstractScrollArea
  {
      Q_OBJECT

    public:
      CnetViewContent(QWidget * parent);
      virtual ~CnetViewContent();
      QSize minimumSizeHint() const;
      QSize sizeHint();
      TreeModel * getModel();
      void setModel(TreeModel * someModel);


    signals:
      void selectionChanged();


    public slots:
      void refresh();


    protected:
      bool eventFilter(QObject * target, QEvent * event);
      void keyPressEvent(QKeyEvent * event);
      void leaveEvent(QEvent * event);
      void mouseDoubleClickEvent(QMouseEvent * event);
      void mouseMoveEvent(QMouseEvent * event);
      void mousePressEvent(QMouseEvent * event);
      void mouseReleaseEvent(QMouseEvent * event);
      void paintEvent(QPaintEvent * event);
      void resizeEvent(QResizeEvent * event);
      void scrollContentsBy(int dx, int dy);


    private:
      CnetViewContent(const CnetViewContent & other);
      CnetViewContent & operator=(const CnetViewContent & other);


    private:
      void nullify();
      void paintItemText(QPainter *, int, QPoint, QPoint);
      void drawCollapsedArrow(QPainter *, QRect);
      void drawExpandedArrow(QPainter *, QRect);
      QRect getArrowRect(AbstractTreeItem * item) const;


    private slots:
      void setAlternatingRowColors(bool);
      void updateItemList();


    private:
      CnetView * parentView;
      TreeModel * model;
      QList< AbstractTreeItem * > * items;

      //! The bool is true if the arrow in the item was pressed
      QPair< AbstractTreeItem *, bool > * pressedItem;

      //! The bool is true if the mouse is hovering over the arrow
      QPair< AbstractTreeItem *, bool > * hoveredItem;

      AbstractTreeItem * prevSelectedItem;
      QPoint * mousePressPos;
      int rowHeight;
      int contentWidth;
      bool alternatingRowColors;


    private:
      static const int ITEM_PADDING = 4;
      static const int ITEM_INDENTATION = 23;
  };
}

#endif

