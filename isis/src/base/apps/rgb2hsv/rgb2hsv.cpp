#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

#include "QColor"
#include "Histogram.h"
#include "Stretch.h"

using namespace std; 
using namespace Isis;

void convert(vector<Buffer *> &in, 
             vector<Buffer *> &out);

Stretch redStretch, greenStretch, blueStretch;

void IsisMain() {
  // Open input cubes 
  ProcessByLine p;  
  Cube* redCube = p.SetInputCube("RED", Isis::OneBand);
  Cube* greenCube = p.SetInputCube("GREEN", Isis::OneBand);
  Cube* blueCube = p.SetInputCube("BLUE", Isis::OneBand);

  // Clear out stretch pairs for multiple runs
  redStretch.ClearPairs();
  greenStretch.ClearPairs();
  blueStretch.ClearPairs();

  // Get information from the GUI to build stretch pairs.  
  // "Automatic" uses histogram percentages for uses in the stretch pairs
  // "Manual" uses pixels values to build the stretch pairs 
  double rMin, rMax, gMin, gMax, bMin, bMax;
  UserInterface &ui = Application::GetUserInterface();

  // Automatic is selected
  if (ui.GetString("MODE") == "AUTOMATIC") {
    Histogram* redHist = redCube->Histogram();
    Histogram* greenHist = greenCube->Histogram();
    Histogram* blueHist = blueCube->Histogram();

    rMin = redHist->Percent(ui.GetDouble("RMINPER"));
    rMax = redHist->Percent(ui.GetDouble("RMAXPER"));
    gMin = greenHist->Percent(ui.GetDouble("GMINPER"));
    gMax = greenHist->Percent(ui.GetDouble("GMAXPER"));
    bMin = blueHist->Percent(ui.GetDouble("BMINPER"));
    bMax = blueHist->Percent(ui.GetDouble("BMAXPER"));
    // Manual is selected		
  } else {
    rMin = ui.GetDouble("RMIN");
    rMax = ui.GetDouble("RMAX");
    gMin = ui.GetDouble("GMIN");
    gMax = ui.GetDouble("GMAX");
    bMin = ui.GetDouble("BMIN");
    bMax = ui.GetDouble("BMAX");
  }

  // Map minimum values to zero
  redStretch.AddPair(rMin, 0.0);
  greenStretch.AddPair(gMin, 0.0);
  blueStretch.AddPair(bMin, 0.0);

  // Map maximum values to one
  redStretch.AddPair(rMax, 1.0);
  greenStretch.AddPair(gMax, 1.0);
  blueStretch.AddPair(bMax, 1.0);

  // Handle special pixels 
  redStretch.SetNull(0.0);
  redStretch.SetLis(0.0);
  redStretch.SetLrs(0.0); 
  redStretch.SetHrs(1.0);
  redStretch.SetHis(1.0);
  greenStretch.SetNull(0.0);
  greenStretch.SetLis(0.0);
  greenStretch.SetLrs(0.0); 
  greenStretch.SetHrs(1.0);
  greenStretch.SetHis(1.0);
  blueStretch.SetNull(0.0);
  blueStretch.SetLis(0.0);
  blueStretch.SetLrs(0.0); 
  blueStretch.SetHrs(1.0);
  blueStretch.SetHis(1.0);

  // Start the processing
  p.SetOutputCube ("HUE");
  p.SetOutputCube ("SATURATION");
  p.SetOutputCube ("VALUE");

  p.StartProcess(convert);
  p.EndProcess();
}   

// Line processing routine 
// This works by creating an RGB QColor from 3 qreals representing Red, Green,
// and Blue.  This is converted into HSV format, and the data from this is
// sent to the output buffers.
void convert (vector<Buffer *> &in,
              vector<Buffer *> &out) {

  // Input file buffers
  Buffer &red = *in[0];
  Buffer &green = *in[1];
  Buffer &blue = *in[2];

  // Output file buffers
  Buffer &hue = *out[0];
  Buffer &saturation = *out[1];
  Buffer &value = *out[2];

  for (int i = 0; i < red.size(); i++) {
    qreal r = redStretch.Map(red[i]);
    qreal g = greenStretch.Map(green[i]);
    qreal b = blueStretch.Map(blue[i]);

    QColor rgb;
    rgb.setRgbF(r, g, b);
    QColor hsv = rgb.toHsv(); 

    /**
     * HSV Stores colors in terms of hue, saturation, and value. The hue determines the color,
     * which is an angle around the color wheel. However, hsv.hueF() in the qt library returns a 
     * percentage around the color wheel and not an angle. For example, 0% means the origin and 
     * 50% means 180 degrees around the circle. We want the hue to be in degrees, so to convert 
     * this percentage we multiple by 360 degrees. The basic colors are: 0 degrees = RED, 
     * 60 degrees = YELLOW, 120 degrees = GREEN, 180 degrees = CYAN, 240 degrees = BLUE and 
     * 300 degrees = PURPLE. 
     * The saturation is how much grey is in the color (intensity of the color). A saturation value of zero means it's perfect, 
     * while a saturation value of 1 would cause any color to become pure grey. As an example, the color RGB(255,0,0)
     * is pure so the saturation would be zero. The value is how bright the color is. A value of 0 is always black,
     * and 100 is the color (if not saturated).
     * In brief,
     * HUE = COLOR (degrees around the color wheel)
     * SATURATION = INTENSITY (0-1, 0 being no color/grey)
     * VALUE = BRIGHTNESS (0 being black)
     * 
     * For more information, see
     * http://en.wikipedia.org/wiki/Color_spaces
     */
    hue[i] = hsv.hueF() * 360.0;  // Hue values range from 0.0 - 360.0
    saturation[i] = hsv.saturationF();
    value[i] = hsv.valueF();
  }
}  
