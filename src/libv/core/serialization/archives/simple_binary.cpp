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

#include <libv/core/auto_load.hpp>
#include <libv/core/serialization/archives/simple_binary.hpp>
#include <libv/core/serialization/serializable.hpp>
#include <algorithm>
#include <fstream>
#include <istream>

#if defined(LIBV_CORE_BOOST_IOSTREAMS_FOUND)
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#endif

namespace v {
namespace core {
namespace {

static const size_t extents[] = {1};

template<class T>
static void load_(std::istream &stream, T *values, const size_t *extents, size_t rank)
{
  stream.read(reinterpret_cast<char *>(values), accumulate(extents, extents + rank, sizeof(T), std::multiplies<size_t>()));
}

template<class T>
static void save_(std::ostream &stream, const T *values, const size_t *extents, size_t rank)
{
  stream.write(reinterpret_cast<const char *>(values), accumulate(extents, extents + rank, sizeof(T), std::multiplies<size_t>()));
}

static void check_format(std::string file, std::string format)
{
  if(file.size() < format.size()) throw file;
  transform(file.rbegin(), file.rbegin() + format.size(), file.rbegin(), tolower);
  if(!equal(format.rbegin(), format.rend(), file.rbegin())) throw file;
}

static struct: register_codec<const std::string, Serializable> {
  void operator()(const std::string &input, Serializable &output) const {
    check_format(input, ".bin");
    std::ifstream file(input.c_str());
    SimpleBinaryInputArchive archive(file);
    v::load(archive, output);
  }} load_bin;

static struct: register_codec<const Serializable, const std::string> {
  void operator()(const Serializable &input, const std::string &output) const {
    check_format(output, ".bin");
    std::ofstream file(output.c_str());
    SimpleBinaryOutputArchive archive(file);
    v::save(archive, input);
  }} save_bin;

#if defined(LIBV_CORE_BOOST_IOSTREAMS_FOUND)

static struct: register_codec<const std::string, Serializable> {
  void operator()(const std::string &input, Serializable &output) const {
    check_format(input, ".bin.gz");
    std::ifstream file(input.c_str());
    boost::iostreams::filtering_istream stream;
    stream.push(boost::iostreams::gzip_decompressor());
    stream.push(file);
    SimpleBinaryInputArchive archive(stream);
    v::load(archive, output);
  }} load_bin_gz;

static struct: register_codec<const Serializable, const std::string> {
  void operator()(const Serializable &input, const std::string &output) const {
    check_format(output, ".bin.gz");
    std::ofstream file(output.c_str());
    boost::iostreams::filtering_ostream stream;
    stream.push(boost::iostreams::gzip_compressor());
    stream.push(file);
    SimpleBinaryOutputArchive archive(stream);
    v::save(archive, input);
  }} save_bin_gz;

#endif

}

/**

Initialize this archive with a stream.

*/
SimpleBinaryInputArchive::SimpleBinaryInputArchive
( std::istream &stream ///< A reference to a stream.
)
: stream(stream)
{
}

bool SimpleBinaryInputArchive::begin(const std::string &)
{
  return true;
}

bool SimpleBinaryInputArchive::begin(std::string *)
{
  return true;
}

bool SimpleBinaryInputArchive::begin()
{
  return true;
}

void SimpleBinaryInputArchive::end()
{
}

void SimpleBinaryInputArchive::begin_list()
{
}

void SimpleBinaryInputArchive::end_list()
{
}

void SimpleBinaryInputArchive::get_extents(size_t *extents, size_t rank)
{
  load_(stream, extents, &rank, 1);
}

/**

Initialize this archive with a stream.

*/
SimpleBinaryOutputArchive::SimpleBinaryOutputArchive
( std::ostream &stream ///< A reference to a stream.
)
: stream(stream)
{
}

void SimpleBinaryOutputArchive::begin(const std::string &)
{
}

void SimpleBinaryOutputArchive::begin()
{
}

void SimpleBinaryOutputArchive::end()
{
}

void SimpleBinaryOutputArchive::begin_list(size_t size)
{
  save_(stream, &size, extents, 1);
}

void SimpleBinaryOutputArchive::end_list()
{
}

/// Define overloads for each fundamental type.
#define _DEFINE_FUNDAMENTAL_TYPE(T)\
\
  void SimpleBinaryInputArchive::serialize(T &value)\
  {\
    load_(stream, &value, extents, 1);\
  }\
\
  void SimpleBinaryInputArchive::serialize(T *values, const size_t *extents, size_t rank)\
  {\
    load_(stream, values, extents, rank);\
  }\
\
  void SimpleBinaryOutputArchive::serialize(T value)\
  {\
    save_(stream, &value, extents, 1);\
  }\
\
  void SimpleBinaryOutputArchive::serialize(const T *values, const size_t *extents, size_t rank)\
  {\
    save_(stream, extents, &rank, 1);\
    save_(stream, values, extents, rank);\
  }\

V_FOR_EACH_FUNDAMENTAL_TYPE(_DEFINE_FUNDAMENTAL_TYPE)

}}
