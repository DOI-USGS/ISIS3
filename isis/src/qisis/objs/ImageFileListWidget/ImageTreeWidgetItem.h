#ifndef ImageTreeWidgetItem_H
#define ImageTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {
  class Image;
  class ImageList;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2011-05-18 Steven Lambright - Resolution now sorts correctly
   *   @history 2012-10-02 Steven Lambright - Added phase angle, aspect ratio, sample resolution,
   *                           line resolution and north azimuth columns, added setColumnValue().
   */
  class ImageTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      ImageTreeWidgetItem(ImageList *imageList, Image *image,
                          QTreeWidget *parent = 0);
      virtual ~ImageTreeWidgetItem();

      Image *image();
      QString imageListName() const;

      void forgetImage();

      void update(bool save);

      // This is the column number for each column
      enum TreeColumn {
        NameColumn = 0,
        FootprintColumn,
        OutlineColumn,
        ImageColumn,
        LabelColumn,

        ResolutionColumn,
        EmissionAngleColumn,
        IncidenceAngleColumn,
        PhaseAngleColumn,
        AspectRatioColumn,
        SampleResolutionColumn,
        LineResolutionColumn,
        NorthAzimuthColumn,

        BlankColumn
      };

      static QString treeColumnToString(TreeColumn);

      using QTreeWidgetItem::parent;

    public slots:
      void onDisplayPropertiesChanged();

    private:
      void setColumnValue(TreeColumn column, double value);
      bool operator<(const QTreeWidgetItem &other) const;

      Image *m_image;
      ImageList *m_imageList;

      Qt::CheckState toCheck(QVariant);
  };
}

#endif

