#ifndef SpkSegment_h
#define SpkSegment_h
/**
 * @file
 * $Revision: 6314 $
 * $Date: 2015-08-12 15:30:27 -0700 (Wed, 12 Aug 2015) $
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
 *   $Id: SpkSegment.h 6314 2015-08-12 22:30:27Z jwbacker@GS.DOI.NET $
 */

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "SpkSpiceSegment.h"
#include "IString.h"
#include "IException.h"


namespace Isis {

class Cube;
class Camera;
class Table;
class PvlObject;

/**
 * @brief Maintain a SPK SPICE kernel segment for conversions and export
 *
 * This class is designed to read SPICE data from ISIS cube blobs and convert
 * them to proper formats for export to NAIF formatted SPK SPICE kernel files.
 * This particular implementation supports NAIF SPK kernel types 9 and 13.
 *
 * @author 2011-02-12 Kris Becker
 * @internal
 *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
 *                          coding standards. References #972.
 *   @history 2013-12-19 Kris Becker Add SPK kernel type parameter to
 *                          constructors; removed adjustTimes() method; modified
 *                          variable names to conform to coding rules.
 *                          References #1739.
 *   @history 2014-03-26 Kris Becker Added overlap() method to compare time
 *                          ranges of another SPK segment. References #1739.
 *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
 *                          were signaled. References #2248.
 */
class SpkSegment : public SpkSpiceSegment {
  public:
    typedef SpkSpiceSegment::SVector SVector;
    typedef SpkSpiceSegment::SMatrix SMatrix;

    SpkSegment();
    SpkSegment(const QString &fname, const int spkType);
    SpkSegment(Cube &cube, const int spkType);
    virtual ~SpkSegment() { }

    void import(Cube &cube);

    /** Returns the number of elements in the vectors */
    int size() const { return (size(m_states)); }

    /** Return kernel type */
    int ktype() const { return (m_spkType) ; }

    /** Returns SPK segment reference frame */
    int BodyCode() const { return (m_body); }
    /** NAIF SPICE instrument code */
    int CenterCode() const { return (m_center); }
    /** Returns CK segment reference frame */
    QString ReferenceFrame() const { return (m_refFrame); }
    /** Get times of each entry */
    SVector Epochs() const { return (m_epochs); }
    /** Returns instance of quaternions */
    const SMatrix &States() const { return (m_states); }

    int Degree() const { return (m_degree);   }

    bool HasVelocityVectors() const { return (m_hasVV); }

    /** Returns a comment summarizing the segment */
    QString getComment() const;

    bool overlaps(const SpkSegment &other) const;

  private:
    enum { MinimumStates = 3, MaximumDegree = 7};  // Sensible? NAIF extremes

    int         m_spkType;      //  Type 9 or 13 kernel type
    int         m_body;         //  NAIF body code of the SPICE segment
    QString     m_bodyFrame;    //  NAIF body frame
    int         m_center;       //  NAIF center code of the SPICE segment
    QString     m_instId;       //  Instrument ID
    QString     m_centerFrame;  //  NAIF center frame
    QString     m_refFrame;     //  NAIF reference frame
    SMatrix     m_states;       //  Position states
    SVector     m_epochs;       //  ET times of records
    bool        m_hasVV;        //  Has velocity vectors?
    int         m_degree;       //  Degree of polynomial to fit in NAIF kernel
    double      m_startOffset;  // time offset between camera model and label start time
    double      m_endOffset;    // time offset between camera model and label end time

    // Internal processing methods
    void init(const int spkType = 13);
    template <class TNTSTORE> int size(const TNTSTORE &t) const { return (t.dim1()); }

    SMatrix load(Table &cache);
    void getStates(Camera &camera, const SMatrix &spice, SMatrix &states,
                   SVector &epochs, bool &hasVV) const;
    SVector makeState(SpicePosition *position, const double &time0,
                      const SVector &stateT, const double &timeT) const;
    void validateType(const int spktype) const;
};

};     // namespace Isis
#endif
