#include "FileTool.h"

#include <QApplication>
#include <QFileDialog>
#include <QImage>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QPrintDialog>

#include "Brick.h"
#include "BrowseDialog.h"
#include "CubeAttribute.h"
#include "FileDialog.h"
#include "MainWindow.h"
#include "MdiCubeViewport.h"
#include "OriginalLabel.h"
#include "Workspace.h"

namespace Qisis {
  /**
   * Constructs a FileTool object.
   *
   * @param parent
   * @internal
   *   @history 2008-12-10 Jeannie Walldren - Added "What's this?"
   *            and shortcut to "Save" action
   */
  FileTool::FileTool(QWidget *parent) : Qisis::Tool(parent) {
    p_parent = parent;
    p_dir = "/thisDirDoesNotExist!";
    p_open = new QAction(parent);
    p_open->setShortcut(Qt::CTRL + Qt::Key_O);
    p_open->setText("&Open...");
    p_open->setIcon(QPixmap(toolIconDir() + "/fileopen.png"));
    p_open->setToolTip("Open cube");
    QString whatsThis =
      "<b>Function:</b> Open an <i>Isis cube</i> in new viewport \
       <p><b>Shortcut:</b>  Ctrl+O\n</p> \
       <p><b>Hint:</b> Use Ctrl or Shift in file dialog to open \
       multiple cubes</p>";
    p_open->setWhatsThis(whatsThis);
    connect(p_open, SIGNAL(activated()), this, SLOT(open()));

    p_browse = new QAction(parent);
    p_browse->setShortcut(Qt::CTRL + Qt::Key_B);
    p_browse->setText("&Browse...");
    p_browse->setToolTip("Browse cubes");
    whatsThis =
      "<b>Function:</b> Browse a <i>Isis cubes</i> in new viewport \
       <p><b>Shortcut:</b>  Ctrl+B\n</p>";
    p_browse->setWhatsThis(whatsThis);
    connect(p_browse, SIGNAL(activated()), this, SLOT(browse()));

    p_save = new QAction(parent);
    p_save->setShortcut(Qt::CTRL + Qt::Key_S);
    p_save->setText("&Save");
    p_save->setIcon(QPixmap(toolIconDir() + "/filesave.png"));
    p_save->setToolTip("Save");
    whatsThis =
      "<b>Function:</b> Save changes to the current Cube \
       <p><b>Shortcut:</b> Ctrl+S</p>";
    p_save->setWhatsThis(whatsThis);
    connect(p_save, SIGNAL(activated()), this, SLOT(confirmSave()));
    p_save->setEnabled(false);

    p_saveAs = new QAction(parent);
    p_saveAs->setText("Save &As...");
    p_saveAs->setIcon(QPixmap(toolIconDir() + "/filesaveas.png"));
    p_saveAs->setToolTip("Save As");
    whatsThis =
      "<b>Function:</b> Save the current Cube to the specified location";
    p_saveAs->setWhatsThis(whatsThis);
    connect(p_saveAs, SIGNAL(activated()), this, SLOT(saveAs()));

    p_exportView = new QAction(parent);
    p_exportView->setText("Export View");
    p_exportView->setIcon(QPixmap(toolIconDir() + "/fileexport.png"));
    p_exportView->setToolTip("Export View");
    whatsThis =
      "<b>Function:</b> Save visible contents of the active \
       viewport as a png, jpg, tiff \
       <p><b>Hint:</b>  Your local installation of Qt may not support \
       all formats.  Reinstall Qt if necessary</p>";
    p_exportView->setWhatsThis(whatsThis);
    connect(p_exportView, SIGNAL(activated()), this, SLOT(exportView()));
    p_exportView->setEnabled(false);

    p_print = new QAction(parent);
    p_print->setText("&Print...");
    p_print->setShortcut(Qt::CTRL + Qt::Key_P);
    p_print->setIcon(QPixmap(toolIconDir() + "/fileprint.png"));
    p_print->setToolTip("Print");
    whatsThis =
      "<b>Function:</b> Print visible contents of the active viewport \
      <p><b>Shortcut:</b> Ctrl+P</b>";
    p_print->setWhatsThis(whatsThis);
    connect(p_print, SIGNAL(activated()), this, SLOT(print()));
    p_print->setEnabled(false);

    p_closeAll = new QAction(parent);
    p_closeAll->setText("&Close All...");
    p_closeAll->setToolTip("Close All");
    whatsThis =
      "<b>Function:</b> Close all cube viewports.";
    p_closeAll->setWhatsThis(whatsThis);

    p_exit = new QAction(parent);
    p_exit->setShortcut(Qt::CTRL + Qt::Key_Q);
    p_exit->setText("E&xit");
    p_exit->setIcon(QPixmap(toolIconDir() + "/fileclose.png"));
    whatsThis =
      "<b>Function:</b>  Quit qview \
      <p><b>Shortcut:</b> Ctrl+Q</p>";
    p_exit->setWhatsThis(whatsThis);
    connect(p_exit, SIGNAL(activated()), this, SLOT(exit()));

    p_lastDir.clear();
    p_lastViewport = NULL;

    activate(true);
  }

