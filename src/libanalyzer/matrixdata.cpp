/***************************************************************************
 *   matrixdata.cpp                                                        *
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
 * This class provides the functionality needed for two dimensional data   *
 * sets                                                                    *
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
//#ifndef _MATRIXDATA_CPP_
//#define _MATRIXDATA_CPP_
#if 0

#include <string>
#include <math.h>

#include "matrixdata.h"

template <typename _Type> matrixData<_Type>::matrixData()
                                           :baseData() {
  m_nDimensions = 2;
  m_resRA = 0;
  m_resDec = 0;
  m_orientation = Astronomical;
  m_coords = RAdec;
}

template <typename _Type> matrixData<_Type>::matrixData(int x, int y,FILETYPE dataID)
                                            :baseData() {
  m_nDimensions = 2;
  m_cols = x;
  m_rows = y;
  m_orientation = Astronomical;
  m_coords = RAdec;
  m_resRA = 0;
  m_resDec = 0;
  m_datatype = dataID;
}

template <typename _Type> matrixData<_Type>::matrixData(matrixData<_Type> *from)
                                            : baseData((baseData*) from) {

  int i, j;
  m_cols = from->cols();
  m_rows = from->rows();
  initialize();
  for (i = 0; i < m_cols; ++i)
    for (j = 0; j < m_rows; ++j)
      m_data[i][j] = (from->access())[i][j];
  
  m_resRA        = from->RARes();
  m_resDec       = from->DecRes();
  m_orientation  = from->orientation();
  m_coords       = from->coordSystem();
  m_matrixFormat = from->format();
  m_matrix       = from->matrixType();
  m_EgretMinE    = from->EgretMinEnergy();
  m_EgretMaxE    = from->EgretMaxEnergy();
  m_EgretPixels  = from->EgretPixels();
  m_EgretUnit    = from->EgretUnit();
}

template <typename _Type> void matrixData<_Type>::print() {

}

//template <typename _Type> matrixData< _Type >& matrixData<_Type>::operator=(const matrixData<_Type >& other) {
template <typename _Type> matrixData<_Type>& matrixData<_Type>::operator=(const matrixData<_Type >& other) {
  int i, j;

  // basedata variables
  baseData::operator=(&other);
/*
  m_datatype     = other.dataType();
  m_maptype      = other.mapType();
  m_spectrumtype = other.spectrumType();
  m_format       = other.fileFormat();
  m_observatory  = other.instrument();
  m_nDimensions  = other.dimensions();
  m_rows         = other.rows();
  m_cols         = other.cols();
  m_slices       = other.slices();
  m_parts        = other.parts();
  m_maxValue     = other.maxValue();
  m_minValue     = other.minValue();
  m_maxIndex     = other.maxIndex();
  m_minIndex     = other.minIndex();
  m_maxBin       = other.maxBin();
  m_minBin       = other.minBin();
  m_bitpix       = other.bitpix();
  m_fileName     = other.fileName();
//  m_showProgress = other.showProgress();
//  m_uiObject     = other.userInterface();
//  m_updateFunc   = other.updateFunction();
*/
  initialize();
  for (i = 0; i < m_cols; ++i)
    for (j = 0; j < m_rows; ++j)
      m_data[i][j] = (other.access())[i][j];

  m_resRA        = other.RARes();
  m_resDec       = other.DecRes();
  m_orientation  = other.orientation();
  m_coords       = other.coordSystem();
  m_matrixFormat = other.format();
  m_matrix       = other.matrixType();
  m_EgretMinE    = other.EgretMinEnergy();
  m_EgretMaxE    = other.EgretMaxEnergy();
  m_EgretPixels  = other.EgretPixels();
  m_EgretUnit    = other.EgretUnit();
  
  return *this;
}

template <typename _Type> int matrixData<_Type>::initialize() {
  int i, j;
  unsigned long long int numOps, updateUnit, currOp = 0;
  numOps =  m_rows * m_cols;
  updateUnit = numOps / 100;
  if(updateUnit < 1) updateUnit = 1;

  m_data.resize(m_cols);
  for (i = 0;i < m_cols;++i) {
    m_data[i].resize(m_rows);
    for (j = 0;j < m_rows;++j) {
      m_data[i][j] = 0;
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
  }
  m_maxValue = 0.0;
  m_minValue = 0.0;
  m_maxIndex = {m_cols, m_rows, 0};
  m_initialized = true;
  
  return currOp;
}

template <typename _Type> void matrixData<_Type>::reset() {
  for (int i = 0; i < m_cols; ++i)
    m_data[i].clear();
  m_data.clear();
  baseData::reset();
}

#endif
