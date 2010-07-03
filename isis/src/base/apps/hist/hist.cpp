#include "Isis.h"

#include <string>
#include <QMenuBar>
#include <qwt_interval_data.h>

#include "Process.h"
#include "Histogram.h"
#include "UserInterface.h"
#include "Progress.h"
#include "LineManager.h"
#include "QHistogram.h"
#include "HistogramToolWindow.h"
#include "HistogramItem.h"
#include "PlotToolCurve.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  Process p;
  Cube *icube = p.SetInputCube("FROM");

  // Setup the histogram
  UserInterface &ui = Application::GetUserInterface();
  Histogram hist(*icube,1,p.Progress());
  if (ui.WasEntered("MINIMUM")) {
    hist.SetValidRange(ui.GetDouble("MINIMUM"),ui.GetDouble("MAXIMUM"));
  }
  if (ui.WasEntered("NBINS")) {
    hist.SetBins(ui.GetInteger("NBINS"));
  }

  // Loop and accumulate histogram
  p.Progress()->SetText("Gathering Histogram");
  p.Progress()->SetMaximumSteps(icube->Lines());
  p.Progress()->CheckStatus();
  LineManager line(*icube);
  for (int i=1; i<=icube->Lines(); i++) {
    line.SetLine(i);
    icube->Read(line);
    hist.AddData(line.DoubleBuffer(),line.size());
    p.Progress()->CheckStatus();
  }

  if(!ui.IsInteractive() || ui.WasEntered("TO")) {
    // Write the results

    if (!ui.WasEntered("TO")) {
      string msg = "The [TO] parameter must be entered";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    string outfile = ui.GetFilename("TO");
    ofstream fout;
    fout.open (outfile.c_str());
   
    fout << "Cube:           " << ui.GetFilename("FROM") << endl;
    fout << "Band:           " << icube->Bands() << endl;
    fout << "Average:        " << hist.Average() << endl;
    fout << "Std Deviation:  " << hist.StandardDeviation() << endl;
    fout << "Variance:       " << hist.Variance() << endl;
    fout << "Median:         " << hist.Median() << endl;
    fout << "Mode:           " << hist.Mode() << endl;
    fout << "Skew:           " << hist.Skew() << endl;
    fout << "Minimum:        " << hist.Minimum() << endl;
    fout << "Maximum:        " << hist.Maximum() << endl;
    fout << endl;
    fout << "Total Pixels:    " << hist.TotalPixels() << endl;
    fout << "Valid Pixels:    " << hist.ValidPixels() << endl;
    fout << "Null Pixels:     " << hist.NullPixels() << endl;
    fout << "Lis Pixels:      " << hist.LisPixels() << endl;
    fout << "Lrs Pixels:      " << hist.LrsPixels() << endl;
    fout << "His Pixels:      " << hist.HisPixels() << endl;
    fout << "Hrs Pixels:      " << hist.HrsPixels() << endl;
   
    //  Write histogram in tabular format
    fout << endl;
    fout << endl;
    fout << "DN,Pixels,CumulativePixels,Percent,CumulativePercent" << endl;
   
    Isis::BigInt total = 0;
    double cumpct = 0.0;
   
    for (int i=0; i<hist.Bins(); i++) {
      if (hist.BinCount(i) > 0) {
        total += hist.BinCount(i);
        double pct = (double)hist.BinCount(i) / hist.ValidPixels() * 100.;
        cumpct += pct;
   
        fout << hist.BinMiddle(i) << ",";
        fout << hist.BinCount(i) << ",";
        fout << total << ",";
        fout << pct << ",";
        fout << cumpct << endl;
      }
    }
    fout.close();
  }
  // If we are in gui mode, create a histogram plot
  if (ui.IsInteractive()) {
    // Set the title for the dialog
    string title;
    if (ui.WasEntered("TITLE")) {
      title = ui.GetString("TITLE");
    }
    else {
      title = "Histogram Plot for " + Filename(ui.GetAsString("FROM")).Name();
    }

    // Create the QHistogram, set the title & load the Isis::Histogram into it

    Qisis::HistogramToolWindow *plot = new Qisis::HistogramToolWindow(title.c_str(), ui.TheGui());

    // Set the xaxis title if they entered one
    if (ui.WasEntered("XAXIS")) {
      string xaxis(ui.GetString("XAXIS"));
      plot->setAxisLabel(QwtPlot::xBottom,xaxis.c_str());
    }

    // Set the yLeft axis title if they entered one
    if (ui.WasEntered("Y1AXIS")) {
      string yaxis(ui.GetString("Y1AXIS"));
      plot->setAxisLabel(QwtPlot::yLeft,yaxis.c_str());
    }

    // Set the yRight axis title if they entered one
    if (ui.WasEntered("Y2AXIS")) {
      string y2axis(ui.GetString("Y2AXIS"));
      plot->setAxisLabel(QwtPlot::yRight,y2axis.c_str());
    }

    //Transfer data from histogram to the plotcurve
    std::vector<double> xarray,yarray,y2array;
    double cumpct = 0.0;
    for (int i=0; i<hist.Bins(); i++) {
      if (hist.BinCount(i) > 0) {
        xarray.push_back(hist.BinMiddle(i));
        yarray.push_back(hist.BinCount(i));

        double pct = (double)hist.BinCount(i) / hist.ValidPixels() * 100.;
        cumpct += pct;
        y2array.push_back(cumpct);
      }
    }

    Qisis::HistogramItem *histCurve = new Qisis::HistogramItem();
    histCurve->setColor(Qt::darkCyan);
    histCurve->setTitle("Frequency");

    Qisis::PlotToolCurve *cdfCurve = new Qisis::PlotToolCurve();
    cdfCurve->setStyle(QwtPlotCurve::Lines);
    cdfCurve->setTitle("Percentage");

    QPen *pen = new QPen(Qt::red);
    pen->setWidth(2);
    histCurve->setYAxis(QwtPlot::yLeft);
    cdfCurve->setYAxis(QwtPlot::yRight);
    cdfCurve->setPen(*pen);

    //These are all variables needed in the following for loop.
    //----------------------------------------------
    QwtArray<QwtDoubleInterval> intervals(xarray.size());
    QwtArray<double> values(yarray.size());
    double maxYValue = DBL_MIN;
    double minYValue = DBL_MAX;
    // --------------------------------------------- 

    for(unsigned int y = 0; y < yarray.size(); y++) {

      intervals[y] = QwtDoubleInterval(xarray[y], xarray[y] + hist.BinSize());
  
      values[y] = yarray[y];  
      if(values[y] > maxYValue) maxYValue = values[y]; 
      if(values[y] < minYValue) minYValue = values[y];
    }
    
    histCurve->setData(QwtIntervalData(intervals, values));
    cdfCurve->setData(&xarray[0],&y2array[0],xarray.size());

    plot->add(histCurve);
    plot->add(cdfCurve);
    plot->fillTable();

    plot->setScale(QwtPlot::yLeft,0,maxYValue);
    plot->setScale(QwtPlot::xBottom,hist.Minimum(),hist.Maximum());

    QLabel *label = new QLabel("  Average = " + QString::number(hist.Average()) + '\n' +
    "\n  Minimum = " + QString::number(hist.Minimum()) + '\n' +
    "\n  Maximum = " + QString::number(hist.Maximum()) + '\n' +
    "\n  Stand. Dev.= " + QString::number(hist.StandardDeviation()) + '\n' +
    "\n  Variance = " + QString::number(hist.Variance()) + '\n' +
    "\n  Median = " + QString::number(hist.Median()) + '\n' +
    "\n  Mode = " + QString::number(hist.Mode()) +'\n' +
    "\n  Skew = " + QString::number(hist.Skew()), plot);
    plot->getDockWidget()->setWidget(label);
 
    plot->showWindow();
  }
  p.EndProcess();
}