  /**
   * Adds the file tool's actions to the menu
   *
   * @param menu
   */
  void FileTool::addTo(QMenu *menu) {
    menu->addAction(p_open);
    menu->addAction(p_browse);
    menu->addAction(p_save);
    menu->addAction(p_saveAs);
    menu->addAction(p_exportView);
    menu->addAction(p_print);
    menu->addAction(p_closeAll);
    menu->addAction(p_exit);
  }

  /**
   * Connects the fileSelected signal to the workspace's addCubeViewport slot
   *
   * @param ws
   */
  void FileTool::addTo(Workspace *ws) {
    p_workSpace = ws;
    Tool::addTo(ws);
    connect(this, SIGNAL(fileSelected(QString)),
            ws, SLOT(addCubeViewport(QString)));

    connect(p_closeAll, SIGNAL(activated()), ws, SLOT(closeAllSubWindows()));
  }

  /**
   * Adds the file tool's actions to the permanent toolbar
   *
   * @param perm
   */
  void FileTool::addToPermanent(QToolBar *perm) {
    perm->addAction(p_open);
    perm->addAction(p_exportView);
    perm->addAction(p_print);
    perm->addAction(p_exit);
  }

  /**
   * This method allows the user to navigate and open a cube with a file dialog.
   *
   */
  void FileTool::open() {
    //Set up the list of filters that are default with this dialog.
    if(!p_filterList.contains("Isis cubes (*.cub)")) {
      p_filterList.append("Isis cubes (*.cub)");
      p_filterList.append("All files (*)");
    }
    if(!p_dir.exists()) {
      p_dir = QDir::current();
    }

    FileDialog *fileDialog = new FileDialog("Open", p_filterList, p_dir, (QWidget *)parent());
    fileDialog->show();
    connect(fileDialog, SIGNAL(fileSelected(QString)),
            p_workSpace, SLOT(addCubeViewport(QString)));
  }

  /**
   * This method allows the user to navigate and browse cubes with a file dialog .
   *
   */
  void FileTool::browse() {
    //Set up the list of filters that are default with this dialog.
    if(!p_filterList.contains("Isis cubes (*.cub)")) {
      p_filterList.append("Isis cubes (*.cub)");
      p_filterList.append("All files (*)");
    }
    if(!p_dir.exists()) {
      p_dir = QDir::current();
    }
    BrowseDialog *browser = new BrowseDialog("Browse", p_filterList, p_dir, (QWidget *)parent());
    browser->show();
    connect(browser, SIGNAL(fileSelected(QString)),
            p_workSpace, SLOT(addBrowseView(QString)));
  }

  /**
   * This method asks the user to confirm if they want to finalize their saved
   * changes.
   *
   */
  void FileTool::confirmSave() {
    if(QMessageBox::question(
          p_parent,
          tr("Save File?"),
          tr("Are you sure you want to save this file?"),
          tr("&Save"), tr("&Cancel"),
          QString(), 0, 1)) {
      return;
    }
    else {
      save();
    }
  }

  /**
   * This method saves any changes made to the current cube, these
   * changes are finalized! There is no undoing once a save has
   * been made.
   *
   */
  void FileTool::save() {
    //If the current viewport is null (safety check), return from this method
    if(cubeViewport() == NULL) {
      QMessageBox::information((QWidget *)parent(), "Error", "No active cube to save");
      return;
    }
    //Emit a signal to notify other objects that this cube has been saved
    emit saveChanges(cubeViewport());
    //Disable the save action
    p_save->setEnabled(false);

    //Essentially, closing the cube saves it's changes, and we want to keep it open,
    //so reopen the current cube!
    cubeViewport()->cube()->ReOpen("rw");
  }

