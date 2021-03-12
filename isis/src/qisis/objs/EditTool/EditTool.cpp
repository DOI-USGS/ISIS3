/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "EditTool.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLine>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QStackedWidget>
#include <QToolButton>

#include "Brick.h"
#include "Cube.h"
#include "MdiCubeViewport.h"
#include "RubberBandTool.h"
#include "SpecialPixel.h"
#include "ToolPad.h"
#include "Workspace.h"


namespace Isis {
  /**
   * Constructs and EditTool object.
   *
   * @param parent  Parent widget
   *
   *
   */
  EditTool::EditTool(QWidget *parent) : Tool(parent) {
    p_dn = Null;
  }

  void EditTool::addTo(Workspace *workspace) {
    Tool::addTo(workspace);

    connect(workspace, SIGNAL(cubeViewportAdded(MdiCubeViewport *)),
            this, SLOT(listenToViewport(MdiCubeViewport *)));
  }

  /**
   * Adds the EditTool to the tool pad.
   *
   * @param pad input  The tool pad that EditTool is to be added to
   *
   * @return QAction*
   */
  QAction *EditTool::toolPadAction(ToolPad *pad) {
    QAction *action = new QAction(pad);
    action->setIcon(QPixmap(toolIconDir() + "/color_line.png"));
    action->setToolTip("Image Edit (E)");
    action->setShortcut(Qt::Key_E);

    QString text  =
      "<b>Function:</b>  Edit active viewport \
      <p><b>Shortcut:</b> E</p> ";
    action->setWhatsThis(text);

    return action;
  }

