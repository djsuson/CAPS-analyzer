/****************************************************************************
                              \main.cpp
Description:\brief (put short description here)

Begin    : Tue Jan 19, 2015
Copyright: (C) 2015 by Daniel Suson
email    : daniel.suson@purduecal.edu

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
must reference the fact that it was developed at Purdue University
Calumet by Daniel Suson
***************************************************************************/
#include <qapplication.h>
#include "mainwindow.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    mainWindow w;
    w.show();
    return a.exec();
}