  /**
   * This method essentially creates a new cube, copies the
   * current cube (and any changes made to it) to the new cube,
   * reverses all changes NOT saved to the current cube and closes
   * it. Finally it sets the cubeviewport's cube to the new saved
   * cube.
   *
   */
  void FileTool::saveAs() {
    //If the current viewport is null (safety check), return from this method
    if(cubeViewport() == NULL) {
      QMessageBox::information((QWidget *)parent(), "Error", "No active cube to save");
      return;
    }
    //Get the new cube's filename
    QString output =
      QFileDialog::getSaveFileName((QWidget *)parent(),
                                   "Choose output file",
                                   p_lastDir,
                                   QString("Cubes (*.cub)"));
    //If the filename is empty, return
    if(output.isEmpty()) return;

    //If the filename is the same as the current cube's filename, just save it
    if(output.toStdString().compare(cubeViewport()->cube()->Filename()) == 0) {
      save();
      return;
    }

    //Save the current cube's changes by reopening it, and open an input cube
    //from the current cube's location
    Isis::Cube *icube = new Isis::Cube();
    cubeViewport()->cube()->ReOpen();
    icube->Open(cubeViewport()->cube()->Filename(), "rw");

    //Create the output cube and default output attribute with the output filename
    Isis::CubeAttributeOutput outAtt(output.toStdString());
    Isis::Cube *ocube = new Isis::Cube();
    //Propogate all labels, tables, blobs, etc from the input to output cube
    try {
      ocube->SetDimensions(icube->Samples(), icube->Lines(), icube->Bands());
      ocube->SetByteOrder(outAtt.ByteOrder());
      ocube->SetCubeFormat(outAtt.FileFormat());
      if(outAtt.DetachedLabel()) ocube->SetDetached();
      if(outAtt.AttachedLabel()) ocube->SetAttached();

      if(outAtt.PropagatePixelType()) {
        ocube->SetPixelType(icube->PixelType());
      }
      else {
        ocube->SetPixelType(outAtt.PixelType());
      }

      if(outAtt.PropagateMinimumMaximum()) {
        if(ocube->PixelType() == Isis::Real) {
          ocube->SetBaseMultiplier(0.0, 1.0);
        }
        else if(ocube->PixelType() >= icube->PixelType()) {
          double base = icube->Base();
          double mult = icube->Multiplier();
          ocube->SetBaseMultiplier(base, mult);
        }
        else if((ocube->PixelType() != Isis::Real) &&
                (ocube->PixelType() != Isis::UnsignedByte) &&
                (ocube->PixelType() != Isis::SignedWord)) {
          std::string msg = "Looks like your refactoring to add different pixel types";
          msg += " you'll need to make changes here";
          throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
        }
        else {
          std::string msg = "You've chosen to reduce your output PixelType for [" +
                            output.toStdString() + "] you must specify the output pixel range too";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }
      }
      else {
        // Not propagating so either the user entered or the programmer did
        ocube->SetMinMax(outAtt.Minimum(), outAtt.Maximum());
      }

      int needLabBytes = icube->LabelBytesUsed() + (1024 * 6);
      if(needLabBytes > ocube->LabelBytes()) {
        ocube->SetLabelBytes(needLabBytes);
      }

      // Allocate the cube
      ocube->Create(output.toStdString());

      // Transfer labels from the first input cube
      Isis::PvlObject &incube = icube->Label()->FindObject("IsisCube");
      Isis::PvlObject &outcube = ocube->Label()->FindObject("IsisCube");
      for(int i = 0; i < incube.Groups(); i++) {
        outcube.AddGroup(incube.Group(i));
      }

      // Transfer tables from the first input cube
      Isis::Pvl &inlab = *icube->Label();
      for(int i = 0; i < inlab.Objects(); i++) {
        if(inlab.Object(i).IsNamed("Table")) {
          Isis::Blob t((std::string)inlab.Object(i)["Name"], inlab.Object(i).Name());
          icube->Read(t);
          ocube->Write(t);
        }
      }

      // Transfer blobs from the first input cube
      inlab = *icube->Label();
      for(int i = 0; i < inlab.Objects(); i++) {
        if(inlab.Object(i).IsNamed("Polygon")) {
          Isis::Blob t((std::string)inlab.Object(i)["Name"], inlab.Object(i).Name());
          icube->Read(t);
          ocube->Write(t);
        }
      }

      // Transfer tables from the first input cube
      inlab = *icube->Label();
      for(int i = 0; i < inlab.Objects(); i++) {
        if(inlab.Object(i).IsNamed("OriginalLabel")) {
          Isis::OriginalLabel ol;
          icube->Read(ol);
          ocube->Write(ol);
        }
      }
    }
    catch(Isis::iException &e) {
      delete ocube;
      throw;
    }
    // Everything was propagated okay!


    //Start the copy process line by line
    Isis::Brick ibrick(*icube, icube->Samples(), 1, 1);
    Isis::Brick obrick(*ocube, ocube->Samples(), 1, 1);

    int numBricks;
    if(ibrick.Bricks() > obrick.Bricks()) numBricks = ibrick.Bricks();
    else numBricks = obrick.Bricks();

    // Loop and let the app programmer work with the bricks
    ibrick.begin();
    obrick.begin();
    for(int i = 0; i < numBricks; i++) {
      icube->Read(ibrick);
      //Copy the contents to the output cube
      copy(ibrick, obrick);
      ocube->Write(obrick);
      ibrick++;
      obrick++;
    }

    //End the process and cleanup the cubes
    icube->Close();
    delete icube;
    ocube->Close();
    delete ocube;


    //Reopen the input cube and discard all of the changes made to it
    //and save it to it's original state
    cubeViewport()->cube()->ReOpen("rw");
    emit discardChanges(cubeViewport());
    cubeViewport()->cube()->Close();
    //Open the output cube and set it to the cubeviewport
    ocube = new Isis::Cube();
    ocube->Open(output.toStdString(), "rw");
    cubeViewport()->setCube(ocube);

    //Emit a signal to other objects that a save has been made
    emit saveChanges(cubeViewport());
    //Disable the save action
    p_save->setEnabled(false);

    p_lastDir = output;

  }

