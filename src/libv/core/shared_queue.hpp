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

#ifndef LIBV_CORE_SHARED_QUEUE_HPP
#define LIBV_CORE_SHARED_QUEUE_HPP

#include "pipeline.hpp"

namespace v {
namespace core {
/// \addtogroup parallelism
/// \{

/**

Exception raised when a SharedQueue is closed.

*/
struct SharedQueueClosed
: std::exception
{
};

/**

A queue that can be shared among several threads.
This class can be used to easily parallelize a set of tasks, by sharing them among a given number of threads.

A typical usage will look like this: \code

  SharedQueue<Data> queue;

  void master()
  {
    while(something_to_do)
      queue.push(task); // feed some task to the queue so other threads can process it
    queue.close();
  }

  void worker() // you can have several workers in different threads
  {
    for(Data &task: queue)
      ... // process incoming tasks
  }

\endcode

En interne on utilise un Pipeline à trois régions :
- la région numéro 0 contient des cases vides dans lesquelles on peut ajouter des élément,
- la région numéro 1 contient des cases pleines en attente de traitement,
- la région numéro 2 contient des cases pleines verrouillées parce qu'actuellement accédées en lecture.

De plus, les cases actuellement accédées en lecture ont leur attribut \c free_ à \e faux, alors que les cases libres l'ont à \e vrai.
Cet attribut permet de savoir quelles cases peuvent sortir de la région 2 pour revenir à la région 0.
En effet, les cases ne sont pas nécessairement verrouillées et libérées dans le même ordre, et on doit attendre que la première case de la région 2 soit libre avant de faire passer une case de la région 2 vers la région 0.

\implements std::Container

*/
template
< class _value_type ///< Any type. Does not need to be copyable or assignable, because the data will never move.
>
struct SharedQueue
{
private:

  struct Value
  {
    bool free_;
    _value_type value_;
  };

  typedef Pipeline<Value> pipeline_type;
  typedef typename pipeline_type::range_type range_type;
  pipeline_type pipeline_;

  struct Iterator
  : PipelineIterator<Iterator, range_type, _value_type>
  {
    typedef PipelineIterator<Iterator, range_type, _value_type> base_class;
    using base_class::range_;
    using typename base_class::reference;
    Value *value_;

    void acquire(boost::mutex::scoped_lock *lock)
    {
      range_.acquire(lock);
      if(range_.good())
      {
        value_ = &range_.front();
        value_->free_ = false;
        range_.release(lock);
      }
    }

    void release(boost::mutex::scoped_lock *lock)
    {
      value_->free_ = true;
      while(!range_.range(+1).empty() && range_.range(+1).front().free_)
      {
        range_.range(+1).release(lock);
      }
    }

    reference dereference() const
    {
      return value_->value_;
    }

    Iterator(const range_type &range)
    : base_class(range)
    {
    }
  };

  struct PushIterator
  : PipelineIterator<PushIterator, range_type, _value_type>
  {
    typedef PipelineIterator<PushIterator, range_type, _value_type> base_class;
    using base_class::range_;
    using typename base_class::reference;

    void acquire(boost::mutex::scoped_lock *lock)
    {
      range_.acquire(lock);
      if(!range_.good())
      {
        throw SharedQueueClosed();
      }
    }

    void release(boost::mutex::scoped_lock *lock)
    {
      range_.release(lock);
    }

    reference dereference() const
    {
      return range_.front().value_;
    }

    PushIterator(const range_type &range)
    : base_class(range)
    {
    }
  };

public:
/// \name Container Concept
/// \{

  typedef _value_type value_type;
  typedef Iterator iterator;
  typedef Iterator const_iterator;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef value_type *pointer;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

  iterator begin()
  {
    return pipeline_.range(1);
  }

  static iterator end()
  {
    return range_type(0, 0);
  }

  size_t size() const
  {
    return pipeline_.range(1).size();
  }

  size_t max_size() const
  {
    return pipeline_.range(1).max_size();
  }

  bool empty() const
  {
    return pipeline_.range(1).empty();
  }

/// \}

  /**

  Initialize this queue.

  */
  SharedQueue
  ( size_t max_size = 10 ///< The maximum number of elements in this queue. The push() method will wait until there are less than \e max_size elements in the queue before inserting a new one.
  )
  : pipeline_(3, max_size)
  {
  }

  /**

  An iterator that allows you to add new elements at the back of this queue.

  \return
  An std::OutputIterator.

  \exception SharedQueueClosed
  if this queue was closed.

  */
  PushIterator push()
  {
    return pipeline_.range(0);
  }

  /**

  Add a new element at the back of this queue.
  Waits until there are less than \e max_size elements in the queue before inserting the new one.

  \exception SharedQueueClosed
  if this queue was closed.

  */
  void push
  ( const_reference value ///< The new element.
  )
  {
    *push()++ = value;
  }

  /**

  Close this queue.

  \post
  Subsequent calls to push() will raise SharedQueueClosed.
  begin() and end() will still work until the queue is empty.

  */
  void close()
  {
    pipeline_.range(0).close();
  }
};

/// \}
}
}

#endif
