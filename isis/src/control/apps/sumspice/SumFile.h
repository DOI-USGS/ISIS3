#ifndef SumFile_h
#define SumFile_h
/**
 * @file
 * $Revision: 6565 $
 * $Date: 2016-02-10 17:15:35 -0700 (Wed, 10 Feb 2016) $
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
   * @history 2015-02-02 Kris Becker Original Version 
   * @history 2016-02-09 Kris Becker - Updated documentation 
   * @history 2016-06-07 Kris Becker - Renamed getIlluminationPosition() to 
   *                        getSunPosition(). updateIlluminationPosition() did
   *                        not have an implementation so it was removed.
   * @history 2016-09-14 Kris Becker - Moved updateTimes() to new SumFinder 
   *                        class as there is not enough information to do it
   *                        correctly in SumFile.
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
