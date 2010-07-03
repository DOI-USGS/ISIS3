/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2008/09/03 16:21:02 $
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
#include "CubeBsqHandler.h"
#include "iException.h"
#include "SpecialPixel.h"
#include <cstdio>

using namespace std;
namespace Isis {
  CubeBsqHandler::CubeBsqHandler(IsisCubeDef &cube) :
                      Isis::CubeIoHandler(cube) {

    Isis::PvlObject &core = p_cube->label.FindObject("IsisCube").FindObject("Core");

    if (!core.HasKeyword("Format")) {
      core += Isis::PvlKeyword("Format","BandSequential");
    }

    p_tileSamples = p_cube->samples;
    p_tileLines = (1024 * 1024) / (p_cube->samples * Isis::SizeOf(p_cube->pixelType));
    if (p_tileLines < 1) p_tileLines = 1;

    p_bytesPerTile = p_tileLines * p_tileSamples * Isis::SizeOf(p_cube->pixelType);
    p_sampleTiles = (p_cube->samples - 1) / p_tileSamples + 1;
    p_lineTiles = (p_cube->lines - 1) / p_tileLines + 1;

  //  p_maxTiles = p_lineTiles;
  //  if (p_maxTiles < p_sampleTiles) p_maxTiles = p_sampleTiles;
  //  if (p_maxTiles < p_bands) p_maxTiles = p_bands;
    p_maxTiles = p_cube->bands;

    p_cube->dataBytes = (streampos) p_cube->samples *
                        (streampos) p_cube->lines *
                        (streampos) p_cube->bands *
                        (streampos) Isis::SizeOf(p_cube->pixelType);

    p_bufList.clear();
    p_cacheList.clear();
    p_lastCache = -1;
    p_nullCache.buf = NULL;
    p_tileAllocated.clear();
  }

  CubeBsqHandler::~CubeBsqHandler() {
    Close();
  }

  void CubeBsqHandler::Create(bool overwrite) {
    Isis::CubeIoHandler::Create(overwrite);

    p_tileAllocated.resize(p_sampleTiles*p_lineTiles*p_cube->bands);
    unsigned int ntiles = p_tileAllocated.size();
    for (unsigned int i=0; i<ntiles; i++) {
      p_tileAllocated[i] = false;
    }
  }

  void CubeBsqHandler::Close(const bool removeFile) {
    // Don't do much if the file wasn't opened
    if (!p_cube->stream.is_open()) return;

    // Empty the cache
    unsigned int listSize = p_cacheList.size();
    for (unsigned int i=0; i<listSize; i++) {
      if (p_cacheList[i]->buf != NULL) {
        InternalCache *cache = p_cacheList[i];
        WriteCache(cache);
        delete [] cache->buf;
        cache->buf = NULL;
      }
      delete p_cacheList[i];
      p_cacheList[i] = NULL;
    }
    p_cacheList.clear();
    p_bufList.clear();

    // Write any tiles which where never allocated
    if (p_nullCache.buf == NULL) MakeNullCache();
    unsigned int ntiles = p_tileAllocated.size();
    for (unsigned int i=0; i<ntiles; i++) {
      if (!p_tileAllocated[i]) {
        WriteTile(p_nullCache.buf,i+1);
      }
    }

    p_tileAllocated.clear();
    delete [] p_nullCache.buf;
    p_nullCache.buf = NULL;

    // Close the stream and possible remove
    p_cube->stream.close();
    if (removeFile) remove (p_cube->dataFile.c_str());
  }