  /**
   * Creates the toolbar containing the edit tool widgets
   *
   * @param active  input  The widget that will contain the edit tool
   *                       specific widgets
   *
   * @history  2007-08-31 Tracie Sucharski - Changed signal on dn line edit
   *                             from returnPressed to editingFinished.
   *
   * @return QWidget*
   */
  QWidget *EditTool::createToolBarWidget(QStackedWidget *active) {
    QWidget *container = new QWidget(active);
    container->setObjectName("EditToolActiveToolBarWidget");

    p_shapeComboBox = new QComboBox;
    p_shapeComboBox->setEditable(false);
    p_shapeComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    p_shapeComboBox->addItem("Point");
    p_shapeComboBox->addItem("Horizontal Line");
    p_shapeComboBox->addItem("Vertical Line");
    p_shapeComboBox->addItem("Start/End Line");
    p_shapeComboBox->addItem("Rectangle");
    p_shapeComboBox->setToolTip("Select shape to edit");
    QString text =
      "<b>Function:</b> The shape in the image that will be replaced with \
      a new value.  If Horizontal line is chosen, clicking anywhere on the \
      image will cause all samples on that line of the cube to be replaced \
      with the replacement value.  If Vertical Line is chosen, a v ...";
    p_shapeComboBox->setWhatsThis(text);
    p_shapeComboBox->setCurrentIndex(1);
    connect(p_shapeComboBox, SIGNAL(activated(int)), this, SLOT(enableRubberBandTool()));

    p_valTypeComboBox = new QComboBox;
    p_valTypeComboBox->setEditable(false);
    p_valTypeComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    p_valTypeComboBox->addItem("Dn", UserDnComboValue);
    p_valTypeComboBox->addItem("Null", NullComboValue);
    p_valTypeComboBox->addItem("Hrs", HrsComboValue);
    p_valTypeComboBox->addItem("Lrs", LrsComboValue);
    p_valTypeComboBox->addItem("His", HisComboValue);
    p_valTypeComboBox->addItem("Lis", LisComboValue);
    p_valTypeComboBox->setToolTip("Value used to replace image data");
    text =
      "<b>Function:</b> The value which will be used to replace image data. ";
    p_valTypeComboBox->setWhatsThis(text);
    p_valTypeComboBox->setCurrentIndex(
        p_valTypeComboBox->findData(NullComboValue));
    connect(p_valTypeComboBox, SIGNAL(activated(int)), this,
            SLOT(selectValType(int)));

    p_dnLineEdit = new QLineEdit;
    p_dnLineEdit->setToolTip("Dn value");
    text =
      "<b>Function:</b> This is the dn used to replace image data";
    p_dnLineEdit->setWhatsThis(text);
    p_dnLineEdit->setEnabled(false);
    connect(p_dnLineEdit, SIGNAL(editingFinished()), this, SLOT(changeDn()));

    p_undoButton = new QToolButton;
    p_undoButton->setIcon(QPixmap(toolIconDir() + "/undo.png"));
    p_undoButton->setToolTip("Undo");
    text =
      "<b>Function:</b> Undo last edit operation";
    p_undoButton->setWhatsThis(text);
    connect(p_undoButton, SIGNAL(clicked()), this, SLOT(undoEdit()));
    p_undoButton->setAutoRaise(true);
    p_undoButton->setIconSize(QSize(22, 22));

    p_redoButton = new QToolButton;
    p_redoButton->setIcon(QPixmap(toolIconDir() + "/redo.png"));
    p_redoButton->setToolTip("Redo");
    text =
      "<b>Function:</b> Redo last undo operation";
    p_redoButton->setWhatsThis(text);
    p_redoButton->setEnabled(false);
    connect(p_redoButton, SIGNAL(clicked()), this, SLOT(redoEdit()));
    p_redoButton->setAutoRaise(true);
    p_redoButton->setIconSize(QSize(22, 22));

    p_saveButton = new QToolButton;
    p_saveButton->setIcon(QPixmap(toolIconDir() + "/filesave.png"));
    p_saveButton->setToolTip("Save");
    text =
      "<b>Function:</b> Save any changes made, these changes are finalized";
    p_saveButton->setWhatsThis(text);
    p_saveButton->setEnabled(false);
    connect(p_saveButton, SIGNAL(clicked()), this, SIGNAL(save()));
    p_saveButton->setAutoRaise(true);
    p_saveButton->setIconSize(QSize(22, 22));

    p_saveAsButton = new QToolButton;
    p_saveAsButton->setIcon(QPixmap(toolIconDir() + "/filesaveas.png"));
    p_saveAsButton->setToolTip("Save As");
    text =
      "<b>Function:</b> Save any changes made to the file specified, these changes are finalized";
    p_saveAsButton->setWhatsThis(text);
    connect(p_saveAsButton, SIGNAL(clicked()), this, SIGNAL(saveAs()));
    p_saveAsButton->setAutoRaise(true);
    p_saveAsButton->setIconSize(QSize(22, 22));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(p_shapeComboBox);
    layout->addWidget(p_valTypeComboBox);
    layout->addWidget(p_dnLineEdit);
    layout->addWidget(p_undoButton);
    layout->addWidget(p_redoButton);
    layout->addWidget(p_saveButton);
    layout->addWidget(p_saveAsButton);
    layout->addStretch(1);
    container->setLayout(layout);

m_container = container;
    return container;
  }

  /**
   * This is a private slot which is called when the user selects a new dn
   * type
   *
   * @param index  input This is the index selected in the valType combo box
   *
   */
  void EditTool::selectValType(int index) {
    if (p_valTypeComboBox->itemData(index) == UserDnComboValue) {
      p_dnLineEdit->setEnabled(true);
    }
    else {
      p_dnLineEdit->setEnabled(false);

      if (p_valTypeComboBox->itemData(index) == NullComboValue) p_dn = Null;
      if (p_valTypeComboBox->itemData(index) == HrsComboValue) p_dn = Hrs;
      if (p_valTypeComboBox->itemData(index) == LrsComboValue) p_dn = Lrs;
      if (p_valTypeComboBox->itemData(index) == HisComboValue) p_dn = His;
      if (p_valTypeComboBox->itemData(index) == LisComboValue) p_dn = Lis;
    }
  }

  /**
   * This is a private slot called when the user hits the enter key after
   * typing a value in the dnLineEdit field.
   *
   */
  void EditTool::changeDn() {
    p_dn = p_dnLineEdit->text().toDouble();
  }

