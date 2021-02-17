#ifndef OverlapNormalization_h
#define OverlapNormalization_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Statistics.h"
#include "LeastSquares.h"

namespace Isis {
  class BasisFunction;

  /**
   * Calculate the bases and multipliers for normalizing
   * overlapping "data sets" (e.g., cubes).  Specifically, this
   * class is designed for normalizing data in equalization
   * applications such as equalizer and hiequal. Given collections
   * for statistical data sets, overlapping statistics between
   * these sets, and at least one data set to be "held", this
   * class will use a least squares solution to calculate gain and
   * offset coefficients for every set of data in the collection.
   *
   * Once the multiplicative and additive corrections have been
   * derived, this class can then be used to calculate new DN
   * values for a known data set.  The actual equation to be used
   * for calculating new DN values with the derived gains and
   * offsets is:
   *
   * @f[
   * newDN = (oldDN - avg(i)) * MULT(i) + avg(i) + BASE(i)
   * @f]
   *
   * where i is the index of a known data set from the statistics
   * list.
   *
   * @author 2009-05-07 Travis Addair
   *
   * @internal
   *   @history 2009-06-05 Mackenzie Boyd - fixed unittest to work on all systems
   *   @history 2009-06-12 Travis Addair - changed public interface
   *                           to use indices instead of file names and renamed
   *                           from IntersectionStatistics
   *   @history 2009-06-15 Travis Addair - documented all
   *                           variables/enums
   *   @history 2009-06-24 Travis Addair - changed gain and offset vectors to
   *                           itialize to 1.0 and 0.0, respectively
   *   @history 2009-11-25 Travis Addair - held images are now
   *                           weighted to ensure gain and offset of 1.0 and 0.0,
   *                           respectively
   *   @history 2013-12-29 Jeannie Backer - Added LeastSquares::SolveMethod
   *                           input parameter to Solve() method. Improved error
   *                           message. Fixes #962,
   *   @history 2013-02-14 Steven Lambright - Added SolutionType GainsWithoutNormalization. Fixes
   *                           #911.
   *   @history 2019-09-05 Makayla Shepherd & Jesse Mapel - Changed weight for hold images from
   *                           1E30 to 1E10 to avoid poorly conditioned normal matrix.
   */

  class OverlapNormalization {
    public:

      OverlapNormalization(std::vector<Statistics *> statsList);

      virtual ~OverlapNormalization();

      /**
       * The result of the attempt to add overlap data to the list of
       * valid overlaps, where Success is a successful add, NoOverlap
       * is a failure due to one or both Statistics objects containing
       * no data, and NoContrast is a failure due to one or both
       * averages being 0
       */
      enum AddStatus {

        /**
         * Overlap is valid and was added successfully
         */
        Success,

        /**
         * Data sets do not overlap one another
         */
        NoOverlap,

        /**
         * One or both areas contain no valid average
         */
        NoContrast
      };

      AddStatus AddOverlap(const Statistics &area1, const unsigned index1,
                           const Statistics &area2, const unsigned index2,
                           double weight = 1.0);

      /**
       * Sets the list of files to be held during the solving process.
       *
       * @param holdIndex The index of a set of data from the list of
       *                  Statistics objects to be held
       */
      inline void AddHold(unsigned holdIndex) {
        m_idHoldList.push_back(holdIndex);
      };

      /**
       * Enumeration for whether user/programmer wants to calculate
       * new gains, offsets, or both when solving
       */
      enum SolutionType {

        /**
         * Calculate only the gains.
         */
        Gains,

        /**
         * Calculate only the offsets
         */
        Offsets,

        /**
         * Calculate both gains and offsets
         */
        Both,

        /**
         * The equation being solved for Gains, Offsets, and Both is
         * output = (input - average) * gain + offset + average
         *
         * This solution type is for the equation
         * output = input * gain
         */
         GainsWithoutNormalization
      };

      void Solve(SolutionType type = Both,
                 LeastSquares::SolveMethod method=LeastSquares::QRD);

      double Average(const unsigned index) const;
      double Gain(const unsigned index) const;
      double Offset(const unsigned index) const;

      double Evaluate(double dn, unsigned index) const;

    private:

      /**
       * Vector of Statistics objects for each data set
       */
      std::vector<Statistics *> m_statsList;

      /**
       * Vector of indices corresponding to the m_statsList vector
       * representing data sets to be held in solution
       */
      std::vector<int> m_idHoldList;

      /**
       * Store statistics pertaining to the overlapping areas and
       * indices (corresponding to the statistics list) for two data
       * sets
       *
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class Overlap {
        public:

          /**
           * Overlapping area for the first data set
           */
          Statistics area1;

          /**
           * Overlapping area for the second data set
           */
          Statistics area2;

          /**
           * Index corresponding to m_statsList for the first
           * overlapping data set
           */
          int index1;

          /**
           * Index corresponding to m_statsList for the second
           * overlapping data set
           */
          int index2;
      };

      /**
       * Vector of valid overlaps collected
       */
      std::vector<Overlap> m_overlapList;

      /**
       * Vector of delta values (differences between the averages of
       * two overlapping data sets) for every valid overlap
       */
      std::vector<double> m_deltas;

      /**
       * Vector of weights for every valid overlap
       */
      std::vector<double> m_weights;


      /**
       * Whether or not the least squares solution has been solved
       */
      bool m_solved;

      /**
       * Whether the user of this class wants to solve for the
       * offsets, the gains, or both
       */
      SolutionType m_solutionType;

      /**
       * Vector of calculated gains filled by the Solve method
       */
      std::vector<double> m_gains;

      /**
       * Vector of calculated offsets filled by the Solve method
       */
      std::vector<double> m_offsets;


      /**
       * The gain function to be solved
       */
      BasisFunction *m_gainFunction;

      /**
       * The offset function to be solved
       */
      BasisFunction *m_offsetFunction;

      /**
       * The least squares object that solves for the new gains
       */
      LeastSquares *m_gainLsq;

      /**
       * The least squares object that calculates offsets
       */
      LeastSquares *m_offsetLsq;

      //! Cannot copy this object
      OverlapNormalization &operator=(const OverlapNormalization &);

      //! Cannot copy this object
      OverlapNormalization(const OverlapNormalization &);
  };
};

#endif
