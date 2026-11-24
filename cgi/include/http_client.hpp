//
// http_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// g++ http_client.cc /usr/local/lib/libboost_thread.a /usr/local/lib/libboost_system.a  -I/usr/local/include/ -lpthread -ohttp_client

#include <iostream>
#include <istream>
#include <fstream>
#include <ostream>
#include <string>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <sstream>

#if DEBUG
extern bool         gIsDebugEnabled;
#endif
template<typename T>
std::string toString(const T& value)
{
   std::ostringstream oss;
   oss << value;
   return oss.str();
}

#define CONTENT_TYPE_JSON 1
#define CONTENT_TYPE_XML  0

using boost::asio::ip::tcp;

class HttpClient {
   public:
   int SendAndReceive(std::string host, int port, std::string uri, std::string method, std::string header, std::string content, std::string &response, int& contentType)
   {
      try
      {
#if DEBUG
         if(gIsDebugEnabled)
         {
         std::cout<<"Request : "<<method<<" http://"<<host<<":"<<port<<"/"<<uri<<"\n";
         std::cout<<"Header  : "<<header;
         std::cout<<"Content : "<<content;
         }
#endif
         if (host.empty() || method.empty())
         {
            //std::cout << "Host & Method not set\n";
            return 1;
         }

         boost::asio::ip::tcp::iostream s;

         // The entire sequence of I/O operations must complete within 60 seconds.
         // If an expiry occurs, the socket is automatically closed and the stream
         // becomes bad.
         //s.expires_after(boost::posix_time::seconds(60));

         // Establish a connection to the server.
         s.connect(host, toString(port));
         if (!s)
         {
            //std::cout << "Unable to connect: " << s.error().message() << "\n";
            return 2;
         }

         // Send the request. We specify the "Connection: close" header so that the
         // server will close the socket after transmitting the response. This will
         // allow us to treat all data up until the EOF as the content.
         s << method << " " << uri << " HTTP/1.1\r\n";
         s << "Host: " << host << "\r\n";
         s << "Accept: */*\r\n";
         s << header;
         //if(content.length() > 0)
         {
            s << "content-length:"<<content.length() << "\r\n";
            //s << "content-type:text/html\r\n";
         }
         s << "Connection: close\r\n";
         s << "\r\n";
         s << content;

         // By default, the stream is tied with itself. This means that the stream
         // automatically flush the buffered output before attempting a read. It is
         // not necessary not explicitly flush the stream at this point.

         // Check that response is OK.
         std::string http_version;
         s >> http_version;
         unsigned int status_code;
         s >> status_code;
         std::string status_message;
         std::getline(s, status_message);
         if (!s || http_version.substr(0, 5) != "HTTP/")
         {
#if DEBUG
         if(gIsDebugEnabled)
         {
            std::cout << "Invalid response\n";
         }
#endif
            return 3;
         }
         if (status_code != 200)
         {
#if DEBUG
         if(gIsDebugEnabled)
         {
            std::cout << "Response returned with status code " << status_code << "\n";
         }
#endif
            return 4;
         }

         // Process the response headers, which are terminated by a blank line.
         std::string header;
         contentType = CONTENT_TYPE_XML;
         boost::regex lExpr( "(Content-Type)(\\s)*(:)(\\s)*(application/json)([\\s\\S])*", boost::regex::extended|boost::regex::icase) ;
         while (std::getline(s, header) && header != "\r")
         {
#if DEBUG
         if(gIsDebugEnabled)
         {
            std::cout <<"Header: "<< header << "\n";
         }
#endif
         if(contentType != CONTENT_TYPE_JSON)
         {
            if(boost::regex_match(header, lExpr))
            {
               contentType = CONTENT_TYPE_JSON;
            }
         } 

         }
         //std::cout << "\n";

         // Write the remaining data to output.
         //std::cout << s.rdbuf();
         //s >> rspData;
         std::string rspData((std::istreambuf_iterator<char>(s.rdbuf())), 
               std::istreambuf_iterator<char>());
         response = rspData;
         //using boost::asio::buffers_begin;
         /*
            boost::asio::streambuf::const_buffers_type bufs = s.rdbuf();
            std::string rspData(
            boost::asio::buffers_begin(bufs),
            boost::asio::buffers_begin(bufs) + s.rdbuf().size());
            */
         //std::cout <<"#################################\n";
#if DEBUG
         if(gIsDebugEnabled)
         {
         std::cout << "Response Content-Type: "<<(contentType==CONTENT_TYPE_JSON?"JSON":"XML")<<"\n";
         std::cout << "Response Data: "<<rspData<<"\n";
         }
#endif
         //std::cout <<"#################################\n";
      }
      catch (std::exception& e)
      {
#if DEBUG
         if(gIsDebugEnabled)
         {
            std::cout << "SendAndReceive Exception: " << e.what() << "\n";
         }
#endif
         return 5;
      }

      return 0;
   }
};
