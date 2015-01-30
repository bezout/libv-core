/**

\file
\author Alexis Wilhelm (2013)
\copyright 2013 Institut Pascal

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef LIBV_CORE_CONVERSIONS_FRAMESERVER_HPP
#define LIBV_CORE_CONVERSIONS_FRAMESERVER_HPP

#include "wrapper.hpp"

#if defined(LIBV_CORE_LIBV_FRAMESERVER_FOUND) || !defined(LIBV_IGNORE_OPTIONAL_DEPENDENCIES)

#include <libv/core/image/all.hpp>
#include <libv/frameserver/FS_Frame/fsframe.hpp>

namespace v {
namespace core {
/// \addtogroup conversions
/// \{

/// Convert a FS_Frame to an Image.
template<class T> Image<T>
convert(FS_Frame &src, const Wrapper<Image<T> > *)
{
  return Image<T>(reinterpret_cast<typename Image<T>::pointer_to_scalar>(src.data), src.height, src.width, src.width);
}

/// \}
}}

#endif
#endif
