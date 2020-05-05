#include <cmath>

#include "grid.h"

#include "Angle.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "GroundGrid.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "ProcessByLine.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "UniversalGroundMap.h"

using namespace std;
using namespace Isis;

namespace Isis {
  class ImageGridFunctor {
    private:
      int m_baseLine;
      int m_baseSample;
      int m_sampleInc;
      int m_lineInc;
      int m_lineWidth;
      int m_tickSize;
      int m_numSamples;
      int m_numLines;
      double m_lineValue;
      double m_bkgndValue;
      bool m_outline;
      bool m_ticks;
      bool m_diagonalTicks;
      bool m_useImage;

    public:
      /**
       * Default Constructor
       * @param offset       The minimum value of the input cube's pixel type
       * @param defaultValue Value used for pixels that are not taken from a cube
       */
      ImageGridFunctor(int baseLine, int baseSample, int sampleInc, int lineInc, int lineWidth, 
                       int numSamples, int numLines, double lineValue, double bkgndValue, bool outline, 
                       bool image, bool ticks, int tickSize, bool diagTicks) {
        m_baseLine = baseLine;
        m_baseSample = baseSample;
        m_sampleInc = sampleInc;
        m_lineInc = lineInc;
        m_lineWidth = lineWidth;
        m_numSamples = numSamples;
        m_numLines = numLines;
        m_lineValue = lineValue;
        m_bkgndValue = bkgndValue;
        m_outline = outline;
        m_useImage = image;
        m_ticks = ticks;
        m_diagonalTicks = diagTicks;
        m_tickSize = tickSize;
      }
      
      /**
       * Copies DN's from the input cube to the tracking cube, subtracts the old offset, and adds
       * the new offset to each pixel.
       *
       * @param in  Input cube
       * @param out Mosaic cube
       */
      void operator()(Buffer &in, Buffer &out) const {
        for (int samp = 1; samp <= in.size(); samp ++) {
          if (m_useImage)
            out[samp - 1] = in[samp - 1];
          else 
            out[samp - 1] = m_bkgndValue;

          if (!m_ticks) {
            if (imageDrawSample(samp) || imageDrawLine(in.Line())) {
              out[samp - 1] = m_lineValue;
            }
          }
          // ticks!
          else {
            // tickSize is the width or height divided by 2, so integer rounding
            //   takes care of the current sample/line while doing +/- tickSize
            //   creates the appropriate width and height.

            // Vertical/Horizontal Ticks
            if (!m_diagonalTicks) {
              // horizontal test
              for (int sampleTest = samp - m_tickSize;
                  (sampleTest <= samp + m_tickSize) && (out[samp - 1] != m_lineValue);
                  sampleTest ++) {
                if (imageDrawLine(in.Line()) && imageDrawSample(sampleTest)) {
                  out[samp - 1] = m_lineValue;
                }
              }

              // vertical test
              for (int lineTest = in.Line() - m_tickSize;
                  (lineTest <= in.Line() + m_tickSize) && (out[samp - 1] != m_lineValue);
                  lineTest ++) {
                if (imageDrawLine(lineTest) && imageDrawSample(samp)) {
                  out[samp - 1] = m_lineValue;
                }
              }
            }
            // Diagonal Ticks
            else {
              // top left to bottom right
              int sampleTest = samp - m_tickSize;
              int lineTest = in.Line() - m_tickSize;

              while ((out[samp - 1] != m_lineValue) &&
                    (lineTest <= in.Line() + m_tickSize) &&
                    (sampleTest <= samp + m_tickSize)) {
                if (imageDrawLine(lineTest) && imageDrawSample(sampleTest)) {
                  out[samp - 1] = m_lineValue;
                }

                sampleTest ++;
                lineTest ++;
              }

              // top right to bottom left
              sampleTest = samp + m_tickSize;
              lineTest = in.Line() - m_tickSize;

              while ((out[samp - 1] != m_lineValue) &&
                    (lineTest <= in.Line() + m_tickSize) &&
                    (sampleTest >= samp - m_tickSize)) {
                if (imageDrawLine(lineTest) && imageDrawSample(sampleTest)) {
                  out[samp - 1] = m_lineValue;
                }

                sampleTest --;
                lineTest ++;
              }
            }
          }
        }

        // draw outline
        if (m_outline) {
          if (in.Line() - 1 <= m_lineWidth * 2 ||
             in.Line() >= m_numLines - m_lineWidth * 2) {
            for (int i = 0; i < in.size(); i++) {
              out[i] = Hrs;
            }
          }
          else {
            for (int i = 0; i <= m_lineWidth * 2; i++)
              out[i] = Hrs;

            for (int i = m_numSamples - m_lineWidth * 2 - 1; i < in.size(); i++)
              out[i] = Hrs;
          }
        }
      }

