#include <string>
#include <zmq.hpp>

int main()
{
   zmq::context_t ctx;
   zmq::socket_t sock(ctx, zmq::socket_type::push);
   sock.bind("inproc://test");

}
