#include "TemplateEditorWidget.h"
#include "ui_TemplateEditorWidget.h"

#include "Directory.h"
#include "Template.h"

namespace Isis {


  TemplateEditorWidget::TemplateEditorWidget(Template* currentTemplate, Directory *directory,
                                             QWidget *parent) : m_ui(new Ui::TemplateEditorWidget) {
    m_ui->setupUi(this);
    m_template = currentTemplate;

    QFile templateFile(m_template->fileName());
    templateFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream textStream(&templateFile);
    m_ui->templateTextEdit->setText(textStream.readAll());
    templateFile.close();

    connect(m_ui->templateTextSave, SIGNAL (released()),this, SLOT (saveText()));
    // connect(m_ui->templateTextSaveAs, SIGNAL (released()),this, SLOT (saveAsText()));

  }


  TemplateEditorWidget::~TemplateEditorWidget() {
    delete m_ui;
  }


  void TemplateEditorWidget::saveText() {
    //We create a new QFile just in case the template's file name has changed
    QFile templateFile(m_template->fileName());

    if (templateFile.open(QFile::WriteOnly | QFile::Text)) {
      templateFile.resize(0);
      templateFile.write(m_ui->templateTextEdit->toPlainText().toUtf8());
      templateFile.close();
    }
  }

  // Future plans to include Save As button

  // void TemplateEditorWidget::saveAsText() {
  //   //We create a new QFile just in case the template's file name has changed
  //
  //   QString templateFileName = QFileDialog::getSaveFileName(
  //       qobject_cast<QWidget *>(parent()),
  //       "Save File",
  //       QString(),);
  //
  //   Template *newTemplate(m_template);
  //   newTemplate->fileName = templateFileName;
  //   m_template = newTemplate;
  //
  //   m_directory->project()-> // add to project item model
  //
  //   QFile templateFile(templateFileName);
  //
  //   if (templateFile.open(QFile::WriteOnly | QFile::Text)) {
  //     templateFile.resize(0);
  //     templateFile.write(m_ui->templateTextEdit->toPlainText().toUtf8());
  //     templateFile.close();
  //   }
  // }
}