      bool imageDrawLine(int line) const {
        bool drawLine = false;

        for (int y = line - m_lineWidth; y <= line + m_lineWidth; y ++) {
          drawLine = drawLine || (y % m_lineInc == m_baseLine % m_lineInc);
        }

        return drawLine;
      }

      bool imageDrawSample(int samp) const {
        bool drawSamp = false;

        for (int x = samp - m_lineWidth; x <= samp + m_lineWidth; x ++) {
          drawSamp = drawSamp || (x % m_sampleInc == m_baseSample % m_sampleInc);
        }

        return drawSamp;
      }
  };

  class GroundGridFunctor {
    private:
      int m_lineWidth;
      int m_tickSize;
      int m_numSamples;
      int m_numLines;
      double m_lineValue;
      double m_bkgndValue;
      bool m_outline;
      bool m_ticks;
      bool m_diagonalTicks;
      bool m_useImage;
      bool m_recalculateForEachBand;
      bool m_walkBoundary;
      bool m_extendGrid;
      Latitude m_baseLat; 
      Longitude m_baseLon;
      Latitude m_minLat;
      Latitude m_maxLat; 
      Longitude m_minLon;
      Longitude m_maxLon; 
      Angle m_latInc;
      Angle m_lonInc;
      UniversalGroundMap *m_gmap;
      GroundGrid *m_latLonGrid;

      int tempValue = 0;
      int *m_currentBand = &tempValue;

    public:
      GroundGridFunctor(int lineWidth, int tickSize, int numSamples, int numLines, double lineValue, 
                        double bkgndValue, bool outline, bool image, bool ticks, bool diagTicks, 
                        bool recalculateForEachBand, bool walkBoundary, bool extendGrid, 
                        Latitude baseLat, Longitude baseLon, Latitude minLat, Latitude maxLat, 
                        Longitude minLon, Longitude maxLon, Angle latInc, Angle lonInc, 
                        UniversalGroundMap *gmap, GroundGrid *latLonGrid) {
        m_lineWidth = lineWidth;
        m_tickSize = tickSize;
        m_numSamples = numSamples;
        m_numLines = numLines;
        m_lineValue = lineValue;
        m_bkgndValue = bkgndValue;
        m_outline = outline;
        m_useImage = image;
        m_ticks = ticks;
        m_diagonalTicks = diagTicks;
        m_recalculateForEachBand = recalculateForEachBand;
        m_walkBoundary = walkBoundary;
        m_extendGrid = extendGrid;
        m_baseLat = baseLat; 
        m_baseLon = baseLon;
        m_minLat = minLat;
        m_maxLat = maxLat; 
        m_minLon = minLon;
        m_maxLon = maxLon;  
        m_latInc = latInc;
        m_lonInc = lonInc;
        m_gmap = gmap;
        m_latLonGrid = latLonGrid;
      }
      
