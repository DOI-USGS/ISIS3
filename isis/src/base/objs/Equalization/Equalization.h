#ifndef Equalization_h
#define Equalization_h

/**
 * @file
 * $Revision: 1.4 $
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

#include <vector>

#include <QString>
#include <QStringList>

#include "FileList.h"
#include "LeastSquares.h"
#include "OverlapNormalization.h"

using namespace std;

namespace Isis {
  class Buffer;
  class OverlapStatistics;
  class Pvl;
  class PvlGroup;
  class PvlObject;
  class Statistics;
  /**
   * This class can be used to calculate, read in, and/or apply equalization
   * statistics for a list of files.
   * <ul>
   *   <li> Calculating equalization statistics
   *     <ul>
   *       <li> An optional list of images to hold may be given before
   *       calculations.
   *       <li> In order to calculate statistics, the class will need to know
   *       the following:
   *       <ul>
   *         <li>percentage of lines to be used for calculations,
   *         <li>the minimum number of points in overlapping area to be used,
   *         <li>whether overlapping areas should be weighted based on number of
   *         valid pixels,
   *         <li>whether to calculate gain, offset, or both
   *         <li>which LeastSquares solve method.
   *        </ul>
   *       <li> Once calculated, these statistics can be returned as a PvlGroup,
   *       written to a text file, and/or applied to the images in the input
   *       file list.
   *     </ul>
   *   <li> Importing equalization statistics
   *     <ul>
   *       <li> Statistics may be imported from a given file name and then
   *       applied to the images in the input file list.
   *     </ul>
   *   <li> Applying equalization statistics
   *     <ul>
   *       <li> Statistics must be calculated or imported before they can be
   *       applied to the images in the input file list.
   *     </ul>
   * </ul>
   *
   * Code example for calculating statistics, writing results to a Pvl, writing
   * results to a file, and applying results:
   * <code>
   * Equalization eq(inputCubeListFileName);
   * // calculate
   * eq.addHolds(holdListFileName);
   * eq.calculateStatistics(samplingPercent, mincnt, wtopt, sType, methodType);
   * // write to PvlGroup
   * PvlGroup resultsGroup = eq.getResults();
   * // write to file
   * eq.write(outputStatisticsFileName);
   * // apply corrections to cubes in input list
   * eq.applyCorrection();
   * </code>
   *
   * Code example for importing statistics and applying them.
   * <code>
   * Equalization eq(listFileName);
   * eq.importStatistics(inputStatisticsFileName);
   * // create new output cubes from equalized cube list and apply corrections
   * eq.applyCorrection(equalizedCubeListFileName);
   * </code>
   *
   * This class contains the classes ImageAdjustment, CalculateFunctor, and
   * ApplyFunctor.
   *
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-04-26 Jeannie Backer - Added includes to Filelist, Pvl,
   *                           PvlGroup, and Statistics classes to the
   *                           implementation file.
   *   @history 2013-01-29 Jeannie Backer - Added input parameter to
   *                           calculateStatistics() method. Added error catch
   *                           to improve thrown message. Fixes #962.
   *   @history 2013-01-29 Jeannie Backer - Fixed bugs from previous checkin.
   *                           Improved test coverage by more than 23% for each
   *                           coverage type.
   *   @history 2013-02-06 Steven Lambright - Made the OverlapNormalization::SolutionType into
   *                           a member required upon instantiation in order to support
   *                           gain adjustments without normalization, which uses a separate
   *                           formula in evaluate() instead of just modifying how statistics
   *                           are computed. Implemented the GainsWithoutNormalization solution
   *                           type. References #911.
   *   @history 2016-07-15 Ian Humphrey - Added recalculateStatistics() method to allow for 
   *                           statistics to be calculated only for new input images, using
   *                           previously calculated statistics for the rest of the input images.
   *                           Added fromPvl() method to unserialize Equalization data from a pvl
   *                           object. Refactored calculateStatistics() to separate functionality
   *                           and allow for statistics recalculation. Added methods to properly 
   *                           free some of the vector members. Added new members to help with 
   *                           unserialization via the fromPvl() method and help with recalulating
   *                           overlap statistics. Added API documentation. Updated unit test.
   *                           Fixes #2282.
   */
  class Equalization {
    public:
      Equalization(OverlapNormalization::SolutionType sType, QString fromListName);
      virtual ~Equalization();

      void addHolds(QString holdListName);

      void calculateStatistics(double samplingPercent, int mincnt, bool wtopt,
          LeastSquares::SolveMethod methodType);
      void recalculateStatistics(QString inStatsFileName);
      void importStatistics(QString instatsFileName);
      void applyCorrection(QString toListName);

      PvlGroup getResults();
      void write(QString outstatsFileName);

      double evaluate(double dn, int imageIndex, int bandIndex) const;

    protected:
      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       *   @history 2013-02-06 Steven Lambright - Added support for GainsWithoutNormalization
       *                           solution type, which uses a different formula. References #911.
       */
      class ImageAdjustment {
        public:
          ImageAdjustment(OverlapNormalization::SolutionType sType) { m_sType = sType; }
          ~ImageAdjustment() {}

          void addGain(double gain) {
            gains.push_back(gain);
          }

          void addOffset(double offset) {
            offsets.push_back(offset);
          }

          void addAverage(double average) {
            avgs.push_back(average);
          }

          double getGain(int index) const {
            return gains[index];
          }

          double getOffset(int index) const {
            return offsets[index];
          }

          double getAverage(int index) const {
            return avgs[index];
          }

          double evaluate(double dn, int index) const {
            double result = Null;

            double gain = gains[index];
            if (m_sType != OverlapNormalization::GainsWithoutNormalization) {
              double offset = offsets[index];
              double avg = avgs[index];
              result = (dn - avg) * gain + offset + avg;
            }
            else {
              result = dn * gain;
            }

            return result;
          }

        private:
          vector<double> gains;
          vector<double> offsets;
          vector<double> avgs;

          OverlapNormalization::SolutionType m_sType;
      };

      /** 
       * This class is used as a functor calculate image statistics
       *
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class CalculateFunctor {
        public:
          /**
           * Constructs a CalculateFunctor
           *
           * @param stats Pointer to a Statistics object to add data to
           * @param percent Sampling percentage of the image, used to calculate a line increment, 
           *                when calculating statistics
           */
          CalculateFunctor(Statistics *stats, double percent) {
            m_stats = stats;
            m_linc = (int) (100.0 / percent + 0.5);
          }

          virtual ~CalculateFunctor() {}

          void operator()(Buffer &in) const;

        protected:
          virtual void addStats(Buffer &in) const;

        private:
          Statistics *m_stats; //!< Calculated statistics
          int m_linc; //!< Line increment value when calculating statistics
      };

      /**
       * This class is used as a functor to apply adjustments (equalize) to an image 
       *
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class ApplyFunctor {
        public:
          ApplyFunctor(const ImageAdjustment *adjustment) {
            m_adjustment = adjustment;
          }

          void operator()(Buffer &in, Buffer &out) const;

        private:
          const ImageAdjustment *m_adjustment; //!< ImageAdjustment to equalize image
      };

    protected:
      Equalization();
      void loadInputs(QString fromListName);
      void setInput(int index, QString value);
      const FileList &getInputs() const;

      // Should these be protected or private? (Recall children, i.e. HiEqual... can't
      // access Equalization's private vars directly)
      void calculateBandStatistics();
      void calculateOverlapStatistics();

      virtual void fillOutList(FileList &outList, QString toListName);
      virtual void errorCheck(QString fromListName);

      void generateOutputs(FileList &outList);
      void loadOutputs(FileList &outList, QString toListName);
      void loadHolds(OverlapNormalization *oNorm);

      void setResults();

      void clearAdjustments();
      void addAdjustment(ImageAdjustment *adjustment);

      void clearNormalizations();
      void clearOverlapStatistics();

      void addValid(int count);
      void addInvalid(int count);

      void fromPvl(const PvlObject &inStats);
      void setSolved(bool solved);
      bool isSolved() const;

    private:
      void init();
      QVector<int> validateInputStatistics(QString instatsFileName);

      bool m_normsSolved; //!< Indicates if corrective factors were solved
      bool m_recalculating; //!< Indicates if recalculating with loaded statistics
      bool m_wtopt; //!< Whether or not overlaps should be weighted

      FileList m_imageList; //<! List of input image filenames

      double m_samplingPercent; /**< Percentage of the lines to consider when gathering cube and
                                     overlap statistics (process-by-line)**/

      int m_validCnt; //!< Number of valid overlaps
      int m_invalidCnt; //!< Number of invalid overlaps
      int m_mincnt; //!< Minimum number of pixels for an overlap to be considered valid

      int m_maxCube; //!< Number of input images
      int m_maxBand; //!< Number of bands in each input image

      QStringList m_badFiles; //!< List of image names that don't overlap

      vector<ImageAdjustment *> m_adjustments; //<! Corrective factors for equalization
      vector<int> m_holdIndices; //!< Indices of images being held
      vector<bool> m_doesOverlapList; //!< Which images have a valid overlap
      vector<bool> m_alreadyCalculated; //!< Which images that have statistics already calculated
      vector<OverlapNormalization *> m_overlapNorms; //!< Normalization data for input images
      vector<OverlapStatistics *> m_overlapStats; //!< Calculated overlap statistics

      //! The normalization solution type for solving normalizations (offsets, gains, or both)
      OverlapNormalization::SolutionType m_sType;
      //! Least squares method for solving normalization correcitve factors
      LeastSquares::SolveMethod m_lsqMethod; 

      Pvl *m_results; //!< Calculation results and normalization corrective factors (if solved)
  };
};

#endif