  void CubeBsqHandler::Read(Isis::Buffer &rbuf) {
    // See if the cache needs to get bigger
    GrowCache(rbuf);

    // Starting corner in the Isis::Buffer
    int ssamp = rbuf.Sample();
    int sline = rbuf.Line();
    int sband = rbuf.Band();

    // Ending corner in the Isis::Buffer
    int esamp = rbuf.Sample(rbuf.size()-1);
    int eline = rbuf.Line(rbuf.size()-1);
    int eband = rbuf.Band(rbuf.size()-1);

    // Current corner of a cache we will work on
    p_sample = ssamp;
    p_line = sline;
    int tempBand = sband;

    InternalCache *cache;
    char *rawbuf = (char *) rbuf.RawBuffer();
    int ss,es,sl,el;

    while (tempBand <= eband) {
      p_band = p_cube->virtualBandList[tempBand-1];
      cache = FindCache();

      ss = (p_sample > cache->startSamp) ? p_sample : cache->startSamp;
      es = (esamp < cache->endSamp) ? esamp : cache->endSamp;
      sl = (p_line > cache->startLine) ? p_line : cache->startLine;
      el = (eline < cache->endLine) ? eline : cache->endLine;

      int cacheIndex = (sl - cache->startLine) * p_tileSamples +
                        ss - cache->startSamp;
      int rawIndex = rbuf.Index(ss,sl,tempBand);
      int rawAdd = rbuf.SampleDimension();
      int ns = es - ss + 1;

      for (int line = sl; line<=el; line++) {
        Move(rawbuf,rawIndex,cache->buf,cacheIndex,ns);
        cacheIndex += p_tileSamples;
        rawIndex += rawAdd;
      }

      p_sample = cache->endSamp + 1;
      if (p_sample > esamp) {
        p_sample = ssamp;
        p_line = cache->endLine + 1;
        if (p_line > eline) {
          p_line = sline;
          tempBand++;
        }
      }
    }
  }

  void CubeBsqHandler::Write(Isis::Buffer &wbuf) {
    // Put an error check here if the access if ReadOnly

    // See if the cache needs to get bigger
    GrowCache(wbuf);

    // Starting corner in the Isis::Buffer
    // We don't care about pixels outside the cube 
    int ssamp = (wbuf.Sample() < 1) ? 1 : wbuf.Sample();
    int sline = (wbuf.Line() < 1) ? 1 : wbuf.Line();
    int sband = (wbuf.Band() < 1) ? 1 : wbuf.Band();

    // Ending corner in the Isis::Buffer
    int esamp = (wbuf.Sample(wbuf.size()-1) > p_cube->samples) ? p_cube->samples : wbuf.Sample(wbuf.size()-1);
    int eline = (wbuf.Line(wbuf.size()-1) > p_cube->lines) ? p_cube->lines : wbuf.Line(wbuf.size()-1);
    int eband = (wbuf.Band(wbuf.size()-1) > p_cube->bands) ? p_cube->bands : wbuf.Band(wbuf.size()-1);

    // Current corner of a cache we will work on
    p_sample = ssamp;
    p_line = sline;
    p_band = sband;

    InternalCache *cache;
    char *rawbuf = (char *) wbuf.RawBuffer();
    int ss,es,sl,el;

    while (p_band <= eband) {
      cache = FindCache();
      cache->dirty = true;

      ss = (p_sample > cache->startSamp) ? p_sample : cache->startSamp;
      es = (esamp < cache->endSamp) ? esamp : cache->endSamp;
      sl = (p_line > cache->startLine) ? p_line : cache->startLine;
      el = (eline < cache->endLine) ? eline : cache->endLine;

      int cacheIndex = (sl - cache->startLine) * p_tileSamples +
                        ss - cache->startSamp;
      int rawIndex = wbuf.Index(ss,sl,p_band);
      int rawAdd = wbuf.SampleDimension();
      int ns = es - ss + 1;

      for (int line = sl; line<=el; line++) {
        Move(cache->buf,cacheIndex,rawbuf,rawIndex,ns);
        cacheIndex += p_tileSamples;
        rawIndex += rawAdd;
      }

      p_sample = cache->endSamp + 1;
      if (p_sample > esamp) {
        p_sample = ssamp;
        p_line = cache->endLine + 1;
        if (p_line > eline) {
          p_line = sline;
          p_band++;
        }
      }
    }
  }

  void CubeBsqHandler::GrowCache(const Isis::Buffer &buf) {
    // The old method created a new cache for every new buffer used
    // on a cube
#if 0
    if (p_nullCache.buf == NULL) MakeNullCache();

    for (unsigned int i=0; i<p_bufList.size(); i++) {
      if (&buf == p_bufList[i]) return;
    }
    p_bufList.push_back(&buf);

    for (int i=0; i<p_maxTiles; i++) {
      InternalCache *cache = new InternalCache;
      cache->buf = NULL;
      p_cacheList.push_back(cache);
    }
#endif

    // The new method makes six tiles sets worth of caches total.
    // Six was used to ensure large highpass filters don't thrash
    if (p_nullCache.buf != NULL) return;
    MakeNullCache();
    for (int j=0; j<3; j++) {
      for (int i=0; i<p_maxTiles; i++) {
        InternalCache *cache = new InternalCache;
        cache->buf = NULL;
        p_cacheList.push_back(cache);
      }
    }
  }

