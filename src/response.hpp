#pragma once

#include "constants.hpp"

template <typename T, typename C>
class Response {
  friend class Context;

  restinio::response_builder_t<T> response;
  const C callback;
  std::size_t bufferSize;

  Response(restinio::response_builder_t<T> && response, C && callback)
      : response{std::move(response)},
        callback{std::move(callback)} {}

  Response(const Response &) = delete;
  Response & operator=(const Response &) = delete;

public:
  Response(Response && response) : response{std::move(response.response)}, callback{std::move(response.callback)} {}

  inline Response && appendHeader(restinio::http_field_t field, std::string && value) && {
    response.append_header(field, std::move(value));
    return std::move(*this);
  }

  inline Response && appendHeader(std::string && field, std::string && value) && {
    response.append_header(std::move(field), std::move(value));
    return std::move(*this);
  }

  inline Response && setBody(restinio::writable_item_t body) && {
    response.set_body(std::move(body));
    return std::move(*this);
  }

  inline Response && connectionClose() && {
    response.connection_close();
    return std::move(*this);
  }

  inline Response && appendChunk(restinio::writable_item_t chunk) && {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.append_chunk(std::move(chunk));
    return std::move(*this);
  }

  inline Response && flush() && {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.flush();
    return std::move(*this);
  }

  inline Response & appendChunk(restinio::writable_item_t chunk) & {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.append_chunk(std::move(chunk));
    return *this;
  }

  inline Response & flush() & {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.flush();
    return *this;
  }

  inline restinio::request_handling_status_t done() {
    callback(response.header().status_line());
    return response.done();
  }

  friend inline Response & operator<<(Response<T, C> & response, const char value) {
    // TODO avoid this string creation
    response.response.append_chunk(std::string{value});
    response.bufferSize++;

    if (response.bufferSize > constant::server::MAX_BUFFER) {
      response.response.flush();
      response.bufferSize = 0;
    }

    return response;
  }

  friend inline Response & operator<<(Response<T, C> & response, const std::string & value) {
    response.response.append_chunk(value);
    response.bufferSize += value.size();

    if (response.bufferSize > constant::server::MAX_BUFFER) {
      response.response.flush();
      response.bufferSize = 0;
    }

    return response;
  }
};
