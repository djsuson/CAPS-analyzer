/***************************************************************************
 *   mapper.h                                                              *
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
#ifndef _MAPPER_H_
#define _MAPPER_H_

#include <string>
#include "datamap.h"

class association;

class Mapper {

  public:
    Mapper();
    Mapper(Mapper* from);
    virtual ~Mapper();

    /*
     * Virtual functions that must be overridden by inherited classes
     */
    virtual int makeMap(dataMap *map,FILETYPE dataTypes,association* dataClasses) {return 0;}

    /**
     * clear map
     */
    void clear();

    /**
     * flag to see if Mapper is already configured
     */
    bool configured()                   { return m_configured; }
    void configured(bool flag)          { m_configured = flag; }

    /**
     * flag to see if map has been made
     */
    bool active()                       { return m_active; }
    void active(bool flag)           { m_active = flag; }

    /**
     * type of information being mapped
     **/
    FILETYPE display()                  { return m_display; }
    void display(FILETYPE type)         { m_display = type; }

    /**
     * type of information being mapped
     **/
    MAPTYPE layout()                    { return m_mapType; }
    void layout(MAPTYPE type)           { m_mapType = type; }

    COLORSCHEME colorScheme() const    {return m_colorScheme;}
    void colorScheme(COLORSCHEME type) {m_colorScheme = type;}

    /**
     * longitude incrementing convention
     * astronomical increases right to left
     * terrestrial increases left to right
     */
    ORIENTATION orientation()           { return m_orientation; }
    void orientation(ORIENTATION type)  { m_orientation = type; }

    /**
     * get current aspect ratio
     */
    double aspectRatio()                { return m_aspect; }
    void aspectRatio(double value)      { m_aspect = value; }

    /**
     * get current size of edge
     */
    double edge()                       { return m_edge; }
    void edge(double value)             { m_edge = value; }

    /**
     * get size of map
     */
    int width()                         { return m_mapX; }
    void width(int value)               { m_mapX = value; }
    int height()                        { return m_mapY; }
    void height(int value)              { m_mapY = value; }

  protected:
    bool                                m_configured;
    bool                                m_active;
    FILETYPE                            m_display;
    MAPTYPE                             m_mapType;
    COLORSCHEME                         m_colorScheme;
    double                              m_aspect;
    double                              m_edge;
    ORIENTATION                         m_orientation;
    int                                 m_mapX;
    int                                 m_mapY;
};

#endif
