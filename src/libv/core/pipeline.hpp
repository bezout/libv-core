/**

\file
\author Alexis Wilhelm (2013-2014)
\copyright 2013-2014 Institut Pascal

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

#ifndef LIBV_CORE_PIPELINE_HPP
#define LIBV_CORE_PIPELINE_HPP

#include <boost/thread.hpp>

#include "miscmath.hpp"

namespace v {
namespace core {
/// \addtogroup parallelism
/// \{

template<class derived_type, class range_type, class value_type> struct PipelineIterator;

/**

The attributes of a range in a Pipeline.

*/
struct PipelineRangeData
{
  PipelineRangeData()
  : begin_(0)
  , count_(0)
  {
  }

  size_t begin_;
  size_t count_;
};

/**

A view on a Pipeline, seen from a particular thread.

\implements boost::SinglePassRange

*/
template
< class _pipeline_type ///< An instance of Pipeline.
>
struct PipelineConstRange
{
/// \name Container Concept
/// \{

  typedef typename _pipeline_type::value_type value_type;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef value_type *pointer;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

  size_t size() const
  {
    return (range_id_ ? 0 : max_size()) + range(-1).data().count_ - data().count_;
  }

  size_t max_size() const
  {
    return pipeline_->values_.size();
  }

  bool empty() const
  {
    return !size();
  }

/// \}

  /**

  Initialize this range.

  */
  PipelineConstRange
  ( _pipeline_type *pipeline ///< The pipeline we are working with.
  , size_t range_id ///< The index of this range in the pipeline.
  )
  : pipeline_(pipeline)
  , range_id_(range_id)
  {
  }

  /**

  Test the validity of this range.
  A range becomes invalid when it is closed, or when it is empty and the previous range is invalid.

  \return
  True if this range is valid.

  */
  bool good() const
  {
    return pipeline_ && data().begin_ != size_t(-1);
  }

  /**

  The first value in this range.

  \return
  A reference to the first element.

  */
  reference front() const
  {
    return pipeline_->values_[data().begin_];
  }

  /**

  An other range in the same pipeline.

  \return
  The requested range.

  */
  PipelineConstRange range
  ( ptrdiff_t index ///< A range index relative to this range. -1 gives the previous range, +1 gives the next range.
  ) const
  {
    return pipeline_->range((range_id_ + pipeline_->ranges_.size() + index) % pipeline_->ranges_.size());
  }

protected:

  _pipeline_type *pipeline_;
  size_t range_id_;

private:

  template<class, class, class> friend struct PipelineIterator;

  const PipelineRangeData &data() const
  {
    return pipeline_->ranges_[range_id_];
  }
};

/**

\copybrief PipelineConstRange

*/
template
< class _pipeline_type ///< An instance of Pipeline.
>
struct PipelineRange
: PipelineConstRange<_pipeline_type>
{
private:

  typedef PipelineConstRange<_pipeline_type> base_class;
  using base_class::pipeline_;
  using base_class::range_id_;

  struct Iterator
  : PipelineIterator<Iterator, PipelineRange, typename PipelineRange::value_type>
  {
    typedef PipelineIterator<Iterator, PipelineRange, typename PipelineRange::value_type> base_class;
    using typename base_class::reference;
    using base_class::range_;

    void acquire(boost::mutex::scoped_lock *lock)
    {
      range_.acquire(lock);
    }

    void release(boost::mutex::scoped_lock *lock)
    {
      range_.release(lock);
    }

    reference dereference() const
    {
      return range_.front();
    }

    Iterator(const PipelineRange &range)
    : base_class(range)
    {
    }
  };

  void close(boost::mutex::scoped_lock *)
  {
    data().begin_ = -1;
    pipeline_->condition_.notify_all();
  }

  PipelineRangeData &data()
  {
    return pipeline_->ranges_[range_id_];
  }

public:

  using base_class::good;

/// \name Container Concept
/// \{

  using base_class::empty;
  using base_class::max_size;
  using typename base_class::value_type;
  typedef Iterator iterator;
  typedef Iterator const_iterator;

  iterator begin() const
  {
    return *this;
  }

  static iterator end()
  {
    return PipelineRange(0, 0);
  }

/// \}

  /**

  \copydoc base_class::PipelineConstRange

  */
  PipelineRange
  ( _pipeline_type *pipeline ///< The pipeline we are working with.
  , size_t range_id ///< The index of this range in the pipeline.
  )
  : base_class(pipeline, range_id)
  {
  }

  /**

  Close this pipeline.
  Processing does not stop immediately.
  The pipeline remains valid until all frames are processed.

  */
  void close()
  {
    boost::mutex::scoped_lock lock(pipeline_->mutex_);
    close(&lock);
  }

  /**

  Wait until an element is passed to this range from the previous range.

  */
  void acquire(boost::mutex::scoped_lock *lock)
  {
    while(good() && empty())
    {
      if(range(-1).good())
      {
        pipeline_->condition_.wait(*lock);
      }
      else
      {
        close(lock);
      }
    }
  }

  /**

  Pass the first element of this range to the next range.

  */
  void release(boost::mutex::scoped_lock *)
  {
    if(good())
    {
      ++data().count_;
      data().begin_ = (data().begin_ + 1) & (max_size() - 1);
      pipeline_->condition_.notify_all();
    }
  }

  /**

  \copydoc base_class::range

  */
  PipelineRange range
  ( ptrdiff_t index ///< A range index relative to this range. -1 gives the previous range, +1 gives the next range.
  ) const
  {
    return pipeline_->range((range_id_ + pipeline_->ranges_.size() + index) % pipeline_->ranges_.size());
  }
};

