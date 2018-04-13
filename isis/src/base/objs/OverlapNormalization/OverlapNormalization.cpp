/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/11/25 22:09:21 $
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

#include "OverlapNormalization.h"

#include <iomanip>

#include "BasisFunction.h"
#include "IException.h"
#include "LeastSquares.h"
#include "Statistics.h"

using namespace std;

namespace Isis {

  /**
   * Constructs an OverlapNormalization object.  Compares and
   * stores the vector, and initializes the basis and least
   * squares functions.  This object will also take ownership of the pointers
   * in the vector parameter.
   *
   * @param statsList The list of Statistics objects corresponding
   *                  to specific data sets (e.g., cubes)
   */
  OverlapNormalization::OverlapNormalization(std::vector<Statistics *> statsList) {
    m_gainFunction = NULL;
    m_gainLsq = NULL;
    m_offsetFunction = NULL;
    m_offsetLsq = NULL;

    m_statsList = statsList;
    m_gainFunction = new BasisFunction("BasisFunction",
                                       statsList.size(), statsList.size());
    m_gainLsq = new LeastSquares(*m_gainFunction);
    m_offsetFunction = new BasisFunction("BasisFunction",
                                         statsList.size(), statsList.size());
    m_offsetLsq = new LeastSquares(*m_offsetFunction);

    m_gains.resize(statsList.size());
    m_offsets.resize(statsList.size());
    for (unsigned int i = 0; i < statsList.size(); i++) {
      m_gains[i] = 1.0;
      m_offsets[i] = 0.0;
    }
    m_solved = false;
  }

  /**
   * Destroys the OverlapNormalization object, frees up
   * pointers
   */
  OverlapNormalization::~OverlapNormalization() {
    if (m_gainFunction != NULL) delete m_gainFunction;
    if (m_offsetFunction != NULL) delete m_offsetFunction;
    if (m_gainLsq != NULL) delete m_gainLsq;
    if (m_offsetLsq != NULL) delete m_offsetLsq;
    for (unsigned int i = 0; i < m_statsList.size(); i++) delete m_statsList[i];
  };

