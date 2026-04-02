#include "libpre.h"
    #include "fn_network.h"
#include "libpost.h"

#include "ogm/interpreter/Variable.hpp"
#include "ogm/common/error.hpp"
#include "ogm/common/util.hpp"
#include "ogm/interpreter/Executor.hpp"
#include "ogm/interpreter/execute.hpp"

#include <string>
#include <cctype>
#include <cstdlib>

using namespace ogm::interpreter;
using namespace ogm::interpreter::fn;

#define frame staticExecutor.m_frame

void ogm::interpreter::fn::network_create_server(VO out, V type, V port, V max_client)
{
    out = frame.m_network.create_server_socket(
        false,
        static_cast<NetworkProtocol>(static_cast<size_t>(type.castCoerce<uint64_t>())),
        port.castCoerce<port_t>(),
        static_cast<size_t>(max_client.castCoerce<uint64_t>()),
        staticExecutor.m_self->m_data.m_id
    );
}


void ogm::interpreter::fn::network_create_server_raw(VO out, V type, V port, V max_client)
{
    out = frame.m_network.create_server_socket(
        true,
        static_cast<NetworkProtocol>(static_cast<size_t>(type.castCoerce<uint64_t>())),
        port.castCoerce<port_t>(),
        static_cast<size_t>(max_client.castCoerce<uint64_t>()),
        staticExecutor.m_self->m_data.m_id
    );
}

void ogm::interpreter::fn::network_create_socket(VO out, V type)
{
    out = frame.m_network.create_socket(
        true,
        static_cast<NetworkProtocol>(static_cast<size_t>(type.castCoerce<uint64_t>())),
        staticExecutor.m_self->m_data.m_id
    );
}

void ogm::interpreter::fn::network_create_socket_ext(VO out, V type, V port)
{
    out = frame.m_network.create_socket(
        true,
        static_cast<NetworkProtocol>(static_cast<size_t>(type.castCoerce<uint64_t>())),
        staticExecutor.m_self->m_data.m_id,
        port.castCoerce<port_t>()
    );
}

void ogm::interpreter::fn::network_connect(VO out, V socket, V url, V port)
{
    out = frame.m_network.connect_socket(
        static_cast<size_t>(socket.castCoerce<uint64_t>()),
        false,
        url.castCoerce<string_t>().c_str(),
        port.castCoerce<port_t>()
    )
        ? 0
        : -1.0;
}

void ogm::interpreter::fn::network_connect_raw(VO out, V socket, V url, V port)
{
    out = frame.m_network.connect_socket(
        static_cast<size_t>(socket.castCoerce<uint64_t>()),
        true,
        url.castCoerce<string_t>().c_str(),
        port.castCoerce<port_t>()
    )
        ? 0
        : -1.0;
}

void ogm::interpreter::fn::network_send_raw(VO out, V socket, V buffer, V size)
{
    // FIXME:
    //   1. NetworkManager::send should deal with the buffer directly.
    //   2. for non-raw packets which are partially sent, the 12-byte header
    //      should not count toward the read for the buffer. See (*) below.
    size_t s = static_cast<size_t>(size.castCoerce<uint64_t>());
    Buffer& b = frame.m_buffers.get_buffer(static_cast<size_t>(buffer.castCoerce<uint64_t>()));
    char* c = new char[s];
    size_t read = b.peek_n(c, s);
    int32_t sent = frame.m_network.send(
        static_cast<size_t>(socket.castCoerce<uint64_t>()),
        read,
        c
    );
    if (sent > 0)
    {
        // (*)
        b.read(c, std::min<size_t>(sent, static_cast<size_t>(size.castCoerce<uint64_t>())));
    }

    delete[] c;
    out = sent;
}

void ogm::interpreter::fn::network_send_udp_raw(VO out, V socket, V url, V port, V buffer, V size)
{
    size_t s = static_cast<size_t>(size.castCoerce<uint64_t>());
    Buffer& b = frame.m_buffers.get_buffer(static_cast<size_t>(buffer.castCoerce<uint64_t>()));
    char* c = new char[s];
    size_t read = b.peek_n(c, s);
    size_t sent = frame.m_network.send(
        static_cast<size_t>(socket.castCoerce<uint64_t>()),
        read,
        c,
        url.castCoerce<string_t>().c_str(),
        port.castCoerce<port_t>()
    );
    b.read(c, sent);

    delete[] c;
}

void ogm::interpreter::fn::network_destroy(VO out, V socket)
{
    frame.m_network.destroy_socket(
        static_cast<size_t>(socket.castCoerce<uint64_t>())
    );
}

void ogm::interpreter::fn::network_set_config(VO out, V key, V value)
{
    frame.m_network.set_option(key.castCoerce<int32_t>(), value.castCoerce<real_t>());
}