#include "RubberBandComboBox.h"

#include <QAction>

#include "Tool.h"

namespace Qisis {
  /**
   * RubberBandComboBox constructor
   *
   *
   * @param bandingOptions
   * @param defaultOption
   * @param showIndicatorColors
   */
  RubberBandComboBox::RubberBandComboBox(unsigned int bandingOptions, unsigned int defaultOption, bool showIndicatorColors) :
    QComboBox() {

    p_showIndicatorColors = showIndicatorColors;
    setDuplicatesEnabled(false);
    setEditable(false);
    setIconSize(QSize(24, 24));

    // We must call setCurrentIndex with the default, or the banding mode won't be set properly. Thus,
    //   we do need a default even if they pass in zero, which means default.
    defaultOption = getDefault(defaultOption, bandingOptions);

    QAction *escapeKeyHit = new QAction(this);
    escapeKeyHit->setShortcut(Qt::Key_Escape);
    connect(escapeKeyHit, SIGNAL(activated()), RubberBandTool::getInstance(), SLOT(escapeKeyPress()));
    addAction(escapeKeyHit);

    // The tool tip and what's this need to be set. The tool tip won't change, but the what's this should detail how to use the
    //   tools. So, set the tool tip and start the what's this. The what's this should only have the available tools, so create it as
    //   we add the tools.
    setToolTip("Select Measurement Type");
    QString whatsThis =
      "<b>Function:</b> This drop down will enable you to switch between drawing various shapes.";

    p_bandingOptionStrings.push_back("Circle");
    if((bandingOptions & Circle) == Circle) {
      QIcon circleIcon(toolIconDir() + "/qview_circle.png");
      circleIcon.addFile(toolIconDir() + "/qview_circle_active.png", QSize(24, 24), QIcon::Active);

      addItem(circleIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)Circle));

      whatsThis += "<br><br>When <b>Circle</b> is selected, you can draw a perfect circle. To draw a circle, \
                     click at a corner of the circle, drag the mouse until the circle is the correct shape, and release the mouse \
                     to complete the circle.";

