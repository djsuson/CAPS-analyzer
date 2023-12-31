/***************************************************************************
 *   coordtransformer.cpp                                                  *
 *   Copyright (C) 2014 by Daniel Suson                                    *
 *   daniel.suson@purduecal.edu                                            *
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
 * This class transforms between various coordinate systems for displaying *
 * data
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
#ifndef _COORDTRANSFORMER_CPP_
#define _COORDTRANSFORMER_CPP_

#include "coordtransformer.h"
#include <watdefs.h>
#include <afuncs.h>

template<typename _Type> int coordTransformer<_Type>::RAdecTogLatLong(matrixData<_Type>& from)
{
  // check that coordinate system is currently RA/dec. If not then exit
  if (from->coordSystem() != RAdec)
    return -1;

  // copy the original matrixData object for access to parameters and placement of revised data
  matrixData<_Type>* scratch = new matrixData<_Type>(from);

  // reinitialize data structure in scratch data set
  int i, j;
  unsigned long long int numOps, updateUnit, currOp = 0;
  numOps =  3 * from->rows() * from->cols();
  updateUnit = numOps / 100;
  if(updateUnit < 1) updateUnit = 1;

  scratch->m_data.resize(from->cols());
  for (i = 0;i < from->cols();++i) {
    scratch->m_data[i].resize(from->rows());
    for (j = 0;j < from->rows();++j) {
      scratch->m_data[i][j] = 0;
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
  }

  // convert coordinate system
  long oldX, oldY, newX, newY;
  double RA, dec, gLat, gLong;
  for (oldX = 0; oldX < from->cols(); ++oldX) {
    for (oldY = 0; oldY < from->rows(); ++oldY) {
      from->bin2angle(oldX,oldY,&dec,&RA);
      ra_dec_to_galactic(RA,dec,&gLat,&gLong);
      scratch->angle2bin(gLat,gLong,&newX,&newY);
      scratch->m_data[newX][newY] += (from->access())[oldX][oldY];
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
  }

  // load new data array into original matrixData
  for (i = 0;i < from->cols();++i) {
    for (j = 0;j < from->rows();++j) {
      (from->access())[i][j] = scratch->m_data[i][j];
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
  }
  from->coordSystem(galLatLong);

  delete scratch;
  return 0;
}

template<typename _Type> int coordTransformer<_Type>::gLatLongToRAdec(matrixData<_Type>& from)
{
  // check that coordinate system is currently galactic Lat/Long. If not then exit
  if (from->coordSystem() != galLatLong)
    return -1;

  // copy the original matrixData object for access to parameters and placement of revised data
  matrixData<_Type>* scratch = new matrixData<_Type>(from);

  // reinitialize data structure in scratch data set
  int i, j;
  unsigned long long int numOps, updateUnit, currOp = 0;
  numOps =  3 * from->rows() * from->cols();
  updateUnit = numOps / 100;
  if(updateUnit < 1) updateUnit = 1;

  scratch->m_data.resize(from->cols());
  for (i = 0;i < from->cols();++i) {
    scratch->m_data[i].resize(from->rows());
    for (j = 0;j < from->rows();++j) {
      scratch->m_data[i][j] = 0;
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
  }

  // convert coordinate system
  long oldX, oldY, newX, newY;
  double RA, dec, gLat, gLong;
  for (oldX = 0; oldX < from->cols(); ++oldX) {
    for (oldY = 0; oldY < from->rows(); ++oldY) {
      from->bin2angle(oldX,oldY,&gLat,&gLong);
      galactic_to_ra_dec(gLat,gLong, &RA,&dec);
      scratch->angle2bin(dec,RA,&newX,&newY);
      scratch->m_data[newX][newY] += (from->access())[oldX][oldY];
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
  }

  // load new data array into original matrixData
  for (i = 0;i < from->cols();++i) {
    for (j = 0;j < from->rows();++j) {
      (from->access())[i][j] = scratch->m_data[i][j];
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
  }
  from->coordSystem(RAdec);

  delete scratch;
  return 0;
}

#endif
