#ifndef MdiCubeViewport_h
#define MdiCubeViewport_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CubeViewport.h"

#include <QMetaType>


class QPaintEvent;
template < class T > class QList;

namespace Isis {
  class Cube;
  class Tool;
  class ViewportBuffer;

  /**
   * @brief Cube display widget for certain Isis MDI applications
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Eric Hyer - Moved MDI specific code here from
   *                        CubeViewport
   *
   * @see Workspace CubeViewport
   *
   * @internal
   *   @history 2012-03-22 Steven Lambright - Added Qt meta type declarations
   *                           for QVariant.
   */
  class MdiCubeViewport : public CubeViewport {
      Q_OBJECT

    public:
      MdiCubeViewport(Cube *cube, Isis::CubeDataThread * cdt = 0,
                      QWidget *parent = 0);
      ~MdiCubeViewport();

      void forceAbstract() {}

      //! Is the viewport linked with other viewports
      bool isLinked() const {
        return p_linked;
      };

      void registerTool(Tool *tool);
      void paintEvent(QPaintEvent *e);
      void restretch(ViewportBuffer *buffer);


    signals:
      void linkChanging(bool);
      void requestRestretch(MdiCubeViewport *, int);


    public slots:
      void setLinked(bool b);
      void viewGray(int band);
      void viewRGB(int red, int green, int blue);


    private:
      bool p_linked;
      QList<Tool *> p_toolList;

  };
}

Q_DECLARE_METATYPE(Isis::MdiCubeViewport *);

#endif