      /**
       * Copies DN's from the input cube to the tracking cube, subtracts the old offset, and adds
       * the new offset to each pixel.
       *
       * @param in  Input cube
       * @param out Mosaic cube
       */
      void operator()(Buffer &in, Buffer &out) const {

        // check to see if we're in the same band 
        if ( (*m_currentBand != in.Band()) && m_recalculateForEachBand ) {
          *m_currentBand = in.Band(); 
          changeBand(*m_currentBand); 
        }


        for (int samp = 1; samp <= in.SampleDimension(); samp++) {
          if (!m_ticks) {
            if (groundDrawPoint(samp, in.Line())) {
              out[samp - 1] = m_lineValue;
            }
            else {
              if (m_useImage) {
                out[samp - 1] = in[samp - 1];
              }
              else {
                out[samp - 1] = m_bkgndValue;
              }
            }
          }
          else {
            // We need to check the grids for overlaps near current point
            out[samp - 1] = in[samp - 1];

            // tickSize is the width or height divided by 2, so integer rounding
            //   takes care of the current sample/line while doing +/- tickSize
            //   creates the appropriate width and height.

            // Vertical/Horizontal Ticks
            if (!m_diagonalTicks) {
              // horizontal test
              for (int sampleTest = samp - m_tickSize;
                  (sampleTest <= samp + m_tickSize) && (out[samp - 1] != Hrs);
                  sampleTest ++) {
                if (groundDrawPoint(sampleTest, in.Line(), true) &&
                    groundDrawPoint(sampleTest, in.Line(), false)) {
                  out[samp - 1] = m_lineValue;
                }
              }

              // vertical test
              for (int lineTest = in.Line() - m_tickSize;
                  (lineTest <= in.Line() + m_tickSize) && (out[samp - 1] != Hrs);
                  lineTest ++) {
                if (groundDrawPoint(samp, lineTest, true) &&
                    groundDrawPoint(samp, lineTest, false)) {
                  out[samp - 1] = m_lineValue;
                }
              }
            }
            // Diagonal ticks
            else {
              // top left to bottom right
              int sampleTest = samp - m_tickSize;
              int lineTest = in.Line() - m_tickSize;


              while ((out[samp - 1] != Hrs) &&
                    (lineTest <= in.Line() + m_tickSize) &&
                    (sampleTest <= samp + m_tickSize)) {
                if (groundDrawPoint(sampleTest, lineTest, true) &&
                    groundDrawPoint(sampleTest, lineTest, false)) {
                  out[samp - 1] = m_lineValue;
                }

                sampleTest++;
                lineTest++;
              }

              // top right to bottom left
              sampleTest = samp + m_tickSize;
              lineTest = in.Line() - m_tickSize;

              while ((out[samp - 1] != Hrs) &&
                    (lineTest <= in.Line() + m_tickSize) &&
                    (sampleTest >= samp - m_tickSize)) {
                if (groundDrawPoint(sampleTest, lineTest, true) &&
                    groundDrawPoint(sampleTest, lineTest, false)) {
                  out[samp - 1] = m_lineValue;
                }

                sampleTest--;
                lineTest++;
              }
            }
          }
        }

        // draw outline
        if (m_outline) {
          if (in.Line() - 1 <= m_lineWidth * 2 ||
             in.Line() >= m_numLines - m_lineWidth * 2) {
            for (int i = 0; i < in.size(); i++) {
              out[i] = Hrs;
            }
          }
          else {
            for (int i = 0; i <= m_lineWidth * 2; i++)
              out[i] = Hrs;

            for (int i = m_numSamples - m_lineWidth * 2 - 1; i < in.size(); i++)
              out[i] = Hrs;
          }
        }
      }

      void changeBand(int band) const { 
        Progress progress;

        // Since changeBand changes m_latLonGrid and this has to be a const method, save the value of
        // m_latLonGrid in a new variable that can be changed
        GroundGrid *latLonGrid = m_latLonGrid;

        // change band of UniversalGroundMap
        m_gmap->SetBand(band);

        // update latLonGrid to use new UniversalGroundMap
        GroundGrid newGrid(m_gmap, m_ticks, m_extendGrid, m_numSamples, m_numLines);
        *latLonGrid = newGrid;

        // re-set old ground limits from GUI
        latLonGrid->SetGroundLimits(m_minLat, m_minLon, m_maxLat, m_maxLon);

        // If the grid is not going to reach the min/max lon warn the user.
        if (!m_extendGrid) {
          // Check that the min/max lon values match the lon domain
          if ( latLonGrid->GetMappingGroup()->findKeyword("LongitudeDomain")[0] == "360" &&
              (latLonGrid->minLongitude().degrees() < 0.0 ||
                latLonGrid->maxLongitude().degrees() > 360.0) ) {
            QString msg = "**WARNING** minimum longitude ["
                          + toString( latLonGrid->minLongitude().degrees() )
                          + "] and maximum longitude ["
                          + toString( latLonGrid->maxLongitude().degrees() )
                          + "] are not in the 360 degree longitude domain and "
                            "the EXTENDGRID parameter is set to false. "
                            "Output grid may not cover the entire map projection for band["
                          + toString(band) + "].";
            std::cerr << msg << std::endl;
          }
          else if ( latLonGrid->GetMappingGroup()->findKeyword("LongitudeDomain")[0] == "180" &&
                    (latLonGrid->minLongitude().degrees() < -180.0 ||
                    latLonGrid->maxLongitude().degrees() > 180.0) ) {
            QString msg = "**WARNING** minimum longitude ["
                          + toString( latLonGrid->minLongitude().degrees() )
                          + "] and maximum longitude ["
                          + toString( latLonGrid->maxLongitude().degrees() )
                          + "] are not in the 180 degree longitude domain and "
                            "the EXTENDGRID parameter is set to false. "
                            "Output grid may not cover the entire map projection for band["
                          + toString(band) + "].";
            std::cerr << msg << std::endl;
          }
        }

        QString progressMessage = QString("Recalculating grid for band %1").arg(band);
        progress.SetText(progressMessage);

        // re-set lat/lon base/in from GUI
        latLonGrid->CreateGrid(m_baseLat, m_baseLon, m_latInc, m_lonInc, &progress);

        if (m_walkBoundary) {
          latLonGrid->WalkBoundary();
        }
      }

