#include "Isis.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QString>

#include "CubePlotCurve.h"
#include "Histogram.h"
#include "HistogramItem.h"
#include "HistogramPlotWindow.h"
#include "LineManager.h"
#include "Process.h"
#include "Progress.h"
#include "QHistogram.h"
#include "UserInterface.h"


using namespace std;
using namespace Isis;

void IsisMain() {
  Process p;
  Cube *icube = p.SetInputCube("FROM");

  UserInterface &ui = Application::GetUserInterface();
  if(!ui.WasEntered("TO") && !ui.IsInteractive()) {
    QString msg = "The [TO] parameter must be entered";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  Histogram *hist;
  if (ui.WasEntered("MINIMUM") && ui.WasEntered("MAXIMUM")){
    int nbins = 0;

    if (ui.WasEntered("NBINS")){
      nbins = ui.GetInteger("NBINS");
    }
    else {
      // Calculate bins based on data type.
      // Bin calculations are based on logic in Histogram::InitializeFromCube
      if (icube->pixelType() == UnsignedByte) {
        nbins = 256;
      }
      else if (icube->pixelType() == SignedWord ||
               icube->pixelType() == UnsignedWord ||
               icube->pixelType() == UnsignedInteger ||
               icube->pixelType() == SignedInteger ||
               icube->pixelType() == Real) {
        nbins = 65536;
      }
      else {
        IString msg = "Unsupported pixel type";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    hist = new Histogram(ui.GetDouble("MINIMUM"), ui.GetDouble("MAXIMUM"), nbins);
  }
  else {
    hist = new Histogram(*icube, 1, p.Progress());
  }
  // Setup the histogram

  // Loop and accumulate histogram
  p.Progress()->SetText("Gathering Histogram");
  p.Progress()->SetMaximumSteps(icube->lineCount());
  p.Progress()->CheckStatus();
  LineManager line(*icube);

  for(int i = 1; i <= icube->lineCount(); i++) {
    line.SetLine(i);
    icube->read(line);
    hist->AddData(line.DoubleBuffer(), line.size());
    p.Progress()->CheckStatus();
  }

  if(!ui.IsInteractive() || ui.WasEntered("TO") ) {
    // Write the results
    QString outfile = ui.GetFileName("TO");
    ofstream fout;
    fout.open(outfile.toLatin1().data());

    fout << "Cube:           " << ui.GetFileName("FROM") << endl;
    fout << "Band:           " << icube->bandCount() << endl;
    fout << "Average:        " << hist->Average() << endl;
    fout << "Std Deviation:  " << hist->StandardDeviation() << endl;
    fout << "Variance:       " << hist->Variance() << endl;
    fout << "Median:         " << hist->Median() << endl;
    fout << "Mode:           " << hist->Mode() << endl;
    fout << "Skew:           " << hist->Skew() << endl;
    fout << "Minimum:        " << hist->Minimum() << endl;
    fout << "Maximum:        " << hist->Maximum() << endl;
    fout << endl;
    fout << "Total Pixels:    " << hist->TotalPixels() << endl;
    fout << "Valid Pixels:    " << hist->ValidPixels() << endl;
    fout << "Null Pixels:     " << hist->NullPixels() << endl;
    fout << "Lis Pixels:      " << hist->LisPixels() << endl;
    fout << "Lrs Pixels:      " << hist->LrsPixels() << endl;
    fout << "His Pixels:      " << hist->HisPixels() << endl;
    fout << "Hrs Pixels:      " << hist->HrsPixels() << endl;

    //  Write histogram in tabular format
    fout << endl;
    fout << endl;
    fout << "DN,Pixels,CumulativePixels,Percent,CumulativePercent" << endl;

    Isis::BigInt total = 0;
    double cumpct = 0.0;

    for(int i = 0; i < hist->Bins(); i++) {
      if(hist->BinCount(i) > 0) {
        total += hist->BinCount(i);
        double pct = (double)hist->BinCount(i) / hist->ValidPixels() * 100.;
        cumpct += pct;

        fout << hist->BinMiddle(i) << ",";
        fout << hist->BinCount(i) << ",";
        fout << total << ",";
        fout << pct << ",";
        fout << cumpct << endl;
      }
    }
    fout.close();
  }
  // If we are in gui mode, create a histogram plot
  if(ui.IsInteractive()) {
    // Set the title for the dialog
    QString title;
    if(ui.WasEntered("TITLE") ) {
      title = ui.GetString("TITLE");
    }
    else {
      title = "Histogram Plot for " + FileName(ui.GetAsString("FROM")).name();
    }

    // Create the QHistogram, set the title & load the Isis::Histogram into it

    HistogramPlotWindow *plot = new HistogramPlotWindow(title.toLatin1().data(),
                                                        ui.TheGui());

    // Set the xaxis title if they entered one
    if(ui.WasEntered("XAXIS") ) {
      QString xaxis(ui.GetString("XAXIS"));
      plot->setAxisLabel(QwtPlot::xBottom, xaxis.toLatin1().data());
    }

    // Set the yLeft axis title if they entered one
    if(ui.WasEntered("FREQAXIS") ) {
      QString yaxis(ui.GetString("FREQAXIS"));
      plot->setAxisLabel(QwtPlot::yRight, yaxis.toLatin1().data());
    }

    // Set the yRight axis title if they entered one
    if(ui.WasEntered("PERCENTAXIS") ) {
      QString y2axis(ui.GetString("PERCENTAXIS") );
      plot->setAxisLabel(QwtPlot::yLeft, y2axis.toLatin1().data());
    }

    //Transfer data from histogram to the plotcurve
    QVector<QPointF> binCountData;
    QVector<QPointF> cumPctData;
    double cumpct = 0.0;
    for(int i = 0; i < hist->Bins(); i++) {
      if(hist->BinCount(i) > 0) {
        binCountData.append(QPointF(hist->BinMiddle(i), hist->BinCount(i) ) );

        double pct = (double)hist->BinCount(i) / hist->ValidPixels() * 100.;
        cumpct += pct;
        cumPctData.append(QPointF(hist->BinMiddle(i), cumpct) );
      }
    }

    HistogramItem *histCurve = new HistogramItem();
    histCurve->setColor(Qt::darkCyan);
    histCurve->setTitle("Frequency");

    CubePlotCurve *cdfCurve = new CubePlotCurve(CubePlotCurve::CubeDN,
                                                CubePlotCurve::Percentage);
    cdfCurve->setStyle(QwtPlotCurve::Lines);
    cdfCurve->setTitle("Percentage");

    QPen *pen = new QPen(Qt::red);
    pen->setWidth(2);
    histCurve->setYAxis(QwtPlot::yRight);
    cdfCurve->setYAxis(QwtPlot::yLeft);
    cdfCurve->setPen(*pen);

    //These are all variables needed in the following for loop.
    //----------------------------------------------
    QVector<QwtIntervalSample> intervals(binCountData.size() );
//     double maxYValue = DBL_MIN;
//     double minYValue = DBL_MAX;
//     // ---------------------------------------------
//
    for(int y = 0; y < binCountData.size(); y++) {

      intervals[y].interval = QwtInterval(binCountData[y].x(),
                                          binCountData[y].x() + hist->BinSize());

      intervals[y].value = binCountData[y].y();
//       if(values[y] > maxYValue) maxYValue = values[y];
//       if(values[y] < minYValue) minYValue = values[y];
    }

    QPen percentagePen(Qt::red);
    percentagePen.setWidth(2);
    cdfCurve->setColor(Qt::red);
    cdfCurve->setMarkerSymbol(QwtSymbol::NoSymbol);

    histCurve->setData(QwtIntervalSeriesData(intervals));
    cdfCurve->setData(new QwtPointSeriesData(cumPctData));

    plot->add(histCurve);
    plot->add(cdfCurve);
//     plot->fillTable();

//     plot->setScale(QwtPlot::yLeft, 0, maxYValue);
//     plot->setScale(QwtPlot::xBottom, hist.Minimum(), hist.Maximum());

    QLabel *label = new QLabel("  Average = " + QString::number(hist->Average()) + '\n' +
           "\n  Minimum = " + QString::number(hist->Minimum()) + '\n' +
           "\n  Maximum = " + QString::number(hist->Maximum()) + '\n' +
           "\n  Stand. Dev.= " + QString::number(hist->StandardDeviation()) + '\n' +
           "\n  Variance = " + QString::number(hist->Variance()) + '\n' +
           "\n  Median = " + QString::number(hist->Median()) + '\n' +
           "\n  Mode = " + QString::number(hist->Mode()) + '\n' +
           "\n  Skew = " + QString::number(hist->Skew()), plot);
    plot->getDockWidget()->setWidget(label);

    plot->showWindow();
  }

  p.EndProcess();
}
