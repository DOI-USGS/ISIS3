#ifndef SumFile_h
#define SumFile_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <ostream>
#include <cfloat>
#include <cmath>

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

#include "Cube.h"
#include "FileList.h"
#include "iTime.h"
#include "Kernels.h"
#include "Quaternion.h"
#include "SpecialPixel.h"

namespace Isis {

  /**
   * @brief Container for a Gaskell SUMFILE
   *
   * This class will parse the contents of a R. Gaskell SUMFILE used in his
   * stereo photoclinometry (SPC) system and provide access to the elements that
   * are stored therein.
   *
   * @author 2015-02-02 Kris Becker
   *
   * @internal
   *   @history 2015-02-02 Kris Becker Original Version
   *   @history 2016-02-09 Kris Becker - Updated documentation
   *   @history 2016-06-07 Kris Becker - Renamed getIlluminationPosition() to
   *                           getSunPosition(). updateIlluminationPosition() did
   *                           not have an implementation so it was removed.
   *   @history 2016-09-14 Kris Becker - Moved updateTimes() to new SumFinder
   *                           class as there is not enough information to do it
   *                           correctly in SumFile.
   */
  class SumFile {
    public:

      SumFile();
      SumFile(const QString &sumfile);
      virtual ~SumFile() { }

      bool isValid() const;
      QString name() const;
      QString utc() const;
      double  et() const;
      const iTime &time() const;

      bool updateSpice(Cube &cube, Camera *camera = 0) const;
      bool updatePointing(Cube &cube, Camera *camera = 0) const;
      bool updatePosition(Cube &cube, Camera *camera = 0) const;

      Quaternion getPointing() const;
      std::vector<double> getPosition() const;
      std::vector<double> getSunPosition()  const;

      std::ostream &brief(std::ostream &outs) const;

    private:
      QString m_id;
      iTime   m_obsTime;
      int     m_numSamples;
      int     m_numLines;
      double  m_dnMin;
      double  m_dnMax;
      double  m_pxlSize;
      double  m_centerSample;
      double  m_centerLine;

      double  m_spacecraftPosition[3];
      double  m_pointingMatrix[3][3];
      double  m_sunPosition[3];
      double  m_kmatrix[6];
      double  m_distortion[4];
      double  m_sigmaSCPos[3];
      double  m_sigmaPntg[3];
      // QList<ControlMeasure>   m_measures;

      void parseSumFile(const QString &sumfile);
      QStringList getSumLine(const QString &data, const int &nexpected = 0,
                             const QString &tag = "") const;
      double cvtDouble(const QString &value) const;

      Quaternion getBFtoJ2000(Camera *camera) const;
      Quaternion getFrameRotation(const int &fromId, const int &toId,
                                  const double &timeEt) const;
      QString    getFrameName(const int &frameid) const;
      int        getFrameCode(const QString &frameName) const;

  };

  ///!<   Shared Resource pointer that everyone can use
  typedef QSharedPointer<SumFile> SharedSumFile;

  ///!<  Define a resource list
  typedef QList<SharedSumFile> SumFileList;


  // Global methods
  SumFileList   loadSumFiles(const FileList &sumfiles);


  /**
   * @brief Ascending order sort functor
   *
   * This is a comparison class used to sort lists of SharedSumFile objects, in
   * ascending order.  Two shared sum files are passed in and the ephemeris
   * times of each is compared using the less than operator.
   *
   * @author 2015-07-28 Kris Becker
   *
   * @internal
   *   @history 2015-07-28 Kris Becker Original Version
   */
  class SortEtAscending {
    public:
      SortEtAscending() { }
      ~SortEtAscending() { }
      inline bool operator()(const SharedSumFile &a, const SharedSumFile &b) const {
        return  ( a->et()<  b->et() );
      }
  };

};  // namespace Isis
#endif
