#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H

#include <QWidget>

class QBoxLayout;
class QTableView;
class QTreeView;

namespace Isis
{
  class ControlNet;
}

namespace Qisis
{
  class CubeModel;

  class CnetEditorWidget : public QWidget 
  {
      Q_OBJECT
    
    public:
      CnetEditorWidget(Isis::ControlNet *);
      virtual ~CnetEditorWidget();

    private:
      QBoxLayout* createMainLayout();
      void nullify();

    private:
      QTreeView* cubeView;
      QTableView* pointView;
      QTableView* measureView;
      CubeModel * cubeModel;
      Isis::ControlNet * controlNet;
  };

}

#endif

