#if !defined(PointPlot_h)
#define PointPlot_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2009/09/09 23:42:41 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include "Camera.h"
#include "Stereo.h"
#include "SmtkPoint.h"
#include "Statistics.h"

namespace Isis {

  /**                                                                       
   * @brief Provide stereo information/data for a point or relationship 
   *  
   * This class serves as a functor object to collect points within an ISIS 
   * output tile, perhaps the most efficient access method to ISIS cubes.  It 
   * will collect pointers to valid SmtkPoints that fall within a the 
   * boundaries of a tile.  It allows for some expansion of the tile is edges 
   * have a little extra point coverage as lat/lon coordinates from the stereo 
   * matching is used to compute the output pixel coordinate of the tile. 
   *  
   * @author  2009-09-11 Kris Becker
   *
   * @internal
   */                                                                       
  class PointPlot  {
    public:

      /**
       * @brief Construct a PointPlot object
       */
      PointPlot () { }
       
      /** Construct with a generalized buffer with extents around the buffer */
      PointPlot(const Buffer &bmap, const double &extent = 0.0) {
        m_extent = extent;
        ssamp = bmap.Sample(0);
        sline = bmap.Line(0);
        esamp = bmap.Sample(bmap.size()-1);
        eline = bmap.Line(bmap.size()-1);
        m_outPnts = 0;
      }

      /** Destructor for PointPlot */
      virtual ~PointPlot() {}

      /** Number of points in buffer */
      int size() const { return (m_points.size()); }

      /** Operator used in functor object point selection */
      void operator() (SmtkPoint &point) {
        if (!inBuffer(point.getLeft(), m_extent))  return;
        m_points.push_back(&point);
        return;
      }

      /** Fill the buffer with the points collected using stereo matching */
      BigInt FillPoints(Camera &lhcam, Camera &rhcam, int boxsize,
                     Buffer &dem, Buffer &stErr, Buffer &eigen,
                     Statistics *stAng = 0) {

        //  Initialize buffer to NULLs
        dem = Null;
        stErr = Null;
        eigen = Null;
        if (size() == 0) return (0);
        
        m_outPnts = 0;
        for (unsigned i = 0 ; i < m_points.size() ; i++) {
          SmtkPoint *point(m_points[i]);
          double line = point->getLeft().getLine();
          double samp = point->getLeft().getSample();
          double ev(point->GoodnessOfFit());
          if (lhcam.SetImage(samp, line) && lhcam.InCube()) {
            double rhLine = point->getRight().getLine();
            double rhSamp = point->getRight().getSample();
            if (rhcam.SetImage(rhSamp, rhLine) && rhcam.InCube()) {
              double radius, lat, lon, sepang, error;
              if (Stereo::Elevation(lhcam, rhcam, radius, lat, lon, sepang, error)) {
                int index;
                if (WithinTile(lhcam, lat, lon, radius, dem, index)) {
                  double elevation = radius - lhcam.LocalRadius().GetMeters();
                  dem[index] = elevation;
                  stErr[index] = error;
                  eigen[index] = ev;
                  if (stAng) stAng->AddData(sepang);
                  m_outPnts++;
                }
              }
            }
          }
        }
        return (m_outPnts);
      }

      /** Number points  */
      BigInt PointsOut() const { return (m_outPnts); }

    private:
      double sline, eline;
      double ssamp, esamp;
      double m_extent;
      std::vector<SmtkPoint *> m_points;
      BigInt m_outPnts;

      /** Checks a point to determine if it falls within a buffer */
      bool inBuffer(const Coordinate &pnt,
                    const double extent = 0.0) {
        if (pnt.getLine() < (sline-extent)) return (false);
        if (pnt.getLine() > (eline+extent)) return (false);
        if (pnt.getSample() < (ssamp-extent)) return (false);
        if (pnt.getSample() > (esamp+extent)) return (false);
        return (true);
      }

      /** Checks a latitude/longitude coordinate if it falls within a buffer */
      bool WithinTile(Camera &cam, const double &latitude, const double &longitude, 
                      const double radius, Buffer &obuf, int &index) {
        if (!(cam.SetUniversalGround(latitude, longitude) && cam.InCube())) {
          return (false);
        }

        double line = cam.Line();
        double samp = cam.Sample();
        if (!inBuffer(Coordinate(line,samp), 0.5)) {
          return (false);
        }
        index = obuf.Index(static_cast<int>(samp+0.5), 
                           static_cast<int>(line+0.5),1);
        return (true);
      }
  };
};

#endif
