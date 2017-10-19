#include "Workspace.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QString>
#include <QToolBar>
#include <QVBoxLayout>
#include <QVector>
#include <QWeakPointer>

#include "AdvancedTrackTool.h"
#include "BandTool.h"
#include "BlinkTool.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "EditTool.h"
#include "FeatureNomenclatureTool.h"
#include "FileName.h"
#include "FileTool.h"
#include "FindTool.h"
#include "HelpTool.h"
#include "HistogramTool.h"
#include "Image.h"
#include "ImageList.h"
#include "IString.h"
#include "MatchTool.h"
#include "MdiCubeViewport.h"
#include "MeasureTool.h"
#include "PanTool.h"
#include "RubberBandTool.h"
#include "QnetFileTool.h"
#include "QnetNavTool.h"
#include "QnetTool.h"
#include "ScatterPlotTool.h"
#include "SpatialPlotTool.h"
#include "SpecialPixelTool.h"
#include "SpectralPlotTool.h"
#include "StatisticsTool.h"
#include "StereoTool.h"
#include "StretchTool.h"
#include "SunShadowTool.h"
#include "TrackTool.h"
#include "ToolList.h"
#include "ToolPad.h"
#include "ViewportMdiSubWindow.h"
#include "WindowTool.h"
#include "ZoomTool.h"

namespace Isis {

  /**
   * Workspace constructor
   *
   * @param parent
   */
  Workspace::Workspace(bool selfContained, QWidget *parent) : QWidget(parent) {
    m_cubeViewportList = NULL;
    m_tools = NULL;

    m_cubeViewportList = new QVector< MdiCubeViewport * >;
    m_tools = new ToolList;

    m_mdi = new QMdiArea(this);

    QGridLayout *layout = new QGridLayout;
    setLayout(layout);

    if (!selfContained) {
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(m_mdi, 0, 0);
    }
    else {
      /*
       * Layout:
       *
       * ----- MENU ----------------------
       * -PERM TOOLBAR-ACTIVE TOOLBAR------
       * |                              |T|
       * |                              |O|
       * |                              |O|
       * |    MDI AREA                  |L|
       * |                              | |
       * |                              |L|
       * |                              |I|
       * |                              |S|
       * |                              |T|
       * ------------Status Bar------------
       *
       * The perm/active tool bar are in an hbox layout, the rest follows the grid.
       */
      int row = 0;

      QMenuBar *menuBar = new QMenuBar;
      layout->addWidget(menuBar, row, 0, 1, 2);
      row++;

      QHBoxLayout *permActiveToolBarLayout = new QHBoxLayout;

      QToolBar *permToolBar = new QToolBar("Standard Tools", this);
      permToolBar->setObjectName("permToolBar");
      permToolBar->setIconSize(QSize(22, 22));
      permActiveToolBarLayout->addWidget(permToolBar);

      QToolBar *activeToolBar = new QToolBar("Active Tool", this);
      activeToolBar->setObjectName("activeToolBar");
      activeToolBar->setIconSize(QSize(22, 22));
      permActiveToolBarLayout->addWidget(activeToolBar);

      layout->addLayout(permActiveToolBarLayout, row, 0, 1, 2);
      row++;

      layout->addWidget(m_mdi, row, 0, 1, 1);

      ToolPad *toolPad = new ToolPad("Tool Pad", this);
      toolPad->setObjectName("toolPad");
      toolPad->setOrientation(Qt::Vertical);
      layout->addWidget(toolPad, row, 1, 1, 1);
      row++;

      QStatusBar *statusBar = new QStatusBar(this);
      layout->addWidget(statusBar, row, 0, 1, 2);
      row++;

      // Create tools
      Tool *defaultActiveTool = NULL;

      m_tools->append(new RubberBandTool(this));
      QnetTool *qnetTool = new QnetTool(this);
      //m_tools->append(new FileTool(this));
      m_tools->append(new QnetFileTool(qnetTool, this));
      m_tools->append(NULL);
      m_tools->append(new BandTool(this));

      defaultActiveTool = new ZoomTool(this);
      m_tools->append(defaultActiveTool);

      m_tools->append(new PanTool(this));
      m_tools->append(new StretchTool(this));
      m_tools->append(new FindTool(this));
      m_tools->append(new BlinkTool(this));
      m_tools->append(new AdvancedTrackTool(this));
      m_tools->append(new EditTool(this));
      m_tools->append(new WindowTool(this));
      m_tools->append(new MeasureTool(this));
      m_tools->append(new SunShadowTool(this));
      m_tools->append(new FeatureNomenclatureTool(this));
      m_tools->append(new SpecialPixelTool(this));
      m_tools->append(new SpatialPlotTool(this));
      m_tools->append(new SpectralPlotTool(this));
      m_tools->append(new ScatterPlotTool(this));
      m_tools->append(new HistogramTool(this));
      m_tools->append(new StatisticsTool(this));
      m_tools->append(new StereoTool(this));
      m_tools->append(new MatchTool(this));
      m_tools->append(new HelpTool(this));
      m_tools->append(new TrackTool(statusBar));

      m_tools->append(new QnetNavTool(qnetTool, this));
      m_tools->append(qnetTool);

      QMap<QString, QMenu *> subMenus;

      for (int i = 0; i < m_tools->count(); i++) {
        Tool *tool = (*m_tools)[i];

        if (tool) {
          tool->addTo(this);
          tool->addToPermanent(permToolBar);
          tool->addToActive(activeToolBar);
          tool->addTo(toolPad);

          if (!tool->menuName().isEmpty()) {
            QString menuName = tool->menuName();

            QMenu *subMenu = subMenus[menuName];

            if (!subMenu) {
              subMenus[menuName] = menuBar->addMenu(menuName);
              subMenu = subMenus[menuName];
            }

            tool->addTo(subMenu);
          }
        }
        else {
          permToolBar->addSeparator();
        }
      }

      permToolBar->addSeparator();
      defaultActiveTool->activate(true);
    }

    connect(m_mdi, SIGNAL(subWindowActivated(QMdiSubWindow *)),
            this, SLOT(activateViewport(QMdiSubWindow *)));
    m_mdi->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  }


