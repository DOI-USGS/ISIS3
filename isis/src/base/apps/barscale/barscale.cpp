#define USE_GUI_QAPP

#include "Isis.h"

#include "Camera.h"
#include "Cube.h"
#include "ProgramLauncher.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Process.h"
#include "SpecialPixel.h"
#include "IException.h"

#include <cmath>

#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QVector>

using namespace std;
using namespace Isis;

// Global variables
Camera *cam;
Projection *proj;
bool noCamera;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Get the camera information if this is not a mosaic. Otherwise, get the
  // projection information
  Process p1;
  Cube *icube = p1.SetInputCube("FROM");
  FileName sfile = FileName::createTempFile("barscale.tif");
  QString scaletif = sfile.expanded();

  noCamera = true;
  try {
    proj = icube->projection();
  }
  catch(IException &e) {
    noCamera = false;
    try {
      cam = icube->camera();
    }
    catch(IException &e) {
      QString msg = "Input file needs to have spiceinit run on it";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  } 

  // Determine where in image to get resolution from and get it
  int nsamps = icube->sampleCount();
  int nlines = icube->lineCount();
  int nbands = icube->bandCount();
  double samp, line;
  if (ui.GetString("PIXRES").toUpper() == "USER") {
    samp = ui.GetDouble("SAMPLE");
    line = ui.GetDouble("LINE");
  }
  else {
    samp = nsamps / 2.0;
    line = nlines / 2.0;
  }

  bool isGood = false;
  double resolution;
  if (noCamera) {
    proj->SetWorld(samp, line);
    if (proj->IsGood()) {
      isGood = true;
      resolution = proj->Resolution();
    }
  }
  else {
    cam->SetImage(samp, line);
    if (cam->HasSurfaceIntersection()) {
      isGood = true;
      resolution = cam->PixelResolution();
    }
  }
  if (!isGood) {
    QString msg = "Unable to obtain resolution from input image at location specified";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Determine the unit of measure used to create the scale and the limit for the
  // right and left sides of the scale
  QString units = ui.GetString("UNITS").toUpper();
  double rlimit = ui.GetInteger("RIGHTLIMIT");
  double llimit = ui.GetInteger("LEFTLIMIT");

  // Determine how many segments will be in right and left sides of the scale
  int rsegs = ui.GetInteger("RIGHTSEGS");
  int lsegs = 0;
  if (llimit > 0) {
    if (ui.WasEntered("LEFTSEGS")) {
      lsegs = ui.GetInteger("LEFTSEGS");
    }
    else {
      QString msg = "Number of segments was not specified for left side of scale - must be at least 1";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  // Determine the units of measurement for the scale bar
  double scunit;
  if (units == "KILOMETER") {
    scunit = 1000.0;
  }
  else {
    scunit = 1.0;
  }

  // Determine where the bar scale will be placed in the image
  bool padimage = false;
  QString padloc;
  int cornerline = 0;
  int cornersample = 0;
  int placeline = 0;
  int placesample = 0;
  if (ui.GetBoolean("PADIMAGE")) {
    padimage = true;
    padloc = ui.GetString("PLACEMENT").toUpper();  
  }
  else {
    if (!ui.WasEntered("CORNERLINE") || !ui.WasEntered("CORNERSAMPLE")) {
      QString msg = "The upper left placement of the scale bar must be specified ";
      msg = msg + "if you are not padding the image.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    placeline = ui.GetInteger("CORNERLINE");
    placesample = ui.GetInteger("CORNERSAMPLE");
  }

  // Determine width/height of the scale based on the resolution that the bar 
  // scale will be printed at
  int barheight = ui.GetInteger("BARHEIGHT");
  if (barheight < 5) {
    QString msg = "Requested BARHEIGHT is too small to produce a bar scale";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  int barwidth = ((rlimit + llimit) * scunit) / resolution + .5;

  // Determine how many pixels are needed to make up each component of the bar
  // scale. The amount of pixels available are determined by height. Each 
  // component takes up a specified percentage of the available pixels. Extra
  // pixels need to be divided between the horizontal measurement line and the
  // space around it.
  int bndline = .083 * barheight;
  if (bndline < 1) bndline = 1;
  int midline = .166 * barheight;
  if (midline < 1) midline = 1;
  int spacing = .333 * barheight;
  if (spacing < 1) spacing = 1;
  int topspace = spacing;
  int botspace = spacing;
  int total = 2*bndline + midline + 2*spacing;
  if (spacing > 2*midline) {
    while (spacing > 2*midline && total < barheight) {
      midline = midline + 1;
      total = total + 1; 
    }
    if (total < barheight) {
      while (total < barheight) {
        topspace = topspace + 1;
        total = total + 1;
        if (total < barheight) {
          botspace = botspace + 1;
          total = total + 1;
        }
        if (total < barheight && topspace > 2*midline) {
          midline = midline + 1;
          total = total + 1;
        }
      }
    }
  }

  QString bkgrnd = ui.GetString("BACKGROUND").toUpper();
  int textht = ui.GetInteger("TEXTHEIGHT");
  QString textloc = ui.GetString("TEXTLOC").toUpper();

  QFont font;
  font.setPixelSize(textht);
  font.setStyleStrategy(QFont::NoAntialias);
  QFontMetrics metric(font);
  int fheight = metric.height();
  int totalheight = barheight + fheight + 16;

  // If there is no left side to the scale bar, then "0"
  // will be the text character that occurs at the left side
  // of the scale bar - set cornersample in slightly to make
  // room for the digit "0" plus some space between the left
  // edge and the "0"
  cornersample = (textht+1)/2 + 10;
  int imgsamps;
  spacing = (textht + 6) / 7;
  if (spacing < 1) spacing = 1;
  int totalwidth = barwidth + (textht+1)/2 + 10;

  if (padimage) {
    imgsamps = nsamps;
  }
  else {
    imgsamps = totalwidth;
  }

  if (padimage) {
    if (padloc == "LEFT") {
      cornersample = (textht+1)/2 + 10;
    }
    else if (padloc == "CENTER") {
      cornersample = imgsamps/2 - barwidth/2;
    }
  }

  // Center line of text area is calculated help in placing
  // the text display area rectangles
  int textctrline = (fheight + 8) / 2;
  if (textloc == "BELOW") {
    textctrline = textctrline + barheight + 8;
  } 

  QRect ctrdisplayarea;
  QRect leftdisplayarea;
  QRect rightdisplayarea;
  QString lblstr;
  int lblstrwidth;
  if (llimit > 0) {
    lblstr.setNum(llimit);
    lblstrwidth = metric.width(lblstr);
    totalwidth = totalwidth + lblstrwidth/2;
    cornersample = cornersample + lblstrwidth/2;
    leftdisplayarea.setRect(cornersample-lblstrwidth/2,textctrline-fheight/2-2,
                            lblstrwidth+10,fheight+8);
  }
  lblstrwidth = metric.width("0");
  ctrdisplayarea.setRect(cornersample+llimit*scunit/resolution+.5-lblstrwidth/2,textctrline-fheight/2-2,
                         lblstrwidth+10,fheight+8);

  lblstr.setNum(rlimit);
  QString unitstr;
  if (units == "KILOMETER") {
    unitstr = "KILOMETER";
    if (rlimit > 1) {
      unitstr = "KILOMETERS";
    }
  }
  else {
    unitstr = "METER";
    if (rlimit > 1) {
      unitstr = "METERS";
    }
  }
  lblstr = lblstr + " " + unitstr;
  lblstrwidth = metric.width(lblstr);
  totalwidth = totalwidth + lblstrwidth + (textht+1)/2 + 10;
  rightdisplayarea.setRect(barwidth+cornersample-lblstrwidth/2,textctrline-fheight/2-2,lblstrwidth+30,fheight+8);

  // Make sure text does not overlap
  if (llimit > 0) {
    if (leftdisplayarea.right() > ctrdisplayarea.left()) {
      int diff = leftdisplayarea.right() - ctrdisplayarea.left();
      leftdisplayarea.setRect(leftdisplayarea.left()-diff,leftdisplayarea.top(),
                              leftdisplayarea.width(),leftdisplayarea.height());
    }
  }
  if (ctrdisplayarea.right() > rightdisplayarea.left()) {
    int diff = ctrdisplayarea.right() - rightdisplayarea.left();
    rightdisplayarea.setRect(rightdisplayarea.left()+diff,rightdisplayarea.top(),
                             rightdisplayarea.width(),rightdisplayarea.height());
  }
  if (llimit > 0) {
    totalwidth = rightdisplayarea.right() - leftdisplayarea.left() + 12;
  }
  else {
    totalwidth = rightdisplayarea.right() - ctrdisplayarea.left() + 12;
  }

  if (padimage) {
    if (padloc == "RIGHT") {
      cornersample = nsamps - 10 - totalwidth;
    }
  }

  if (textloc == "ABOVE") {
    cornerline = totalheight - 8;
  }
  else {
    cornerline = barheight + 8;
  }

  QImage myQImage;
  if (totalwidth > imgsamps) {
    myQImage = QImage(totalwidth, totalheight, QImage::Format_RGB32);
  }
  else {
    myQImage = QImage(imgsamps, totalheight, QImage::Format_RGB32);
  }
  if (bkgrnd == "WHITE") {
    myQImage.fill(qRgb(255, 255, 255));
  }
  else {
    myQImage.fill(qRgb(0, 0, 0));
  }

  QPainter painter(&myQImage);

  painter.setRenderHint(QPainter::Antialiasing, false);

  QPen pen;
  if (bkgrnd == "WHITE") {
    pen.setColor(qRgb(0, 0, 0));
  }
  else {
    pen.setColor(qRgb(255, 255, 255));
  }
  pen.setStyle(Qt::SolidLine);
  pen.setWidth(1);
  painter.setPen(pen);

  // Need to keep track of location of vertical division
  // lines so horizontal lines can be drawn without 
  // overshooting them
  QVector<int> vertline;
  vertline.push_back(cornersample);

  // Draw outline
  QPoint pt1(cornersample,cornerline);
  QPoint pt2(barwidth+cornersample,cornerline);
  for (int i=0; i<bndline; i++) {
    pt1.setY(cornerline-i);
    pt2.setY(cornerline-i);
    painter.drawLine(pt1,pt2);
  }
  pt1.setY(cornerline);
  pt2.setY(cornerline-barheight+1);
  for (int i=0; i<bndline; i++) {
    pt1.setX(cornersample+i);
    pt2.setX(cornersample+i);
    painter.drawLine(pt1,pt2);
  }
  pt1.setX(barwidth+cornersample);
  pt2.setX(cornersample);
  for (int i=0; i<bndline; i++) {
    pt1.setY(cornerline-barheight+1+i);
    pt2.setY(cornerline-barheight+1+i);
    painter.drawLine(pt1,pt2);
  }
  pt1.setY(cornerline-barheight+1);
  pt2.setY(cornerline);
  for (int i=0; i<bndline; i++) {
    pt1.setX(barwidth+cornersample+i);
    pt2.setX(barwidth+cornersample+i);
    painter.drawLine(pt1,pt2);
  }
  // Draw center line of barscale (only for left limit)
  if (llimit > 0.0) {
    vertline.insert(1, cornersample+llimit*scunit/resolution+.5);
    pt1.setY(cornerline-barheight+1);
    pt2.setY(cornerline);
    for (int i=0; i<bndline; i++) {
      pt1.setX(cornersample+llimit*scunit/resolution+.5+i);
      pt2.setX(cornersample+llimit*scunit/resolution+.5+i);
      painter.drawLine(pt1,pt2);
    }
  }
  // Draw segment marks in left and right sides of bar scale
  if (llimit > 0.0 && lsegs > 0) {
    double ticspace = (llimit*scunit/resolution)/lsegs;
    pt1.setY(cornerline-barheight+1);
    pt2.setY(cornerline);
    for (int i=1; i<lsegs; i++) {
      vertline.insert(i, cornersample+ticspace*i);
      for (int j=0; j<bndline; j++) {
        pt1.setX(cornersample+ticspace*i+j);
        pt2.setX(cornersample+ticspace*i+j);
        painter.drawLine(pt1,pt2);
      }
    }
  }
  if (rsegs > 0) {
    double ticspace = (rlimit*scunit/resolution)/rsegs;
    pt1.setY(cornerline-barheight+1);
    pt2.setY(cornerline);
    for (int i=1; i<rsegs; i++) {
      vertline.insert(vertline.size(), cornersample+llimit*scunit/resolution+ticspace*i);
      for (int j=0; j<bndline; j++) {
        pt1.setX(cornersample+llimit*scunit/resolution+ticspace*i+j);
        pt2.setX(cornersample+llimit*scunit/resolution+ticspace*i+j);
        painter.drawLine(pt1,pt2);
      }
    }
  }

  vertline.push_back(cornersample+barwidth);
  // Draw horizontal measurement lines
  int vertlineidx = 0;
  for (int i=0; i<lsegs; i+=2) {
    pt1.setX(vertline[i]);
    pt2.setX(vertline[i+1]);
    vertlineidx = i + 2;
    for (int j=0; j<midline; j++) {
      pt1.setY(cornerline-bndline-botspace-j);
      pt2.setY(cornerline-bndline-botspace-j);
      painter.drawLine(pt1,pt2);
    }
  }
  int starttic = 0;
  if ((lsegs%2)*2 != lsegs && (lsegs%2) != 0) {
    starttic = 1;
  }
  for (int i=starttic; i<rsegs; i+=2) {
    pt1.setX(vertline[vertlineidx]);
    pt2.setX(vertline[vertlineidx+1]);
    vertlineidx = vertlineidx + 2;
    for (int j=0; j<midline; j++) {
      pt1.setY(cornerline-bndline-botspace-j);
      pt2.setY(cornerline-bndline-botspace-j);
      painter.drawLine(pt1,pt2);
    }
  }

  font.setStyleStrategy(QFont::NoAntialias);
  painter.setFont(font);
  painter.setLayoutDirection(Qt::LeftToRight);
  if (llimit > 0) {
    lblstr.setNum(llimit);
    lblstrwidth = metric.width(lblstr);
    painter.drawText(leftdisplayarea,lblstr);
  }
  lblstr.setNum(0);
  lblstrwidth = metric.width(lblstr);
  painter.drawText(ctrdisplayarea,lblstr);
  lblstr.setNum(rlimit);
  lblstr = lblstr + " " + unitstr;
  painter.drawText(rightdisplayarea,lblstr);

  QString infile = ui.GetFileName("FROM");
  QString outfile = ui.GetFileName("TO");
  sfile = FileName::createTempFile("barscale.cub");
  QString scalecub = sfile.expanded();
  myQImage.save(scaletif,"TIFF",100);

  // Convert bar scale to ISIS cube
  QString parameters = "FROM=" + scaletif + " TO=" + scalecub + " MODE=GRAYSCALE";
  ProgramLauncher::RunIsisProgram("std2isis", parameters);
  Cube scube;
  scube.open(scalecub, "r");
  Statistics *stats = scube.statistics(1);
  sfile = FileName::createTempFile("barscalestr.cub");
  QString scalestrcub = sfile.expanded();
  if (stats->ValidPixels() > 0) {
    parameters = "FROM=" + scalecub + " TO=" + scalestrcub + " NULLMIN=0 NULLMAX=130" +
                 " HRSMIN=131 HRSMAX=255"; 
  }
  else {
    parameters = "FROM=" + scalecub + " TO=" + scalestrcub + " NULLMIN=0 NULLMAX=0" +
                 " HRSMIN=255 HRSMAX=255";
  }
  delete stats;
  ProgramLauncher::RunIsisProgram("specpix", parameters);

  if (!padimage) {
    // Paste bar scale onto image
    int pasteline = placeline;
    for (int i=1; i<=nbands; i++) {
      if (i == 1) {
        parameters = "FROM=" + infile + " MOSAIC=" + outfile + " PRIORITY=ONTOP OUTSAMPLE=1" +
                     " OUTLINE=1" + " OUTBAND=1" + " MATCHBANDBIN=NO CREATE=YES" +
                     " NSAMPLES=" + toString(nsamps) + " NLINES=" + toString(nlines) +
                     " NBANDS=" + toString(nbands);
        ProgramLauncher::RunIsisProgram("handmos", parameters);
      }
      parameters = "FROM=" + scalestrcub + " MOSAIC=" + outfile + " PRIORITY=ONTOP OUTSAMPLE=" +
                   toString(placesample) + " OUTLINE=" + toString(pasteline) + " OUTBAND=" + 
                   toString(i) + " MATCHBANDBIN=NO NULL=YES HIGHSATURATION=YES";
      ProgramLauncher::RunIsisProgram("handmos", parameters);
    }
  }

  // Pad image with bar scale
  if (padimage) {
    parameters = "FROM=" + infile + " TO=" + outfile + " BOTTOM=" + toString(totalheight);
    ProgramLauncher::RunIsisProgram("pad", parameters);
    for (int i=1; i<=nbands; i++) {
      parameters = "FROM=" + scalestrcub + " MOSAIC=" + outfile + " PRIORITY=ONTOP OUTSAMPLE=1" +
                   " OUTLINE=" + toString(nlines+1) + " OUTBAND=" + toString(i) + " MATCHBANDBIN=NO " +
                   " NULL=YES HIGHSATURATION=YES";
      ProgramLauncher::RunIsisProgram("handmos", parameters);
    }
  }
  QFile::remove(scaletif);
  QFile::remove(scalecub);
  QFile::remove(scalestrcub);
}
