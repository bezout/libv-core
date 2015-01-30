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

#ifndef LIBV_CORE_SERIALIZATION_SERIALIZABLE_HPP
#define LIBV_CORE_SERIALIZATION_SERIALIZABLE_HPP

#include "archives/base.hpp"

namespace v {
namespace core {
/// \addtogroup serialization
/// \{

/**

A polymorphic serializable object.
You can serialize such an object without knowing its actual type.

*/
struct Serializable
{
  /**

  Load this object from an input archive.

  */
  virtual void serialize(InputArchive &) = 0;

  /**

  Save this object in an output archive.

  */
  virtual void serialize(OutputArchive &) const = 0;
  virtual ~Serializable() {}
};

/**

Load an object from an input archive.

*/
inline void load
( InputArchive &archive ///< An archive.
, Serializable &value ///< An object.
)
{
  value.serialize(archive);
}

/**

Save an object in an output archive.

*/
inline void save
( OutputArchive &archive ///< An archive.
, const Serializable &value ///< An object.
)
{
  value.serialize(archive);
}

/// \}
/// \addtogroup serialization_io
/// \{

LIBV_CORE_EXPORT void load(Serializable &);
LIBV_CORE_EXPORT void load(const std::string &, Serializable &);
LIBV_CORE_EXPORT void save(const std::string &, const Serializable &);
LIBV_CORE_EXPORT void pretty_print(std::ostream &, const Serializable &);

/// \}
}}

#endif
