/***************************************************************************
 *   inputmatrixdata.cpp                                                   *
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
 * This is a special version of matrixData designed to handle basic        *
 * input data from the user                                                *
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
#include <math.h>
#include <fstream>
#include <gsl/gsl_sf_legendre.h>
#include <gsl/gsl_matrix.h>

#include "inputmatrixdata.h"

inputMatrixData::inputMatrixData(int x, int y, FILETYPE type)
               :matrixData<double>(x,y,type) {
  m_resRA = 360.0 / x;
  m_resDec = 180.0 / y;
}

inputMatrixData::inputMatrixData(double resRA, double resDec,FILETYPE type)
               : matrixData<double>(resRAtoBins(resRA),resDectoBins(resDec),type) {
  m_resRA = resRA;
  m_resDec = resDec;
}

inputMatrixData::inputMatrixData(inputMatrixData* from)
               : matrixData<double>(from->m_cols,from->m_rows,from->m_datatype){
  
}

/*
inputMatrixData& inputMatrixData::operator=(inputMatrixData& other) {
  // basedata variables
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
  m_maxIndex     = other.maxIndex();
  m_minIndex     = other.minIndex();
  m_maxBin       = other.maxBin();
  m_minBin       = other.minBin();
  m_maxValue     = other.maxValue();
  m_minValue     = other.minValue();
  m_bitpix       = other.bitpix();
  m_fileName     = other.fileName();
//  m_showProgress = other.showProgress();
//  m_uiObject     = other.userInterface();
//  m_updateFunc   = other.updateFunction();

  int columns = (other.access()).size();
  int rows = ((other.access())[0]).size();
  if (columns != m_cols)
    m_cols = columns;
  if (rows != m_rows)
    m_rows = rows;
  initialize();
  m_data = other.access();
  
  m_resRA        = other.RARes();
  m_resDec       = other.DecRes();
  m_maxValue     = other.maxValue();
  m_orientation  = other.orientation();
  m_matrixFormat = other.format();
  m_matrix       = other.matrixType();
  m_EgretMinE    = other.EgretMinEnergy();
  m_EgretMaxE    = other.EgretMaxEnergy();
  m_EgretPixels  = other.EgretPixels();
  m_EgretUnit    = other.EgretUnit();

  return *this;
}
*/

/* Uniform and zero -- fast access function */
long inputMatrixData::create(double value) {
  int i,j;
  unsigned long long int numOps, updateUnit, currOp;
  numOps =  m_rows * m_cols;
  updateUnit = numOps / 100;
  if(updateUnit < 1) updateUnit = 1;

  currOp = 0;
  i = 0;
  while(i < m_cols) {
    j = 0;
    while(j < m_rows) {
      m_data[i][j] += value;
      if (m_data[i][j] > m_maxValue)
        m_maxValue = m_data[i][j];
      if (m_data[i][j] < m_minValue)
        m_minValue = m_data[i][j];
      j++;
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      currOp++;
    }
    i++;
  }
  m_minIndex = {0, 0, 0};
  m_maxIndex = {m_cols, m_rows, 0};

  m_initialized = true;
  return 0;
}

/* Checkerboard function: uses discrete array addresses directly. */
long inputMatrixData::create(double checkerWidth, double checkerHeight, double value) {
  int row, col, rowAccum, colAccum, width, height;
  unsigned long long int numOps, updateUnit, currOp;

  width = (int)(fabs(checkerWidth) / m_resRA);
  height = (int)(fabs(checkerHeight) / m_resDec);
  if(!width) width = 1;
  if(!height) height = 1;
  numOps = m_cols * m_rows / 2;
  updateUnit = numOps / 100;
  if(updateUnit < 1) updateUnit = 1;
  currOp = 0;

  rowAccum = 0;
  colAccum = 0;
  row = 0;
  while(row < m_rows) {
    col = 0;
    if(rowAccum >= height)
       col = width;
    if(rowAccum >= (2 * height)) {
       rowAccum = 0;
       col = 0;
    }

    colAccum = 0;
    while(col < m_cols) {
      if(colAccum >= width) {
        col += width;
        if(col >= m_cols)
          continue;
        colAccum = 0;
      }
      m_data[col][row] += value;
      if (m_data[col][row] > m_maxValue)
        m_maxValue = m_data[col][row];
      if (m_data[col][row] < m_minValue)
        m_minValue = m_data[col][row];

      colAccum++;
      col++;
      currOp++;
    }
//    if(m_showProgress && !(currOp % updateUnit))
//      informProgress(currOp / updateUnit);
    row++;
    rowAccum++;
  }
  m_minIndex = {0, 0, 0};
  m_maxIndex = {width, height, 0};

  m_initialized = true;
  return 0;
}

/* Uniform patch function using spherical coordinates (retains projection). */
long inputMatrixData::create(double highDec, double lowDec, double highRA, double lowRA, double value) {
  unsigned long long int numOps, updateUnit, currOp;
  double theta,phi;
  long x,y;
  double spanTheta = fabs(highDec - lowDec);
  double spanPhi = fabs(highRA - lowRA);

  int minCurIndex = 0, maxCurIndex = 0;
  numOps = (spanTheta / m_resDec) * (spanPhi / m_resRA);
  updateUnit = numOps / 100;
  if(updateUnit < 1)
    updateUnit = 1;
  currOp = 0;

  theta = lowDec;
  phi = lowRA;
  angle2bin(theta,phi,x,y);
  m_minIndex = {(int)x, (int)y, 0};
  while(theta <= (lowDec + spanTheta)) {
    phi = lowRA;
    while(phi <= (lowRA + spanPhi)) {
      angle2bin(theta,phi,x,y);
      m_data[x][y] += value;
      if (m_data[x][y] > m_maxValue)
        m_maxValue = m_data[x][y];
      if (m_data[x][y] < m_minValue)
        m_minValue = m_data[x][y];

      currOp++;
//      if (m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      phi += m_resRA;
    }
    theta += m_resDec;
  }
  angle2bin(theta,phi,x,y);
  m_maxIndex = {(int)x, (int)y, 0};


  m_initialized = true;
  return 0;
}

