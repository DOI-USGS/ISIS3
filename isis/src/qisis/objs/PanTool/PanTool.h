#ifndef PanTool_h
#define PanTool_h

#include "Tool.h"


class QComboBox;
class QLineEdit;

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport
   *                                 - Fixed multiple include problems
   */
  class PanTool : public Tool {
      Q_OBJECT

    public:
      PanTool(QWidget *parent);
      void addTo(QMenu *);

    protected:
      QString menuName() const {
        return "&View";
      };
      QAction *toolPadAction(ToolPad *pad);
      QWidget *createToolBarWidget(QStackedWidget *parent);

    protected slots:
      void mouseButtonPress(QPoint p, Qt::MouseButton s);
      void mouseMove(QPoint p);
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private slots:
      void panRight() {
        pan(panRate(true), 0);
      };
      void panLeft() {
        pan(-panRate(true), 0);
      };
      void panUp() {
        pan(0, -panRate(false));
      };
      void panDown() {
        pan(0, panRate(false));
      };
      void setCustom();
      void updateLineEdit();
      void writeSettings();
      void readSettings();

    private:
      void pan(int x, int y);
      int panRate(bool horz);

      QAction *p_panRight;
      QAction *p_panLeft;
      QAction *p_panUp;
      QAction *p_panDown;

      QComboBox *p_panRateBox;
      QLineEdit *p_lineEdit;

      bool p_dragPan;
      QPoint p_lastPoint;
  };
};

#endif