  /**
   * This is a virtual function belonging to the Tool class which is called
   * when the user selects a different viewport.  This sets up the
   * signal/slot for destruction of a viewport so the viewport is removed
   * from the undoEdit hash.
   *
   * @internal
   */
  void EditTool::updateTool() {
    MdiCubeViewport *vp = cubeViewport();

    if (vp != NULL) {
      //If the current viewport has no undo history
      if (!p_undoEdit.contains(cubeViewport()) || p_undoEdit.value(vp)->empty()) {
        p_undoButton->setEnabled(false);
        p_saveButton->setEnabled(false);
      }
      //Otherwise enable the undo button and save button
      else {
        p_undoButton->setEnabled(true);
        p_saveButton->setEnabled(true);
      }
      //If the current viewport has no redo history
      if (!p_redoEdit.contains(cubeViewport()) || p_redoEdit.value(vp)->empty()) {
        p_redoButton->setEnabled(false);
      }
      //Otherwise enable the redo button and save button
      else {
        p_redoButton->setEnabled(true);
        p_saveButton->setEnabled(true);
      }
    }
  }

  /**
   * This method is called any time the RubberBandTool is
   * complete. It checks if the viewport is writable, checks which
   * mode it is in, either line or rectangle, and if
   * RubberBandTool is valid. It then writes the data from the
   * RubberBandTool to the cube.
   *
   */
  void EditTool::rubberBandComplete() {
    MdiCubeViewport *vp = cubeViewport();
    if (vp == NULL) return;

    if (vp->cube()->isReadOnly()) {

      QString fileName = vp->cube()->fileName();
      //  ReOpen cube as read/write
      //  If cube readonly print error
      try {
        vp->cube()->reopen("rw");
      }
      catch(IException &) {
        vp->cube()->open(fileName, "r");
        QMessageBox::information((QWidget *)parent(), "Error", "Cannot open cube read/write");
        return;
      }
    }
    if (vp->isColor()) {
      QMessageBox::information((QWidget *)parent(),
                               "Error", "Cannot edit in color mode");
      return;
    }

    int issamp, isline, iesamp, ieline;
    double ssamp, sline, esamp, eline;
    QList<QPoint *> *linePts = NULL;

    // Rectangle is selected
    if (p_shapeComboBox->currentIndex() == Rectangle) {

      if (!rubberBandTool()->isValid()) return;

      QRect r = rubberBandTool()->rectangle();
      if ((r.width() < 1) || (r.height() < 1)) return;

      vp->viewportToCube(r.left(), r.top(), ssamp, sline);
      vp->viewportToCube(r.right(), r.bottom(), esamp, eline);

      issamp = (int)(ssamp + 0.5);
      isline = (int)(sline + 0.5);
      iesamp = (int)(esamp + 0.5);
      ieline = (int)(eline + 0.5);

      //Clamp the rectangles coordinates to within the cube's dimensions
      if (issamp < 0) issamp = 0;
      if (iesamp < 0) iesamp = 0;
      if (isline < 0) isline = 0;
      if (ieline < 0) ieline = 0;

      if (issamp > vp->cubeSamples()) issamp = vp->cubeSamples();
      if (iesamp > vp->cubeSamples()) iesamp = vp->cubeSamples();
      if (isline > vp->cubeLines()) isline = vp->cubeLines();
      if (ieline > vp->cubeLines()) ieline = vp->cubeLines();

      //If the rectangle is completely out of bounds on either side, display an error and return
      if (issamp > iesamp || isline > ieline) {
        QMessageBox::information((QWidget *)parent(),
                                 "Error", "Rectangle is out of bounds");
        return;
      }
    }
    // Line is selected
    else if (p_shapeComboBox->currentIndex() == StartEndLine) {
      //  Convert rubber band line to cube coordinates
      if (!rubberBandTool()->isValid()) return;
      vp->viewportToCube(rubberBandTool()->vertices()[0].rx(), rubberBandTool()->vertices()[0].ry(),
                         ssamp, sline);
      vp->viewportToCube(rubberBandTool()->vertices()[1].rx(), rubberBandTool()->vertices()[1].ry(),
                         esamp, eline);

      QLine l((int)ssamp, (int)sline, (int)esamp, (int)eline);

      linePts = EditTool::LineToPoints(l);

      //If the line is completely out of bounds, display an error and return
      if (linePts->empty()) {
        QMessageBox::information((QWidget *)parent(),
                                 "Error", "No points in edit line");
        return;
      }

      //  Find bounding rectangle for the line
      issamp = std::min(linePts->front()->x(), linePts->back()->x());
      isline = std::min(linePts->front()->y(), linePts->back()->y());
      iesamp = std::max(linePts->front()->x(), linePts->back()->x());
      ieline = std::max(linePts->front()->y(), linePts->back()->y());
    }
    // Neither mode is selected, so this is an incorrect mode for this RubberBandTool
    else {
      return;
    }

    // Write the data to the cube
    writeToCube(iesamp, issamp, ieline, isline, linePts);
    if (linePts) delete linePts;
  }

