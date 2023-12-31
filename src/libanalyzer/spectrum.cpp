/****************************************************************************
                              \spectrum.cpp
Description:\brief (put short description here)

Begin    : Fri Jul 1 2005
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
#include <math.h>
#include <gsl/gsl_sf_coupling.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_rng.h>
#include <healpix_cxx/arr.h>
#include <healpix_cxx/xcomplex.h>
#include <healpix_cxx/planck_rng.h>
#include <healpix_cxx/powspec.h>
#include <healpix_cxx/alm.h>
#include <healpix_cxx/alm_powspec_tools.h>
#include <healpix_cxx/healpix_map.h>

#include "spectrum.h"

Spectrum::Spectrum() {
  m_configured = false;
  m_active = false;
  m_minIndex  = 0;
  m_maxIndex  = 1;
  m_minBin    = 0;
  m_maxBin    = 1;
  m_indices   = 1;
  m_maskIndex = 0;
  m_lowBinL   = 0;
  m_binning = true;
  m_invert  = true;
}

Spectrum::Spectrum(Spectrum* from) {
  m_configured = from->configured();
  m_active = from->active();
  m_minIndex  = from->minIndex();
  m_maxIndex  = from->maxIndex();
  m_minBin    = from->minBin();
  m_maxBin    = from->maxBin();
  m_indices   = from->indices();
  m_maskIndex = from->maskIndex();
  m_lowBinL   = from->lowBinL();
  m_binning = from->binning();
  m_invert  = from->invert();
}


Spectrum::~Spectrum() {
  clear();
}

void Spectrum::initialize() {
  m_mode_modeMatrix   = gsl_matrix_calloc(m_maxIndex,m_maxIndex);
  m_inverseModeMatrix = gsl_matrix_calloc(m_maxIndex,m_maxIndex);

  if (m_binning) {
    m_maxBin = m_maxIndex / m_indices;
    if (m_maxIndex%m_maxBin)
      m_maxBin++;
    m_minBin = m_minIndex / m_indices;

    m_bin_indexMatrix  = gsl_matrix_calloc(m_maxBin,m_maxIndex);
    m_index_binMatrix  = gsl_matrix_calloc(m_maxIndex,m_maxBin);
    m_bin_binMatrix    = gsl_matrix_calloc(m_maxBin,m_maxBin);
    m_inverseBinMatrix = gsl_matrix_calloc(m_maxBin,m_maxBin);
  }

 m_configured = true;
}

void Spectrum::loadCouplingMatrix(association *asc) {
  long i,j;

  if (asc->couplingMatrix()->format() == Mode)
    for (i = 0; i < m_maxIndex; i++)
      for (j = 0; j < m_maxIndex; j++)
        gsl_matrix_set(m_mode_modeMatrix,i,j,(*asc->couplingMatrix())[i][j]);
  else
    for (i = 0; i < m_maxBin; i++)
      for (j = 0; j < m_maxBin; j++)
        gsl_matrix_set(m_bin_binMatrix,i,j,(*asc->couplingMatrix())[i][j]);
}

void Spectrum::createModeCouplingMatrix(association *asc) {
  long l1, l2, l3;  // individual l values
  double sum_l3;    // sum of l3 contributions
  double cg2;       // Clebsch-Gordan coefficient squared
  double value;     // value stored in mode matrix

  for (l1 = 0; l1 < m_maxIndex; l1++) {
    for (l2 = 0; l2 < m_maxIndex; l2++) {
      sum_l3 = 0.0;
      for (l3 = 0; l3 < m_maxIndex; l3++) {
        cg2     = coupling_3j(l1,l2,l3);
        cg2    *= cg2;
        sum_l3 += (*(asc->transformedWeights()))[l3] * (2 * l3 + 1) * cg2;
      }
      value = (2 * l2 + 1) * sum_l3 / (4 * M_PI);
      if (asc->transformedFilter())
        value *= (*(asc->transformedFilter()))[l2];
      if (asc->transformedBeam())
        value *= (*(asc->transformedBeam()))[l2] * (*asc->transformedBeam())[l2];

      gsl_matrix_set(m_mode_modeMatrix,l1,l2,value);
      if (!asc->couplingMatrix()->initialized())
        (asc->couplingMatrix())->initialize();
      (*(asc->couplingMatrix()))[l1][l2] = value;
    }
  }
}

void Spectrum::calculateBinCouplingMatrix(association *asc) {
  long i, j, bin1, bin2;
  double value;

  for (bin1 = 0; bin1 < m_maxBin; bin1++) {
    for (bin2 = 0; bin2 < m_maxBin; bin2++) {
      value = 0.0;
      for (i = 0; i < m_maxIndex; i++)
        for (j = 0; j < m_maxIndex; j++)
          value += gsl_matrix_get(m_bin_indexMatrix,bin1,i) * gsl_matrix_get(m_mode_modeMatrix,i,j) * gsl_matrix_get(m_index_binMatrix,j,bin2);
      gsl_matrix_set(m_bin_binMatrix,bin1,bin2,value);
      if (asc->couplingMatrix()->format() == Bin)
        (*asc->couplingMatrix())[bin1][bin2] = value;
    }
  }
}

void Spectrum::calculateBinIndexMatrix(long binOffset, long indexOffset, long binRecords, long indexRecords) {
  long bin = 0, index = 0;
  double value;

  if (binOffset == 0 && indexOffset == 0)  // at start of matrix
    m_lowBinL = 0;

  for (bin = binOffset; bin < binOffset + binRecords; bin++) {
    for (index = indexOffset; index < indexOffset + indexRecords; index++) {
      if (index < m_lowBinL || index >= m_lowBinL+m_indices)
        value = 0.0;
      else {
        if (m_weight)
          if (index == 0)
            value = 1.0 / (2.0 * M_PI * (double)m_indices);
          else
            value = index * (index+1) / (2 * M_PI * (double)m_indices);
        else
          value = 1.0 / (double)m_indices;
      }
      gsl_matrix_set(m_bin_indexMatrix,bin,index,value);
    }
    if (index >= m_lowBinL + m_indices)
      m_lowBinL += m_indices;
  }
}

void Spectrum::calculateIndexBinMatrix(long binOffset, long indexOffset, long binRecords, long indexRecords) {
  long bin = 0, index = 0;
  double value;

  if (binOffset == 0 && indexOffset == 0)
    m_lowBinL = 0;
  for (bin = binOffset; bin < binOffset + binRecords; bin++) {
    for (index = indexOffset; index < indexOffset + indexRecords; index++) {
      if (index < m_lowBinL || index >= m_lowBinL+m_indices)
        value = 0.0;
      else {
        if (m_weight)
          if (index == 0)
            value = 2.0 * M_PI;
          else
            value = (2.0 * M_PI) / (index * (index + 1));
        else
          value = 1.0;
      }
      gsl_matrix_set(m_index_binMatrix,index,bin,value);
    }
    if (index >= m_lowBinL + m_indices)
      m_lowBinL += m_indices;
  }
}

void Spectrum::loadInverseMatrix(association *asc) {
  int i, j;

  if (asc->inverseMatrix()->format() == Bin)
    for (i = 0; i < m_maxBin; i++)
      for (j = 0;j < m_maxBin;j++)
        gsl_matrix_set(m_inverseBinMatrix,i,j,(*asc->inverseMatrix())[i][j]);
  else
    for (i = 0; i < m_maxIndex; i++)
      for (j = 0; j < m_maxIndex; j++)
        gsl_matrix_set(m_inverseModeMatrix,i,j,(*asc->inverseMatrix())[i][j]);
}

int Spectrum::invertMatrix(association *asc) {
  int gslErrorNumber = 0;
  gsl_permutation *p;
  int i, j;

  if (m_binning) {
    p = gsl_permutation_alloc(m_maxBin);
    gsl_linalg_LU_decomp(m_bin_binMatrix,p,&gslErrorNumber);

    if (gsl_linalg_LU_det(m_bin_binMatrix,gslErrorNumber) == 0)	{
      gsl_permutation_free(p);
      return -1;
    }

    gsl_linalg_LU_invert((const gsl_matrix*)m_bin_binMatrix,(const gsl_permutation*)p,m_inverseBinMatrix);
    for (i = 0;i < m_maxBin;i++)
      for (j = 0;j < m_maxBin;j++)
      {
	double valueIn = gsl_matrix_get(m_inverseBinMatrix,i,j);
	double valueOut = (*asc->inverseMatrix())[i][j];
        (*asc->inverseMatrix())[i][j] = gsl_matrix_get(m_inverseBinMatrix,i,j);
      }
  }
  else {
    p = gsl_permutation_alloc(m_maxIndex);
    gsl_linalg_LU_decomp(m_mode_modeMatrix,p,&gslErrorNumber);
    if (gsl_linalg_LU_det(m_mode_modeMatrix,gslErrorNumber) == 0) {
      gsl_permutation_free(p);
      return -1;
    }
    gsl_linalg_LU_invert((const gsl_matrix*)m_mode_modeMatrix,(const gsl_permutation*)p,m_inverseModeMatrix);
    for (i = 0;i < m_maxIndex;i++)
      for (j = 0;j < m_maxIndex;j++)
        (*asc->inverseMatrix())[i][j] = gsl_matrix_get(m_inverseModeMatrix,i,j);
  }

  gsl_permutation_free(p);
  return 0;
}

void Spectrum::calculateSpectrum(association* asc) {
  long element, i, j;
  double value, intermediate, max, min;

  for (element = 0; element < m_maxIndex; element++) {
    value = 0.0;
    if (m_binning) {
      for (i = 0; i < m_maxBin; i++) {
        for (j = 0; j < m_maxIndex; j++) {
          intermediate = gsl_matrix_get(m_inverseBinMatrix,element,i) * gsl_matrix_get(m_bin_indexMatrix,i,j);
          if (asc->transformedNoise())
            intermediate *= (double)((*asc->transformedData())[j]) - (double)((*asc->transformedNoise())[j]);
          else
            intermediate *= (double)((*asc->transformedData())[j]);
          value += intermediate;
        }
      }
    }
    else {
      for (i = 0; i < m_maxIndex; i++) {
        intermediate = gsl_matrix_get(m_inverseModeMatrix,element,i);
        if (asc->transformedNoise())
          intermediate *= (double)((*asc->transformedData())[i]) - (double)((*asc->transformedNoise())[i]);
        else
          intermediate *= (double)((*asc->transformedData())[i]);
        value += intermediate;
      }
    }

//    if (value < 0.0)  // need to validate making these positive
//      value = -value;
    
    (*asc->spectrumData())[element] = value;
  }

  max = min = (*asc->spectrumData())[0];

  for (element = 1; element < m_maxIndex; element++) {
    value = (*asc->spectrumData())[element];
    if (value > max)
      max = value;
    if (value < min)
      min = value;
  }

  asc->spectrumData()->maxValue(max);
  asc->spectrumData()->minValue(min);
}

void Spectrum::calculateEnsembleAverage(association* asc, int size) {
/*
In the HealPIX c++ code, there is a routine called create_alm (and create_alm_pol for polarized spectra). It uses an inputted power spectra (would this be the C_l defined in equation 13 in the Hivon paper?). I'm thinking of creating a method that takes the PCL created by the program based on equation 13 as well as the number of simulated PCLs that you want to use in the averaging. For each iteration of the loop, the method would pass the original PCL and the random number generator to create_alm, then put the randomized alm into HealPIX's extract_powspec to get out the associated PCL. This would be loaded into an array with max_l rows and n columns, where n is the number of simulated PCLs passed into the routine. Once this is loaded via the loop, a final PCL would be created by summing each element and then dividing by n to get the ensemble average. This would be passed back out of the method. Does this sound like a reasonable way to create the ensemble average?
*/
  /* create and initialize an array to store the various pseudo-power spectra created for averaging */
  int i = 0;
  std::vector<std::vector<double> > ensemblePseudoCl(size);
  int length = asc->spectrumData()->rows();
  for (i = 0; i < size; ++i)
      ensemblePseudoCl[i].resize(length);

  /* initialize gsl random number generator */
  int seed = 1234;
  planck_rng *generator = new planck_rng(seed);

  /* set up HealPIX objects */
  PowSpec *original = 0, *randomized = 0;
  Alm<xcomplex<double> > *randAlm = 0;

  original = new PowSpec(1,length);
  randomized = new PowSpec(1,length);
  randAlm = new Alm<xcomplex<double> >(length,length);
  for (i = 0; i < length; ++i)
    original->tt(i) = (*(asc->spectrumData()))[i];

  /* create randomized alm instances of the psuedo power spectrum */
  int row = 0;
  for (i = 0; i < size; ++i) {
    create_alm(*original,*randAlm,*generator);
    extract_powspec(*randAlm,*randomized);
    for (row = 0; row < length; ++row)
      ensemblePseudoCl[i][row] = randomized->tt(row);
  }

  /* average each l value */
  for (row = 0; row < length; ++row) {
    (*asc->ensembleData())[row] = 0;
    double sum = 0.0;
    for (i = 0; i < size; ++i)
      sum += ensemblePseudoCl[i][row];
    (*asc->ensembleData())[row] = sum / size;
  }
}