  Workspace::Workspace(const Workspace &other) : m_cubeViewportList(NULL) {
    m_cubeViewportList =
      new QVector< MdiCubeViewport * >(*other.m_cubeViewportList);
  }


  Workspace::~Workspace() {
    delete m_cubeViewportList;
    m_cubeViewportList = NULL;

    delete m_tools;
    m_tools = NULL;
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
    else if(!m_mdi->currentSubWindow()) {
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
    m_cubeViewportList->clear();

    for(int i = 0; i < m_mdi->subWindowList().size(); i++) {
      m_cubeViewportList->push_back(
          (MdiCubeViewport *)m_mdi->subWindowList()[i]->widget()->layout()->itemAt(0)->widget());
    }

    return m_cubeViewportList;
  }


  Workspace &Workspace::operator=(Workspace other) {
    delete m_cubeViewportList;
    m_cubeViewportList = NULL;

    m_cubeViewportList = new QVector< MdiCubeViewport * >;
    *m_cubeViewportList = *other.m_cubeViewportList;
    return *this;
  }


  void Workspace::addImages(ImageList *images) {
    foreach (Image *image, *images) {
      addCubeViewport(image->cube());
    }
  }


  bool Workspace::confirmClose() {
    QVector< MdiCubeViewport * > viewports = *cubeViewportList();

    bool confirmed = true;

    for (int viewportIndex = 0; confirmed && viewportIndex < viewports.count(); viewportIndex++) {
      confirmed = viewports[viewportIndex]->confirmClose();
    }

    return confirmed;
  }


  QWidget *Workspace::cubeToMdiWidget(Cube *cube) {
    QWidget *result = NULL;

    for (int i = 0; !result && i < m_cubeViewportList->count(); i++) {
      MdiCubeViewport *viewport = (*m_cubeViewportList)[i];

      if (viewport->cube() == cube) {
        result = qobject_cast<QWidget *>(viewport->parent());
      }
    }

    return result;
  }


  QMdiArea *Workspace::mdiArea() {
    return m_mdi;
  }


  /**
   * Add a cubeViewport to the workspace, open the cube.
   *
   * @param cubename  (QString) cubename
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
   *  @history 2015-05-13 Ian Humphrey - Caught exception now handled by sending a QMessageBox
   *                          to the Workspace. This prevents undefined behavior caused by not 
   *                          handling an exception within a connected slot.
   *  @history 2017-08-17 Adam Goins - Added the ability for this method to receive a cubelist
   *                          file as input to qview, parse out the .cub files from the
   *                          cubelist and then attempt to add each one into their own
   *                          viewport.
   *  @history 2017-10-12 Kristin Berry - Reverted to using relative instead of full file paths,
   *                          as this caused errors when working with cubelists that contained
   *                          relative paths. Fixes # 5177
   */
  void Workspace::addCubeViewport(QString cubename) {
    
    QFileInfo cubeFileName(cubename);

    QList<QString> cubesToOpen;

    // If the file is a cub file, we add the path to it to our list of cubes to open.
    if ( cubeFileName.suffix() == "cub" || cubeFileName.suffix() == "cube" ) {
       // cubesToOpen.append(cubeFileName.absoluteFilePath());
       cubesToOpen.append(cubeFileName.filePath());
    }
    else {
      // If the file received isn't a .cub, it has to be a cubelist. We read every cube in the cubelist
      // And append it to the cubesToOpen QList so that we can open them.
      QFile file(cubename);
      file.open(QIODevice::ReadOnly);

      QTextStream in(&file);

      while(!file.atEnd()){
        QString line = file.readLine().replace("\n", "");
        cubesToOpen.append(line);
      }
      file.close();
    }
    
    if (cubesToOpen.size() == 0){
        QMessageBox::critical((QWidget *)parent(), "Error", "No cubes to open from [" + cubename + "]");
        return;
    }

    for (int i = 0; i < cubesToOpen.size(); i++) {
      
      QString cubename;
      try {
        Cube *cube = new Cube;
        cubename = cubesToOpen.value(i);

        // Read in the CubeAttribueInput from the cube name
        CubeAttributeInput inAtt(cubename);
        std::vector<QString> bands = inAtt.bands();

        // Set the virtual bands to the bands specified by the input
        cube->setVirtualBands(bands);
        cube->open(cubename);
          
        MdiCubeViewport *cvp = addCubeViewport(cube);

        // Check for RGB format (#R,#G,#B)
        if(bands.size() == 3) {
          IString st = IString(bands.at(0));
          int index_red = st.ToInteger();
          st = IString(bands.at(1));
          int index_green = st.ToInteger();
          st = IString(bands.at(2));
          int index_blue = st.ToInteger();
          cvp->viewRGB(index_red, index_green, index_blue);
        }
      }
      catch (IException &e) {
        QString title("Error opening cube from list...");
        QString message(e.toString() + "\n\nWould you like to continue?");
        
        int response = QMessageBox::critical((QWidget *)parent(), 
                                             title, 
                                             message, 
                                             QMessageBox::Yes|QMessageBox::No);
        
        if (response == QMessageBox::Yes) { 
          continue;
        }
        else {
          return;
        }
      }
    }
  }

  /**
   * Add a cubeViewport to the workspace.
   *
   * @param cube  (Cube *)  cube information
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

      m_mdi->addSubWindow(window);

      window->show();

      result = window->viewport();
      emit cubeViewportAdded(result);
    }
    catch(IException &e) {
      throw IException(e,
                       IException::Programmer,
                       tr("Error when attempting to show cube [%1]")
                         .arg(cube->fileName()),
                       _FILEINFO_);
    }

    return result;
  }


  void Workspace::addBrowseView(QString cubename) {
    /* Close the last browse window if necessary.  */
    if (m_mdi->subWindowList().size()) {
      QPointer<QMdiSubWindow> windowToRemove =
          m_mdi->subWindowList().last();

      m_mdi->removeSubWindow(windowToRemove.data());

      delete windowToRemove.data();
    }

    addCubeViewport(cubename);
  }
}
