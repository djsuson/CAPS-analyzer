/***************************************************************************
 *   controldatadlg.h                                                      *
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
 * This class defines the control data dialog box structure associated     *
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
#ifndef _CONTROLDATADIALOG_H_
#define _CONTROLDATADIALOG_H_

#include <QtWidgets>

#include "../libanalyzer/atypes.h"
#include "../libanalyzer/association.h"

namespace Ui {
  class controlDataDialog;
}

class controlDataDialog : public QDialog {
  Q_OBJECT
  
  public:
    controlDataDialog(association* assoc);
    virtual ~controlDataDialog();
    bool configured() {return isConfigured;}
    
    CONTROLTYPE dataPattern() const {return dataSet;}
    double RAPeak() const {return peakRA;}
    double decPeak() const {return peakDec;}
    double fullWidthHalfMax() const {return fwhm;}
    double checkerRAWidth() const {return checkRA;}
    double checkerDecWidth() const {return checkDec;}
    long harmonicL() const {return l;}
    long harmonicM() const {return m;}
    double RAResolution() const {return resRA;}
    double decResolution() const {return resDec;}
    double controlSignalStrength() const {return signalStrength;}
    double highDec() const {return top;}
    double lowDec() const {return bottom;}
    double highRA() const {return to;}
    double lowRA() const {return from;}
    M_OP controlOperation() const {return operation;}
    COORDSYSTEM coordSys() const {return coords;}

  public Q_SLOTS:
    void configure(bool open);
    void reset();

  Q_SIGNALS:
    void dataCreationCancelled();
    void buildControlData(FILETYPE,bool);

private:
    Ui::controlDataDialog *ui;
    QSignalMapper *mapper;
    
    association* dataAssoc;
    CONTROLTYPE dataSet;
    FILETYPE dataType;
    double peakRA, peakDec, fwhm;
    double checkRA, checkDec;
    long l, m;
    double resRA, resDec;
    double signalStrength;
    double top, bottom, to, from;
    M_OP operation;
    COORDSYSTEM coords;
    bool dirty, isConfigured;

  private Q_SLOTS:
    bool validate();
    void finalize();
    void addData(QAbstractButton* button);
    void cancel();
    void help();
    void changeStack(int frame);
};
#endif