  CubeBsqHandler::InternalCache *CubeBsqHandler::FindCache () {
    // See if its outside the image
    if ((p_sample < 1) || (p_line < 1) || (p_band < 1) ||
        (p_sample > p_cube->samples) || (p_line > p_cube->lines) ||
        (p_band > p_cube->bands)) {

      if (p_sample <= 0) {
        p_nullCache.startSamp = (p_sample - p_tileSamples) / p_tileSamples * p_tileSamples + 1;
      }
      else {
        p_nullCache.startSamp = (p_sample - 1) / p_tileSamples * p_tileSamples + 1;
      }

      if (p_line <= 0) {
        p_nullCache.startLine = (p_line - p_tileLines) / p_tileLines * p_tileLines + 1;
      }
      else {
        p_nullCache.startLine = (p_line - 1) / p_tileLines * p_tileLines + 1;
      }

      p_nullCache.endSamp = p_nullCache.startSamp + p_tileSamples - 1;
      p_nullCache.endLine = p_nullCache.startLine + p_tileLines - 1;

      p_nullCache.band = p_band;
      p_nullCache.dirty = false;
      return &p_nullCache;
    }

    // Look through the cache list to see if we already have the cache
    // but check the last cache first
    if (p_lastCache >= 0) {
      InternalCache *cache;
      int next = p_lastCache;
      int count = p_cacheList.size();
      for (int i=0; i<count; i++) {
        cache = p_cacheList[next];
        if (cache->buf != NULL) {
          if ((p_sample >= cache->startSamp) && (p_sample <= cache->endSamp) &&
              (p_line >= cache->startLine)   && (p_line <= cache->endLine)   &&
              (p_band == cache->band)) {
           p_lastCache = next;
           return cache;
          }
        }
        next++;
        if (next >= count) next = 0;
      }
    }

    // Ok its not in the cache see if there is an open slot
    InternalCache *cache = NULL;
    unsigned int listSize = p_cacheList.size();
    for (unsigned int i=0; i<listSize; i++) {
      if (p_cacheList[i]->buf == NULL) {
        cache = p_cacheList[i];
        cache->buf = new char [p_bytesPerTile];
        cache->dirty = false;
        p_lastCache = i;
        break;
      }
    }

    // If there are no open slots so chose one to get rid of
    if (cache == NULL) {
      p_lastCache++;
      if (p_lastCache >= (int)p_cacheList.size()) p_lastCache = 0;
      cache = p_cacheList[p_lastCache];
    }

    // Write out the tile
    WriteCache(cache);

    // Set up for reading the  tile
    cache->startSamp = (p_sample - 1) / p_tileSamples * p_tileSamples + 1;
    cache->startLine = (p_line - 1)   / p_tileLines   * p_tileLines   + 1;
    cache->endSamp = cache->startSamp + p_tileSamples - 1;
    cache->endLine = cache->startLine + p_tileLines - 1;
    if (cache->endLine > p_cube->lines) cache->endLine = p_cube->lines;
    cache->band = p_band;
    cache->dirty = false;

    int startTile = (cache->band - 1) * p_sampleTiles * p_lineTiles +
                    (cache->startLine - 1) / p_tileLines * p_sampleTiles +
                    (cache->startSamp - 1) / p_tileSamples + 1;

    // If this cube is being created the tile may not exist so we
    // shouldn't try to read it
    if (p_tileAllocated.size() > 0) {
      if (!p_tileAllocated[startTile-1]) {
        memmove(cache->buf,p_nullCache.buf,p_bytesPerTile);
        p_tileAllocated[startTile-1] = true;
        cache->dirty = true;
        return cache;
      }
    }

    // Ok looks like we need to read the tile
    streampos sbyte = (streampos) (p_cube->startByte - 1) +
                      (streampos) (cache->band - 1) *
                      (streampos) (p_cube->lines) *
                      (streampos) (p_cube->samples * Isis::SizeOf(p_cube->pixelType)) +
                      (streampos)(cache->startLine - 1) *
                      (streampos)(p_cube->samples * Isis::SizeOf(p_cube->pixelType));

    p_cube->stream.seekg(sbyte,std::ios::beg);
    if (!p_cube->stream.good()) {
      string msg = "Error preparing to read data from cube";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    int bytes = (streampos)(cache->endLine - cache->startLine + 1) *
                (streampos)(cache->endSamp - cache->startSamp + 1) *
                (streampos)Isis::SizeOf(p_cube->pixelType);
    p_cube->stream.read(cache->buf,bytes);
    if (!p_cube->stream.good()) {
      string msg = "Error reading data from cube";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    // We have a cache containing the line sample
    return cache;
  }

  void CubeBsqHandler::Move(char *dest, int dindex,
                            char *src, int sindex,
                            int nelements) {
    // Don't change the null cache
    if (dest == p_nullCache.buf) return;

    int nbytes = Isis::SizeOf(p_cube->pixelType);
    if ((p_native) || (nbytes == 1) || (!p_native && (src == p_nullCache.buf))) {
      memmove(&dest[dindex*nbytes],&src[sindex*nbytes],nelements*nbytes);
    }
    else if (nbytes == 2) {
      int d = dindex*nbytes;
      int s = sindex*nbytes;
      for (int i=0; i<nelements; i++) {
        dest[d] = src[s+1];
        dest[d+1] = src[s];
        d+=2;
        s+=2;
      }
    }
    else {
      int d = dindex*nbytes;
      int s = sindex*nbytes;
      for (int i=0; i<nelements; i++) {
        dest[d] = src[s+3];
        dest[d+1] = src[s+2];
        dest[d+2] = src[s+1];
        dest[d+3] = src[s];
        d+=4;
        s+=4;
      }
    }
  }

  void CubeBsqHandler::MakeNullCache() {
    p_nullCache.buf = new char[p_bytesPerTile];

    for (int i=0; i<p_tileSamples*p_tileLines; i++) {
      if (p_cube->pixelType == Isis::UnsignedByte) {
        ((unsigned char *)p_nullCache.buf)[i] = Isis::NULL1;
      }
      else if (p_cube->pixelType == Isis::SignedWord) {
        ((short *)p_nullCache.buf)[i] = Isis::NULL2;
      }
      else if (p_cube->pixelType == Isis::Real) {
        ((float *)p_nullCache.buf)[i] = Isis::NULL4;
      }
      else {
        string msg = "Unsupported pixel type";
        throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
      }
    }
  }

  void CubeBsqHandler::WriteCache (CubeBsqHandler::InternalCache *cache) {
    // Do nothing if the cache isn't dirty
    if (!cache->dirty) return;

    // Otherwise compute sbyte and number of bytes to write
    streampos sbyte = (streampos) (p_cube->startByte - 1) +
                      (streampos) (cache->band - 1) *
                      (streampos) (p_cube->lines) *
                      (streampos) (p_cube->samples * Isis::SizeOf(p_cube->pixelType)) +
                      (streampos)(cache->startLine - 1) *
                      (streampos)(p_cube->samples * Isis::SizeOf(p_cube->pixelType));

    streamsize nbytes = (streamsize)(cache->endLine - cache->startLine + 1) *
                       (streamsize)(cache->endSamp - cache->startSamp + 1) *
                       (streamsize)Isis::SizeOf(p_cube->pixelType);

    WriteBuf(cache->buf,sbyte,nbytes);
  }

  void CubeBsqHandler::WriteTile (char *buf, int tile) {
    int tilesPerBand = (p_tileAllocated.size() - 1) / p_cube->bands + 1;
    int band = (tile - 1) / tilesPerBand + 1;

    streampos sbyte = (streampos) (p_cube->startByte - 1) +
                      (streampos) (band - 1) *
                      (streampos) (p_cube->lines) *
                      (streampos) (p_cube->samples * Isis::SizeOf(p_cube->pixelType));

    int relativeTile = (tile - (band - 1) * tilesPerBand);
    sbyte += (relativeTile - 1) * p_bytesPerTile;

    streamsize nbytes;
    if (relativeTile == tilesPerBand) {
      int lines = p_cube->lines - (relativeTile - 1) * p_tileLines;
      nbytes = (streamsize) lines *
               (streamsize) (p_cube->samples * Isis::SizeOf(p_cube->pixelType));
    }
    else {
      nbytes = p_bytesPerTile;
    }

    WriteBuf(buf,sbyte,nbytes);
  }

  void CubeBsqHandler::WriteBuf (char *buf, std::streampos sbyte, std::streamsize nbytes) {
    p_cube->stream.seekp(sbyte,std::ios::beg);
    if (!p_cube->stream.good()) {
      string msg = "Error preparing to write data to cube";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    p_cube->stream.write(buf,nbytes);
    if (!p_cube->stream.good()) {
      string msg = "Error writing data to cube";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }
  }
}