      if((defaultOption & Circle) == Circle) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    p_bandingOptionStrings.push_back("Ellipse");
    if((bandingOptions & Ellipse) == Ellipse) {
      QIcon ellipseIcon(toolIconDir() + "/qview_ellipse.png");
      ellipseIcon.addFile(toolIconDir() + "/qview_ellipse_active.png", QSize(24, 24), QIcon::Active);

      addItem(ellipseIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)Ellipse));

      whatsThis += "<br><br>When <b>Ellipse</b> is selected, you can draw an ellipse. To draw an ellipse, \
                     click at a corner of the ellipse, drag the mouse until the ellipse is the correct shape, and release the mouse \
                     to complete the ellipse.";

      if((defaultOption & Ellipse) == Ellipse) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    p_bandingOptionStrings.push_back("Rectangle");
    if((bandingOptions & Rectangle) == Rectangle) {
      QIcon rectangleIcon(toolIconDir() + "/qview_rectangle.png");
      rectangleIcon.addFile(toolIconDir() + "/qview_rectangle_active.png", QSize(24, 24), QIcon::Active);

      addItem(rectangleIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)Rectangle));

      whatsThis += "<br><br>When <b>Rectangle</b> is selected, you can draw a rectangle. To draw a rectangle, \
                     click a corner of the rectangle, drag the mouse until the rectangle is the correct size and shape, and release the mouse \
                     to complete the rectangle.";

      if((defaultOption & Rectangle) == Rectangle) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    p_bandingOptionStrings.push_back("Rotated Rectangle");
    if((bandingOptions & RotatedRectangle) == RotatedRectangle) {
      QIcon rotatedRectangleIcon(toolIconDir() + "/qview_rotated_rectangle.png");

      addItem(rotatedRectangleIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)RotatedRectangle));

      whatsThis += "<br><br>When <b>Rotated Rectangle</b> is selected, you can draw a rectangle rotated about its center. To draw a rotated rectangle, \
                     click, drag the mouse to form the first side of the rectangle and determine it's rotation, and release the mouse. \
                     Now, the final side of the rotated rectangle should follow the mouse. \
                     Click again when the rotated rectangle is the correct size to complete the rotated rectangle.";

      if(p_showIndicatorColors) {
        whatsThis += " The data will be calculated starting from the initial (GREEN) side of the rotated rectangle and ending at the opposite \
                      side of the rotated rectangle.";
      }

      if((defaultOption & RotatedRectangle) == RotatedRectangle) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    p_bandingOptionStrings.push_back("Polygon");
    if((bandingOptions & Polygon) == Polygon) {
      QIcon polygonIcon(toolIconDir() + "/qview_polygon.png");

      addItem(polygonIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)Polygon));

      whatsThis += "<br><br>When <b>Polygon</b> is selected, you can draw any closed polygon. To begin drawing a polygon, \
                     click where you want the start point. Now, click for any new vertices or click and drag the mouse to form irregular shapes. \
                     When completed, double click the final vertex and the figure will close.";

      if((defaultOption & Polygon) == Polygon) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    p_bandingOptionStrings.push_back("Line");
    if((bandingOptions & Line) == Line) {
      QIcon lineIcon(toolIconDir() + "/qview_line.png");
      lineIcon.addFile(toolIconDir() + "/qview_line_active.png", QSize(24, 24), QIcon::Active);

      addItem(lineIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)Line));

      whatsThis += "<br><br>When <b>Line</b> is selected, you can draw a line. To draw a line, \
                     click the starting point of the line, drag the mouse until the line is the correct length in the correct direction, \
                     and release the mouse.";

      if((defaultOption & Line) == Line) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    p_bandingOptionStrings.push_back("Segmented Line");
    if((bandingOptions & SegmentedLine) == SegmentedLine) {
      QIcon segmentedLineIcon(toolIconDir() + "/qview_segmentedline.png");

      addItem(segmentedLineIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)SegmentedLine));

      whatsThis += "<br><br>When <b>Segmented Line</b> is selected, you can draw any open polygon. To begin drawing a segmented line, \
                     click where you want the start point. Now, click for any new vertices or click and drag the mouse to form irregular shapes. \
                     When completed, double click the final vertex.";

      if((defaultOption & SegmentedLine) == SegmentedLine) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    p_bandingOptionStrings.push_back("Angle");
    if((bandingOptions & Angle) == Angle) {
      QIcon angleIcon;
      angleIcon.addFile(toolIconDir() + "/qview_angle.png", QSize(24, 24), QIcon::Normal);
      //angleIcon.addFile(toolIconDir()+"/qview_angle_active.png", QSize(24,24), QIcon::Active);

      addItem(angleIcon, p_bandingOptionStrings[p_bandingOptionStrings.size()-1], QVariant((int)Angle));

      whatsThis += "<br><br>When <b>Angle</b> is selected, you can draw an angle from zero to 180 degrees. To draw an angle, \
                     click where the first corner should go, then click where the vertex should go. Finally, click where the final \
                     side of the angle should end to complete the angle.";

      if((defaultOption & Angle) == Angle) {
        setCurrentIndex(findText(p_bandingOptionStrings[ p_bandingOptionStrings.size()-1 ]));
      }
    }

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionChanged(int)));
    connect(this, SIGNAL(activated(int)), this, SLOT(selectionChanged(int)));

    setWhatsThis(whatsThis);
  }


  /**
   * This slot is called whenever the rubberband shape selection
   * has changed.
   *
   *
   * @param index
   */
  void RubberBandComboBox::selectionChanged(int index) {
    switch(itemData(index).toInt()) {
      case Angle:
        showAngle();
        break;
      case Circle:
        showCircle();
        break;
      case Ellipse:
        showEllipse();
        break;
      case Line:
        showLine();
        break;
      case Rectangle:
        showRectangle();
        break;
      case RotatedRectangle:
        showRotatedRectangle();
        break;
      case Polygon:
        showPolygon();
        break;
      case SegmentedLine:
        showSegmentedLine();
        break;
      default:
        // This shouldn't happen
        RubberBandTool::disable();
        break;
    }
  }

  /**
   * Resets the selection.
   *
   */
  void RubberBandComboBox::reset() {
    selectionChanged(currentIndex());
  }


  /**
   * Returns the default option.
   *
   *
   * @param defaultOption
   * @param bandingOptions
   *
   * @return unsigned int
   */
  unsigned int RubberBandComboBox::getDefault(unsigned int defaultOption, unsigned int bandingOptions) {
    // The default will always be to set
    //   the default selection to the first, and thus the least significant bit in bandingOptions.
    //   We're going to find the least significant bit by shifting left and right the same amount of times,
    //   causing the bits to be truncated, until we see the result isn't the same as the beginning which means
    //   a bit was truncated. When we see this, we know where the least significant bit is so shift a '1' left by
    //  the appropriate amount.
    if(defaultOption == 0) {
      defaultOption = bandingOptions;
    }

    // Find the least significant bit, that's our default.
    int power = 0;

    // While cuting off the next significant bit does nothing, keep going.
    while(((defaultOption >> (power + 1)) << (power + 1)) == defaultOption) {
      power ++;
    }

    defaultOption = 1 << power;

    // We are done figuring out the default option.
    return defaultOption;
  }
}
