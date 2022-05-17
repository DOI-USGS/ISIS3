#ifndef SpiceDbGen_h
#define SpiceDbGen_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


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
 *   @history 2021-12-22 Amy Stamile - Added the ability to include timing offset information
 *                          in db if information exists in the kernel comments.
 *                          This information will be use in spiceinit to handle time offsets
 *                          when selecting smithed kernels. References #3363.
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
