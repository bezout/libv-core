/**

\example

Usage of the SharedQueue class.

\author Alexis Wilhelm (2014)

*/

#undef NDEBUG

#include <boost/foreach.hpp>
#include <libv/core/shared_queue.hpp>
#include <libv/core/test.hpp>

namespace {

using namespace v;
using namespace boost;
using namespace std;

void master(SharedQueue<int> *queue, const vector<int> *input)
{
  copy(input->begin(), input->end(), queue->push());
  queue->close();
}

struct Slave
{
  void operator()()
  {
    copy(queue_->begin(), queue_->end(), back_inserter(output_));
  }

  SharedQueue<int> *queue_;
  vector<int> output_;
  thread thread_;
};

}

int main()
{
  SharedQueue<int> queue;
  vector<int> input, output;
  generate_n(back_inserter(input), 100000, rand);
  thread master_thread(master, &queue, &input);
  Slave slaves[5];

  BOOST_FOREACH(Slave &slave, slaves)
  {
    slave.queue_ = &queue;
    slave.thread_ = thread(ref(slave));
  }
  BOOST_FOREACH(Slave &slave, slaves)
  {
    slave.thread_.join();
    copy(slave.output_.begin(), slave.output_.end(), back_inserter(output));
  }

  master_thread.join();
  sort(input.begin(), input.end());
  sort(output.begin(), output.end());
  assert(input == output);
}
