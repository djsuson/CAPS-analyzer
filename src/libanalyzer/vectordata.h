/***************************************************************************
 *   vectordata.h                                                          *
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
 * This class provides the functionality needed for one dimensional data   *
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
#ifndef _VECTORDATA_H_
#define _VECTORDATA_H_

#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <valarray>

#include "basedata.h"

template <typename _Type> class vectorData : public baseData {

  public:
    vectorData();
    vectorData(int rows, FILETYPE dataID = fileType::Null);
    vectorData(vectorData<_Type> *from);
    ~vectorData() { reset(); }

  public:
    // define inherited methods first

    /**
     * print data
     */
    void print();

    // define specific methods next

    /**
     * initialize matrix to zeros
     */
    int initialize();

    /**
     * access data vector
     */
    std::vector<_Type> roAccess() const {return m_data;}
    std::vector<_Type>& rwAccess()       {return m_data;}

    /**
     * overload [] operator
     */
    _Type& operator[](long i) {return m_data[i];}

    /**
     * overload = operator
     */
    vectorData<_Type>& operator=(const vectorData<_Type>& other);

    /**
     * access to binned flag
     */
    void binned(bool flag) {m_binned = flag;}
    bool binned() const {return m_binned;}

    /**
     * access to index mask
     */
    void mask(int value) {m_maskIndex = value;}
    int mask() const {return m_maskIndex;}

    /**
     * access pixelization scheme
     */
    void pixelScheme(PIXELSCHEME type) {m_pixelScheme = type;}
    PIXELSCHEME pixelScheme() const {return m_pixelScheme;}

    /**
     * access healPIX layout
     */
    void layout(LAYOUT type) {m_layout = type;}
    LAYOUT layout() const {return m_layout;}

    /**
     * access transformer scheme
     */
    void transformerScheme(TRANSFORMERSCHEME type) {m_transformerScheme = type;}
    TRANSFORMERSCHEME transformerScheme() const {return m_transformerScheme;}

    /**
     * access number of sides for healPIX pixelized data
     */
    void sides(int value) {m_sides = value;}
    int sides() const {return m_sides;}

     /**
     * access number of pixels in data
     */
    void numberOfPixels(long nPixels) {m_nPixels = nPixels;}
    long numberOfPixels() const {return m_nPixels;}

    /**
     * access other variables needed for copy constructor
     */
    double maskIndex() const {return m_maskIndex;}
    
  private:
     /**
     * reset data vector
     */
    void reset();

  protected:
    std::vector<_Type>  m_data;
    PIXELSCHEME         m_pixelScheme;
    LAYOUT              m_layout;
    TRANSFORMERSCHEME   m_transformerScheme;
    int                 m_sides;
    long                m_nPixels;
    int                 m_maskIndex;
    bool                m_binned;
};

template <typename _Type> vectorData<_Type>::vectorData()
                                           : baseData() {
  m_pixelScheme = NotPixelized;
  m_transformerScheme = NotTransformed;
  m_layout = Unordered;
  m_sides = 0;
  m_maskIndex = 0;
  m_binned = false;
}

template <typename _Type> vectorData<_Type>::vectorData(int rows, FILETYPE dataID)
                                           : baseData() {
  m_pixelScheme = NotPixelized;
  m_transformerScheme = NotTransformed;
  m_layout = Unordered;
  m_nDimensions = 1;
  m_rows = rows;
  m_datatype = dataID;
  m_sides = 0;
  m_maskIndex = 0;
  m_binned = false;
  initialize();
}

template <typename _Type> vectorData<_Type>::vectorData(vectorData<_Type> *from)
                                           : baseData((baseData*) from) {
  int row;
  m_rows = from->rows();
  initialize();
//  for (row = 0; row < m_rows; ++row)
//    m_data[row] = (from->access())[row];
  m_data = from->roAccess();

  m_pixelScheme       = from->pixelScheme();
  m_layout            = from->layout();
  m_transformerScheme = from->transformerScheme();
  m_sides             = from->sides();
  m_nPixels           = from->numberOfPixels();
  m_maskIndex         = from->maskIndex();
  m_binned            = from->binned();
}

template <typename _Type> vectorData<_Type>& vectorData<_Type>::operator=(const vectorData<_Type> &other) {
  int row;

  // basedata variables
  baseData::operator=((baseData&) other);
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
  m_maxIndex     = other.maxIndex();
  m_minIndex     = other.minIndex();
  m_maxValue     = other.maxValue();
  m_minValue     = other.minValue();
  m_bitpix       = other.bitpix();
  m_fileName     = other.fileName();
//  m_showProgress = other.showProgress();
//  m_uiObject     = other.userInterface();
//  m_updateFunc   = other.updateFunction();
*/
  m_rows = other.rows();
  initialize();
  m_data = other.roAccess();

  m_pixelScheme       = other.pixelScheme();
  m_layout            = other.layout();
  m_transformerScheme = other.transformerScheme();
  m_sides             = other.sides();
  m_nPixels           = other.numberOfPixels();
  m_maskIndex         = other.maskIndex();
  m_binned            = other.binned();

  return *this;
}

template <typename _Type> void vectorData<_Type>::print() {

}

template <typename _Type> int vectorData<_Type>::initialize()
{
  int row = 0;
  unsigned long long int numOps, updateUnit, currOp = 0;
  numOps =  m_rows;
  updateUnit = numOps / 100;
  if(updateUnit < 1) updateUnit = 1;

  m_data.resize(m_rows);
  m_data.assign(m_rows,0);
//  for (row = 0; row < m_rows; ++row) {
//    m_data[row] = 0;
//    if(m_showProgress && !(currOp % updateUnit))
//      informProgress(currOp / updateUnit);
//    currOp++;
//  }

  m_minIndex = {0, 0, 0};
  m_maxIndex = {1, m_rows, 0};
  m_initialized = true;

//  return currOp;
  return m_rows;
}

template <typename _Type> void vectorData<_Type>::reset() {
  m_data.clear();
  baseData::reset();
}

#endif