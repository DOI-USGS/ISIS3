#ifndef FindGapsFunctor_h
#define FindGapsFunctor_h

/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/22 19:44:53 $
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

class QString;

namespace Isis {
  class Brick;
  class Buffer;
  class Cube;
  class Pvl;
  class PvlGroup;

  /**
   * @brief Find gaps in cubes
   *
   * This functor has a method to find the gaps and store their information. The start of the gap,
   * end of the gap, the correlation, and the band that the gap is in will be stored in a PvlGroup.
   * The group of gaps will be stored in a Pvl. The  functor can also write a cube with the gaps
   * and buffers nulled out. Fixes #582.
   * 
   * @author 2012-06-15 Kimberly Oyama and Steven Lambright
   *
   * @internal
   */
  class FindGapsFunctor {
    public:
      FindGapsFunctor(int inputLineCount, double correlationTol,
                      int borderSizeBeforeGap, int borderSizeAfterGap);
      ~FindGapsFunctor();

      // Accessors
      bool endsInGap();
      PvlGroup gap();
      Pvl gaps();

      void setModification(QString newModValue);

      // Processors
      void operator() (Buffer &in) const;
      void operator() (Buffer &in, Buffer &out) const;
      

    private:
      void copy(Buffer &in) const;

      void addGapToGroup() const;

      //! True if the current line is in a gap
      bool *m_inGap;

      //! Correlation Tolerance Coefficient
      double m_corTol;

      //! The last line of the band, used to see if the gap extends to the end of the band.
      int m_lineCount;

      //! Number of of pixels nulled before the gap
      int m_bufferSizeBeforeGap;

      //! Number of pixels nulled after the gap
      int m_bufferSizeAfterGap;
      
      //! Stores the previous line for comparison to find gaps
      Brick *m_previous;

      /**
       * Pointer to a group that is a single gap, this group stores the start line, end line,
       * and band of the gap
       */
      PvlGroup *m_gap;

      /**
       * Each gap (start line, end line, and band) is stored in this container. This group
       * of gaps is output in the log file and used to null the gaps and their buffers.
       */
      Pvl *m_gaps;
  };
}

#endif
