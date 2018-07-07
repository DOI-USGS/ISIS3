#include "TemplateEditorWidget.h"
#include "ui_TemplateEditorWidget.h"

#include "Directory.h"
#include "IException.h"
#include "Template.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace Isis {


  TemplateEditorWidget::TemplateEditorWidget(Template* currentTemplate, Directory *directory,
                                             QWidget *parent) : m_ui(new Ui::TemplateEditorWidget) {
    m_ui->setupUi(this);
    m_template = currentTemplate;
    m_directory = directory;
    m_fileType = m_template->templateType();
    m_textChanged = false;

    QFile templateFile(m_template->fileName());
    templateFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream textStream(&templateFile);
    m_ui->templateTextEdit->setText(textStream.readAll());
    templateFile.close();

    connect(m_ui->templateTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(m_ui->templateTextSave, SIGNAL (released()),this, SLOT (saveText()));
    connect(m_ui->templateTextSaveAs, SIGNAL (released()),this, SLOT (saveAsText()));

  }


  TemplateEditorWidget::~TemplateEditorWidget() {
    
    saveOption();
    
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
    
    m_textChanged = false;
  }

  // Future plans to include Save As button

  void TemplateEditorWidget::saveAsText() {
    //We create a new QFile just in case the template's file name has changed
  
    // First need to get the filename to save to via a QFileDialog
    QFileDialog *fileDialog = new QFileDialog(qobject_cast<QWidget *>(parent()), 
                                                            "Save Template File");
    QGridLayout *layout = static_cast<QGridLayout*>(fileDialog->layout());
    QCheckBox *checkbox = new QCheckBox(this);
    checkbox->setText("Automatically Import Template File on Save");
    checkbox->setCheckState(Qt::Checked);
    layout->addWidget( checkbox );
    
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setDirectory(QDir::currentPath());
    fileDialog->setNameFilter("DEF (*.def);;MAP (*.map);;All (*)"); // Need .pvl?
    fileDialog->exec();
    
    // Add file extension based on selected filter if extension not provided, defaulting to ".def"
    QString extension = fileDialog->selectedNameFilter().split("(")[1].mid(1, 4);
    
    if (QString::compare(extension, ".def") == 0 || QString::compare(extension, ".map") == 0) {
      fileDialog->setDefaultSuffix(extension);
    }
    else {
      fileDialog->setDefaultSuffix(".def");
    }
    QString templateFileName = fileDialog->selectedFiles().first();
    
    // Write the file out
    QFile templateFile(templateFileName);
    if (templateFile.open(QFile::WriteOnly | QFile::Text)) {
      templateFile.resize(0);
      templateFile.write(m_ui->templateTextEdit->toPlainText().toUtf8());
      templateFile.close();
    } 
    
    // Import the newly created template file to the project if user checked the box.
    if (checkbox->checkState()) {
      if (templateFile.exists()) {
        QDir templateFolder = m_directory->project()->addTemplateFolder(m_fileType + "/import");
        
        TemplateList *templateList = new TemplateList(templateFolder.dirName(), m_fileType, m_fileType 
                                                      + "/" + templateFolder.dirName() );
            
        QFile::copy(templateFileName, templateFolder.path() + "/" + templateFileName.split("/").last());
        templateList->append(new Template(templateFolder.path() + "/" 
                             + templateFileName.split("/").last(), 
                             m_fileType, 
                             templateFolder.dirName()));

        if (m_fileType == "maps") {
          m_directory->project()->addMapTemplates(templateList);
        }
        else if (m_fileType == "registrations") {
          m_directory->project()->addRegTemplates(templateList);
        }
        m_directory->project()->setClean(false);
      }
      else {
        throw IException(IException::Io,
                         QString("Could not import file [%1]").arg(templateFileName),
                         _FILEINFO_);      }
    }

    m_textChanged = false;
  }
  
  
  void TemplateEditorWidget::textChanged() {
    m_textChanged = true;
  }

  
  void TemplateEditorWidget::saveOption() {
    // The active control is checked here for modification because this was the simplest solution
    // vs changing the project clean state every time the control is modified or saved.
    if (m_textChanged) {
      QMessageBox *box = new QMessageBox(QMessageBox::NoIcon, QString("Current Template Has Unsaved Changes"),
                             QString("Would you like to save your current template?"),
                             NULL, qobject_cast<QWidget *>(parent()), Qt::Dialog);
      QPushButton *save = box->addButton("Save", QMessageBox::AcceptRole);
      box->addButton("Don't Save", QMessageBox::RejectRole);
      QPushButton *cancel = box->addButton("Cancel", QMessageBox::NoRole);
      box->exec();
  
      if (box->clickedButton() == (QAbstractButton*)cancel) {
        return;
      }
      else if (box->clickedButton() == (QAbstractButton*)save) {
        saveAsText();
      }
    }
    m_textChanged = false;
  }
}
