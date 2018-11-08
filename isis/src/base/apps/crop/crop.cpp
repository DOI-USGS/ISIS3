#include "Isis.h"

//#include <cmath>

#include "CropApp.h"


using namespace std;
using namespace Isis;

void IsisMain() {


  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  QString from = ui.GetAsString("FROM");
  QString to = ui.GetAsString("FROM");
  CubeAttributeInput inAtt(from);
  Cube *cube = new Cube();
  cube->setVirtualBands(inAtt.bands());
  from = ui.GetFileName("FROM");
  cube->open(from);
  bool propspice =false;

  // Determine the sub-area to extract

  int ss = ui.GetInteger("SAMPLE");
  int sl = ui.GetInteger("LINE");
  //int sb = 1;


  int ns;
  int nl;
  if (ui.WasEntered("NSAMPLES"))
    ns = ui.GetInteger("NSAMPLES");
  else
    ns = -1;

  if (ui.WasEntered("NLINES"))
    nl = ui.GetInteger("LINES");
  else
    nl = -1;



  int es = cube->sampleCount();
  int el = cube->lineCount();
  //int eb = cube->bandCount();

  int sinc = ui.GetInteger("SINC");
  int linc = ui.GetInteger("LINC");

  // Make sure starting positions fall within the cube
  if (ss > cube->sampleCount()) {
    cube->close();
    QString msg = "[SAMPLE] exceeds number of samples in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (sl > cube->lineCount()) {
    cube->close();
    QString msg = "[LINE] exceeds number of lines in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Make sure the number of elements do not fall outside the cube
  if (es > cube->sampleCount()) {
    cube->close();
    QString msg = "[SAMPLE+NSAMPLES-1] exceeds number of ";
    msg += "samples in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (el > cube->lineCount()) {
    cube->close();
    QString msg = "[LINE+NLINES-1] exceeds number of ";
    msg += "lines in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (ui.GetBoolean("PROPSPICE") ) {
    propspice = true;
  }


  CropApp *capp = new CropApp(from,to,ss,ns,sinc,sl,nl,linc,propspice,cube);
  capp->start();

  // Write the results to the log
  //Application::Log(results);
}

