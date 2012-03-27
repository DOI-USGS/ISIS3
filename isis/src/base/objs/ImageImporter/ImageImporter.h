#ifndef ImageImporter_h
#define ImageImporter_h

namespace Isis {
  class Buffer;
  class Cube;
  class CubeAttributeOutput;
  class Filename;
  class ImageImporter;

  /**
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   */
  class ImageImporter {
    public:
      ImageImporter(Filename inputName);
      virtual ~ImageImporter();

      void operator()(Buffer &out) const;

      Cube * import(Filename outputName);
      Cube * import(Filename outputName, CubeAttributeOutput &att);

      void setNullRange(double min, double max);
      void setLrsRange(double min, double max);
      void setHrsRange(double min, double max);

      void setSamples(int s);
      void setLines(int l);
      void setBands(int b);

      int samples() const;
      int lines() const;
      int bands() const;

      Filename filename() const;

      virtual bool isGrayscale() const = 0;
      virtual bool isRgb() const = 0;
      virtual bool isArgb() const = 0;

      static ImageImporter * fromFilename(Filename inputName);

    protected:
      typedef int (ImageImporter::*GetChannelMethod)(int pixel) const;

      Cube * createOutput(Filename outputName, CubeAttributeOutput &att);

      void setDefaultBands();
      double testSpecial(double pixel) const;

      virtual GetChannelMethod getBandChannel(int band) const;
      virtual int convertRgbToGray(int pixel) const;

      virtual void updateRawBuffer(int line, int band) const = 0;
      virtual int getPixel(int s, int l) const = 0;

      virtual int getGray(int pixel) const = 0;
      virtual int getRed(int pixel) const = 0;
      virtual int getGreen(int pixel) const = 0;
      virtual int getBlue(int pixel) const = 0;
      virtual int getAlpha(int pixel) const = 0;

    private:
      Filename *m_inputName;
      Cube *m_outCube;

      int m_samples;
      int m_lines;
      int m_bands;

      double m_nullMin;
      double m_nullMax;
      double m_lrsMin;
      double m_lrsMax;
      double m_hrsMin;
      double m_hrsMax;
  };
};


#endif
