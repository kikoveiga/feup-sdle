#include <zmq.hpp>
#include <iostream>
#include "../common/messages.hpp"

int main() {
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::rep};
    socket.bind("tcp://*:5555");

    while (true) {
        zmq::message_t request;
        socket.recv(request, zmq::recv_flags::none);

        std::string request_str = request.to_string();
        Message msg = Message::from_string(request_str);

        std::string response;
        if (msg.operation == "create_list") {
            std::cout << "Cria lista: " << msg.data["name"] << std::endl;
            response = R"({"status": "success", "message": "Lista criada"})";
        } else {
            response = R"({"status": "error", "message": "Operação desconhecida"})";
        }

        socket.send(zmq::buffer(response), zmq::send_flags::none);
    }

    return 0;
}