  void EditTool::listenToViewport(MdiCubeViewport *newViewport) {
    connect(newViewport, SIGNAL(saveChanges(CubeViewport *)),
            this, SLOT(save(CubeViewport *)));
    connect(newViewport, SIGNAL(discardChanges(CubeViewport *)),
            this, SLOT(undoAll(CubeViewport *)));
    connect(newViewport, SIGNAL(destroyed(QObject *)),
            this, SLOT(removeViewport(QObject *)));
  }


  /**
   * This is a slot called when any mouse button is released inside of a
   * viewport.
   *
   * @param p  input  The point under the cursor
   *
   * @param m  input  Mouse button released (left,middle,right)
   *
   * @history  2007-03-02  Tracie Sucharski - Reopen cube read/write
   *                             Workspace::addCubeViewport was opening cube
   *                             read/write, changed to open read only so that
   *                             the cube date was not changed unless EditTool
   *                             is invoked.
   * @history  2007-02-26  Tracie Sucharski - Remove test for off image editing.
   *                             It is no longer needed since the bug in the
   *                             Cube class is fixed.
   * @history  2008-05-23  Noah Hilt - Removed the rectangle and
   *                             start/end line functionality from
   *                             the mouseButtonRelease and moved
   *                             it to the rubberBandComplete
   *                             method.
   * @history  2017-08-11 Adam Goins - Added a "vp->cube()->open("r")" call to reopen the cube with
   *                               "r" permissions if the attempted "rw" permission didn't succeed.
   *                               Fixes segfault issue if editing is attempted on cube without "w" access.
   *                               (ref # 2097)
   */
  void EditTool::mouseButtonRelease(QPoint p, Qt::MouseButton m) {
    MdiCubeViewport *vp = cubeViewport();
    if (vp == NULL) return;
    if (p_valTypeComboBox->itemData(p_valTypeComboBox->currentIndex()) ==
        UserDnComboValue && p_dnLineEdit->text() == "" && m != Qt::RightButton)
      return;

    //  If cube readonly try to open read/write
    if (vp->cube()->isReadOnly()) {

      // Get cube filename to recreate it with "r" privileges if the "rw" access fails.
      QString fileName = vp->cube()->fileName();

      //  ReOpen cube as read/write
      //  If cube readonly print error
      try {
        vp->cube()->reopen("rw");
      }
      catch(IException &) {
        vp->cube()->open(fileName, "r");
        QMessageBox::information((QWidget *)parent(), "Error", "Cannot open cube read/write");
        return;
      }
    }
    if (vp->isColor()) {
      QMessageBox::information((QWidget *)parent(),
                               "Error", "Cannot edit in color mode");
      return;
    }

    int issamp, isline, iesamp, ieline;
    double ssamp, sline;

    //  If right mouse button, pick up dn value under the cursor which will be
    //  used as the edit value.
    if (m == Qt::RightButton &&
        p_valTypeComboBox->itemData(p_valTypeComboBox->currentIndex()) ==
        UserDnComboValue) {
      vp->viewportToCube(p.x(), p.y(), ssamp, sline);
      issamp = (int)(ssamp + 0.5);
      isline = (int)(sline + 0.5);
      Brick *pntBrick = new Brick(1, 1, 1,
                                              vp->cube()->pixelType());
      pntBrick->SetBasePosition(issamp, isline, vp->grayBand());
      vp->cube()->read(*pntBrick);
      p_dn = (*pntBrick)[0];
      p_dnLineEdit->setText(QString::number(p_dn));
      delete pntBrick;
      return;
    }

    else if (p_shapeComboBox->currentIndex() == Point ||
        p_shapeComboBox->currentIndex() == HorizLine ||
        p_shapeComboBox->currentIndex() == VertLine) {
      vp->viewportToCube(p.x(), p.y(), ssamp, sline);
      if ((ssamp < 0.5) || (sline < 0.5) ||
          (ssamp > vp->cubeSamples() + 0.5) || (sline > vp->cubeLines() + 0.5)) {
        QApplication::beep();
        return;
      }
      issamp = (int)(ssamp + 0.5);
      isline = (int)(sline + 0.5);
      iesamp = issamp;
      ieline = isline;
      if (p_shapeComboBox->currentIndex() == HorizLine) {
        issamp = 1;
        iesamp = vp->cube()->sampleCount();
      }
      if (p_shapeComboBox->currentIndex() == VertLine) {
        isline = 1;
        ieline = vp->cube()->lineCount();
      }
      // Write the changes to the cube.
      writeToCube(iesamp, issamp, ieline, isline, NULL);
    }
  }

