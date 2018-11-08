#include "CropApp.h"

#include <QString>
#include <QThread>
#include <QDebug>

#include <cmath>

#include "Cube.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "FileName.h"
#include "IException.h"
#include "Projection.h"
#include "AlphaCube.h"
#include "Table.h"
#include "SubArea.h"

using namespace std;
using namespace Isis;

namespace Isis {


int cropper::m_sline= -1;
int cropper::m_linc=-1;
int cropper::m_sband=-1;
int cropper::m_ssample=-1;
int cropper::m_sinc=-1;
Cube * cropper::m_cube=NULL;
LineManager * cropper::m_in = NULL;

int cropper::m_osamples =-1;
int cropper::m_olines=-1;
int cropper::m_obands=-1;

QString cropper::m_outputCubeName="";
bool cropper::m_propspice = false;


         CropApp::CropApp(QString &from, QString &to, int ssample,int nsamples,
                          int sinc, int sline,int nlines,int linc,bool propspice,Cube *cube){

            m_fromCube =from;
            m_toCube = to;
            m_ssample = ssample;
            m_nsamples = nsamples;
            m_sinc = sinc;
            m_sline = sline;
            m_nlines = nlines;
            m_linc = linc;
            m_propspice = propspice;
            m_sband = 1;
            m_in = NULL;
            m_cube = NULL;

            //CubeAttributeInput inAtt(from);
            //m_cube = new Cube();
            //m_cube->setVirtualBands(inAtt.bands());
            //m_cube->open(from);


            cropper *crop = new cropper(ssample,nsamples,sinc,
                                          sline,nlines,linc,propspice,to,m_cube);
            crop->moveToThread(&cropThread);

            connect(&cropThread, &QThread::finished,crop, &QObject::deleteLater);
            connect(this, &CropApp::operate, crop, &cropper::cropit);
            connect(crop, &cropper::resultReady, this, &CropApp::handleResults);
            cropThread.start();

         }

         void CropApp::start() {


          emit (operate (m_fromCube,m_toCube,m_ssample,m_nsamples,
                       m_sinc,m_sline,m_nlines,m_linc,m_propspice,m_cube));


         }

         void CropApp::handleResults() {
           qDebug() << "Results are handled here.";


         }


  cropper::cropper(int ssample,int nsamples,
          int sinc, int sline,int nlines,int linc,bool propspice,QString to,Cube *cube){


          m_sline= sline;
          m_linc= linc;
          m_sband=1;
          m_ssample=ssample;
          m_sinc=sinc;
          m_cube=cube;
          m_outputCubeName=to;
          m_propspice = propspice;


  }
        void cropper::crop(Buffer &out) {
        // Read the input line
        int iline = m_sline + (out.Line() - 1) * m_linc;
        m_in->SetLine(iline, m_sband);
        m_cube->read(*m_in);

        // Loop and move appropriate samples
        for(int i = 0; i < out.size(); i++) {
          out[i] = (*m_in)[(m_ssample - 1) + i * m_sinc];
        }

        if(out.Line() == m_olines) m_sband++;

        }

        void cropper::cropit(QString &from, QString &to, int ssample,int nsamples,
                      int sinc, int sline,
                      int nlines,int linc,bool propspice,Cube *cube) {

             ProcessByLine p;

             int origns = cube->sampleCount();
             int orignl = cube->lineCount();
             int es = cube->sampleCount();
             int el = cube->lineCount();
             int eb = cube->bandCount();
             if (nsamples !=-1) {
               es = ssample + nsamples -1;
             }

             if (nlines != -1) {
               el = sline + nlines -1;
             }

             m_osamples = ceil((double)(es - m_ssample + 1) / m_sinc);
             m_olines = ceil((double)(el - m_sline + 1) / m_linc);
             m_obands = eb;

             es = m_ssample + (m_osamples - 1) * m_sinc;
             el = m_sline + (m_olines - 1) * m_linc;

             p.SetInputCube(cube);
             p.PropagateTables(false);
             CubeAttributeOutput outAtt(to);
             Cube * ocube = p.SetOutputCube(m_outputCubeName,outAtt,m_osamples,m_olines,m_obands);

             p.ClearInputCubes();

           // propagate tables manually
           Pvl &inLabels = *cube->label();

           // Loop through the labels looking for object = Table
           for(int labelObj = 0; labelObj < inLabels.objects(); labelObj++) {
             PvlObject &obj = inLabels.object(labelObj);

             if(obj.name() != "Table") continue;

             // If we're not propagating spice data, dont propagate the following tables...
             if( propspice ) {
               if((IString)obj["Name"][0] == "InstrumentPointing") continue;
               if((IString)obj["Name"][0] == "InstrumentPosition") continue;
               if((IString)obj["Name"][0] == "BodyRotation") continue;
               if((IString)obj["Name"][0] == "SunPosition") continue;
             }

             // Read the table into a table object
             Table table(obj["Name"], from);

             // Write the table
             ocube->write(table);
           }

           Pvl &outLabels = *ocube->label();
           if(! propspice && outLabels.findObject("IsisCube").hasGroup("Kernels")) {
             PvlGroup &kerns = outLabels.findObject("IsisCube").findGroup("Kernels");

             QString tryKey = "NaifIkCode";
             if(kerns.hasKeyword("NaifFrameCode")) {
               tryKey = "NaifFrameCode";
             }

             if(kerns.hasKeyword(tryKey)) {
               PvlKeyword ikCode = kerns[tryKey];
               kerns = PvlGroup("Kernels");
               kerns += ikCode;
             }
           }

           // Create a buffer for reading the input cube
           LineManager * m_in = new LineManager(*cube);

           // Crop the input cube
           p.StartProcess(this->crop);

           delete m_in;
           m_in = NULL;

           // Construct a label with the results
           PvlGroup results("Results");
           results += PvlKeyword("InputLines", toString(orignl));
           results += PvlKeyword("InputSamples", toString(origns));
           results += PvlKeyword("StartingLine", toString(m_sline));
           results += PvlKeyword("StartingSample", toString(m_ssample));
           results += PvlKeyword("EndingLine", toString(el));
           results += PvlKeyword("EndingSample", toString(es));
           results += PvlKeyword("LineIncrement", toString(m_linc));
           results += PvlKeyword("SampleIncrement", toString(m_sinc));
           results += PvlKeyword("OutputLines", toString(m_olines));
           results += PvlKeyword("OutputSamples", toString(m_osamples));

           // Update the Mapping, Instrument, and AlphaCube groups in the output
           // cube label
           SubArea *s;
           s = new SubArea;
           s->SetSubArea(orignl, origns, m_sline, m_ssample, el, es, m_linc, m_sinc);
           s->UpdateLabel(cube, ocube, results);
           delete s;
           s = NULL;

           // Cleanup
           p.EndProcess();
           cube->close();
           delete cube;
           cube = NULL;


           QString result;

           emit resultReady(result);

          }//end cropit

}
