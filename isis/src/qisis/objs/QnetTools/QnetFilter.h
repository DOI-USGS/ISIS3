#ifndef QnetFilter_h
#define QnetFilter_h

#include <QWidget>

namespace Isis {
  class ControlNet;
  class QnetNavTool;
  class SerialNumberList;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class QnetFilter : public QWidget {
      Q_OBJECT

    public:
      QnetFilter(QnetNavTool *navTool, QWidget *parent);
      virtual ~QnetFilter();
      virtual void filter();

    protected:
      QList<int> &filteredImages();
      const QList<int> &filteredImages() const;

      QList<int> &filteredPoints();
      const QList<int> &filteredPoints() const;

      ControlNet *controlNet();
      const ControlNet *controlNet() const;

      SerialNumberList *serialNumberList();
      const SerialNumberList *serialNumberList() const;

    signals:
      void filteredListModified();

    private:
      QnetNavTool *m_navTool;
  };
};

#endif
