#include "JigsawDialog.h"

#include "BundleAdjust.h"
#include "BundleSettings.h"
#include "Control.h"
#include "JigsawSetupDialog.h"
#include "ui_JigsawDialog.h"
#include "Project.h"

namespace Isis {

  JigsawDialog::JigsawDialog(Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JigsawDialog) {
    ui->setupUi(this);

    m_project = project;
  }

  JigsawDialog::~JigsawDialog() {
    delete ui;
  }

  void JigsawDialog::on_JigsawSetupButton_pressed()
  {
    JigsawSetupDialog setupdlg(m_project, this);
    if (setupdlg.exec() == QDialog::Accepted) {
      m_bundleSettings = setupdlg.bundleSettings();
    }
  }

  void JigsawDialog::on_JigsawRunButton_clicked() {

    SerialNumberList snlist;

    QList<ImageList *> imagelist = m_project->images();

    for ( int i = 0; i < imagelist.size(); i++) {
        ImageList* list = imagelist.at(i);
        for (int j = 0; j < list->size(); j++) {
          Image* image = list->at(j);
          snlist.Add(image->fileName());
        }
    }

//    QString filename;
//    foreach(filename, cubeFiles) {
//      try {
//        m_serialNumbers->Add(filename);
//      }
//      catch(IException &) {
//      }
//    }

    ControlNet* cnet = m_pselectedControl->controlNet();
    BundleAdjust ba(m_bundleSettings, *cnet, snlist, false);
    
// run bundle (thread with BundleThread::QThread) - pump output to modeless dialog
    ba.solveCholesky();
  }
}

