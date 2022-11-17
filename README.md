# r3dio

Provides import/export functionality for r3d::Mesh objects from/to various formats as well as
functionality to export r3d::Mesh objects to PDF (via suitable pdflatex processor). At time of
writing, only Adobe Reader supports the rendering of 3D models inside PDFs.

Download [libbuild](https://github.com/richeytastic/libbuild) for easy build and install of this library.

## Prerequisites
- [r3d](../../../r3d)

- [AssImp](https://github.com/assimp) 5.2.5+

- pdflatex
    Optionally required for PDF generation from LaTeX files - must be on the PATH.
    Part of the [MiKTeK](https://miktex.org/) distribution on Windows, but usually
    installed in /usr/bin if installed systemwide on Linux as part of
    [TeX Live](https://www.tug.org/texlive/)

- [IDTFConverter](https://www2.iaas.msu.ru/tmp/u3d/u3d-1.4.5_current.zip)
    (with thanks to Michail Vidiassov)

    Optionally required for conversion of IDTF format models to U3D models
    (usually prior to embedding in PDFs via creation of a suitable LaTeX
    file before processing by pdflatex and the media9 package).
