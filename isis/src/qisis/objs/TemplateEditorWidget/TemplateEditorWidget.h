#ifndef TemplateEditorWidget_H
#define TemplateEditorWidget_H

#include <QFile>
#include <QFrame>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QTextStream>

namespace Ui {
  class TemplateEditorWidget;
}

namespace Isis {
  class Directory;
  class Template;

  /**
   * @brief Widget for displaying information about a target
   *
   * @author 2017-12-05 Christopher Combs
   *
   * @internal
   *   @history 2017-12-05 Christopher Combs - Original version.
   *
   */

   class TemplateEditorWidget : public QFrame {
     Q_OBJECT

     public:
       explicit TemplateEditorWidget(Template * currentTemplate, Directory *directory, QWidget *parent = 0);

       ~TemplateEditorWidget();

     public slots:
       void saveText();

     private:
       Ui::TemplateEditorWidget *m_ui;

       Directory *m_directory;
       Template *m_template;
   };
}



#endif
