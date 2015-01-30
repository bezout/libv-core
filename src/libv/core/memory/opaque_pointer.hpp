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

#ifndef LIBV_CORE_MEMORY_OPAQUE_POINTER_HPP
#define LIBV_CORE_MEMORY_OPAQUE_POINTER_HPP

#include "../global.hpp"

namespace v {
namespace core {
/// \addtogroup opaque
/// \{

/**

Declare an opaque pointer on a \e Private nested class.
The nested class can be defined in another source file.
Private members can be accessed through the \c that() method.

You must initialize this pointer in the constructor's initialization list with \c that_(new Private).

\post The class contains the following new members: \code

  struct Private; // the private structure (declared, not defined)
  Private &that(void); // mutable getter
  const Private &that(void) const; // constant getter

\endcode

*/
#define V_DECLARE_PRIVATE\
\
  protected:\
  struct Private;\
  private:\
  friend struct Private;\
  v::OpaquePointer<Private> that_;\
  protected:\
\
  /** The private members. */\
  /** \return A reference to the private members. */\
  Private &\
  that(void)\
  {\
    return *that_.ptr;\
  }\
\
  /** \copydoc that */\
  const Private &\
  that(void) const\
  {\
    return *that_.ptr;\
  }\

/**

Define a \e Config nested class and declare an opaque pointer on a \e Private nested class.
This macro is provided for convenience, since many algorithms will need both the \e Config and \e Private classes (and most of the time \e Private will be a subclass of \e Config).

\see V_DECLARE_PRIVATE
\see V_DEFINE_CONFIG

*/
#define V_DECLARE_PRIVATE_CONFIG\
  V_DECLARE_PRIVATE\
  public:\
  V_DEFINE_CONFIG\

/**

A pointer to a private structure.
This works just like a \c scoped_ptr, but \e T can be an incomplete type.

*/
template<class T>
struct OpaquePointer
{
  /// The managed pointer.
  T *const ptr;

  /// Constructor
  explicit OpaquePointer(T *ptr)
    : ptr(ptr)
    , destructor_(&OpaquePointer::delete_)
  {
  }

  ~OpaquePointer(void)
  {
    (this->*destructor_)(ptr);
  }

private:

  void
  delete_(T *ptr)
  {
    delete ptr;
  }

  void (OpaquePointer::*const destructor_)(T *);
  OpaquePointer(const OpaquePointer &);
  void operator=(const OpaquePointer &);
};

/// \}
}}

#ifdef DOXYGEN
/// \cond false

// Astonishingly, Doxygen is less confused with this definition than without.
// We need this for Doxygen to draw the collaboration graph properly.
#define PrivatePointer

/// \endcond
#endif
#endif