  /**
   * This method copies from the input buffer to the output buffer
   *
   * @param in
   * @param out
   */
  void FileTool::copy(Isis::Buffer &in, Isis::Buffer &out) {
    out.Copy(in);
  }

  /**
   * This slot emits a signal to discard all changes to the
   * current viewport
   *
   */
  void FileTool::discard() {
    emit discardChanges(cubeViewport());
  }

  /**
   * This method allows the user to export the current view as an image file.
   *
   */
  void FileTool::exportView() {
    if(cubeViewport() == NULL) {
      QMessageBox::information((QWidget *)parent(), "Error", "No active cube to export");
      return;
    }

    QString output =
      QFileDialog::getSaveFileName((QWidget *)parent(),
                                   QString("Choose output file"),
                                   p_lastDir,
                                   QString("PNG (*.png);;JPG (*.jpg);;TIF (*.tif)"));
    if(output.isEmpty()) return;

    p_lastDir = output;

    QString format = QFileInfo(output).suffix();

    if(format.isEmpty()) {
      if(output.endsWith('.')) {
        output.append(QString("png"));
      }
      else {
        output.append(QString(".png"));
      }
    }
    else if(format.compare("png", Qt::CaseInsensitive) &&
            format.compare("jpg", Qt::CaseInsensitive) &&
            format.compare("tif", Qt::CaseInsensitive)) {

      QMessageBox::information((QWidget *)parent(), "Error", format + " is an invalid extension.");
      return;
    }

    QPixmap pm = QPixmap::grabWidget(cubeViewport()->viewport());

    //if (!cubeViewport()->pixmap().save(output,format.toStdString().c_str())) {

    if(!pm.save(output)) {
      QMessageBox::information((QWidget *)parent(), "Error", "Unable to save " + output);
      return;
    }
  }

