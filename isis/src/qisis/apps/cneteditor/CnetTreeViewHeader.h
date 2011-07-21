#ifndef CnetTreeViewHeader_H
#define CnetTreeViewHeader_H

#include <QWidget>

class QMouseEvent;
class QPainter;
class QPaintEvent;
class QSize;
class QString;

namespace Isis
{

  class CnetTreeViewContent;

  class CnetTreeViewHeader : public QWidget
  {

      Q_OBJECT

    public:
      explicit CnetTreeViewHeader(CnetTreeViewContent * someContent,
          QWidget * parent);
      CnetTreeViewHeader(const CnetTreeViewHeader & other);
      virtual ~CnetTreeViewHeader();
      QSize minimumSizeHint() const;
      QString getText();
      void setText(QString text);
      QSize sizeHint()
      {
        return minimumSizeHint();
      }
      CnetTreeViewHeader & operator=(const CnetTreeViewHeader & other);


    public slots:
      void setActive(bool);
      void handleFilterCountsChanged(int visibleTopLevelItemCount,
          int topLevelItemCount);


    signals:
      void activated();


    protected:
      void mouseReleaseEvent(QMouseEvent * event);
      void paintEvent(QPaintEvent * event);


    private: // methods
      void nullify();
      void paintHeader(QPainter * painter, int rowheight);
      void paintProgress(QPainter * painter, const QRect & rect, int min,
          int max, int value);


    private slots:
      void updateFilterProgress(int newProgress);
      void updateFilterProgressRange(int min, int max);
      void updateRebuildProgress(int newProgress);
      void updateRebuildProgressRange(int min, int max);


    private: // data
      CnetTreeViewContent * content;
      QString * headerText;
      int filterProgress;
      int filterProgressMin;
      int filterProgressMax;
      int rebuildProgress;
      int rebuildProgressMin;
      int rebuildProgressMax;
      bool active;
      int visibleCount;
      int totalCount;
  };
}


#endif
