/**

\example

Usage of the SharedQueue class.

\author Alexis Wilhelm (2014)

*/

#define sample_size 100

#include <boost/foreach.hpp>
#include <libv/core/shared_queue.hpp>
#include <libv/core/test.hpp>

namespace {

using namespace v;
using namespace boost;
using namespace std;

void sleep()
{
  this_thread::sleep_for(chrono::milliseconds(rand() % 10));
}

void master(SharedQueue<int> *queue, const vector<int> *input)
{
  BOOST_FOREACH(int frame, *input)
  {
    queue->push(frame);
    sleep();
  }
  queue->close();
}

void slave(SharedQueue<int> *queue, vector<int>::const_iterator begin)
{
  int count = 0;
  BOOST_FOREACH(int frame, *queue)
  {
    V_TEST_EQUAL(frame, *begin);
    ++begin;
    ++count;
    sleep();
  }
  V_TEST_EQUAL(count, sample_size);
}

}

int main()
{
  SharedQueue<int> queue;
  vector<int> input;
  generate_n(back_inserter(input), sample_size, rand);

  thread threads[] = {
    thread(master, &queue, &input),
    thread(slave, &queue, input.begin()),
  };

  BOOST_FOREACH(thread &t, threads) t.join();
}