// #define PART_MAG(x,y) ((x)*(x) + (y)*(y))

 /* Gaussian/delta function. */
long inputMatrixData::create(double peakRA, double peakDec, double sigma, double value) {
  
  if (sigma <= 0) {  /* delta or default action on invalid sigma. */
    long peakRABin, peakDecBin;
    angle2bin(peakDec,peakRA,peakRABin,peakDecBin);
    m_data[peakRABin][peakDecBin] += value;
    if (m_data[peakRABin][peakDecBin] > m_maxValue)
      m_maxValue = m_data[peakRABin][peakDecBin];
    if (m_data[peakRABin][peakDecBin] < m_minValue)
      m_minValue = m_data[peakRABin][peakDecBin];
    m_minIndex = {(int)peakRABin, (int)peakDecBin, 0};
    m_maxIndex = {(int)peakRABin, (int)peakDecBin, 0};
  }
  else { /* gaussian */
    unsigned long long int numOps, updateUnit, currOp;
    double theta,phi;
    long x,y;
    double twoSigmaSquare =  2.0 * sigma * sigma;
    double gaussSpanTheta = 5.0 * sigma;
    double gaussSpanPhi = 5.0 * sigma;

    numOps = (2.0 * gaussSpanTheta / m_resDec) * (2.0 * gaussSpanPhi / m_resRA);
    updateUnit = numOps / 100;
    if(updateUnit < 1)
      updateUnit = 1;
    currOp = 0;

    theta = peakDec - gaussSpanTheta;
    phi = peakRA - gaussSpanPhi;
    angle2bin(theta,phi,x,y);
    m_minIndex = {(int)x, (int)y, 0};
    while(theta < (peakDec + gaussSpanTheta)) {
      phi = peakRA - gaussSpanPhi;
      while(phi < (peakRA + gaussSpanPhi)) {
        angle2bin(theta,phi,x,y);
        /*
            When the vector magnitude taken from the peak coordinate is
            substituted for 'x - mean' in (x - mean) ^ 2, the
            square-square root operations cancel.
        */
        double deltaTheta2 = (theta - peakDec) * (theta - peakDec);
        double deltaPhi2 = (phi - peakRA) * (phi - peakRA);
        double sine2 = sin((90.0 - theta) * (M_PI / 180.0)) * sin((90.0 - theta) * (M_PI / 180.0));
        double deviation2 = deltaTheta2 + (sine2 * deltaPhi2);
        m_data[x][y] += value * exp(-deviation2 / twoSigmaSquare);
        if (m_data[x][y] > m_maxValue)
          m_maxValue = m_data[x][y];
        if (m_data[x][y] < m_minValue)
          m_minValue = m_data[x][y];
        currOp++;
//        if(m_showProgress && !(currOp % updateUnit))
//          informProgress(currOp / updateUnit);
        phi += m_resRA;
      }
      theta += m_resDec;
    }
    angle2bin(theta,phi,x,y);
    m_maxIndex = {(int)x, (int)y, 0};
  }
  m_initialized = true;
  return 0;
}

/* Spherical harmonic function. */
long inputMatrixData::create(int l, int m, double value) {
  long x, y;
  unsigned long long int numOps, updateUnit, currOp;
  double theta, phi, thetaInc, phiInc, convTheta, convPhi,PlmValue, sign = 1.0;
  double toDegrees = 180.0 / M_PI;
  double twoPi = 2.0 * M_PI;

  thetaInc = m_resDec * (M_PI / 180.0);
  phiInc = m_resRA * (M_PI / 180.0);
  numOps =  m_rows * m_cols;
  updateUnit = numOps/100;
  if(updateUnit < 1)
    updateUnit = 1;
  currOp = 0;

  if(m < 0) {
    m = -m;
    if(m%2)
      sign = -1.0; // recreates alternating sequence behavior in original code.
  }

  theta = 0;
  while(theta < M_PI) {
    PlmValue = value * gsl_sf_legendre_sphPlm(l,m,sign * cos(theta));
    convTheta = 90.0 - (theta * toDegrees);
    phi = 0;
    while(phi < twoPi) {
      convPhi = (phi * toDegrees) - 180.0;
//      angle2bin(convTheta,convPhi,x,y);
      angle2bin(theta,phi,x,y);
      /* use cosine term for now */
      m_data[x][y] = PlmValue * cos(m * phi);
      if (m_data[x][y] > m_maxValue)
        m_maxValue = m_data[x][y];
      if (m_data[x][y] < m_minValue)
        m_minValue = m_data[x][y];

      currOp++;
//      if(m_showProgress && !(currOp % updateUnit))
//        informProgress(currOp / updateUnit);
      phi += phiInc;
    }
    theta += thetaInc;
  }
  m_minIndex = {0, 0, 0};
  m_maxIndex = {m_cols, m_rows, 0};

  m_initialized = true;
  return 1;
}

/*
   Functions to guard against divide by zero on matrixData constructor
   call.
*/

int inputMatrixData::resRAtoBins(double resRA) {
  if(resRA < 0.000001)
    resRA = 0.000001;
  return  (int)(360.0 / resRA);
}

int inputMatrixData::resDectoBins(double resDec) {
  if(resDec < 0.000001)
    resDec = 0.000001;
  return (int)(180.0 / resDec);
}
