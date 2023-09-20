/****************************************************************************
                              \rsht.cpp
Description:\brief (put short description here)

Begin    : Wed Jun 8 2005
Copyright: (C) 2005 by Daniel Suson
email    : daniel.suson@tamuk.edu

-----------------------------------------------------------------------------
                                Change Log
Date       Name                Description

-----------------------------------------------------------------------------

This program was developed at Texas A&M University-Kingsville by Daniel Suson.
It is free software; you may redistribute and/or modify it under the terms
of the GNU General License as published by the Free Software Foundation;
either version 2 of the License. or (at your option) any later version.

This software evolved from work that was done by Eric Hivon at the
California Institute of Technology on the MASTER algorithm (Hivon, E.,
et al., ApJ, vol 567, p 2).  It adapts or interfaces with these packages:

HealPix         (http://www.eso.org/science/healpix/)
FTOOLS & FITSIO (http://heasarc.gsfc.nasa.gov/docs/corp/software.html)

Any publications or presentations that use results generated by this program
must reference the fact that it was developed at Texas A&M
University-Kingsville by Daniel Suson
***************************************************************************/
#include <vector>
#include <healpix_cxx/arr.h>
#include <healpix_cxx/alm_healpix_tools.h>
#include <healpix_cxx/alm_powspec_tools.h>
#include <math.h>

#include "rsht.h"
#include "healpix.h"

RshtTransformer::RshtTransformer()
               : Transformer() {
  m_name            = "rsht";
//  m_association     = 0;
  m_dataMap         = 0;
  m_weightsMap      = 0;
  m_map             = 0;
  m_filterMap       = 0;
  m_beamMap         = 0;
  m_noiseMap        = 0;
  m_invDataMap      = 0;
  m_invMaskMap      = 0;
  m_invFilterMap    = 0;
  m_invBeamMap      = 0;
  m_invNoiseMap     = 0;
  m_almDataValue    = 0;
  m_almDataGrad     = 0;
  m_almDataCurl     = 0;
  m_almWeightsValue = 0;
  m_almWeightsGrad  = 0;
  m_almWeightsCurl  = 0;
  m_almValue        = 0;
  m_almGrad         = 0;
  m_almCurl         = 0;
  m_almFilterValue  = 0;
  m_almFilterGrad   = 0;
  m_almFilterCurl   = 0;
  m_almBeamValue    = 0;
  m_almBeamGrad     = 0;
  m_almBeamCurl     = 0;
  m_almNoiseValue   = 0;
  m_almNoiseGrad    = 0;
  m_almNoiseCurl    = 0;
  m_spectrum        = 0;
  m_cosThetaCut     = 0.0;
  m_scheme          = Rsht;
  m_sides           = 0;
}

RshtTransformer::RshtTransformer(RshtTransformer* from)
                :Transformer(from) {
  m_name            = from->name();
//  m_association     = from->association();
  m_dataMap         = from->dataMap();
  m_weightsMap      = from->weightsMap();
  m_map             = from->map();
  m_filterMap       = from->filterMap();
  m_beamMap         = from->beamMap();
  m_noiseMap        = from->noiseMap();
  m_invDataMap      = from->invDataMap();
  m_invMaskMap      = from->invMaskMap();
  m_invFilterMap    = from->invFilterMap();
  m_invBeamMap      = from->invBeamMap();
  m_invNoiseMap     = from->invNoiseMap();
  m_almDataValue    = from->almDataValue();
  m_almDataGrad     = from->almDataGrad();;
  m_almDataCurl     = from->almDataCurl();
  m_almWeightsValue = from->almWeightsValue();
  m_almWeightsGrad  = from->almWeightsGrad();
  m_almWeightsCurl  = from->almWeightsCurl();
  m_almValue        = from->almValue();
  m_almGrad         = from->almGrad();
  m_almCurl         = from->almCurl();;
  m_almFilterValue  = from->almFilterValue();
  m_almFilterGrad   = from->almFilterGrad();
  m_almFilterCurl   = from->almFilterCurl();
  m_almBeamValue    = from->almBeamValue();
  m_almBeamGrad     = from->almBeamGrad();
  m_almBeamCurl     = from->almBeamCurl();
  m_almNoiseValue   = from->almNoiseValue();
  m_almNoiseGrad    = from->almNoiseGrad();
  m_almNoiseCurl    = from->almNoiseCurl();
  m_spectrum        = from->spectrum();
  m_cosThetaCut     = from->cosThetaCut();
  m_scheme          = from->scheme();
  m_sides           = from->sides();
}

