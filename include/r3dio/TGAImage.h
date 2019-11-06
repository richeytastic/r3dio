/************************************************************************
 * Copyright (C) 2019 Richard Palmer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#ifndef R3DIO_IMAGE_IO_H
#define R3DIO_IMAGE_IO_H

#include "r3dio_Export.h"
#include <opencv2/opencv.hpp>
#include <string>


namespace r3dio {

r3dio_EXPORT bool saveTGA( const cv::Mat&, const std::string& fname);

// Load TGA from file - returns an empty matrix on failure.
r3dio_EXPORT cv::Mat loadTGA( const std::string& fname);

}   // end namespace

#endif
