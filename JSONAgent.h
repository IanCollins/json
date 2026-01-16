/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013, all rights reserved
 *
 *  MODULE:      CGI JSON
 *
 *  DESCRIPTION: JSON TCP Agent client/server
 *
 ******************************************************************************/


#pragma once

#include "JSON.h"
#include <system/Agent.h>
#include <system/AutoFd.h>

#include <cstdlib>
#include <mutex>
#include <utility>

namespace masuma::json
{
  class Agent : public system::Agent
  {
    void inner() override;
    void process() override;

    virtual Object processRequest( const Object& ) = 0;

    void sendToServer( const Object& );

    Object getRequest();
    void sendResponse( const Object& );
    void sendError( const std::string& );

    bool handleMoreRequests {};

    const std::string portString;

    std::string getPortString()
    {
      char tmp[16];
      sprintf( tmp, "%u", port);
      return tmp;
    }

  public:

    static constexpr bool oneShot {false};
    static constexpr bool handleMultiple {true};

    void sendData( const Object& data );

    Agent( uint16_t port, bool many )
      : system::Agent {port},
        handleMoreRequests {many},
        portString {getPortString()}
    {}

    ~Agent() override = default;

    ssize_t read( void* p, size_t size )
    {
      return readSocket.read( p, size );
    }
  };

  class Client
  {
    const std::string peer;
    const uint16_t    port;
    const std::chrono::milliseconds connectTimeout {system::SocketAutoFd::noTimeout};

    system::SocketAutoFd fd;

  protected:

    void connectToPeer();

  public:

    struct ConnectionLost : system::Exception
    {
      ConnectionLost()
        : system::Exception{ "Client: Connection Lost" } {}
    };

    struct ProcessingError : system::Exception
    {
      ProcessingError( std::string detail ) : system::Exception {std::move(detail)} {}
    };

    Client( std::string peer, uint16_t port );
    Client( std::string peer, uint16_t port, std::chrono::milliseconds );
    virtual ~Client() = default;

    system::SocketAutoFd get() { return fd; }

    void disconnect();

    json::Object getResponse( std::chrono::milliseconds );
    json::Object getResponse();

    void sendAsync( const Object& command );

    json::Object send( const Object& command );
    json::Object send( const Object& command, std::chrono::milliseconds );

    void sendAsyncRequest( const std::string& action )
    {
      json::Object request;
      request["action"] = action;

      sendAsync( request );
    }

    template <typename T>
    void sendAsyncRequest( const std::string& action, const T& parameter )
    {
      json::Object request;
      request["action"] = action;
      request["parameters"] << parameter;

      sendAsync( request );
    }

    template <typename T0, typename T1>
    void sendAsyncRequest( const std::string& action,
                           const T0& p0, const T1& p1 )
    {
      json::Object request;
      request["action"] = action;
      request["parameters"] << p0 << p1;

      sendAsync( request );
    }

    template <typename T>
    Object sendSyncRequest( const std::string& action, const T& parameter )
    {
      sendAsyncRequest<T>( action, parameter );

      return getResponse();
    }

    template <typename T0, typename T1>
    Object sendSyncRequest( const std::string& action,
                            const T0& p0, const T1& p1 )
    {
      sendAsyncRequest<T0,T1>( action, p0, p1 );

      return getResponse();
    }

    template <typename T>
    Object sendSyncRequest( std::chrono::milliseconds timeout,
                            const std::string& action,
                            const T& parameter )
    {
      sendAsyncRequest<T>( action, parameter );

      return getResponse(timeout);
    }

    template <typename T0, typename T1>
    Object sendSyncRequest( std::chrono::milliseconds timeout,
                            const std::string& action,
                            const T0& p0, const T1& p1 )
    {
      sendAsyncRequest<T0,T1>( action, p0, p1 );

      return getResponse(timeout);
    }

    ssize_t write( const void* p, size_t size )
    {
      return fd.write( p, size );
    }
  };
}
