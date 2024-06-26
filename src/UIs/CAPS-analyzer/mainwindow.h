/***************************************************************************
 *   mainwindow.h                                                          *
 *   Copyright (C) 2014 by Daniel Suson                                    *
 *   suson@calumet.purdue.edu                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   In addition, as a special exception, the copyright holders give       *
 *   permission to link the code of this program with any edition of       *
 *   the Qt library by Trolltech AS, Norway (or with modified versions     *
 *   of Qt that use the same license as Qt), and distribute linked         *
 *   combinations including the two.  You must obey the GNU General        *
 *   Public License in all respects for all of the code used other than    *
 *   Qt.  If you modify this file, you may extend this exception to        *
 *   your version of the file, but you are not obligated to do so.  If     *
 *   you do not wish to do so, delete this exception statement from        *
 *   your version.                                                         *
 ***************************************************************************
 * Description:                                                            *
 *                                                                         *
 * This class defines the main window structure associated with the user   *
 * Qt5 user interface.                                                     *
 * ----------------------- Change Log ------------------------------------ *
 * Date    Name         Description                                        *
 *                                                                         *
 ***************************************************************************
 * This software evolved from work that was done by Eric Hivon at the      *
 * California Institute of Technology on the MASTER algorithm (Hivon, E.,  *
 * et al., ApJ, vol 567, p 2).  It adapts or interfaces with these         *
 * packages:                                                               *
 *                                                                         *
 * HealPix         (http://www.eso.org/science/healpix/)                   *
 * FTOOLS & FITSIO (http://heasarc.gsfc.nasa.gov/docs/corp/software.html)  *
 *                                                                         *
 * Any publications or presentations that use results generated by this    *
 * program must reference the fact that it was developed by Daniel Suson   *
 ***************************************************************************/
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

//#include "../libanalyzer/analyzer.h"
#include "../libanalyzer/atypes.h"
#include "../libanalyzer/association.h"
#include "../libanalyzer/filemanager.h"
#include "../libanalyzer/pixelizer.h"
#include "../libanalyzer/transformer.h"
#include "../libanalyzer/spectrum.h"
#include "../libgraphics/mapper.h"
#include "../libgraphics/grapher.h"
#include "../libgraphics/progress.h"

#include <QtGui>

#include "controldatadlg.h"
#include "energydlg.h"
#include "mapperdlg.h"
#include "mapselectdlg.h"
#include "graphdlg.h"
#include "graphselectdlg.h"
#include "pixelizerdlg.h"
#include "healpixdlg.h"
#include "transformerdlg.h"
#include "rshtdlg.h"
#include "spectrumdlg.h"
#include "qledlabel.h"

namespace Ui {
  class MainWindow;
}

class mainWindow : public QMainWindow {
  Q_OBJECT
  
  public:
    mainWindow();
    virtual ~mainWindow();
    static void progressBarWrapper(void* uiObj, int value);
    void updateProgressBar(int value);
    
  public Q_SLOTS:
    void openFile();
    void readData(double minEnergy, double maxEnergy);
    void saveFile();
    void pixelize();
    void transform();
    void analyze();
    void createControlData(FILETYPE dataType,bool complete);
    void configureDisplay(FILETYPE dataType);
    void buildMaps();
    void displayMap(ASSOCIATEDMAP map);
    void buildGraphs();
    void displayGraph(ASSOCIATEDSPECTRUM graph);
    void configurePixelizer(PIXELSCHEME scheme);
    void configureTransformer(TRANSFORMERSCHEME scheme);
    
  Q_SIGNALS:
    void redrawMap();
    void redrawGraph();
    void selectEnergies(double,double);
    void selectMapDisplay(unsigned int);
    void selectGraphDisplay(unsigned int);
    void dataReady(FILETYPE);

  private:
    void configureMaps();
    void configureGraphs();
    void clearMaps();
    void clearGraphs();
    void readData();
    bool pixelize(FILETYPE inputDataType, FILETYPE pixelDataType);
    bool transform(FILETYPE pixelDataType, FILETYPE transDataType);

    Ui::MainWindow *ui;
    
    controlDataDialog    *ctrlDlg;
    energyDialog         *energyDlg;
    pixelizerDialog      *pixSelectDlg;
    healpixDialog        *healpixDlg;
    transformerDialog    *transSelectDlg;
    rshtDialog           *rshtDlg;
    spectrumDialog       *specDlg;
    mapperDialog         *mapperDlg;
    mapSelectDialog      *mapSelectDlg;
    graphDialog          *graphDlg;
    graphSelectDialog    *graphSelectDlg;
    
    progress             *s_progressInterface;

    association          *s_association;
    FILETYPE             selectedDataType;
    GENERICTYPE          selectedDataStream;
    FORMAT               dataFormat;
    unsigned char        *activeMap;
    unsigned char        *activeGraph;
    QString              fileName;
    QDir                 directory;

    int pbValue;
    
  private Q_SLOTS:
    void paintMap(unsigned char* map);
    void paintGraph(unsigned char* graph);
    void reset();
    void selectPixelizer();
    void selectTransformer();
};
#endif