  /**
   * This method allows the user to print the current viewport.
   *
   */
  void FileTool::print() {
    // Is there anything to print
    if(cubeViewport() == NULL) {
      QMessageBox::information((QWidget *)parent(), "Error", "No active cube to print");
      return;
    }

    // Initialize a printer
    static QPrinter *printer = NULL;
    if(printer == NULL) printer = new QPrinter;
    printer->setPageSize(QPrinter::Letter);
    printer->setColorMode(QPrinter::GrayScale);
    if(cubeViewport()->isColor()) printer->setColorMode(QPrinter::Color);

    QPrintDialog printDialog(printer, (QWidget *)parent());
    if(printDialog.exec() == QDialog::Accepted) {
      // Get display widget as a pixmap and convert to an image
      QPixmap pixmap = QPixmap::grabWidget(cubeViewport()->viewport());
      QImage img = pixmap.toImage();

      // C++ Gui Programmign with Qt, page 201
      QPainter painter(printer);
      QRect rect = painter.viewport();
      QSize size = img.size();
      size.scale(rect.size(), Qt::KeepAspectRatio);
      painter.setViewport(rect.x(), rect.y(),
                          size.width(), size.height());
      painter.setWindow(img.rect());
      painter.drawImage(0, 0, img);
    }
  }

  /**
   * Try to close all open cubes and save/discard if necessary.
   */
  bool FileTool::closeAll() {
    //  Close all cubes
    // We must create a temporary list.  If not the actual
    // list size gets modified when a close occurs and not all
    // windows were being closed.
    MdiCubeViewport *d;
    QVector< Qisis::MdiCubeViewport * > tempList(*cubeViewportList());
    for(int i = 0; i < (int)tempList.size(); i++) {
      d = tempList.at(i);
      //Set the current viewport to the one being closed
      setCubeViewport(d);

      //If the user cancels the close operation, delete any viewports
      //that WERE closed and set the viewportlist to the temp list and return
      if(!d->parentWidget()->close()) {
        //tempList.erase(tempList.begin(), tempList.begin() + i);
        //cubeViewportList()->assign(tempList.begin(), tempList.end());
        return false;
      }
    }
    return true;
  }

  /**
   * Exit the program, this slot called when the exit is chosen from the File menu
   *
   * @internal
   * @history  2007-02-13  Tracie Sucharski,  Close all cubes before exiting
   */
  void FileTool::exit() {
    if(closeAll()) {
      /*This is OK to cast the p_parent because we know it's sub-subclassed from
       *Qisis::MainWindow and we know that Qisis::MainWindow has a close method*/
      ((MainWindow *)p_parent)->close();
      qApp->quit();
    }
  }

  /**
   * This slot enables or disables save and save as.
   *
   * @param enable
   */
  void FileTool::enableSave(bool enable) {
    p_save->setEnabled(enable);
  }

  /**
   * This method is called when the tool is updated.
   *
   */
  void FileTool::updateTool() {
    if(cubeViewport() == NULL) {
      if(p_lastViewport != NULL) {
        p_lastViewport = NULL;
      }
      p_print->setEnabled(false);
      p_save->setEnabled(false);
      p_exportView->setEnabled(false);
    }
    else {
      if(p_lastViewport == NULL) {
        //Set the last viewport to the current viewport and connect signals to save and discard
        p_lastViewport = cubeViewport();
        connect(p_lastViewport, SIGNAL(saveChanges()), this, SLOT(save()));
        connect(p_lastViewport, SIGNAL(discardChanges()), this, SLOT(discard()));
      }
      else {
        if(p_lastViewport != cubeViewport()) {
          //If the viewport has changes made to it enable the save action
          if(cubeViewport()->windowTitle().endsWith("*")) {
            p_save->setEnabled(true);
          }
          //Else disable it
          else {
            p_save->setEnabled(false);
          }
          //disconnect signals from the old viewport and connect them to the new viewport
          disconnect(p_lastViewport, SIGNAL(saveChanges()), this, SLOT(save()));
          disconnect(p_lastViewport, SIGNAL(discardChanges()), this, SLOT(discard()));
          p_lastViewport = cubeViewport();
          connect(p_lastViewport, SIGNAL(saveChanges()), this, SLOT(save()));
          connect(p_lastViewport, SIGNAL(discardChanges()), this, SLOT(discard()));
        }
      }
      p_print->setEnabled(true);
      p_exportView->setEnabled(true);
    }
  }
}
