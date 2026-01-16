/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:     CGI JSON
 *
 *  Created:    August 28, 2013, 4:25 PM
 *
 ******************************************************************************/

#include "JSONAgent.h"

#include <utils/Log.h>
#include <utils/AutoStreambuf.h>
#include <utils/Time.h>
#include <sstream>
#include <utility>

LOG_ENTRAILS_DEF

namespace masuma
{
  using system::ErrorLog;
  using system::WarnLog;
  using system::InfoLog;
  using namespace std::chrono_literals;

  namespace json
  {
    void
    Agent::inner()
    {
      try
      {
        do
        {
          process();
        }
        while( handleMoreRequests );
      }
      catch( const std::exception& e )
      {
        InfoLog() << "Agent done: " << e.what() << std::endl;
      }
    }

    Object
    Agent::getRequest()
    {
      static const size_t bufferSize = 16*1024;
      char buffer[bufferSize];

      system::AutoIStreambuf isb( readSocket, buffer, bufferSize );
      if( timeout )
      {
        isb.setTimeout( timeout );
      }
      std::istream in(&isb);

      try
      {
        return Object::scan( in );
      }
      catch( const std::exception& e )
      {
        std::cerr << "Agent::getRequest() exception " << e.what() << '\n';
        std::cerr << isb << '\n';
      }
      return {};
    }

    void
    Agent::sendResponse( const Object& result )
    {
      std::ostringstream out;
      
      out << result;
      
      readSocket.write( out.str().c_str(), out.str().size() );
    }

    void
    Agent::sendData( const Object& data )
    {
      Object result {"ok", true};

      if( !data.empty() )
      {
        result["data"] = data;
      }

      sendResponse( result );
    }

    void
    Agent::sendError( const std::string& what )
    {
      WarnLog() << name() << ": " << what << std::endl;

      sendResponse( {"ok", false, "error", what} );
    }

    void
    Agent::process()
    {
      static unsigned requests = 0;
      static unsigned errors = 0;
      
      static Object meta {"logLevel", system::Log::getLevel()};
        
      const auto startTime {system::now<long long>()};
      
      const Object request = getRequest();

      meta[portString]["state"] = "processing";
      meta[portString]["requests"] = ++requests;
      
      try
      {
        if( request.has("getMeta") )
        {
          sendData( meta );
        }
        else if( request.has("setLogLevel") )
        {
          int level {request["setLogLevel"]};
          system::Log::setLevelTo( system::Log::Level(level));
          
          meta["logLevel"] = system::Log::getLevel();
          sendData( meta );
        }
        else
        {
          sendData( processRequest( request ) );
        }
        
        const auto inHere {system::since(startTime)};
      
        if( inHere > 20'000'000 )
        {
          WarnLog() << "process took " << inHere/1'000'000.0 << "mS\n";
          WarnLog() << request << '\n';
        }
        
        meta[portString]["lastDuration"] = inHere;
        meta[portString]["state"] = "idle";
      }
      catch( const std::exception& e )
      {
        meta["errors"] = ++errors;

        sendError( e.what() );
      }
    }

    Client::Client( std::string peer, uint16_t port )
      : peer {std::move(peer)}, port {port}
    {
    }

    Client::Client( std::string peer, uint16_t port, 
                    std::chrono::milliseconds timeout )
      : peer {std::move(peer)}, port {port}, connectTimeout {timeout}
    {
    }

    void
    Client::connectToPeer()
    {
      if( !fd.isConnected() )
      {
        fd = ::socket( PF_INET, SOCK_STREAM, 0 );
        fd.connect( peer, port, connectTimeout );
      }
    }

    void
    Client::disconnect()
    {
      if( fd.isConnected() )
      {
        fd = -1;
      }
    }

    void
    Client::sendAsync( const Object& command )
    {
      connectToPeer();

      static const size_t bufferSize = 16*1024;
      static char buffer[bufferSize];

      system::AutoOStreambuf osb( fd, buffer, bufferSize );

      std::ostream out(&osb);

      //Log(Log::Debug) << "Command: " << command << std::endl;

      try
      {
        out << command << std::flush;
      }
      catch( const system::Exception& e )
      {
        fd = -1;
        ErrorLog() << "sendAsync(), what: " << e.what() << std::endl;
        throw ConnectionLost();
      }
    }

    json::Object
    Client::getResponse( std::chrono::milliseconds timeout )
    {
      static const size_t bufferSize = 16*1024;
      static char buffer[bufferSize];

      system::AutoIStreambuf isb( fd, buffer, bufferSize );

      isb.setTimeout( timeout.count() );

      std::istream in(&isb);

      Object result;

      try
      {
        result = Object::scan( in );
      }
      catch( const system::Exception& e )
      {
        fd = -1;
        ErrorLog() << "getResponse(), what: " << e.what() << std::endl;
        throw ConnectionLost();
      }

      //Log(Log::Debug) << "Response: " << result << std::endl;

      CheckCondition( result.has( "ok" ) );

      if( result["ok"] != true )
      {
        throw ProcessingError( result["error"] );
      }

      return result.has("data") ? result["data"] : json::Object{};
    }

    json::Object
    Client::getResponse()
    {
      return getResponse( 0ms );      
    }
    
    Object
    Client::send( const Object& command )
    {
      sendAsync( command );

      return getResponse();
    }

    Object
    Client::send( const Object& command, std::chrono::milliseconds timeout )
    {
      sendAsync( command );

      return getResponse( timeout );
    }
  }
}
