#ifndef SpiceDbGen_h
#define SpiceDbGen_h

/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/05/02 18:39:28 $
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

#include <iostream>

#include <QStringList>

#include <SpiceUsr.h>

#include "FileName.h"
#include "FileList.h"
#include "IException.h"
#include "IString.h"
#include "KernelDb.h"
#include "Pvl.h"

/**
 * @brief Utility for SPICE kernel interrogation and kerneldb generation
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2010-04-27 Stuart Sides - Modified Direct member to use a vector
 *                           of filters instead of a single QString
 *   @history 2013-02-15 Steven Lambright - Added support for extra kernel dependencies
 *   @history 2018-01-10 Christopher Combs - Added passing startOffset and endOffset to allow
 *                           the passing of time offsets to to FormatIntervals. Fixes #5272.
 *   @history 2018-05-09 Kristin Berry - Added information about the Spice time coverage level
 *                            to this class: m_coverageLevel, setCoverageLevel, and this class will
 *                            now select spice "time intervals" at the level specified. This is
 *                            either a SPICE Segment (coarse) or a SPICE interval (fine.)
 *                            Fixes #5410.
 *   @history 2019-08-13 Jesse Mapel - Added the ability to pass an explicit lsit
 *                           of files to the Direct method. Fixes #3390.
 *
 */
class SpiceDbGen {

  public:
    SpiceDbGen(QString type);
    Isis::PvlObject Direct(QString quality, QString location,
                           std::vector<QString> & filter, double startOffset, double endOffset);
    Isis::PvlObject Direct(QString quality, Isis::FileList fileList,
                           double startOffset, double endOffset);
    void FurnishDependencies(QList<Isis::FileName> sclks, QList<Isis::FileName> fks,
                             QList<Isis::FileName> extras);
    void setCoverageLevel(QString level);

  private:
    QStringList GetFiles(Isis::FileName location, QString filter);
    Isis::PvlGroup AddSelection(Isis::FileName fileIn, double startOffset, double endOffset);
    Isis::PvlGroup FormatIntervals(SpiceCell &coverage, QString type, double startOffset, double endOffset);
    Isis::PvlGroup GetIntervals(SpiceCell &cover);
    //private instance variables
    QString p_type;
    QString m_coverageLevel; //! The time coverage level of the database: INTERVAL or SEGMENT
    static const char *calForm;
};

#endif
