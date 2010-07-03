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

Stretch hueStretch, satStretch, valStretch;
double valueScalar;

void IsisMain() {
	UserInterface &ui = Application::GetUserInterface();
	valueScalar = ui.GetDouble("SCALAR");
	
  // Clear out stretch pairs to handle multiple runs
  hueStretch.ClearPairs();
  satStretch.ClearPairs();
  valStretch.ClearPairs();

  // Handle special pixels
  hueStretch.SetNull(0.0);
  hueStretch.SetLis(0.0);
  hueStretch.SetLrs(0.0);
  hueStretch.SetHrs(360.0);
  hueStretch.SetHis(360.0);
  satStretch.SetNull(0.0);
  satStretch.SetLis(0.0);
  satStretch.SetLrs(0.0);
  satStretch.SetHrs(1.0);
  satStretch.SetHis(1.0);
  valStretch.SetNull(0.0);
  valStretch.SetLis(0.0);
  valStretch.SetLrs(0.0);
  valStretch.SetHrs(1.0);
  valStretch.SetHis(1.0);

  // Start the processing
  ProcessByLine p;  
  p.SetInputCube("HUE", Isis::OneBand);
  p.SetInputCube("SATURATION", Isis::OneBand);
  p.SetInputCube("VALUE", Isis::OneBand);

  p.SetOutputCube("RED");
  p.SetOutputCube("GREEN");
  p.SetOutputCube("BLUE");

  p.StartProcess(convert);
  p.EndProcess();
}  

// Line processing routine 
// This works by building an HSV QColor from the input buffers.  This is
// converted to RGB format, then the RGB data is sent to the output file buffers.
void convert (vector<Buffer *> &in,
              vector<Buffer *> &out) {
  // Input file buffers
  Buffer &hue = *in[0];
  Buffer &saturation = *in[1];
  Buffer &value = *in[2];

  // Output file buffers
  Buffer &red = *out[0];
  Buffer &green = *out[1];
  Buffer &blue = *out[2];

  for (int i = 0; i < hue.size(); i++) {
    // Convert range: [0, 360] -> [0, 1]

    /**
     * HSV Stores colors in terms of hue, saturation, and value. The hue determines the color,
     * which is an angle around the color wheel. However, QColor::fromHsvF() in the qt library expects a 
     * percentage around the color wheel and not an angle. For example, 0% means the origin and 
     * 50% means 180 degrees around the circle. The hue is in degrees and we went it in percentages, so to convert 
     * this value we divide by 360 degrees. The basic colors are: 0 degrees = RED, 
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

    // Make sure hue[i] is in the 0 - 360 degree range because the colors may have been
    // shifted by the user. For example, they may have had all 0-360 degree values and
    // added 90 degrees to shift the colors. Well, 360 degrees + 90 degrees = 450 degrees.
    // 450 degrees on the color wheel is the same as 90 degrees on the color wheel, and
    // this will verify the inputs are valid.
    double hueVal = hueStretch.Map(hue[i]);

    // while hue is less than 0 degrees, increase it (example: -10 degrees would become 350 degrees)
    while(hueVal < 0) { 
      hueVal += 360;
    }

    // while hue is greater than 360 degrees, decrease it (example: 450 degrees would become 90 degrees)
    while(hueVal > 360) {
      hueVal -= 360;
    }

    // QColor::fromHsvF expects a percentage around the circle and not degrees, so we divide by 360.
    hueVal /= 360.0;
 
    double sat = satStretch.Map(saturation[i]);

    // Saturation should be between zero and one
    if(sat < 0) {
      sat = 0;
    }
    else if(sat > 1) {
      sat = 1;
    }

    double val = valStretch.Map(value[i]);

    // Value should be between zero and one before valueScalar is applied
    if(val < 0) {
      val = 0;
    }
    else if(val > 1) {
      val = 1;
    }

    val *= valueScalar;

    QColor hsv = QColor::fromHsvF(hueVal, sat, val);
    QColor rgb = hsv.toRgb(); 

    red[i] = rgb.redF();
    green[i] = rgb.greenF();
    blue[i] = rgb.blueF();
  }
}   
