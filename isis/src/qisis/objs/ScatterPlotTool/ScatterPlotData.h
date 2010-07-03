#ifndef ScatterPlotData_h
#define ScatterPlotData_h

#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>

#include "Cube.h"
#include "Stretch.h"
#include "Histogram.h"
#include "Brick.h"
#include "CubeViewport.h"

#include <vector>

namespace Qisis {

  class ScatterPlotData: public QwtRasterData {
  
  public:
      ScatterPlotData(CubeViewport *cube1, int band1, int numbBins1, CubeViewport *cube2, int band2, int numBins2);
      ~ScatterPlotData();
      virtual QwtRasterData *copy() const;
      virtual QwtDoubleInterval range() const;
      virtual double value(double x, double y) const;

      int bandOne() const { return p_band1; };
      int bandTwo() const { return p_band2; };

      int numberOfBins() const { return p_numBins1; };
      
      double minOne() const { return p_min1; };
      double minTwo() const { return p_min2; };

      double maxOne() const { return p_max1; };
      double maxTwo() const { return p_max2; };

      Isis::Cube * cubeOne() const { return p_cube1; };
      Isis::Cube * cubeTwo() const { return p_cube2; };
      
  
  private:
    Isis::Cube *p_cube1; //!< Cube 1
    Isis::Cube *p_cube2; //!< Cube 2
    CubeViewport *p_cube1Viewport; //!< CubeViewport 1
    CubeViewport *p_cube2Viewport; //!< CubeViewport 2
    Isis::Stretch p_str1; //!< Stretch 1
    Isis::Stretch p_str2; //!< Stretch 2
    std::vector< std::vector<int> > p_counts; //!< 2D Vector of ints.
    double p_min1; //!< Minimum for cube 1
    double p_min2; //!< Minimum for cube 2
    double p_max1; //!< Maximum for cube 1
    double p_max2; //!< Maximum for cube 2

    int p_band1; //!< Band for cube 1
    int p_band2; //!< Band for cube 2
    int p_numBins1; //!< Number of Bins
    int p_numBins2;
  
  };
};

#endif