/**

An iterator over a range of a Pipeline.

\implements std::InputIterator

*/
template
< class _derived_class ///< CRTP.
, class _range_type ///< An instance of PipelineRange.
, class _value_type ///< The value type exposed to the user. May be different than _range_type::value_type if the pipeline is wrapped in an other kind of queue.
>
struct PipelineIterator
{
private:

  struct PostincrementIterator
  {
    typedef typename _derived_class::pointer pointer;
    typedef typename _derived_class::reference reference;
    _derived_class *iterator_;

    reference operator*() const
    {
      return iterator_->operator*();
    }

    pointer operator->() const
    {
      return iterator_->operator->();
    }

    PostincrementIterator(_derived_class *iterator)
    : iterator_(iterator)
    {
    };

    ~PostincrementIterator()
    {
      ++*iterator_;
    }
  };

  _derived_class *derived()
  {
    return static_cast<_derived_class *>(this);
  }

  const _derived_class *derived() const
  {
    return static_cast<const _derived_class *>(this);
  }

public:
/// \name Input Iterator Concept
/// \{

  typedef typename _range_type::difference_type difference_type;
  typedef _value_type value_type;
  typedef _value_type *pointer;
  typedef _value_type &reference;
  typedef std::input_iterator_tag iterator_category;

  reference operator*() const
  {
    return derived()->dereference();
  }

  pointer operator->() const
  {
    return &**this;
  }

  PipelineIterator &operator++()
  {
    boost::mutex::scoped_lock lock(range_.pipeline_->mutex_);
    derived()->release(&lock);
    derived()->acquire(&lock);
    return *this;
  }

  PostincrementIterator operator++(int)
  {
    return derived();
  }

  bool operator==(const PipelineIterator &other) const
  {
    return range_.good() == other.range_.good() && ( !range_.good() || &**this == &*other );
  }

  bool operator!=(const PipelineIterator &other) const
  {
    return !(*this == other);
  }

/// \}
protected:

  /**

  Initialize this iterator.
  Blocks until the pointed range is non-empty.

  */
  PipelineIterator
  ( const _range_type &range ///< A range of a pipeline.
  )
  : range_(range)
  {
    if(range_.good())
    {
      boost::mutex::scoped_lock lock(range_.pipeline_->mutex_);
      derived()->acquire(&lock);
    }
  }

  _range_type range_;
};

/**

A pipeline.
This class can be used to easily parallelize a sequence of instructions, by executing each instruction in its own thread and passing the output of one instruction as the input of the next one.

A typical usage will look like this: \code

  Pipeline<Data> pipeline(3);

  void thread_0()
  {
    for(Data &frame: pipeline.range(0))
      if(something_to_do)
        frame = ... // feed some data to the pipeline so other threads can process them
      else
        pipeline.close();
  }

  void thread_1()
  {
    for(Data &frame: pipeline.range(1))
      ... // process incoming frames, then pass them to the next worker
  }

  void thread_2()
  {
    for(Data &frame: pipeline.range(2))
      ... // process incoming frames, then pass them back to thread_0
  }

\endcode

Une file d'attente circulaire classique découpe le buffer en deux régions : les cases pleines et les cases vides.
Lorsqu'on ajoute un élément à la file, la première case vide devient pleine.
Lorsqu'on consomme un élément, la première case pleine devient vide.
Cette classe généralise cette idée en découpant le buffer en un nombre quelconque de régions, accessibles par la méthode range().

Ainsi, avec deux régions, on a le comportement d'une file d'attente circulaire classique en écrivant : \code

  void push(Pipeline<Data> *queue, Data x)
  {
    *queue->range(0).begin()++ = x; // remplit la première case vide
  }

  Data pop(Pipeline<Data> *queue)
  {
    return *queue->range(1).begin()++; // consomme la première case pleine
  }

\endcode

Avec plus que deux régions, on a le comportement d'un pipeline : l'ouvrier numéro 0 manipule un objet, puis le fait passer à l'ouvrier numéro 1 qui le manipule à son tour avant de le faire passer à l'ouvrier numéro 2, et ainsi de suite.
Chaque région correspond à un ouvrier qui manipule son premier élément.
Les autres éléments de sa région sont en attente.

Pour l'implémentation, on s'inspire de la technique décrite dans http://en.wikipedia.org/wiki/Circular_buffer#Mirroring.

*/
template
< class _value_type ///< Any type. Does not need to be copyable or assignable, because the data will never move.
>
struct Pipeline
{
  /**

  The type of the elements of this pipeline.

  */
  typedef _value_type value_type;

  /**

  A range of this pipeline.

  */
  typedef PipelineRange<Pipeline> range_type;

  /**

  \copydoc range_type

  */
  typedef PipelineConstRange<const Pipeline> const_range_type;

  /**

  Initialize this pipeline.

  */
  Pipeline
  ( size_t range_count ///< The number of ranges in this pipeline.
  , size_t max_value_count = 10 ///< The maximum number of elements that can be stored in this pipeline at any given time. Will be rounded to the next power of two.
  )
  : values_(ceil_pow2(max_value_count))
  , ranges_(range_count)
  {
  }

  /**

  \copydoc range_type

  \returns
  An object with begin() and end() methods.

  */
  range_type range
  ( size_t range_id ///< A range number.
  )
  {
    return range_type(this, range_id);
  }

  /**

  \copydoc range

  */
  const_range_type range
  ( size_t range_id ///< A range number.
  ) const
  {
    return const_range_type(this, range_id);
  }

private:

  template<class> friend struct PipelineRange;
  template<class> friend struct PipelineConstRange;
  template<class, class, class> friend struct PipelineIterator;
  boost::mutex mutex_;
  boost::condition_variable condition_;
  std::vector<_value_type> values_;
  std::vector<PipelineRangeData> ranges_;
};

/// \}
}
}

#endif