  /**
   *
   *
   * @param iesamp   input
   * @param issamp   input
   * @param ieline   input
   * @param isline   input
   * @param linePts  input   If the input data is a line, this
   *                         will be the list of points in the
   *                         line.
   */
  void EditTool::writeToCube(int iesamp, int issamp, int ieline, int isline, QList<QPoint *> *linePts) {
    Brick *brick = NULL;
    try {
      MdiCubeViewport *vp = cubeViewport();

      int nsamps = iesamp - issamp + 1;
      int nlines = ieline - isline + 1;

      brick = new Brick(nsamps, nlines, 1, vp->cube()->pixelType());
      brick->SetBasePosition(issamp, isline, vp->grayBand());
      vp->cube()->read(*brick);

      //  Save for Undo operation,  See if viewport has undo entry.  If it
      //  does, get Stack, push new undo and put stack back in hash.  If not,
      //  create stack, push new undo and add to hash.
      QStack<Brick *> *s;
      if (p_undoEdit.contains(vp)) {
        s = p_undoEdit.value(vp);
      }
      else {
        s = new QStack<Brick *>;
      }
      s->push(brick);
      p_undoEdit[vp] = s;

      //Remove any redo stack if it exists
      if (p_redoEdit.contains(vp)) {
        QStack<Brick *> *temp = p_redoEdit.value(vp);
        while(!temp->isEmpty()) {
          delete temp->pop();
        }
        p_redoEdit.remove(vp);
        p_redoButton->setEnabled(false);
      }

      //  no deep copy constructor so re-read brick for editing....
      brick = new Brick(nsamps, nlines, 1, vp->cube()->pixelType());
      brick->SetBasePosition(issamp, isline, vp->grayBand());
      vp->cube()->read(*brick);

      //  Now that we have where, do the actual edits
      if (p_shapeComboBox->currentIndex() == StartEndLine) {
        for(int i = 0; linePts && i < (int)linePts->size(); i++) {
          QPoint *pt = (*linePts)[i];
          int is = pt->x();
          int il = pt->y();
          int brickIndex = (il - isline) * nsamps + (is - issamp);
          (*brick)[brickIndex] = (double)p_dn;
        }
      }
      else {
        for(int i = 0; i < brick->size(); i++)(*brick)[i] = (double)p_dn;
      }

      //Signal that this cube has been changed, enable the undo and save buttons
      emit cubeChanged(true);
      p_undoButton->setEnabled(true);
      p_saveButton->setEnabled(true);
      vp->cube()->write(*brick);
      vp->cubeChanged(true);
      vp->setCaption();

      //      QRect r(issamp,isline,nsamps,nlines);
      QRect r(brick->Sample(), brick->Line(),
              brick->SampleDimension(), brick->LineDimension());
      vp->cubeContentsChanged(r);
      delete brick;
    }
    catch(...) {
      if (brick) {
        delete brick;
      }
      //If the brick failed to initialize due to insufficient memory
      QMessageBox::information((QWidget *)parent(), "Error", "Not enough memory to complete this operation.");
      return;
    }
  }