void Spectrum::clear() {
  if (!m_configured)
    return;

  if (m_mode_modeMatrix)
    gsl_matrix_free(m_mode_modeMatrix);
  if (m_inverseModeMatrix)
    gsl_matrix_free(m_inverseModeMatrix);

  if (m_binning) {
    if (m_bin_indexMatrix)
      gsl_matrix_free(m_bin_indexMatrix);
    if (m_index_binMatrix)
      gsl_matrix_free(m_index_binMatrix);
    if (m_bin_binMatrix)
      gsl_matrix_free(m_bin_binMatrix);
    if (m_inverseBinMatrix)
      gsl_matrix_free(m_inverseBinMatrix);
  }

  m_active = false;
  m_configured = false;
}

double Spectrum::gammln(double xx) {
  static double cof[6] = {
    76.18009172947146,
    -86.50532032941677,
    24.01409824083091,
    -1.231739572450155,
    0.1208650973866179e-2,
    -0.5395239384953e-5
  };

  double x, y, tmp, ser;
  int j;
  double result;

  y = x = xx;
  tmp  = x + 5.5;
  tmp -= (x + 0.5) * log(tmp);
  ser  = 1.000000000190015;

  for (j = 0; j <= 5; j++)
    ser += cof[j] / ++y;
  result = -tmp + log(2.506628274631005 * ser / x);

  return result;
}