RshtTransformer::~RshtTransformer() {
//  clear(m_association);
}

int RshtTransformer::initialize(association* dataClasses, FILETYPE type) {
  vectorData<double>* pix = 0;
  Healpix_Ordering_Scheme order;
  
//  if (!m_association)
//    m_association = dataClasses;
  
  switch (type) {
    case fileType::PixelizedData:
      pix = dataClasses->pixelizedData();
      m_sides = pix->sides();
      if (pix->layout() == Ring)
        order = RING;
      else
        order = NEST;
      m_dataMap = new Healpix_Map<double>(m_sides,order,SET_NSIDE);
      m_almDataValue = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      if (m_polarization > 1) {
        m_almDataGrad = new Alm<hPoint>(m_maxIndex,m_maxIndex);
        m_almDataCurl = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      }
      break;
    case fileType::PixelizedWeights:
      pix = dataClasses->pixelizedWeights();
      m_sides = pix->sides();
      if (pix->layout() == Ring)
        order = RING;
      else
        order = NEST;
      m_weightsMap = new Healpix_Map<double>(m_sides,order,SET_NSIDE);
      m_almWeightsValue = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      if (m_polarization > 1) {
        m_almWeightsGrad = new Alm<hPoint>(m_maxIndex,m_maxIndex);
        m_almWeightsCurl = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      }
      break;
    case fileType::PixelizedNoise:
      pix = dataClasses->pixelizedNoise();
      m_sides = pix->sides();
      if (pix->layout() == Ring)
        order = RING;
      else
        order = NEST;
      m_noiseMap = new Healpix_Map<double>(m_sides,order,SET_NSIDE);
      m_almNoiseValue = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      if (m_polarization > 1) {
        m_almNoiseGrad = new Alm<hPoint>(m_maxIndex,m_maxIndex);
        m_almNoiseCurl = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      }
      break;
    case fileType::PixelizedFilter:
      pix = dataClasses->pixelizedFilter();
      m_sides = pix->sides();
      if (pix->layout() == Ring)
        order = RING;
      else
        order = NEST;
      m_filterMap = new Healpix_Map<double>(m_sides,order,SET_NSIDE);
      m_almFilterValue = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      if (m_polarization > 1) {
        m_almFilterGrad = new Alm<hPoint>(m_maxIndex,m_maxIndex);
        m_almFilterCurl = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      }
      break;
    case fileType::PixelizedBeam:
      pix = dataClasses->pixelizedBeam();
      m_sides = pix->sides();
      if (pix->layout() == Ring)
        order = RING;
      else
        order = NEST;
      m_beamMap = new Healpix_Map<double>(m_sides,order,SET_NSIDE);
      m_almBeamValue = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      if (m_polarization > 1) {
        m_almBeamGrad = new Alm<hPoint>(m_maxIndex,m_maxIndex);
        m_almBeamCurl = new Alm<hPoint>(m_maxIndex,m_maxIndex);
      }
      break;
    default:
      return -1;
  }

  return m_sides;
}

int RshtTransformer::initialize(association* dataClasses) {
  FILETYPE type = fileType::PixelizedData;
  int numType = static_cast<int>(type);
  int retCode = -1;
  
//  m_association = dataClasses;
  
  while (type != fileType::InverseData) {
    if (type == fileType::PixelOccupancy || type == fileType::WeightedPixel) {
      type = static_cast<FILETYPE>(++numType);
      continue;
    }
    
    if (dataClasses->exists(type))
      retCode = initialize(dataClasses,type);

    type = static_cast<FILETYPE>(++numType);
  }
  
  m_spectrum = new PowSpec(1,m_maxIndex);
  for (unsigned int i = 0; i < m_maxIndex; ++i)
    m_spectrum->tt(i) = 0.0;
  
  return retCode;
}

/* This needs to be resolved in a consistent way... (vector ref issue ) */
int RshtTransformer::loadMap(association* dataClasses, FILETYPE type) {
  vectorData<double> *pix = 0;
  Healpix_Map<double> *map = 0;
  std::vector<double> *vec = 0;
   
  /* Do input variable checks */
  if (!(type == fileType::PixelizedData || type == fileType::PixelizedWeights ||
        type == fileType::PixelizedFilter || type == fileType::PixelizedNoise || type == fileType::PixelizedBeam))
    return -1;
  if (!dataClasses)
    return -1;

  switch (type) {
    case fileType::PixelizedData:
      pix = dataClasses->pixelizedData();
      map = m_dataMap;
      break;
    case fileType::PixelizedWeights:
      pix = dataClasses->pixelizedWeights();
      map = m_weightsMap;
      break;
    case fileType::PixelizedFilter:
      pix = dataClasses->pixelizedFilter();
      map = m_filterMap;
      break;
    case fileType::PixelizedBeam:
      pix = dataClasses->pixelizedBeam();
      map = m_beamMap;
      break;
    case fileType::PixelizedNoise:
      pix = dataClasses->pixelizedNoise();
      map = m_noiseMap;
      break;
    default:
      return -1;
  }

  int size = pix->roAccess().size();
  vec = new std::vector<double>(size);
  *vec = pix->roAccess();
  for (int i = 0; i < size; ++i)
    (*map)[i] = (*vec)[i];
  
  if (vec)
    delete vec;

  return size;
}

