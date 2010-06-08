#ifndef K3DSDK_QTUI_CONVERT_H
#define K3DSDK_QTUI_CONVERT_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/convert.h>
#include <k3dsdk/ustring.h>
#include <k3dsdk/types.h>
#include <QString>

namespace k3d
{

/// Specialization of k3d::convert that converts k3d::string_t to QString.
template<>
inline QString convert(const string_t& From)
{
	return QString(From.c_str());
}

/// Specialization of k3d::convert that converts k3d::ustring to QString.
template<>
inline QString convert(const ustring& From)
{
	return QString::fromUtf8(From.utf8_str().c_str());
}

} // namespace k3d

#endif // !K3DSDK_QTUI_CONVERT_H