double Spectrum::factln(int n) {
  static double a[101];

  if (n < 0)
    return -1.0;

  if (n <= 1)
    return 0.0;

  if (n <= 100) {
    if (a[n] == 0)
      a[n] = gammln(n + 1.0);
    return a[n];
  }

  return gammln(n + 1.0);
}

double Spectrum::coupling_3j(int l1, int l2, int l3) {
  int L;
  double diff_l1, diff_l2, diff_l3, half_l1, half_l2, half_l3;
  double lnDiff_l1, lnDiff_l2, lnDiff_l3, ln_L,
         lnHalf_L, lnHalf_l1, lnHalf_l2, lnHalf_l3;
  double rootTerm, fract;
  double coefficient;

  L = l1 + l2 + l3;

  if ((L % 2) || (l2 < abs(l1 - l3)) || (l2 > l1 + l3))
    return 0.0;

  diff_l1 = (double)(L - 2.0 * l1);
  diff_l2 = (double)(L - 2.0 * l2);
  diff_l3 = (double)(L - 2.0 * l3);
  half_l1 = (double)(L / 2.0 - l1);
  half_l2 = (double)(L / 2.0 - l2);
  half_l3 = (double)(L / 2.0 - l3);

  lnDiff_l1 = factln((int)diff_l1);
  lnDiff_l2 = factln((int)diff_l2);
  lnDiff_l3 = factln((int)diff_l3);
  ln_L      = factln((int)L + 1);
  lnHalf_L  = factln((int)L / 2);
  lnHalf_l1 = factln((int)half_l1);
  lnHalf_l2 = factln((int)half_l2);
  lnHalf_l3 = factln((int)half_l3);

  rootTerm    = (lnDiff_l1 + lnDiff_l2 + lnDiff_l3 - ln_L) / 2.0;
  fract       = lnHalf_L - lnHalf_l1 - lnHalf_l2 - lnHalf_l3;
  coefficient = exp(rootTerm + fract);

  return coefficient;
}
