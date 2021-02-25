#ifndef SaveAsDialog_h
#define SaveAsDialog_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QCheckBox>
#include <QRadioButton>
#include "FileDialog.h"

namespace Isis {

  /**
  * @brief Widget to save(Save As) Isis cubes(used in qview) to display the
  * FileDialog to select the output cube. This dialog additionally displays
  * radio buttons for choices FullImage, ExportAsIs, ExportFullRes. These choices
  * are located at the bottom of the dialog.
  *
  * FullImage     - copies the entire image into the user specified output file
  * ExportAsIs    - copies the image as displayed in the qview app window
  * ExportFullRes - copies the image as displayed in the qview app window but with
  *                 full resolution
  *
  * @ingroup Visualization Tools
  *
  * @author 2011-05-11 Sharmila Prasad
  *
  * @internal
  *   @history 2011-05-11  Sharmila Prasad - Initial Version
  *   @history 2017-06-01  Kaj Williams - renamed "save as ..." radio buttons to reflect their actual usage. Updated the associated tooltips.
  *                        Also updated the "enum saveAsType"  documentation.
  */
  class SaveAsDialog : public FileDialog {
      Q_OBJECT
    public:
      SaveAsDialog(QString pTitle, QStringList &pFilterList, QDir &pDir, QWidget *pParent = 0);
      /**
      * @param FullImage,ExportAsIs, ExportFullRes
      **/
      enum saveAsType {FullImage, ExportAsIs, ExportFullRes};
      int getSaveAsType();

    public slots:
      void setFullImage(bool);
      void setAsIs(bool);
      void setFullResolution(bool);

    private:
      QDir &p_dir;//!<The directory to open the dialog with
      QRadioButton *p_fullImage;     //!< FullImage Button
      QRadioButton *p_exportAsIs;    //!< ExportAsIs Button
      QRadioButton *p_exportFullRes; //!< ExportFullRes Button
      saveAsType p_saveAsType;       //!< Current Save Type
  };
};

#endif
