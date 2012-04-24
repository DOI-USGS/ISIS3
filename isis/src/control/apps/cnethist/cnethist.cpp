#include "Isis.h"

#include <string>
#include  <QColor>
#include <QMenuBar>

#include "ControlNet.h"
#include "ControlMeasure.h"
#include "FileList.h"
#include "Process.h"
#include "Histogram.h"
#include "UserInterface.h"
#include "Progress.h"
#include "LineManager.h"
#include "QHistogram.h"
#include "HistogramPlotWindow.h"
#include "HistogramItem.h"
#include "iString.h"
#include "CubePlotCurve.h"

using namespace std;
using namespace Isis;

QColor curveColor(int i);

void IsisMain() {
  Process p;
  Progress progress;
  //Cube *icube = p.SetInputCube("FROM");
  UserInterface &ui = Application::GetUserInterface();

  //iString filename = ui.GetFilename("FROM");

  FileList fList(ui.GetFilename("NETLIST")); //empty FileList

  HistogramPlotWindow *plot=NULL;

  //setup plot tile and axis labels (if any)
  if(ui.IsInteractive()) { 
    // Set the title for the dialog
    string title;
    if(ui.WasEntered("TITLE")) {
      title = ui.GetString("TITLE");
    }
    else {
      title = "Control Net Histograms";
    }

    // Create the QHistogram, set the title & load the Isis::Histogram into it

    plot = new HistogramPlotWindow(title.c_str(), ui.TheGui());

    // Set the xaxis title if they entered one
    if(ui.WasEntered("XAXIS")) {
      string xaxis(ui.GetString("XAXIS"));
      plot->setAxisLabel(QwtPlot::xBottom, xaxis.c_str());
    }

    // Set the yLeft axis title if they entered one
    if(ui.WasEntered("FREQAXIS")) {
      string yaxis(ui.GetString("FREQAXIS"));
      plot->setAxisLabel(QwtPlot::yLeft, yaxis.c_str());
    }
    else {
      string yaxis = "Frequencey";
      plot->setAxisLabel(QwtPlot::yLeft, yaxis.c_str());
    }

    string yaxis = "";
    plot->setAxisLabel(QwtPlot::yRight, yaxis.c_str()); 
  }

  //open text report file (if any)
  ofstream fout; 
  if(!ui.IsInteractive() || ui.WasEntered("TO")) { 
    // Write the results

    if(!ui.WasEntered("TO")) {
      string msg = "The [TO] parameter must be entered";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    string outfile = ui.GetFilename("TO");     
    fout.open(outfile.c_str());
  }

  //loop throught the control nets writing reports and drawing histograms as needed
  for (unsigned int i=0;i<fList.size();i++) {

    ControlNet net(fList[i],&progress);
    // Setup the histogram
    Histogram hist(net, &ControlMeasure::GetResidualMagnitude, ui.GetDouble("BIN_WIDTH"));
       

    //Tabular Histogram Data 
    if(!ui.IsInteractive() || ui.WasEntered("TO")) {
      fout << "Network:        " << fList[i] << endl;
      fout << "Average:        " << hist.Average() << endl;
      fout << "Std Deviation:  " << hist.StandardDeviation() << endl;
      fout << "Variance:       " << hist.Variance() << endl;
      fout << "Median:         " << hist.Median() << endl;
      fout << "Mode:           " << hist.Mode() << endl;
      fout << "Skew:           " << hist.Skew() << endl;
      fout << "Minimum:        " << hist.Minimum() << endl;
      fout << "Maximum:        " << hist.Maximum() << endl;
      fout << "Total Measures: " << hist.TotalPixels() << endl;
        
      //  Write histogram in tabular format
      fout << endl;
      fout << "DN,Measures,CumulativePixels,Percent,CumulativePercent" << endl;

      Isis::BigInt total = 0;
      double cumpct = 0.0;

      for(int j = 0; j < hist.Bins(); j++) {
        if(hist.BinCount(j) > 0) {
          total += hist.BinCount(j);
          double pct = (double)hist.BinCount(j) / hist.ValidPixels() * 100.;
          cumpct += pct;
    
          fout << hist.BinMiddle(j) << ",";
          fout << hist.BinCount(j) << ",";
          fout << total << ",";
          fout << pct << ",";
          fout << cumpct << endl;
        }
      }
      fout << endl;
      fout << endl;
      fout << endl;
      fout << endl;
    }

    // If we are in gui mode, add a plot
    if(ui.IsInteractive()) {
      //Transfer data from histogram to the plotcurve
      QVector<QPointF> binCountData;
      for(int j = 0; j < hist.Bins(); j++) {
        if(hist.BinCount(j) > 0) {
          binCountData.append(QPointF(hist.BinMiddle(j), hist.BinCount(j)));
        }
      }

      CubePlotCurve *histCurve = new CubePlotCurve(CubePlotCurve::CubeDN,
                                                CubePlotCurve::Percentage);
      histCurve->setColor(curveColor(i));
      iString baseName = Filename(fList[i]).Basename();
      histCurve->setTitle(baseName);

 
      QPen *pen = new QPen(curveColor(i));
      pen->setWidth(2);
      histCurve->setYAxis(QwtPlot::yLeft);
      histCurve->setPen(*pen);  
      QwtSymbol symbol(histCurve->markerSymbol());
      symbol.setStyle(QwtSymbol::NoSymbol);
      histCurve->setMarkerSymbol(symbol);
  
      histCurve->setData(new QwtPointSeriesData(binCountData));

      plot->add(histCurve);
    }
  }
  if (ui.IsInteractive()) plot->showWindow();
  if (!ui.IsInteractive() || ui.WasEntered("TO")) fout.close();
  p.EndProcess();
}

QColor curveColor(int i) {
  int  j = i%16;

  switch (j) {
  case 0:
    return QColor("red");
  case 6:
    return QColor("DeepPink");
  case 5:
    return QColor("orange");
  case 3:
    return QColor("black");
  case 4:
    return QColor("Indigo");
  case 2:
    return QColor("green");
  case 1:
    return QColor("Blue");
  case 7:
    return QColor("SaddleBrown");
  case 8:
    return QColor("darkred");
  case 9:
    return QColor("palevioletred");
  case 10:
    return QColor("coral");
  case 11:
    return QColor("darkkhaki");
  case 12:
    return QColor("orchid");
  case 13:
    return QColor("lime");
  case 14:
    return QColor("cyan");
  case 15:
    return QColor("goldenrod");
  }

  return QColor("olivedrab");
}
