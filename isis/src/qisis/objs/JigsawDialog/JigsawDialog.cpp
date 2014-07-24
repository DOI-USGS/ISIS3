#include "JigsawDialog.h"

#include <QDebug>

#include "BundleAdjust.h"
#include "BundleResults.h"
#include "BundleSettings.h"
#include "Control.h"
#include "iTime.h"
#include "JigsawSetupDialog.h"
#include "ui_jigsawdialog.h"
#include "Project.h"

namespace Isis {

  JigsawDialog::JigsawDialog(Project *project, QWidget *parent) : 
                      QDialog(parent), m_ui(new Ui::JigsawDialog) {
    m_ui->setupUi(this);

    m_project = project;
    m_selectedControl = NULL;
    m_bundleSettings = NULL;

    setWindowFlags(Qt::WindowStaysOnTopHint);
  }

  JigsawDialog::~JigsawDialog() {
    if (m_ui) {
      delete m_ui;
    }
    m_ui = NULL;
  }



  void JigsawDialog::on_JigsawSetupButton_pressed() {

    JigsawSetupDialog setupdlg(m_project, this);

    // if (m_bundleSettings != NULL && setupdlg.useLastSettings()) {
    // }

    if (setupdlg.exec() == QDialog::Accepted) {
      m_selectedControl = setupdlg.selectedControl();
      m_bundleSettings = setupdlg.bundleSettings();
    }
  }



  void JigsawDialog::on_JigsawRunButton_clicked() {

    // ??? warning dialogs ???
    if (m_selectedControl == NULL) {
    }

    if (m_project->images().size() == 0) {
    }

    // assume defaults or send warning???
    if (m_bundleSettings == NULL) {
    }

    SerialNumberList snlist;

    QList<ImageList *> imagelists = m_project->images();

    for (int i = 0; i < imagelists.size(); i++) {
      ImageList* list = imagelists.at(i);
      for (int j = 0; j < list->size(); j++) {
        Image* image = list->at(j);
        snlist.Add(image->fileName());
        qDebug() << image->fileName();
      }
    }

    ControlNet *cnet = m_selectedControl->controlNet();

//    BundleAdjust ba(*m_bundleSettings, *cnet, snlist, false);
// run bundle (thread with BundleThread::QThread) - pump output to modeless dialog
    // BundleResults results = ba.solveCholesky();
//    ba.solveCholesky();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // create dummy bundle results and add to project
    BundleSettings *settings = new BundleSettings();
    QString fname = m_selectedControl->fileName();
//    BundleResults *bundleResults = new BundleResults(*settings,*cnet, fname);
    BundleResults *bundleResults = new BundleResults(*settings, fname);
    bundleResults->setRunTime(Isis::iTime::CurrentLocalTime().toAscii().data());
    m_project->addBundleResults(bundleResults);


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  }
}

