#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <iostream>
#include <fstream>

#include "GuiLog.h"

namespace Isis {
  //! Constructor
  GuiLog::GuiLog(QWidget *parent) : QWidget (parent) {

    QVBoxLayout *lo = new QVBoxLayout;
    setLayout(lo);

//    QLabel *lb = new QLabel("Log");
//    lo->addWidget(lb);

    p_textEdit = new QTextEdit;
    p_textEdit->setFont(QFont("Courier"));
    p_textEdit->setFontPointSize(10);
//    p_textEdit->setReadOnly(true);

    lo->addWidget(p_textEdit);
  }

  //! Destructor
  GuiLog::~GuiLog() {
  }

  //! Add more information to the log widget
  void GuiLog::Write(const QString &string) {
    p_textEdit->append(string);
  }

  //! Clear the contents of the log widget
  void GuiLog::Clear() {
    p_textEdit->clear();
    p_textEdit->setFont(QFont("Courier"));
    p_textEdit->setFontPointSize(10);
  }

  //! Save the contents of the log widget to a file
  void GuiLog::Save() {
    QString s = QFileDialog::getSaveFileName(this,"Save log to file");
    if (s != "") {
      std::ofstream fout;
      std::string filename(s.toStdString());
      fout.open(filename.c_str());
      fout << p_textEdit->toPlainText().toStdString();
      fout.close();
    }
  }
}
