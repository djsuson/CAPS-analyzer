/***************************************************************************
 *   graphdlg.h                                                            *
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
 * This class defines the grapher dialog box structure associated          *
 * with the Qt4 user interface.                                            *
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
#ifndef _GRAPHDIALOG_H_
#define _GRAPHDIALOG_H_

#include <QtWidgets>

//#include "../libanalyzer/analyzer.h"
#include "../libanalyzer/atypes.h"
#include "../libanalyzer/association.h"

namespace Ui {
  class graphDialog;
}

class graphDialog : public QDialog {
  Q_OBJECT
  
  public:
    graphDialog(association *assoc);
    virtual ~graphDialog();
    bool configured() {return isConfigured;}
    
  public Q_SLOTS:
    void configure();
    void configure(bool triggered);
    void reset();
    
  Q_SIGNALS:
    void grapherCancelled();
    void grapherReady();
    
  private:
    void validate();
    int configureInterface();

    Ui::graphDialog *ui;
    association *dataAssoc;
    long xSize;
    long ySize;
    bool dirty, isConfigured;
//    analyzer_graph_hdl currentHandle;

  private Q_SLOTS:
    void finalize();
    void help();
    void activateCustom(bool check);
    void cancel();
};
#endif
