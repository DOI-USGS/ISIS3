#ifndef CnetEditorSortConfigDialog_h
#define CnetEditorSortConfigDialog_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDialog>
#include <QPointer>

class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;

namespace Isis {
  class CnetEditorWidget;

  /**
   * @brief Configure user's sorting settings for the cneteditor widget
   *
   * This dialog enables the user to configure the sorting options in the
   * cneteditor widget.
   *
   * @author 2012-04-02 Steven Lambright and Jai Rideout
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2018-06-12 Kaitlyn Lee - Set the window title to "Table Sorting"
   *                           with setWindowTitle().
   */
  class CnetEditorSortConfigDialog : public QDialog {
      Q_OBJECT

    public:
      CnetEditorSortConfigDialog(CnetEditorWidget *cnetWidget);
      ~CnetEditorSortConfigDialog();

    public slots:
      void applySettings();
      void readSettings();

    private slots:
      void refreshWidgetStates();

    private:
      //! The cneteditor widget we're configuring
      QPointer<CnetEditorWidget> m_cnetWidget;

      //! Enable sorting on the point table
      QPointer<QCheckBox> m_pointSortingCheckBox;

      //! When less than this number, sorting is enabled on the point table
      QPointer<QSpinBox> m_pointTableLimitSpinBox;

      //! Say (very clearly) if sorting is disabled and why
      QPointer<QLabel> m_pointTableWarningsLabel;

      //! Enable sorting on the measure table
      QPointer<QCheckBox> m_measureSortingCheckBox;

      //! When less than this number, sorting is enabled on the measure table
      QPointer<QSpinBox> m_measureTableLimitSpinBox;

      //! Say (very clearly) if sorting is disabled and why
      QPointer<QLabel> m_measureTableWarningsLabel;

  };
}

#endif