  /**
   *  This is a private slot called when the user selects the undo button.
   *  With each call, another edit is reversed.
   *
   */
  void EditTool::undoEdit() {
    Brick *redoBrick = NULL;
    try {
      MdiCubeViewport *vp = cubeViewport();
      if (vp == NULL) return;

      //  If viewport not in hash, beep
      if (p_undoEdit.empty() || p_undoEdit.count(vp) == 0) {
        QApplication::beep();
        return;
      }

      //  If cube readonly print error
      if (vp->cube()->isReadOnly()) {
        QMessageBox::information((QWidget *)parent(), "Error", "Cube is Read Only");
        return;
      }


      QStack<Brick *> *s = p_undoEdit.value(vp);
      Brick *brick = s->top();

      //Write the current cube to the a brick and add it to the redo stack
      redoBrick = new Brick(brick->SampleDimension(), brick->LineDimension(), 1, vp->cube()->pixelType());
      redoBrick->SetBasePosition(brick->Sample(), brick->Line(), vp->grayBand());
      vp->cube()->read(*(redoBrick));

      QStack<Brick *> *redo;
      if (p_redoEdit.contains(vp)) {
        redo = p_redoEdit.value(vp);
      }
      else {
        redo = new QStack<Brick *>;
      }
      redo->push(redoBrick);
      p_redoEdit[vp] = redo;

      //  Write the saved brick to the cube
      vp->cube()->write(*(brick));

      //  Update the viewport
      QRect r(brick->Sample(), brick->Line(),
              brick->SampleDimension(), brick->LineDimension());
      vp->cubeContentsChanged(r);

      //Enable the redo button since we have made an undo
      p_redoButton->setEnabled(true);
      p_saveButton->setEnabled(true);
      emit cubeChanged(true);
      vp->cubeChanged(true);
      vp->setCaption();

      //Pop this element off the stack, if the undo stack is empty, disable the undo button
      s->pop();
      if (s->empty()) {
        p_undoButton->setEnabled(false);
      }
      delete brick;
    }
    catch(...) {
      if (redoBrick) {
        delete redoBrick;
      }
      //If the brick failed to initialize due to insufficient memory
      QMessageBox::information((QWidget *)parent(), "Error", "Not enough memory to complete this operation.");
      return;
    }
  }

  /**
   * This method is used to discard any changes made to this viewport. If the
   * viewport has been saved, then it will only discard changes to that save
   * point.
   *
   * @param vp
   */
  void EditTool::undoAll(CubeViewport *vp) {
    try {
      if (vp != NULL) {
        //  If cube readonly print error
        if (vp->cube()->isReadOnly()) {
          QMessageBox::information((QWidget *)parent(), "Error", "Cube is Read Only");
          return;
        }

        QStack<Brick *> *undo;
        QStack<Brick *> *redo;
        int marker = 0;
        //If edits have been made
        if (p_undoEdit.contains(vp)) {
          undo = p_undoEdit.value(vp);

          //If a save has been made
          if (p_saveMarker.contains(vp)) {
            marker = p_saveMarker.value(vp);
          }

          //Undo up to the save point
          for(int i = undo->count() - 1; i >= marker; i--) {
            Brick *brick = undo->at(i);
            //  Write the saved brick to the cube
            vp->cube()->write(*(brick));
          }

          //If undos have been made past the save point, we need to redo them
          //Set the marker to the correct index
          marker = marker - undo->count();
        }

        //If undos have been made
        if (p_redoEdit.contains(vp)) {
          redo = p_redoEdit.value(vp);

          //If undos have been made past a save point, we need to redo them
          for(int i = redo->count() - 1; i >= redo->count() - marker; i--) {
            Brick *brick = redo->at(i);

            //  Write the saved brick to the cube
            vp->cube()->write(*(brick));
          }
        }
      }
    }
    catch(...) {
      //If the brick failed to initialize due to insufficient memory
      QMessageBox::information((QWidget *)parent(), "Error",
                               "Not enough memory to complete this operation.");
      return;
    }
  }

