// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include "./gtest_net_common.h"

namespace libj {
namespace node {

static const UInt NUM_CONNS = 7;

TEST(GTestNetTcp, TestTcp) {
    Int port = 10000;
    net::Server::Ptr server = net::createServer();
    server->on(
        net::Server::EVENT_CONNECTION,
        JsFunction::Ptr(new GTestNetServerOnConnection(server, NUM_CONNS)));
    server->listen(port);

    for (Size i = 0; i < NUM_CONNS; i++) {
        net::Socket::Ptr socket = net::createConnection(port);
        GTestOnData::Ptr onData(new GTestOnData());
        JsFunction::Ptr onEnd(new GTestOnEnd(onData));
        JsFunction::Ptr onClose(new GTestOnClose());
        JsFunction::Ptr onConnect(new GTestNetSocketOnConnect(socket));
        socket->on(net::Socket::EVENT_DATA, onData);
        socket->on(net::Socket::EVENT_END, onEnd);
        socket->on(net::Socket::EVENT_CLOSE, onClose);
        socket->on(net::Socket::EVENT_CONNECT, onConnect);
    }

    node::run();

    ASSERT_EQ(NUM_CONNS, GTestOnClose::count());

    JsArray::CPtr messages = GTestOnEnd::messages();
    Size numMsgs = messages->length();
    ASSERT_EQ(NUM_CONNS, numMsgs);
    for (Size i = 0; i < numMsgs; i++) {
        console::printf(console::LEVEL_INFO, ".");
        String::CPtr msg = messages->getCPtr<Buffer>(i)->toString();
        ASSERT_TRUE(msg->equals(str("abc")));
    }
    console::printf(console::LEVEL_INFO, "\n");

    clearGTestCommon();
}

}  // namespace node
}  // namespace libj
