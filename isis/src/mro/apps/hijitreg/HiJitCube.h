/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   * @internal
   *   @history 2012-09-25 Steven Lambright - Pixel pitch value can now come from either the
   *                           instrument group or the naif keywords group. References #1094.
   *   @history 2015-07-31 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *            were signaled. References #2248.
   */
  class HiJitCube : public Cube {
    public:

      /**
       * Structure that contains all pertinent data for the cube
       */
      struct JitInfo {
        QString filename;
        QString productId;
        int lines;
        int samples;
        int sampOffset;
        int lineOffset;
        int tdiMode;
        int summing;
        int channelNumber;
        int cpmmNumber;
        QString ccdName;
        double dltCount;
        QString UTCStartTime;
        QString scStartTime;
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
      HiJitCube(const QString &filename);
      HiJitCube(const QString &filename, PvlObject &shift);
      ~HiJitCube();

      void setSampleOffset(int soff);
      void setLineOffset(int loff);

      /**
       * Returns the sample offset for this image
       */
      inline int getSampleOffset() const {
        return (jdata.sampOffset);
      }
      /**
       * Returns the line offset for this image
       */
      inline int getLineOffset() const {
        return (jdata.lineOffset);
      }

      void OpenCube(const QString &filename);
      void OpenCube(const QString &filename, PvlObject &shift);
      inline const JitInfo &GetInfo() const {
        return (jdata);
      }
      double getLineTime(double line = 1.0) const;


      void Compatable(HiJitCube &cube);

      geos::geom::Polygon const *Poly() const {
        return (fpGeom);
      }
      bool intersects(const HiJitCube &cube) const;
      bool overlap(const HiJitCube &cube, Corners &ovlCorners);
      /**
       * Returns the string representation of the overlapping region
       */
      QString PolyToString() const {
        return (fpGeom->toString().c_str());
      }

    private:
      JitInfo  jdata;            //!< Cube information
      geos::geom::Polygon *fpGeom;     //!< Polygon construction and manipulation
      static bool naifLoaded;    //!< Status of NAIF required kernels for times

      void initLocal();
      void loadNaifTiming();
      void computeStartTime();
      void Init();
      int getBinModeIndex(int summing) const;
      void computePoly();
      Corners FocalPlaneToImage(const Corners &fp) const;
      Corners getCorners(const geos::geom::Geometry &poly) const;
  };

}  // namespace Isis
#endif