int RshtTransformer::loadAlmMap(association* dataClasses, FILETYPE type) {
  cubeData<std::complex<double> > *data = 0;
  Alm<hPoint> *value = 0, *grad = 0, *curl = 0;
  std::vector<std::vector<std::vector<complex<double> > > > *vec = 0;
  unsigned int slice, col, row;

  /* Do input variable checks */
  if (!(type == fileType::AlmData || type == fileType::AlmWeights ||
                type == fileType::AlmBeam || type == fileType::AlmFilter || type == fileType::AlmNoise))
    return -1;
  if (!dataClasses)
    return -1;

  switch (type) {
    case fileType::AlmData:
      data = dataClasses->almData();
      value = m_almDataValue;
      grad = m_almDataGrad;
      curl = m_almDataCurl;
      break;
    case fileType::AlmWeights:
      data = dataClasses->almWeights();
      value = m_almWeightsValue;
      grad = m_almWeightsGrad;
      curl = m_almWeightsCurl;
      break;
    case fileType::AlmFilter:
      data = dataClasses->almFilter();
      value = m_almFilterValue;
      grad = m_almFilterGrad;
      curl = m_almFilterCurl;
      break;
    case fileType::AlmBeam:
      data = dataClasses->almBeam();
      value = m_almBeamValue;
      grad = m_almBeamGrad;
      curl = m_almBeamCurl;
      break;
    case fileType::AlmNoise:
      data = dataClasses->almNoise();
      value = m_almNoiseValue;
      grad = m_almNoiseGrad;
      curl = m_almNoiseCurl;
      break;
    default:
      return -1;
  }

  vec = new std::vector<std::vector<std::vector<std::complex<double> > > >(data->roAccess().size());
  *vec = data->roAccess();
  for (col = 0; col < m_maxIndex; col++)
    for (row = 0; row < m_maxIndex; row++)
      switch ((unsigned int)m_polarization) {
        case 3:
          ((*curl)(col,row)).real((*vec)[2][col][row].real());
          ((*curl)(col,row)).imag((*vec)[2][col][row].imag());
        case 2:
          ((*grad)(col,row)).real((*vec)[1][col][row].real());
          ((*grad)(col,row)).imag((*vec)[1][col][row].imag());
        case 1:
          ((*value)(col,row)).real((*vec)[0][col][row].real());
          ((*value)(col,row)).imag((*vec)[0][col][row].imag());
        default:
          break;
      }
  
  if (vec)
    delete vec;
  
  return data->roAccess().size();
}

