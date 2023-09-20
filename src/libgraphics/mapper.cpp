/***************************************************************************
 *   mapper.cpp                                                            *
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
 * This class provides the basic interface to the map making functions.    *
 * Specific map layouts are daughter classes of this one. These classes    *
 * provide a device independent mechanism for creating an image map of     *
 * the various data classes.                                               *
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

#include <healpix_cxx/alm_healpix_tools.h>
#include "mapper.h"
#include "../libanalyzer//atypes.h"
#include "../libanalyzer/association.h"

Mapper::Mapper() {
  m_configured  = false;
  m_active      = false;
  m_display     = fileType::Null;
  m_mapType     = Blank;
  m_orientation = NoOrientation;
  m_colorScheme = Dark;
  m_aspect      = 1.0;
  m_edge        = 0.0;
  m_mapX        = 0;
  m_mapY        = 0;
}

Mapper::Mapper(Mapper* from) {
  m_configured  = from->configured();
  m_active      = from->active();
  m_display     = from->display();
  m_mapType     = from->layout();
  m_orientation = from->orientation();
  m_aspect      = from->aspectRatio();
  m_edge        = from->edge();
  m_mapX        = from->width();
  m_mapY        = from->height();
}

Mapper::~Mapper() {
  clear();
}

void Mapper::clear() {

}