  /**
   * This method is called to redo any edit operations that have been undone.
   *
   */
  void EditTool::redoEdit() {
    Brick *undoBrick = NULL;
    try {
      MdiCubeViewport *vp = cubeViewport();
      if (vp == NULL) return;

      //  If viewport not in hash, beep
      if (p_redoEdit.empty() || p_redoEdit.count(vp) == 0) {
        QApplication::beep();
        return;
      }

      //  If cube readonly print error
      if (vp->cube()->isReadOnly()) {
        QMessageBox::information((QWidget *)parent(), "Error", "Cube is Read Only");
        return;
      }

      QStack<Brick *> *s = p_redoEdit.value(vp);
      Brick *brick = s->top();

      //Write the current cube to the a brick and add it to the undo stack
      undoBrick = new Brick(brick->SampleDimension(),
                                  brick->LineDimension(), 1,
                                  vp->cube()->pixelType());
      undoBrick->SetBasePosition(brick->Sample(), brick->Line(), vp->grayBand());
      vp->cube()->read(*(undoBrick));

      QStack<Brick *> *undo;
      if (p_undoEdit.contains(vp)) {
        undo = p_undoEdit.value(vp);
      }
      else {
        undo = new QStack<Brick *>;
      }
      undo->push(undoBrick);
      p_undoEdit[vp] = undo;

      //  Write the saved brick to the cube
      vp->cube()->write(*(brick));
      //  Update the viewport

      QRect r(brick->Sample(), brick->Line(),
              brick->SampleDimension(), brick->LineDimension());
      vp->cubeContentsChanged(r);

      p_undoButton->setEnabled(true);
      p_saveButton->setEnabled(true);
      vp->cubeChanged(true);
      vp->setCaption();
      emit cubeChanged(true);

      //Pop this element off the stack, if the redo stack is empty, disable the redo button
      s->pop();
      if (s->empty()) {
        p_redoButton->setEnabled(false);
      }
      delete brick;
    }
    catch(...) {
      if (undoBrick) {
        delete undoBrick;
      }
      //If the brick failed to initialize due to insufficient memory
      QMessageBox::information((QWidget *)parent(), "Error", "Not enough memory to complete this operation.");
      return;
    }
  }

  /**
   * This method saves by removing any undo history for the
   * viewport vp and reopening the cube. These changes are
   * finalized! There is no undoing after a save has been made.
   *
   * @param vp
   */
  void EditTool::save(CubeViewport *vp) {
    if (vp != NULL) {
      //Set the 'save point' for this viewport, if we undo discard any changes
      //We will only discard to this point
      int marker;
      if (p_undoEdit.contains(vp)) {
        marker = p_undoEdit.value(vp)->count();
      }
      else {
        marker = 0;
      }
      p_saveMarker[vp] = marker;

      p_saveButton->setEnabled(false);
      vp->cubeChanged(false);
      vp->setCaption();
    }
  }


