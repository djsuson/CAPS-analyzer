/***************************************************************************
 *   cubedata.cpp                                                          *
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
 * This class provides the functionality needed for three dimensional data *
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
//#ifndef _CUBEDATA_CPP_
//#define _CUBEDATA_CPP_
#if 0

#include <string>
#include <math.h>

#include "cubedata.h"

template <typename _Type> cubeData<_Type>::cubeData()
                                         : baseData() {
  m_polarization = 1;
  m_nDimensions = 3;
}

template <typename _Type> cubeData<_Type>::cubeData(int x, int y, int z, FILETYPE dataID) {
  m_nDimensions = 3;
  m_cols = x;
  m_rows = y;
  m_slices = z;
  m_polarization = z;
  m_index = y;
  m_datatype = dataID;
}

template <typename _Type> cubeData<_Type>::cubeData(cubeData<_Type> *from)
                                         : baseData((baseData*) from) {
  int i, j, k;
  m_cols = from->cols();
  m_rows = from->rows();
  m_slices = from->slices();
  m_data.resize(m_slices);
  for (k = 0; k < m_slices; ++k) {
    m_data[k].resize(m_cols);
    for (i = 0; i < m_cols; ++i)
      m_data[k][i].resize(m_rows);
  }

  initialize();
  for (k = 0; k < m_slices; ++k)
    for (i = 0; i < m_cols; ++i)
      for (j = 0; j < m_rows; ++j)
        m_data[j][i][k] = (from->access())[j][i][k];

  m_index        = from->index();
  m_polarization = from->polarization();
  for (i = 0; i < 100; ++i) {
    m_minEngs[i] = from->minEng(i);
    m_maxEngs[i] = from->maxEng(i);
  }
}

template <typename _Type> cubeData<_Type>& cubeData<_Type>::operator=(const cubeData<_Type>& other) {
  int i, j, k;
  
  baseData::operator=(&other);

  // basedata variables  
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
  m_bitpix       = other.bitpix();
  m_fileName     = other.fileName();
//  m_showProgress = other.showProgress();
//  m_uiObject     = other.userInterface();
//  m_updateFunc   = other.updateFunction();
*/
  m_cols = other.cols();
  m_rows = other.rows();
  m_slices = other.slices();
  m_data.resize(m_slices);
  for (k = 0; k < m_slices; ++k) {
    m_data[k].resize(m_cols);
    for (i = 0; i < m_cols; ++i)
      m_data[k][i].resize(m_rows);
  }

  initialize();
  for (k = 0; k < m_slices; ++k)
    for (i = 0; i < m_cols; ++i)
      for (j = 0; j < m_rows; ++j)
        m_data[j][i][k] = (other.access())[j][i][k];

  m_index        = other.index();
  m_polarization = other.polarization();
  for (i = 0; i < 100; ++i) {
    m_minEngs[i] = other.minEng(i);
    m_maxEngs[i] = other.maxEng(i);  
  }

  return *this;
}


template <typename _Type> void cubeData<_Type>::print() {

}

template <typename _Type> int cubeData<_Type>::initialize() {
  int i, j, k;
  unsigned long long int numOps, updateUnit, currOp = 0;
  numOps =  m_rows * m_cols * m_slices;
  updateUnit = numOps / 100;
  if(updateUnit < 1) updateUnit = 1;

  m_data.resize(m_slices);
  for (k = 0; k < m_slices; ++k) {
    m_data[k].resize(m_cols);
    for (i = 0; i < m_cols; ++i) {
      m_data[k][i].resize(m_rows);
      for (j = 0; j < m_rows; ++j) {
        m_data[k][i][j] = 0;
//        if(m_showProgress && !(currOp % updateUnit))
//          informProgress(currOp / updateUnit);
        currOp++;
      }
    }
  }

  m_maxIndex = {m_cols, m_rows, m_slices};
  m_maxValue = 0.0;
  m_minValue = 0.0;
  m_initialized = true;
  return m_data.size();
}

 /* not the same as initialize to zero -- is a destructor */
template <typename _Type> void cubeData<_Type>::reset() {
  for (int k = 0; k < m_slices; ++k) {
    for (int i = 0; i < m_cols; ++i)
       m_data[k][i].clear();
    m_data[k].clear();
  }
  m_data.clear();
  baseData::reset();
}

#endif
