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

// TODO Don't include
#include "FileList.h"
// The following includes are needed since class enumerations are used as input
// parameters (calculateStatistics)
#include "LeastSquares.h"
#include "OverlapNormalization.h"


namespace Isis {
  class Buffer;
  class Cube;
  class FileList;
  class OverlapStatistics;
  class Pvl;
  class PvlGroup;
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
   * eq.calculateStatistics(sampPercent, mincnt, wtopt, sType, methodType); 
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
   */
  class Equalization {
    protected:
      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class ImageAdjustment {
        public:
          ImageAdjustment() {}
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
            double gain = gains[index];
            double offset = offsets[index];
            double avg = avgs[index];
            return (dn - avg) * gain + offset + avg;
          }

        private:
          std::vector<double> gains;
          std::vector<double> offsets;
          std::vector<double> avgs;
      };

      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class CalculateFunctor {
        public:
          CalculateFunctor(Statistics *stats, double percent) {
            m_stats = stats;
            m_linc = (int) (100.0 / percent + 0.5);
          }

          virtual ~CalculateFunctor() {}

          void operator()(Buffer &in) const;

        protected:
          virtual void addStats(Buffer &in) const;

        private:
          Statistics *m_stats;
          int m_linc;
      };

      /**
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
          const ImageAdjustment *m_adjustment;
      };

      Equalization();

    public:
      Equalization(QString fromListName);
      virtual ~Equalization();

      void addHolds(QString holdListName);

      void calculateStatistics(double sampPercent, int mincnt,
          bool wtopt, OverlapNormalization::SolutionType sType, 
          LeastSquares::SolveMethod methodType);
      void importStatistics(QString instatsFileName);
      void applyCorrection(QString toListName);

      PvlGroup getResults();
      void write(QString outstatsFileName);

      double evaluate(double dn, int imageIndex, int bandIndex) const;

    protected:
      void loadInputs(QString fromListName);
      void setInput(int index, QString value);
      const FileList & getInputs() const;

      virtual void fillOutList(FileList &outList, QString toListName);
      virtual void errorCheck(QString fromListName);

      void generateOutputs(FileList &outList);
      void loadOutputs(FileList &outList, QString toListName);
      void loadHolds(OverlapNormalization *oNorm);

      void setResults(std::vector<OverlapStatistics> &overlapStats);
      void setResults();

      void clearAdjustments();
      void addAdjustment(ImageAdjustment *adjustment);

      void addValid(int count);
      void addInvalid(int count);

    private:
      void init();
      std::vector<int> validateInputStatistics(QString instatsFileName);

      FileList m_imageList;
      std::vector<ImageAdjustment *> m_adjustments;
      std::vector<int> m_holdIndices;

      int m_validCnt;
      int m_invalidCnt;

      int m_mincnt;
      bool m_wtopt;

      int m_maxCube;
      int m_maxBand;

      Pvl *m_results;
  };
};

#endif
