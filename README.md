# r3dio
Provides import/export functionality for r3d::Mesh objects from/to various formats as well as
functionality to export r3d::Mesh objects to PDF (via suitable pdflatex processor). At time of
writing, only Adobe Reader supports the rendering of 3D models inside PDFs.

## Prerequisites
- [r3d](../../../r3d)

- [Boost](http://www.boost.org) 1.68+

- [AssImp](https://github.com/assimp)
    Requires version 4.1 *NB* When configuring using CMake on Windows:
    - *UNCHECK* `AddGTest_FOUND`
    - *UNCHECK* `ASSIMP_BUILD_TESTS`
    - *UNCHECK* `ASSIMP_BUILD_ASSIMP_VIEW` (references deprecated DirectX SDK)

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

Download [libbuild](../../../libbuild) for easy build and install of this library.
