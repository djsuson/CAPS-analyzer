/***************************************************************************
 *   raster.h                                                              *
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
#ifndef _RASTER_H_
#define _RASTER_H_

#include "stroke_font.h"
#include <ft2build.h> 
#include FT_FREETYPE_H 

#define VECTOR_FONT_ROMAN           (&StrokeRoman)
#define VECTOR_FONT_MONO_ROMAN      (&StrokeMonoRoman)

/* A simple vector graphics utility class for raster images.
It is loosely modeled after OpenGL... */

typedef enum {
   RASTER_NONE = 0,
   RASTER_POINT,
   RASTER_LINES,
   RASTER_LINE_STRIP,
   RASTER_LINE_LOOP,
   RASTER_ROTATE,
   RASTER_TRANSLATE,
   RASTER_SCALE,
   RASTER_NEAREST,
   RASTER_LINEAR
}
raster_enum_t;

typedef struct { unsigned char r,g,b,a; } raster_color_t;

typedef struct raster_point_t raster_point_t;
struct raster_point_t {
   raster_point_t *prev,*next;
   double x, y;
   raster_color_t clr;
};

typedef struct raster_trans_t raster_trans_t;
struct raster_trans_t {
   raster_trans_t *next;
   raster_enum_t op;
   double deg, x, y;
};

class Raster {
  public:
    Raster(int width, int height, int bytes_per_pixel, unsigned char *raster);
    ~Raster();

    void begin(raster_enum_t op);
    void vertex(double x, double y);
    void end(void);

    void color(double r, double g, double b, double a = 0);
    void pointSize(int val);
    void pixelMode(raster_enum_t mode);

    void identity(void);
    void rotate(double deg);
    void translate(double x, double y);
    void scale(double x, double y);
    void undoLast(void);

    /* Vector font adapted from GLUT... */
    void vectorFontString(VectorFont font, const char *string);      
    double vectorFontWidth(VectorFont font, char c);
    double vectorFontLength(VectorFont font, const char *string);      

    /* True type bitmap fonts using FreeType2... */
    void bitmapFontFace(const char *font, int face_pixels);
    void bitmapFontString(const char *string);
    double bitmapFontWidth(char c);
    double bitmapFontLength(const char *string);

    void toBitmapFile(const char* file_name);

  private:

    void do_op(raster_trans_t *trans, raster_point_t *vtx_lst);
    void add_op(raster_enum_t op, double deg, double x = 0, double y = 0);
    void rasterize(void);
    void write_point(void);
    void blend_points(void);

    unsigned char                 *m_raster;
    int                           m_raster_width;
    int                           m_raster_height;
    int                           m_raster_Bpp;
    int                           m_point_size;
    raster_enum_t                 m_mode;
    raster_enum_t                 m_pixel_mode;
    raster_color_t                m_color;

    raster_trans_t                *m_trans_lst;
    raster_point_t                *m_verts_lst;

    double                        m_at_x;
    double                        m_at_y;

    FT_Library                    m_ft2_library;      
    FT_Face                       m_ft2_face;     
    int                           m_ft2_pixel_size;
      
};

#endif /* _RASTER_H_ */