  /**
   * Attempts to add the given overlap data to a collection of
   * valid overlaps, and returns the success or failure of that
   * attempt.
   *
   * @param area1 The statistics for the overlap area of the first
   *              overlapping data set
   * @param index1 The index in the list of Statistics of the
   *            first data set
   * @param area2 The statistics for the overlap area of the
   *              second data set
   * @param index2 The index in the list of Statistics of the
   *            second overlapping data set
   * @param weight Relative significance of this overlap.  Default
   *               value = 1.0
   *
   * @return AddStatus An enumeration representing either a
   *         successful add, or the reason for failure
   *
   * @throws Isis::iException::Programmer - Identifying index 1
   *             must exist in the statsList
   * @throws Isis::iException::Programmer - Identifying index 2
   *             must exist in the statsList
   * @throws Isis::iException::Programmer - Weights must be all
   *             positive real numbers
   */
  OverlapNormalization::AddStatus OverlapNormalization::AddOverlap(
    const Statistics &area1, const unsigned index1,
    const Statistics &area2, const unsigned index2,
    double weight) {
    if (index1 >= m_statsList.size()) {
      string msg = "The index 1 is outside the bounds of the list.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    if (index2 >=  m_statsList.size()) {
      string msg = "The index 2 is outside the bounds of the list.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // If there is no overlapping area, then the overlap is invalid
    if (area1.ValidPixels() == 0 || area2.ValidPixels() == 0) {
      return NoOverlap;
    }

    // The weight must be a positive real number
    if (weight <= 0.0) {
      string msg = "All weights must be positive real numbers.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    OverlapNormalization::Overlap o;
    o.area1 = area1;
    o.area2 = area2;
    o.index1 = index1;
    o.index2 = index2;

    double avg1 = area1.Average();
    double avg2 = area2.Average();

    // Averages must not be 0 to avoid messing up least squares
    if (avg1 == 0 || avg2 == 0) return NoContrast;

    m_overlapList.push_back(o);
    m_deltas.push_back(avg2 - avg1);
    m_weights.push_back(weight);
    m_solved = false;
    return Success;
  }

  /**
   * Attempts to solve the least squares equation for all data sets.
   *
   * @param type The enumeration clarifying whether the offset, gain, or both
   *             should be solved here
   * @param method The enumeration clarifying the LeastSquares::SolveMethod to
   *             be used.
   *
   * @return bool Is the least squares equation now solved
   *
   * @throws Isis::iException::User - Number of overlaps and
   *             holds must be greater than the number of data
   *             sets
   */
  void OverlapNormalization::Solve(SolutionType type,
                                   LeastSquares::SolveMethod method) {
    // Make sure that there is at least one overlap
    if (m_overlapList.size() == 0) {
      string msg = "None of the input images overlap";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Make sure the number of valid overlaps + hold images is greater than the
    // number of input images (otherwise the least squares equation will be
    // unsolvable due to having more unknowns than knowns)
    if (m_overlapList.size() + m_idHoldList.size() < m_statsList.size()) {
      string msg = "Unable to normalize overlaps. The number of overlaps and "
                   "holds must be greater than the number of input images";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Calculate offsets
    if (type != Gains && type != GainsWithoutNormalization) {
      // Add knowns to least squares for each overlap
      for (int overlap = 0; overlap < (int)m_overlapList.size(); overlap++) {
        Overlap curOverlap = m_overlapList[overlap];
        int id1 = curOverlap.index1;
        int id2 = curOverlap.index2;

        vector<double> input;
        input.resize(m_statsList.size());
        for (int i = 0; i < (int)input.size(); i++) input[i] = 0.0;
        input[id1] = 1.0;
        input[id2] = -1.0;

        m_offsetLsq->AddKnown(input, m_deltas[overlap], m_weights[overlap]);
      }

      // Add a known to the least squares for each hold image
      for (int h = 0; h < (int)m_idHoldList.size(); h++) {
        int hold = m_idHoldList[h];

        vector<double> input;
        input.resize(m_statsList.size());
        for (int i = 0; i < (int)input.size(); i++) input[i] = 0.0;
        input[hold] = 1.0;
        m_offsetLsq->AddKnown(input, 0.0, 1e30);
      }

      // Solve the least squares and get the offset coefficients to apply to the
      // images
      m_offsets.resize(m_statsList.size());
      m_offsetLsq->Solve(method);
      for (int i = 0; i < m_offsetFunction->Coefficients(); i++) {
        m_offsets[i] = m_offsetFunction->Coefficient(i);
      }
    }

    // Calculate Gains
    if (type != Offsets) {
      // Add knowns to least squares for each overlap
      for (int overlap = 0; overlap < (int)m_overlapList.size(); overlap++) {
        Overlap curOverlap = m_overlapList[overlap];
        int id1 = curOverlap.index1;
        int id2 = curOverlap.index2;

        vector<double> input;
        input.resize(m_statsList.size());
        for (int i = 0; i < (int)input.size(); i++) input[i] = 0.0;
        input[id1] = 1.0;
        input[id2] = -1.0;

        double tanp;

        if (type != GainsWithoutNormalization) {
          if (curOverlap.area1.StandardDeviation() == 0.0) {
            tanp = 0.0;    // Set gain to 1.0
          }
          else {
            tanp = curOverlap.area2.StandardDeviation()
                   / curOverlap.area1.StandardDeviation();
          }
        }
        else {
          if (curOverlap.area1.Average() == 0.0) {
            tanp = 0.0;
          }
          else {
            tanp = curOverlap.area2.Average() / curOverlap.area1.Average();
          }
        }

        if (tanp > 0.0) {
          m_gainLsq->AddKnown(input, log(tanp), m_weights[overlap]);
        }
        else {
          m_gainLsq->AddKnown(input, 0.0, 1e30); // Set gain to 1.0
        }
      }

      // Add a known to the least squares for each hold image
      for (int h = 0; h < (int)m_idHoldList.size(); h++) {
        int hold = m_idHoldList[h];

        vector<double> input;
        input.resize(m_statsList.size());
        for (int i = 0; i < (int)input.size(); i++) input[i] = 0.0;
        input[hold] = 1.0;
        m_gainLsq->AddKnown(input, 0.0, 1e30);
      }

      // Solve the least squares and get the gain coefficients to apply to the
      // images
      m_gains.resize(m_statsList.size());
      m_gainLsq->Solve(method);
      for (int i = 0; i < m_gainFunction->Coefficients(); i++) {
        m_gains[i] = exp(m_gainFunction->Coefficient(i));
      }
    }

    m_solved = true;
  }

  /**
   * Returns the calculated average DN value for the given
   * data set
   *
   * @param index The index in the Statistics list corresponding
   *              to the data set desired
   *
   * @return double The average for the data
   *
   * @throws Isis::iException::Programmer - Identifying index must
   *             exist in the statsList
   */
  double OverlapNormalization::Average(const unsigned index) const {
    if (index >= m_statsList.size()) {
      string msg = "The index was out of bounds for the list of statistics.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return m_statsList[index]->Average();
  }

  /**
   * Returns the calculated gain (multiplier) for the given
   * data set
   *
   * @param index The index in the Statistics list corresponding
   *              to the data set desired
   *
   * @return double The gain for the data
   *
   * @throws Isis::iException::Programmer - Identifying index must
   *             exist in the statsList
   */
  double OverlapNormalization::Gain(const unsigned index) const {
    if (index >= m_statsList.size()) {
      string msg = "The index was out of bounds for the list of statistics.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return m_gains[index];
  }

  /**
   * Returns the calculated offset (base) for the given
   * data set
   *
   * @param index The index in the Statistics list corresponding
   *              to the data set desired
   *
   * @return double The offset for the data
   *
   * @throws Isis::iException::Programmer - Identifying index must
   *             exist in the statsList
   */
  double OverlapNormalization::Offset(const unsigned index) const {
    if (index >= m_statsList.size()) {
      string msg = "The index was out of bounds for the list of statistics.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return m_offsets[index];
  }

  /**
   * Returns a new DN from an old using the calculated gains and
   * offsets of the data set the pixel belongs to
   *
   * @param dn The value of the pixel prior to equalization
   *
   * @param index The index in the Statistics list corresponding
   *              to the data set for the pixel
   *
   * @return double The newly calculated DN value
   *
   * @throws Isis::iException::Programmer - Least Squares equation
   *             must be solved before returning the gain
   */
  double OverlapNormalization::Evaluate(double dn, unsigned index) const {
    if (!m_solved) {
      string msg = "The least squares equation has not been successfully ";
      msg += "solved yet.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (Isis::IsSpecial(dn)) return dn;
    return (dn - Average(index)) * Gain(index) + Average(index) + Offset(index);
  }
}