  /**
   *  This is a private slot called to clean up when a viewport is destroyed.
   *  This viewport is removed from the undoEdit hash.
   *
   * @param vp  input  Viewport to be removed from the undo edit hash
   *
   */
  void EditTool::removeViewport(QObject *vp) {
    //  Connect destruction of CubeViewport to Remove slot to remove viewport
    //  from the undo and redo Hashes.
    if (p_undoEdit.contains((MdiCubeViewport *) vp)) {
      QStack<Brick *> *temp = p_undoEdit.value((MdiCubeViewport *) vp);
      while(!temp->isEmpty()) {
        delete temp->pop();
      }
      p_undoEdit.remove((MdiCubeViewport *) vp);
    }
    if (p_redoEdit.contains((MdiCubeViewport *) vp)) {
      QStack<Brick *> *temp = p_redoEdit.value((MdiCubeViewport *) vp);
      while(!temp->isEmpty()) {
        delete temp->pop();
      }
      p_redoEdit.remove((MdiCubeViewport *) vp);
    }
  }




  /**
   *   Convert rubber band line to points
   *
   *   This routine takes two points (sx,sy) and (ex,ey) and determines all
   *   the points which make up that line segment. This is useful
   *   for drawing graphic on a display or image
   *
   * @author  Jan 1 1996 Jeff Anderson, Original version
   *
   * @param line  input  Line to be converted to points
   *
   * @return QList<QPoint*>*  A pointer to a QList of points representing
   *                          line
   */
  QList<QPoint *> *EditTool::LineToPoints(const QLine &line) {
    MdiCubeViewport *vp = cubeViewport();
    if (vp == NULL) return new QList<QPoint *>();
    double slope;
    int i;
    int x, y, xinc, yinc;
    int xsize, ysize;


    QList<QPoint *> *points = new QList<QPoint *>;

    int sx = line.p1().x();
    int ex = line.p2().x();
    int sy = line.p1().y();
    int ey = line.p2().y();
    if (sx > ex) {
      xsize = sx - ex + 1;
      xinc = -1;
    }
    else {
      xsize = ex - sx + 1;
      xinc = 1;
    }

    if (sy > ey) {
      ysize = sy - ey + 1;
      yinc = -1;
    }
    else {
      ysize = ey - sy + 1;
      yinc = 1;
    }

    if (ysize > xsize) {
      slope = (double)(ex - sx) / (double)(ey - sy);
      y = sy;
      for(i = 0; i < ysize; i++) {
        x = (int)(slope * (double)(y - sy) + (double) sx + 0.5);

        //If the x or y coordinates are not within the cube dimensions, don't add them
        if (x >= 0 && y >= 0 && x <= vp->cubeSamples() && y <= vp->cubeLines()) {
          QPoint *pt = new QPoint;
          pt->setX(x);
          pt->setY(y);
          points->push_back(pt);
        }
        y += yinc;
      }
    }
    else if (xsize == 1) {
      //If the x or y coordinates are not within the cube dimensions, don't add them
      if (sx >= 0 && sy >= 0 && sx <= vp->cubeSamples() && sy <= vp->cubeLines()) {
        QPoint *pt = new QPoint;
        pt->setX(sx);
        pt->setY(sy);
        points->push_back(pt);
      }
    }
    else {
      slope = (double)(ey - sy) / (double)(ex - sx);
      x = sx;
      for(i = 0; i < xsize; i++) {
        y = (int)(slope * (double)(x - sx) + (double) sy + 0.5);

        //If the x or y coordinates are not within the cube dimensions, don't add them
        if (x >= 0 && y >= 0 && x <= vp->cubeSamples() && y <= vp->cubeLines()) {
          QPoint *pt = new QPoint;
          pt->setX(x);
          pt->setY(y);
          points->push_back(pt);
        }
        x += xinc;
      }
    }

    return points;
  }

  /**
   *  This method sets up the RubberBandTool depending on which
   *  mode is enabled. If a valid RubberBandTool mode is not
   *  enabled the RubberBandTool is disabled.
   *
   */
  void EditTool::enableRubberBandTool() {
    int index = p_shapeComboBox->currentIndex();
    if (index == 3) {
      rubberBandTool()->enable(RubberBandTool::LineMode);
      rubberBandTool()->setDrawActiveViewportOnly(true);
    }
    else if (index == 4) {
      rubberBandTool()->enable(RubberBandTool::RectangleMode);
      rubberBandTool()->setDrawActiveViewportOnly(true);
    }
    else {
      rubberBandTool()->disable();
    }
  }
}
