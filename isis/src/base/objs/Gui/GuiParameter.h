#ifndef Isis_GuiParameter_h
#define Isis_GuiParameter_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $ 
 * $Date: 2009/12/15 20:44:57 $ 
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

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QToolButton>

#include "iString.h"

namespace Isis {
  class UserInterface;

/**
 * @author 2006-10-31 ??? 
 *                                                                        
 * @internal 
 *   @history 2009-11-10 Mackenzie Boyd - Refactored to reduce
 *            code duplication in children GuiCubeParameter and
 *            GuiFilenameParameter, specifically, SelectFile
 *            method.
 *   @history 2009-12-15 Travis Addair - Moved the SelectFile
 *            method back to children.
 */

  class GuiParameter : public QObject {

    Q_OBJECT

    public:

      GuiParameter (QGridLayout *grid, UserInterface &ui, int group, int param);
      virtual ~GuiParameter ();

      //! Return the name of the parameter
      iString Name() const { return p_name; };

      void SetToDefault ();

      void SetToCurrent ();

      virtual iString Value () = 0;

      virtual void Set (iString newValue) = 0;

      void SetEnabled (bool enabled);

      //! Is the parameter enabled
      bool IsEnabled () const { return p_label->isEnabled(); }

      virtual bool IsModified();

      void Update();

      void RememberWidget(QWidget *w);

      QWidget *AddHelpers(QObject *lo);

      virtual std::vector<std::string> Exclusions();

      enum ParameterType { IntegerWidget, DoubleWidget, StringWidget,
                           ListWidget, FilenameWidget, CubeWidget,
                           BooleanWidget };
      ParameterType Type() { return p_type; };

    protected:

      QToolButton *p_fileButton;
      QLineEdit *p_lineEdit;

      int p_group;
      int p_param;
      iString p_name;
      UserInterface *p_ui;

      QLabel *p_label;

      QList<QWidget *> p_widgetList;

      ParameterType p_type;

    private:
      QMenu *p_helperMenu;

    signals:
      void ValueChanged();
      void HelperTrigger(const QString &);

  };
};

#endif
