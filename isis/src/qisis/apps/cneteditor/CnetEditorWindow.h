#ifndef CnetEditorWindow_H
#define CnetEditorWindow_H

#include <QMainWindow>


namespace Qisis
{

  class CnetEditorWidget;

  class CnetEditorWindow : public QMainWindow 
  {
      Q_OBJECT
      
    public:
      CnetEditorWindow();
      virtual ~CnetEditorWindow();

    private:
      void nullify();
      
    private:
      CnetEditorWidget * editorWidget;
  };

}

#endif