      bool groundDrawPoint(int samp, int line, bool latGrid = true) const {
        bool drawPoint = false;

        for (int x = samp - m_lineWidth; x <= samp + m_lineWidth; x ++) {
          drawPoint = drawPoint || m_latLonGrid->PixelOnGrid(x - 1, line - 1, latGrid);
        }

        for (int y = line - m_lineWidth; y <= line + m_lineWidth; y ++) {
          drawPoint = drawPoint || m_latLonGrid->PixelOnGrid(samp - 1, y - 1, latGrid);
        }

        return drawPoint;
      }
  };

  void grid(UserInterface &ui, Pvl *log) {
    Cube icube;
    CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
    if (inAtt.bands().size() != 0) {
      icube.setVirtualBands(inAtt.bands());
    }
    icube.open(ui.GetFileName("FROM"));
    grid(&icube, ui, log);
  }

  void grid(Cube *icube, UserInterface &ui, Pvl *log) {
    // We will be processing by line
    ProcessByLine p;
    p.SetInputCube(icube);

    QString mode = ui.GetString("MODE");

    bool outline = ui.GetBoolean("OUTLINE");
    bool ticks = ui.GetBoolean("TICKS");
    bool extendGrid = ui.GetBoolean("EXTENDGRID");

    int tickSize;
    bool diagonalTicks;
    if (ticks) {
      tickSize = ui.GetInteger("TICKSIZE") / 2;
      diagonalTicks = ui.GetBoolean("DIAGONALTICKS");
    }
    else {
      tickSize = 0;
      diagonalTicks = false;
    }

    int lineWidth = ui.GetInteger("LINEWIDTH") / 2;
   
    QString bval = ui.GetString("BKGNDVALUE").toUpper();

    bool image = false;
    double bkgndValue;

    if (bval == "IMAGE") {
      image = true;
      bkgndValue = Null;
    }
    if (bval == "HRS") {
      bkgndValue = Hrs;
    }
    else if (bval == "LRS") {
      bkgndValue = Lrs;
    }
    else if (bval == "DN") {
      bkgndValue = ui.GetDouble("BKGNDDNVALUE");
    }

    QString lval = ui.GetString("LINEVALUE").toUpper();
    double lineValue;
    if (lval == "HRS") {
      lineValue = Hrs;
    }
    else if (lval == "LRS") {
      lineValue = Lrs;
    }
    else if (lval == "NULL") {
      lineValue = Null;
    }
    else if (lval == "DN") {
      if (ui.WasEntered("DNVALUE")) {
        lineValue = ui.GetDouble("DNVALUE");
      }
      else {
        throw IException(IException::User, "Must enter value in DNVALUE", _FILEINFO_);
      }
    }

    int inputSamples = icube->sampleCount();
    int inputLines = icube->lineCount();
    int inputBands = icube->bandCount();

    // Line & sample based grid
    if (mode == "IMAGE") {
      p.SetOutputCube("TO");
      int baseLine = ui.GetInteger("BASELINE");
      int baseSample = ui.GetInteger("BASESAMPLE");
      int lineInc = ui.GetInteger("LINC");
      int sampleInc = ui.GetInteger("SINC");
      ImageGridFunctor imageGrid(baseLine, baseSample, sampleInc, lineInc, lineWidth, 
                                        inputSamples, inputLines, lineValue, bkgndValue, outline, 
                                        image, ticks, tickSize, diagonalTicks);

      p.ProcessCube(imageGrid, false);
      p.EndProcess();
    }
    // Lat/Lon based grid
    else {
      p.SetOutputCube("TO");

      // if > 1 input band and IsBandIndependent = false, need to regenerate grid for 
      // each band
      bool recalculateForEachBand = false;
      if (icube->hasGroup("Instrument")) {
        if ((inputBands >= 2) && !(icube->camera()->IsBandIndependent())) {
          recalculateForEachBand = true; 
        }
      }

      UniversalGroundMap *gmap = new UniversalGroundMap(*icube, UniversalGroundMap::ProjectionFirst);

      GroundGrid *latLonGrid = new GroundGrid(gmap, ticks, extendGrid, inputSamples, inputLines);

      Latitude baseLat = Latitude(ui.GetDouble("BASELAT"), *latLonGrid->GetMappingGroup(), Angle::Degrees);
      Longitude baseLon = Longitude(ui.GetDouble("BASELON"), *latLonGrid->GetMappingGroup(), Angle::Degrees);
      Angle latInc = Angle(ui.GetDouble("LATINC"), Angle::Degrees);
      Angle lonInc = Angle(ui.GetDouble("LONINC"), Angle::Degrees);

      Progress progress;
      progress.SetText("Calculating Grid");

      Latitude minLat;
      if (ui.WasEntered("MINLAT")) {
        minLat = Latitude(ui.GetDouble("MINLAT"), *latLonGrid->GetMappingGroup(), Angle::Degrees);
      }

      Latitude maxLat;
      if (ui.WasEntered("MAXLAT")) {
        maxLat = Latitude(ui.GetDouble("MAXLAT"), *latLonGrid->GetMappingGroup(), Angle::Degrees);
      }

      Longitude minLon;
      if (ui.WasEntered("MINLON")) {
        minLon = Longitude(ui.GetDouble("MINLON"), *latLonGrid->GetMappingGroup(), Angle::Degrees);
      }

      Longitude maxLon;
      if (ui.WasEntered("MAXLON")) {
        maxLon = Longitude(ui.GetDouble("MAXLON"), *latLonGrid->GetMappingGroup(), Angle::Degrees);
      }

      latLonGrid->SetGroundLimits(minLat, minLon, maxLat, maxLon);

      // If the grid is not going to reach the min/max lon warn the user.
      if (!extendGrid) {
        // Check that the min/max lon values match the lon domain
        if ( latLonGrid->GetMappingGroup()->findKeyword("LongitudeDomain")[0] == "360" &&
            (latLonGrid->minLongitude().degrees() < 0.0 ||
              latLonGrid->maxLongitude().degrees() > 360.0) ) {
          QString msg = "**WARNING** minimum longitude ["
                        + toString( latLonGrid->minLongitude().degrees() )
                        + "] and maximum longitude ["
                        + toString( latLonGrid->maxLongitude().degrees() )
                        + "] are not in the 360 degree longitude domain and "
                          "the EXTENDGRID parameter is set to false. "
                          "Output grid may not cover the entire map projection.";
          std::cerr << msg << std::endl;
        }
        else if ( latLonGrid->GetMappingGroup()->findKeyword("LongitudeDomain")[0] == "180" &&
                  (latLonGrid->minLongitude().degrees() < -180.0 ||
                  latLonGrid->maxLongitude().degrees() > 180.0) ) {
          QString msg = "**WARNING** minimum longitude ["
                        + toString( latLonGrid->minLongitude().degrees() )
                        + "] and maximum longitude ["
                        + toString( latLonGrid->maxLongitude().degrees() )
                        + "] are not in the 180 degree longitude domain and "
                          "the EXTENDGRID parameter is set to false. "
                          "Output grid may not cover the entire map projection.";
          std::cerr << msg << std::endl;
        }
      }

      latLonGrid->CreateGrid(baseLat, baseLon, latInc, lonInc, &progress);

      bool walkBoundary = false;
      if (ui.GetBoolean("BOUNDARY")) {
        latLonGrid->WalkBoundary();
        walkBoundary = true;
      }

      GroundGridFunctor groundGrid(lineWidth, tickSize, inputSamples, inputLines, lineValue, 
                        bkgndValue, outline, image, ticks, diagonalTicks, recalculateForEachBand, 
                        walkBoundary, extendGrid, baseLat, baseLon, minLat, maxLat, minLon, maxLon, 
                        latInc, lonInc, gmap, latLonGrid);
      p.ProcessCube(groundGrid, false);
      p.EndProcess();

      // delete latLonGrid;
      // latLonGrid = NULL;
      std::cout<< "Deleted latLonGrid" << std::endl;
      delete gmap;
      std::cout<< "Deleted gmap" << std::endl;
      gmap = NULL;
    }
  }
}