int RshtTransformer::storeAlmMap(association* dataClasses, FILETYPE type) {
  cubeData<std::complex<double> > *data = 0;
  Alm<hPoint> *value = 0, *grad = 0, *curl = 0;
  std::vector<std::vector<std::vector<std::complex<double> > > > *vec = 0;
  unsigned int slice, col, row;
  std::complex<double> zeroValue = std::complex<double>(0.0,0.0);

  /* Do input variable checks */
  if (!(type == fileType::AlmData || type == fileType::AlmWeights ||
        type == fileType::AlmBeam || type == fileType::AlmFilter || type == fileType::AlmNoise))
    return -1;
  if (!dataClasses)
    return -1;

  switch (type) {
    case fileType::AlmData:
      data = dataClasses->almData();
      value = m_almDataValue;
      grad = m_almDataGrad;
      curl = m_almDataCurl;
      break;
    case fileType::AlmWeights:
      data = dataClasses->almWeights();
      value = m_almWeightsValue;
      grad = m_almWeightsGrad;
      curl = m_almWeightsCurl;
      break;
    case fileType::AlmFilter:
      data = dataClasses->almFilter();
      value = m_almFilterValue;
      grad = m_almFilterGrad;
      curl = m_almFilterCurl;
      break;
    case fileType::AlmBeam:
      data = dataClasses->almBeam();
      value = m_almBeamValue;
      grad = m_almBeamGrad;
      curl = m_almBeamCurl;
      break;
    case fileType::AlmNoise:
      data = dataClasses->almNoise();
      value = m_almNoiseValue;
      grad = m_almNoiseGrad;
      curl = m_almNoiseCurl;
      break;
    default:
      return -1;
  }

  vec = new std::vector<std::vector<std::vector<std::complex<double> > > >(data->roAccess().size());
  *vec = data->rwAccess();
  for (col = 0; col < m_maxIndex; col++)
    for (row = 0; row < m_maxIndex; row++)
      switch ((unsigned int)m_polarization) {
        case 3:
          (*vec)[2][col][row].real(((*curl)(col,row)).real());
          (*vec)[2][col][row].imag(((*curl)(col,row)).imag());
          if (row > col)
            (*vec)[2][col][row] = zeroValue;
        case 2:
          (*vec)[1][col][row].real(((*grad)(col,row)).real());
          (*vec)[1][col][row].imag(((*grad)(col,row)).imag());
          if (row > col)
            (*vec)[1][col][row] = zeroValue;
        case 1:
          (*vec)[0][col][row].real(((*value)(col,row)).real());
          (*vec)[0][col][row].imag(((*value)(col,row)).imag());
          if (row > col)
            (*vec)[0][col][row] = zeroValue;
        default:
          break;
      }
  
  if (vec)
    delete vec;
  
  return data->roAccess().size();
}

void RshtTransformer::transform(association* dataClasses, FILETYPE type) {
  unsigned long i = 0;
  vectorData<double>* data = 0;
  Alm<hPoint>* alm = 0;
  Healpix_Map<double>* map = 0;

  /* Do input variable checks */
  if (!(type == fileType::TransformedData || type == fileType::TransformedWeights ||
        type == fileType::TransformedBeam || type == fileType::TransformedFilter || type == fileType::TransformedNoise))
    return;
  if (!dataClasses)
    return;

  switch (type) {
    case fileType::TransformedData:
      data = dataClasses->transformedData();
      map = m_dataMap;
      alm = m_almDataValue;
      break;
    case fileType::TransformedWeights:
      data = dataClasses->transformedWeights();
      map = m_weightsMap;
      alm = m_almWeightsValue;
      break;
    case fileType::TransformedFilter:
      data = dataClasses->transformedFilter();
      map = m_filterMap;
      alm = m_almFilterValue;
      break;
    case fileType::TransformedBeam:
      data = dataClasses->transformedBeam();
      map = m_beamMap;
      alm = m_almBeamValue;
      break;
    case fileType::TransformedNoise:
      data = dataClasses->transformedNoise();
      map = m_noiseMap;
      alm = m_almNoiseValue;
      break;
    default:
      return;
  }
  if (!data)
    return;
  
  /* for weights, data array is a uniform value */
  arr<double> weightRing(2 * m_sides);
  weightRing.fill(1.0);

  /* RshtTransformer uses HealPix style layouts - make sure that the layout is a ring structure */
  if (map->Scheme() == NEST)
      map->swap_scheme();

    if (m_iterations > 1)
      map2alm_iter(*map,*alm,m_iterations,weightRing);
    else
      map2alm(*map,*alm,weightRing);

    /* calculate spectrum */
    extract_powspec(*alm,*m_spectrum);

    for (i = m_minIndex; i < m_maxIndex; i++)
      (*data)[i] = m_spectrum->tt(i);

    data->minYIndex(m_minIndex);
    data->maxYIndex(m_maxIndex);
    data->transformerScheme(Rsht);
 
    return;
}

void RshtTransformer::invert(association* dataClasses, FILETYPE type) {
  int i;
  vectorData<double>* data = 0;
  Alm<hPoint>* alm = 0;
  Healpix_Map<double>* map = 0;

  if (!(type == fileType::TransformedData || type == fileType::TransformedWeights ||
        type == fileType::TransformedBeam || type == fileType::TransformedFilter || type == fileType::TransformedNoise))
    return;
  if (!dataClasses)
    return;

  switch (type) {
    case fileType::TransformedData:
      data = dataClasses->inverseData();
      map = m_invDataMap;
      alm = m_almDataValue;
      break;
    case fileType::TransformedWeights:
      data = dataClasses->inverseWeights();
      map = m_invMaskMap;
      alm = m_almWeightsValue;
      break;
    case fileType::TransformedFilter:
      data = dataClasses->inverseFilter();
      map = m_invFilterMap;
      alm = m_almFilterValue;
      break;
    case fileType::TransformedBeam:
      data = dataClasses->inverseBeam();
      map = m_invBeamMap;
      alm = m_almBeamValue;
      break;
    case fileType::TransformedNoise:
      data = dataClasses->inverseNoise();
      map = m_invNoiseMap;
      alm = m_almNoiseValue;
      break;
    default:
      return;
  }
  if (!data)
    return;

  double offset = (*alm)(0,0).real() / sqrt(4.0 * M_PI);
  m_dataSize = map->Npix();

  /* calculate individual alms */
  alm2map(*alm,*map);
  map->Add(offset);

  for (i = 0; i < m_dataSize; i++)
    (*data)[i] = (*map)[i];
}

