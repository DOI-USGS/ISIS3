#ifndef CkSpiceSegment_h
#define CkSpiceSegment_h
/**
 * @file
 * $Revision$
 * $Date$
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
 *
 *   $Id$
 */

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <QString>
#include <QVector>

#include <SpiceZdf.h>

#include <tnt/tnt_array1d.h>
#include <tnt/tnt_array1d_utils.h>
#include <tnt/tnt_array2d.h>
#include <tnt/tnt_array2d_utils.h>

#include "Kernels.h"
#include "IException.h"
#include "IString.h"


namespace Isis {

class Cube;
class Camera;
class Table;
class PvlObject;

/**
 * @brief Maintain a SPICE kernel segment for conversions and export
 *
 * This class is designed to read SPICE data from ISIS cube blobs and convert
 * them to proper formats for export to NAIF formatted SPICE kernel files.  This
 * particular implementation supports NAIF CK kernel types 1, 2 and 3.
 *
 * @author 2010-11-10 Kris Becker
 * @internal
 * @history 2010-12-09 Kris Becker Added more documentation
 * @history 2011-05-04 Kris Becker Added pad time.
 * @history 2011-05-29 Debbie A. Cook Changed name of local Kernels class to
 *                                    LocalKernels to avoid confusion
 * @history 2011-06-15 Kris Becker Removed use of LocalKernels and use version
 *                                  in system; added loading of IAK kernel
 *                                  specifically for Cassini support; removed
 *                                  conditionalized obsolete code.
 * @history 2011-09-19 Debbie A. Cook Added section of code to ignore
 *                                  Instrument group and use OriginalInstrument
 *                                  group in the case of the IdealCamera.  The
 *                                  output kernel will be based on the original
 *                                  instrument code since the IdealCamera has
 *                                  no code.  This is safe to do since the
 *                                  camera is not being used to manipulate
 *                                  pixels.
 * @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
 *                          coding standards. References #972.
 *
 * @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
 *                          were signaled. References #2248.
 *
 * @history 2019-12-05 Adam Paquette - Changed how kernels are loaded so CkSpiceSegment
 *                          no longer needs to use the Spice class.
 *
 * @history 2021-12-22 Amy Stamile - Added timing offset information to kernel comments
 *                          by calculating difference between label StartTime and camera model
 *                          StartTime. This is to allow for spiceinit to handle offsets
 *                          when finding associated smithed kernels. References #3363.
 */
class CkSpiceSegment {
  public:
    typedef TNT::Array1D<SpiceDouble> SVector;       //!<  1-D Buffer
    typedef TNT::Array2D<SpiceDouble> SMatrix;       //!<  2-D buffer

    CkSpiceSegment();
    CkSpiceSegment(const QString &fname);
    CkSpiceSegment(Cube &cube, const QString &tblname = "CkSpiceSegment");
    virtual ~CkSpiceSegment() { }

    /** Returns the number of elements in the quaternions */
    int size() const { return (size(_quats)); }

    /** Returns the name of the segment, typically the ProductId */
    QString Id() const { return (_name); }
    void setId(const QString &id);

    /** Start time of segment in ET */
    double startTime() const { return (_startTime); }
    /** End time of segment in ET */
    double endTime() const { return (_endTime); }

    bool operator<(const CkSpiceSegment &segment) const {
      return (startTime() < segment.startTime());
    }

    // Elements for writing NAIF SPICE kernels
    int FurnshKernelType(const QString &ktypes) const;
    int UnloadKernelType(const QString &ktypes = "") const;
    const Kernels &kernels() const { return (_kernels); }

    /** Returns CK segment reference frame */
    QString ReferenceFrame() const { return (_refFrame); }
    /** NAIF SPICE instrument code */
    int InstCode() const { return (_instCode); }
    /** Returns instance of quaternions */
    const SMatrix &Quaternions() const { return (_quats); }
    /** Determines if angular velocites exist */
    bool hasAngularVelocities() const { return ((size(_avvs) > 0)); }
    /** Returns instance of angular velocity vectors */
    const SMatrix &AngularVelocities() const { return (_avvs); }

    /** Returns times in units of SCLK */
    const SVector &SCLKTimes() const { return (_times); }

    /** Returns a comment summarizing the segment */
    QString getComment() const;

    // Mostly type 2 needs but can be used for type 3 (start times anyway)
    virtual SVector SCLKStartIntervals() const;
    virtual SVector SCLKStopIntervals() const;
    virtual SVector TickRate() const;

  private:
    typedef TNT::Array1D<SMatrix>     SMatSeq;       //!<  Time ordered matrices

    // Mutable for full loading/unloading of kernels w/o restrictions
    mutable Kernels  _kernels;
    int         _camVersion;
    QString     _name;
    QString     _fname;
    double      _startTime;
    double      _endTime;
    QString     _utcStartTime; //  Need to store these as conversion from ET
    QString     _utcEndTime;   //  requires leap seconds kernel
    QString     _instId;       //  Instrument ID
    QString     _target;       //  Target name
    double      _startOffset;  // time offset between camera model and label start time
    double      _endOffset;    // time offset between camera model and label end time
    int         _instCode;     //  NAIF instrument code of the SPICE segment
    QString     _instFrame;    //  NAIF instrument frame
    QString     _refFrame;     //  NAIF reference frame
    SMatrix     _quats;
    SMatrix     _avvs;
    SVector     _times;
    double      _tickRate;     // Number of seconds per tick for s/c


    // Internal processing methods
    void init();
    template <class TNTSTORE> int size(const TNTSTORE &t) const { return (t.dim1()); }

    QString getKeyValue(PvlObject &label, const QString &keyword);
    void import(Cube &cube, const QString &tblname = "CkSpiceSegment");
    SMatrix load(Table &cache);
    SMatrix getQuaternions(const SMatrix &spice) const;
    SMatrix getAngularVelocities(const SMatrix &spice) const;
    SVector getTimes(const SMatrix &spice) const;

    bool getTimeDependentFrameIds(Table &table, int &toId, int &fromId) const;
    bool getFrameChains(Table &table, const int &leftBase,
                        const int &rightBase, QVector<int> &leftChain,
                        QVector<int> &rightChain) const;

    QString getFrameName(int frameid) const;

    SMatrix getConstantRotation(Table &table) const;
    SMatrix getIdentityRotation(const int &nelements = 3) const;
    SMatrix computeStateRotation(const QString &frame1,
                                 const QString &frame2,
                                 double etTime) const;
    SMatrix computeChainRotation(const QVector<int> &fChain,
                                 const int &ckId, const double &etTime) const;
    void getRotationMatrices(Cube &cube, Camera &camera, Table &table,
                             SMatSeq &lmats, SMatSeq &rmat, SVector &sclks);

    SVector convertTimes(int scCode, const SVector &etTimes);
    void convert(const SMatrix &quats, const SMatrix &avvs,
                 const SMatSeq &lmats, const SMatSeq &rmats,
                 SMatrix &ckQuats, SMatrix &ckAvvs) const;
    const SMatrix &getMatrix(const SMatSeq &seq, const int &nth) const;

    SMatrix expand(int ntop, int nbot, const SMatrix &matrix) const;
    SVector expand(int ntop, int nbot, const SVector &vec) const;

    double SCLKtoET(SpiceInt scCode, double sclk) const;
    double ETtoSCLK(SpiceInt scCode, double et) const;

    QString toUTC(const double &et) const;

};

};     // namespace Isis
#endif
