/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2008/08/19 22:37:41 $                                                                 
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
#ifndef HiJitCube_h
#define HiJitCube_h

#include "Cube.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/CoordinateArraySequence.h"

namespace Isis {

  class PvlObject;
/**                                                                       
 * @brief HiRISE cube detailer for jitter characterization
 *     
 * HiJitCube is designed to open and manage HiRISE image cubes for the
 * purpose of jitter characterization. Inheriting the Isis Cube class,
 * it provides opening and closeing of the cube, but its main function
 * is gathering data from the label, validating against other instances
 * of this same object on other HiRISE cubes and computing focal plane
 * mapping expressly to determine overlapping regions of images.
 * 
 * @ingroup MarsReconnaissanceOrbiter                                                
 *                                                                        
 * @author 2006-05-04 Kris Becker
 * 
 */                                                                       
  class HiJitCube : public Cube {
   public:

     /** 
      * Structure that contains all pertinent data for the cube
      */
     struct JitInfo {
      std::string filename;
      std::string productId;
      int lines;
      int samples;
      int sampOffset;
      int lineOffset;
      int tdiMode;
      int summing;
      int channelNumber;
      int cpmmNumber;
      std::string ccdName;
      double dltCount;
      std::string UTCStartTime;
      std::string scStartTime;
      double obsStartTime;
      double unBinnedRate;
      double linerate;
      int fpSamp0;    
      int fpLine0;
      double pixpitch;
     };

     /**
      * Structure containing cube coordinates
      */
     struct CubeCoords {
       double line;
       double sample;
       CubeCoords() : line(0.0), sample(0.0) { }
     };

     /**
      * Structure containing corners of a region
      */
     struct Corners {
       CubeCoords topLeft;
       CubeCoords lowerRight;
       bool good;
       Corners() : topLeft(), lowerRight(), good(false) { }
     };

    public:
      HiJitCube();
      HiJitCube(const std::string &filename);
      HiJitCube(const std::string &filename, PvlObject &shift);
      ~HiJitCube();

      void setSampleOffset(int soff);
      void setLineOffset(int loff);

      /**
       * Returns the sample offset for this image
       */
      inline int getSampleOffset() const { return (jdata.sampOffset); }
      /**
       * Returns the line offset for this image
       */
      inline int getLineOffset() const { return (jdata.lineOffset); }

      void OpenCube(const std::string &filename);
      void OpenCube(const std::string &filename, PvlObject &shift);
      inline const JitInfo &GetInfo() const { return (jdata); }
      double getLineTime(double line = 1.0) const;


      void Compatable(HiJitCube &cube) throw (iException &);

      geos::geom::Polygon const *Poly() const { return (fpGeom); }
      bool intersects(const HiJitCube &cube) const;
      bool overlap(const HiJitCube &cube, Corners &ovlCorners);
      /**
       * Returns the string representation of the overlapping region
       */
      std::string PolyToString() const { return (fpGeom->toString()); }

    private:
      JitInfo  jdata;            //!< Cube information 
      geos::geom::Polygon *fpGeom;     //!< Polygon construction and manipulation
      static bool naifLoaded;    //!< Status of NAIF required kernels for times

      void initLocal();
      void loadNaifTiming();
      void computeStartTime();
      void Init( ) throw (iException &);
      int getBinModeIndex(int summing) const throw (iException &);
      void computePoly();
      Corners FocalPlaneToImage(const Corners &fp) const;
      Corners getCorners(const geos::geom::Geometry &poly) const;
  };

}  // namespace Isis
#endif
