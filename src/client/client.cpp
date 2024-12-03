#include <zmq.hpp>
#include <iostream>
#include "../common/messages.hpp"

int main() {
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:5555");

    Message msg;
    msg.operation = "create_list";
    msg.data = {{"name", "Groceries"}};

    std::string serialized_msg = msg.to_string();
    socket.send(zmq::buffer(serialized_msg), zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply, zmq::recv_flags::none);

    std::cout << "Resposta do servidor: " << reply.to_string() << std::endl;

    return 0;
}

