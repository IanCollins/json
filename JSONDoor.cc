/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:     CGI JSON
 *
 *  Created:    August 28, 2013, 4:25 PM
 *
 *******************************************************************************/
#if defined __sun && defined _USE_DOORS

#include "JSONDoor.h"

#include <utils/Log.h>
#include <sstream>

namespace masuma
{
  using utils::Log;

  namespace json
  {
    void
    DoorServer::processCall( const char* argp, size_t argSize, std::ostream& out )
    {
      Object result;

      std::istringstream in;

      in.str( std::string(argp, argSize) );

      try
      {
        const Object request = Object::scan( in );

        ptl::MutexLock lock( mutex );

        result["data"] = processRequest( request );
        result["ok"]   = true;
      }
      catch( const std::exception& e)
      {
        Log( Log::Warn ) << e.what() << std::endl;

        result["error"] = e.what();
        result["ok"]    = false;
      }

      out << result;
    }

    void
    DoorServer::doCall( const char* argp, size_t argSize )
    {
      // Remember no temporary object will be destroyed!
      //
      static std::ostream out(buf);

      buf->reset();

      processCall( argp, argSize, out );

      out << std::flush;
    }

    void
    DoorClient::flushInput()
    {
      while( io )
      {
        char c;
        io.read( &c, 1 );
      }
      io.clear();
    }

    Object
    DoorClient::send( const Object& command )
    {
      flushInput();

      io << command << std::flush;

      Object result = Object::scan( io );

      CheckCondition( result.has( "ok" ) );

      if( result["ok"] != true )
      {
        throw std::runtime_error( result["error"] );
      }

      return result["data"];
    }
  }
}

#endif
