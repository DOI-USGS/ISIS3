#include "Workspace.h"

#include <QProgressBar>
#include <QVBoxLayout>
#include <QVector>
#include <QWeakPointer>

#include "Cube.h"
#include "CubeAttribute.h"
#include "iString.h"
#include "MdiCubeViewport.h"
#include "ViewportMdiSubWindow.h"

namespace Isis {

  /**
   * Workspace constructor
   *
   * @param parent
   */
  Workspace::Workspace(QWidget *parent) : QMdiArea(parent) {
    p_cubeViewportList = NULL;
    p_cubeViewportList = new QVector< MdiCubeViewport * >;

    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow *)),
            this, SLOT(activateViewport(QMdiSubWindow *)));
    setActivationOrder(ActivationHistoryOrder);
  }


  Workspace::Workspace(const Workspace &other) : p_cubeViewportList(NULL) {
    p_cubeViewportList =
      new QVector< MdiCubeViewport * >(*other.p_cubeViewportList);
  }


  Workspace::~Workspace() {
    if(p_cubeViewportList) {
      delete p_cubeViewportList;
      p_cubeViewportList = NULL;
    }
  }


  /**
   * This gets called when a window is activated or the workspace loses focus.
   *
   * @param w
   */
  void Workspace::activateViewport(QMdiSubWindow *w) {
    if(w) {
      emit cubeViewportActivated((MdiCubeViewport *) w->widget()->layout()->itemAt(0)->widget());
    }
    //Check if there is no current window (on close)
    else if(!currentSubWindow()) {
      emit cubeViewportActivated((MdiCubeViewport *)NULL);
    }
  }

  /**
   * Repopulates the list of MdiCubeViewports and returns a pointer to this list.
   * Ownership is not given to the caller.
   *
   * @return std::vector<MdiCubeViewport*>*
   */
  QVector< MdiCubeViewport * > * Workspace::cubeViewportList() {
    p_cubeViewportList->clear();

    for(int i = 0; i < subWindowList().size(); i++) {
      p_cubeViewportList->push_back((MdiCubeViewport *)
                                    subWindowList()[i]->widget()->layout()->itemAt(0)->widget());
    }

    return p_cubeViewportList;
  }

  bool Workspace::confirmClose() {
    QVector< MdiCubeViewport * > viewports = *cubeViewportList();

    bool confirmed = true;

    for (int viewportIndex = 0; confirmed && viewportIndex < viewports.count(); viewportIndex++) {
      confirmed = viewports[viewportIndex]->confirmClose();
    }

    return confirmed;
  }

  /**
   * Add a cubeViewport to the workspace, open the cube.
   *
   * @param cubename[in]  (QString)  cube name
   *
   * @internal
   *  @history 2006-06-12 Tracie Sucharski,  Clear errors after catching when
   *                           attempting to open cube.
   *  @history 2007-02-13 Tracie Sucharski,  Open cube read, not read/write.
   *                          Opening read/write was done to accomodate the
   *                          EditTool. EditTool will now reopen the cube
   *                          read/write.
   *  @history 2008-05-27 Noah Hilt, When opening a cube now if a
   *           user specifies extra arguments to the cube name,
   *           the cube will be opened using a CubeAttributeInput
   *           specifically for the number and index of bands to
   *           be opened. Additionally, if a cube is opened with 3
   *           bands it will be opened in RGB mode with red,
   *           green, and blue set to the 3 bands respectively.
   *  @history 2008-12-04 Jeannie Walldren - Removed "delete cube"
   *           since this was causing a segfault and this
   *           deallocation is already taking place in
   *           addCubeViewport(cube).
   *
   */
  void Workspace::addCubeViewport(QString cubename) {
    Cube *cube = new Cube;

    //Read in the CubeAttribueInput from the cube name
    CubeAttributeInput inAtt(cubename.toStdString());
    std::vector<std::string> bands = inAtt.Bands();

    //Set the virtual bands to the bands specified by the input
    cube->setVirtualBands(bands);
    cube->open(cubename.toStdString());

    MdiCubeViewport *cvp = addCubeViewport(cube);

    // Check for RGB format (#R,#G,#B)
    if(bands.size() == 3) {
      iString st = iString(bands.at(0));
      int index_red = st.ToInteger();
      st = iString(bands.at(1));
      int index_green = st.ToInteger();
      st = iString(bands.at(2));
      int index_blue = st.ToInteger();
      cvp->viewRGB(index_red, index_green, index_blue);
    }
  }

  /**
   * Add a cubeViewport to the workspace.
   *
   * @param cube[in]  (Cube *)  cube information
   *
   * @internal
   *
   * @history  2007-04-13  Tracie Sucharski - Load entire image instead of
   *                           fitting smallest dimension.
   * @history  2008-05-27  Noah Hilt - Now returns a MdiCubeViewport
   *           in order for the addCubeViewport(QString cubename)
   *           method to modify the MdiCubeViewport.
   * @history 2008-08-20 Stacy Alley - Changed the setScale call
   *          to match the zoomTool's fit in view
   * @history 2008-12-04 Jeannie Walldren - Added try/catch to
   *          close the MdiCubeViewport if showCube() is not
   *          successful.
   */
  MdiCubeViewport *Workspace::addCubeViewport(Cube *cube) {
    MdiCubeViewport *result = NULL;

    try {
      ViewportMdiSubWindow *window(new ViewportMdiSubWindow(cube));
      window->setAttribute(Qt::WA_DeleteOnClose);

      addSubWindow(window);

      window->show();

      result = window->viewport();
      emit cubeViewportAdded(result);
    }
    catch(IException &e) {
      throw IException(e,
                       IException::Programmer,
                       tr("Error when attempting to show cube [%1]")
                         .arg(cube->getFileName().ToQt()),
                       _FILEINFO_);
    }

    return result;
  }

  void Workspace::addBrowseView(QString cubename) {
    /* Close the last browse window if necessary.  */
    if (subWindowList().size()) {
      QWeakPointer<QMdiSubWindow> windowToRemove =
          subWindowList()[subWindowList().size() - 1];

      removeSubWindow(windowToRemove.data());

      delete windowToRemove.data();
    }

    addCubeViewport(cubename);
  }


  const Workspace &Workspace::operator=(Workspace other) {
    delete p_cubeViewportList;
    p_cubeViewportList = NULL;

    p_cubeViewportList = new QVector< MdiCubeViewport * >;
    *p_cubeViewportList = *other.p_cubeViewportList;
    return *this;
  }
}
