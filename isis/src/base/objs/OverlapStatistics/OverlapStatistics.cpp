/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2009/06/26 17:08:30 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "OverlapStatistics.h"
#include "MultivariateStatistics.h"
#include "Cube.h"
#include "Filename.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "iException.h"
#include "Brick.h"
#include "Progress.h"
#include <cfloat>
#include <iomanip>

using namespace std;
namespace Isis {

 /**
  * Constructs an OverlapStatistics object.  Compares the two input cubes and
  * finds where they overlap.
  *
  * @param x The first input cube
  * @param y The second input cube
  * @param progressMsg (Default value of "Gathering Overlap Statistics") Text 
  *         for indicating progress during statistic gathering
  * @param sampPercent (Default value of 100.0) Sampling percent, or the percentage 
  *       of lines to consider during the statistic gathering procedure
  *
  * @throws Isis::iException::User - All images must have the same number of
  *                                  bands
  */
  OverlapStatistics::OverlapStatistics(Isis::Cube &x, Isis::Cube &y, 
        std::string progressMsg, double sampPercent) {
    // Test to ensure sampling percent in bound
    if (sampPercent <= 0.0 || sampPercent > 100.0) {
      string msg = "The sampling percent must be a decimal (0.0, 100.0]";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
    
    p_sampPercent = sampPercent;

    // Extract filenames and band number from cubes
    p_xFile = x.Filename();
    p_yFile = y.Filename();

    // Make sure number of bands match
    if (x.Bands() != y.Bands()) {
      string msg = "Number of bands do not match between cubes [" +
                   p_xFile.Name() + "] and [" + p_yFile.Name() + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }    
    p_bands = x.Bands();
    p_stats.resize(p_bands);

    //Create projection from each cube
    Projection *projX = x.Projection();
    Projection *projY = y.Projection();

    // Test to make sure projection parameters match
    if (*projX != *projY) {
      string msg = "Mapping groups do not match between cubes [" +
                   p_xFile.Name() + "] and [" + p_yFile.Name() + "]";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    // Figure out the x/y range for both images to find the overlap
    double Xmin1 = projX->ToProjectionX(0.5);
    double Ymax1 = projX->ToProjectionY(0.5);
    double Xmax1 = projX->ToProjectionX(x.Samples()+0.5);
    double Ymin1 = projX->ToProjectionY(x.Lines()+0.5);

    double Xmin2 = projY->ToProjectionX(0.5);
    double Ymax2 = projY->ToProjectionY(0.5);
    double Xmax2 = projY->ToProjectionX(y.Samples()+0.5);
    double Ymin2 = projY->ToProjectionY(y.Lines()+0.5);

    // Find overlap
    if ((Xmin1<Xmax2) && (Xmax1>Xmin2) && (Ymin1<Ymax2) && (Ymax1>Ymin2)) {
      double minX = Xmin1 > Xmin2 ? Xmin1 : Xmin2;
      double minY = Ymin1 > Ymin2 ? Ymin1 : Ymin2;
      double maxX = Xmax1 < Xmax2 ? Xmax1 : Xmax2;
      double maxY = Ymax1 < Ymax2 ? Ymax1 : Ymax2;

      // Find Sample range of the overlap
      p_minSampX = (int)(projX->ToWorldX(minX) + 0.5);
      p_maxSampX = (int)(projX->ToWorldX(maxX) + 0.5);
      p_minSampY = (int)(projY->ToWorldX(minX) + 0.5);
      p_maxSampY = (int)(projY->ToWorldX(maxX) + 0.5);
      p_sampRange = p_maxSampX - p_minSampX + 1;

      // Test to see if there was only sub-pixel overlap
      if (p_sampRange <= 0) return;

      // Find Line range of overlap
      p_minLineX = (int)(projX->ToWorldY(maxY) + 0.5);
      p_maxLineX = (int)(projX->ToWorldY(minY) + 0.5);
      p_minLineY = (int)(projY->ToWorldY(maxY) + 0.5);
      p_maxLineY = (int)(projY->ToWorldY(minY) + 0.5);
      p_lineRange = p_maxLineX - p_minLineX + 1;

      // Print percent processed
      Progress progress;
      progress.SetText(progressMsg);   
      
      int linc = (int)(100.0 / sampPercent + 0.5); // Calculate our line increment      
      
      // Define the maximum number of steps to be our line range divided by the
      // line increment, but if they do not divide evenly, then because of
      // rounding, we need to do an additional step for each band
      int maxSteps = (int)(p_lineRange / linc + 0.5);
      
      if (p_lineRange % linc != 0) maxSteps += 1;
      maxSteps *= p_bands;
      
      
      progress.SetMaximumSteps(maxSteps);
      progress.CheckStatus();

      // Collect and store off the overlap statistics
      for (int band=1; band<=p_bands; band++) {
        Brick b1(p_sampRange,1,1,x.PixelType());
        Brick b2(p_sampRange,1,1,y.PixelType());        
        
        int i=0;
        while (i<p_lineRange) {          
          b1.SetBasePosition(p_minSampX,(i+p_minLineX),band);
          b2.SetBasePosition(p_minSampY,(i+p_minLineY),band);
          x.Read(b1);
          y.Read(b2);
          p_stats[band-1].AddData(b1.DoubleBuffer(), b2.DoubleBuffer(), p_sampRange);          
          
          // Make sure we consider the last line
          if (i+linc > p_lineRange-1 && i != p_lineRange-1) {
            i = p_lineRange-1;
            progress.AddSteps(1);
          }
          else i+=linc; // Increment the current line by our incrementer
          
          progress.CheckStatus();
        }
      }
    }
  }

 /**
  * Checks all bands of the cubes for an overlap, and will only return false
  * if none of the bands overlap
  *
  * @return bool Returns true if any of the bands overlap, and false if none
  *              of the bands overlap
  */
  bool OverlapStatistics::HasOverlap() {
    for (int b=0; b<p_bands; b++) {
      if (p_stats[b].ValidPixels()>0) return true;
    }
    return false;
  }

 /**
  * Creates a pvl of various useful data obtained by the overlap statistics
  * class.  The pvl is returned in an output stream
  *  
  * @param os The output stream to write to
  * @param stats The OverlapStatistics object to write to os
  * @return ostream Pvl of useful statistics
  */
  std::ostream& operator<<(std::ostream &os, Isis::OverlapStatistics &stats) {
    // Output the private variables
    try {
    PvlObject o ("OverlapStatistics");
    PvlGroup gX ("File1");
    PvlKeyword stsX ("StartSample", stats.StartSampleX());
    PvlKeyword ensX ("EndSample", stats.EndSampleX());
    PvlKeyword stlX ("StartLine", stats.StartLineX());
    PvlKeyword enlX ("EndLine", stats.EndLineX());
    PvlKeyword avgX ("Average");
    PvlKeyword stdX ("StandardDeviation");
    PvlKeyword varX ("Variance");    
    for (int band=1; band<=stats.Bands(); band++) {
      if (stats.HasOverlap(band)) {
        avgX += stats.GetMStats(band).X().Average();
        stdX += stats.GetMStats(band).X().StandardDeviation();
        varX += stats.GetMStats(band).X().Variance();
      }
    }
    gX += stsX;
    gX += ensX;
    gX += stlX;
    gX += enlX;
    gX += avgX;
    gX += stdX;
    gX += varX;   
    
    PvlGroup gY ("File2");
    PvlKeyword stsY ("StartSample", stats.StartSampleY());
    PvlKeyword ensY ("EndSample", stats.EndSampleY());
    PvlKeyword stlY ("StartLine", stats.StartLineY());
    PvlKeyword enlY ("EndLine", stats.EndLineY());
    PvlKeyword avgY ("Average");
    PvlKeyword stdY ("StandardDeviation");
    PvlKeyword varY ("Variance");    
    for (int band=1; band<=stats.Bands(); band++) {
      if (stats.HasOverlap(band)) {
        avgY += stats.GetMStats(band).Y().Average();
        stdY += stats.GetMStats(band).Y().StandardDeviation();
        varY += stats.GetMStats(band).Y().Variance();
      }
    }
    gY += stsY;
    gY += ensY;
    gY += stlY;
    gY += enlY;
    gY += avgY;
    gY += stdY;
    gY += varY;    
    
    o += PvlKeyword("File1", stats.FilenameX().Name());
    o += PvlKeyword("File2", stats.FilenameY().Name());
    o += PvlKeyword("Width", stats.Samples());
    o += PvlKeyword("Height", stats.Lines());
    o += PvlKeyword("SamplingPercent", stats.SampPercent());
    o.AddGroup(gX);
    o.AddGroup(gY);
    
    PvlKeyword cov ("Covariance");
    PvlKeyword cor ("Correlation");
    
    PvlKeyword valid ("ValidOverlap");
    PvlKeyword val ("ValidPixels");
    PvlKeyword inv ("InvalidPixels");
    PvlKeyword tot ("TotalPixels");
    for (int band=1; band<=stats.Bands(); band++) {
      if (stats.HasOverlap(band)) {
        std::string validStr = "false";
        if (stats.IsValid(band)) validStr = "true";
        valid += validStr;
        cov += stats.GetMStats(band).Covariance();
        cor += stats.GetMStats(band).Correlation();
        val += stats.GetMStats(band).ValidPixels();
        inv += stats.GetMStats(band).InvalidPixels();
        tot += stats.GetMStats(band).TotalPixels();
      }
    }
    o += valid;
    o += cov;
    o += cor;
    o += val;
    o += inv;
    o += tot;
    
    for (int band=1; band<=stats.Bands(); band++) {
      if (stats.HasOverlap(band)) {
        iString bandNum (band);
        std::string bandStr = "LinearRegression" + bandNum;
        PvlKeyword LinReg(bandStr);
        double a,b;
        try {
          stats.GetMStats(band).LinearRegression(a,b);
          LinReg += a;
          LinReg += b;
        }
        catch (iException &e) {
          // It is possible one of the overlaps was constant and therefore
          // the regression would be a vertical line (x=c instead of y=ax+b)
          e.Clear();
        }
        o += LinReg;
      }
    } 
    
    os << o << endl;
    return os;
    }
    catch (iException &e) {
      string msg = "Trivial overlap between [" + stats.FilenameX().Name();
      msg += "] and [" + stats.FilenameY().Name() + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

  }


}
