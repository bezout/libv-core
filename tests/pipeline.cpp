/**

\example

Usage of the Pipeline class.

\author Alexis Wilhelm (2014)

*/

#define sample_size 100

#include <boost/foreach.hpp>
#include <libv/core/pipeline.hpp>
#include <libv/core/test.hpp>

namespace {

using namespace v;
using namespace boost;
using namespace std;

void sleep()
{
  this_thread::sleep_for(chrono::milliseconds(rand() % 10));
}

void thread_0(Pipeline<int>::range_type pipeline, vector<int>::const_iterator begin, vector<int>::const_iterator end)
{
  int count = 0;
  BOOST_FOREACH(int &frame, pipeline)
  {
    if(begin != end)
    {
      frame = *begin;
      ++begin;
      ++count;
      sleep();
    }
    else
    {
      pipeline.close();
    }
  }
  V_TEST_EQUAL(count, sample_size);
}

void thread_1(Pipeline<int>::range_type pipeline)
{
  int count = 0;
  BOOST_FOREACH(int &frame, pipeline)
  {
    frame = frame * frame;
    ++count;
    sleep();
  }
  V_TEST_EQUAL(count, sample_size);
}

void thread_2(Pipeline<int>::range_type pipeline, vector<int>::const_iterator begin)
{
  int count = 0;
  BOOST_FOREACH(int &frame, pipeline)
  {
    V_TEST_EQUAL(frame, *begin * *begin);
    ++begin;
    ++count;
    sleep();
  }
  V_TEST_EQUAL(count, sample_size);
}

}

int main()
{
  Pipeline<int> pipeline(3);
  vector<int> input;
  generate_n(back_inserter(input), sample_size, rand);

  thread threads[] = {
    thread(thread_0, pipeline.range(0), input.begin(), input.end()),
    thread(thread_1, pipeline.range(1)),
    thread(thread_2, pipeline.range(2), input.begin()),
  };

  BOOST_FOREACH(thread &t, threads) t.join();
}
