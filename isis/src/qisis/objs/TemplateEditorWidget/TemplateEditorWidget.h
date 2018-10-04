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
   *   @history 2018-07-07 Summer Stapleton - Implemented the SaveAs option in the widget. This 
   *                          allows a user to save the file externally to the project as well as
   *                          to immediately import the template with a simple checkbox in the
   *                          filedialog. Users are also prompted to save if there have been 
   *                          changes made to the template file since saving if the widget or the 
   *                          ipce main window is closed.
   *
   */

   class TemplateEditorWidget : public QFrame {
     Q_OBJECT

     public:
       explicit TemplateEditorWidget(Template * currentTemplate, Directory *directory, QWidget *parent = 0);

       ~TemplateEditorWidget();

     public slots:
       void saveText();
       void saveAsText();
       void saveOption();
       void textChanged();

     private:
       Ui::TemplateEditorWidget *m_ui;

       Directory *m_directory;  // The directory of the open project
       Template *m_template;    // The template being modified
       QString m_fileType;      // The file type of the template ("Maps" or "Registrations")
       bool m_textChanged;      // Whether the text in the widget has been changed since last save
   };
}



#endif
