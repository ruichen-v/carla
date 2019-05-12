// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/NonCopyable.h"
#include "carla/streaming/Message.h"
#include "carla/streaming/detail/Types.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <functional>
#include <memory>

namespace carla {
namespace streaming {
namespace detail {
namespace tcp {

  /// A client that connects to a single stream.
  ///
  /// @warning The client should not be destroyed before the @a io_service is
  /// stopped.
  class Client : private NonCopyable {
  public:

    using endpoint = boost::asio::ip::tcp::endpoint;
    using callback_function_type = std::function<void (std::shared_ptr<Message>)>;

    Client(
        boost::asio::io_service &io_service,
        endpoint ep,
        stream_id_type stream_id,
        callback_function_type callback);

    ~Client();

    stream_id_type GetStreamId() const {
      return _stream_id;
    }

    void Stop();

  private:

    void Connect();

    void Reconnect();

    void ReadData();

    const endpoint _endpoint;

    const stream_id_type _stream_id;

    callback_function_type _callback;

    boost::asio::ip::tcp::socket _socket;

    boost::asio::io_service::strand _strand;

    boost::asio::deadline_timer _connection_timer;

    bool _done = false;
  };

} // namespace tcp
} // namespace detail
} // namespace streaming
} // namespace carla