void RshtTransformer::clear(association* dataClasses) {
  clearArrays(dataClasses);
//  m_association = 0;
  m_active = false;
  m_configured = false;
}

void RshtTransformer::clearArrays(association* dataClasses) {
  clearData(dataClasses);
  clearAlms();
  clearMaps();
}

void RshtTransformer::clearData(association* dataClasses, FILETYPE ft)
{
  if (ft == fileType::Null) {
    FILETYPE dataType = fileType::MAP_LIMIT;
    int count = static_cast<int>(dataType);
    while (dataType != fileType::TRANSFORM_LIMIT) {
      if (dataType == fileType::MAP_LIMIT || dataType == fileType::GRAPH_LIMIT) {
        dataType = static_cast<FILETYPE>(++count);
        continue;
      }
      if (dataClasses->exists(dataType))
        dataClasses->discardRelation(dataType);
    }
  }
  else
    if (dataClasses->exists(ft))
      dataClasses->discardRelation(ft);
    
  return;
}

void RshtTransformer::clearAlms()
{
  if (m_almDataValue)
    delete m_almDataValue;
  m_almDataValue = 0;
  if (m_almDataGrad)
    delete m_almDataGrad;
  m_almDataGrad = 0;
  if (m_almDataCurl)
    delete m_almDataCurl;
  m_almDataCurl = 0;

  if (m_almWeightsValue)
    delete m_almWeightsValue;
  m_almWeightsValue = 0;
  if (m_almWeightsGrad)
    delete m_almWeightsGrad;
  m_almWeightsGrad = 0;
  if (m_almWeightsCurl)
    delete m_almWeightsCurl;
  m_almWeightsCurl = 0;

  if (m_almValue)
    delete m_almValue;
  m_almValue = 0;
  if (m_almGrad)
    delete m_almGrad;
  m_almGrad = 0;
  if (m_almCurl)
    delete m_almCurl;
  m_almCurl = 0;

  if (m_almFilterValue)
    delete m_almFilterValue;
  m_almFilterValue = 0;
  if (m_almFilterGrad)
    delete m_almFilterGrad;
  m_almFilterGrad = 0;
  if (m_almFilterCurl)
    delete m_almFilterCurl;
  m_almFilterCurl = 0;

  if (m_almBeamValue)
    delete m_almBeamValue;
  m_almBeamValue = 0;
  if (m_almBeamGrad)
    delete m_almBeamGrad;
  m_almBeamGrad = 0;
  if (m_almBeamCurl)
    delete m_almBeamCurl;
  m_almBeamCurl = 0;

  if (m_almNoiseValue)
    delete m_almNoiseValue;
  m_almNoiseValue = 0;
  if (m_almNoiseGrad)
    delete m_almNoiseGrad;
  m_almNoiseGrad = 0;
  if (m_almNoiseCurl)
    delete m_almNoiseCurl;
  m_almNoiseCurl = 0;

  return;
}

void RshtTransformer::clearMaps()
{
  if (m_dataMap)
    delete m_dataMap;
  m_dataMap = 0;
  
  if (m_weightsMap)
    delete m_weightsMap;
  m_weightsMap = 0;
  
  if (m_map)
    delete m_map;
  m_map = 0;
  
  if (m_filterMap)
    delete m_filterMap;
  m_filterMap = 0;
  
  if (m_beamMap)
    delete m_beamMap;
  m_beamMap = 0;
  
  if (m_invDataMap)
    delete m_invDataMap;
  m_invDataMap = 0;
  
  if (m_invMaskMap)
    delete m_invMaskMap;
  m_invMaskMap = 0;
  
  if (m_invBeamMap)
    delete m_invBeamMap;
  m_invBeamMap = 0;
  
  if (m_invFilterMap)
    delete m_invFilterMap;
  m_invFilterMap = 0;
  
  if (m_invNoiseMap)
    delete m_invNoiseMap;
  m_invNoiseMap = 0;
  
  return;
}