#ifndef SOAP_CLIENT_HPP
#define SOAP_CLIENT_HPP

#include <memory>
#include <iostream>
#include <locale>
#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "http_client.hpp"

#include "plustache/template.hpp"
#include "plustache/plustache_types.hpp"
#include "plustache/context.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#if DEBUG
extern bool         gIsDebugEnabled;
#endif
using namespace std;
using namespace boost::property_tree;

//typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

typedef boost::gregorian::date Date;

using namespace std;

namespace SimpleSoap{

  struct Result {
    //shared_ptr<SimpleWeb::ClientBase<SimpleWeb::HTTP>::Response> httpResponse;
    shared_ptr<string> httpResponse;
    std::shared_ptr<string> httpStatus;
    std::shared_ptr<boost::property_tree::ptree> xmlTree;
  } SoapResult;

  class Client {
    string host;
    string querypath;
    string xml;
    int    port;
    string method;
    string header;
  public:
    Client(string _host, string _querypath) : host(_host), querypath(_querypath), port(80), method(std::string("POST")), header(std::string("")){}
    Client(string _host, int _port, string _querypath) : host(_host), querypath(_querypath), port(_port), method(std::string("POST")), header(std::string("")){}
    Client(string _host, int _port, string _querypath, string _method, string _header) : host(_host), querypath(_querypath), port(_port), method(_method), header(_header){}
    ~Client(){}
    template<class T>
    int compile(const string tplFile, const T& o, int isFile){
      std::string tpl="";
      //read entire template file 
      if(isFile)
      {
      std::ifstream file(tplFile.c_str()); //, std::ifstream::in);
      std::string tplData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      tpl=tplData;
      }
      else
      {
         tpl=tplFile;
      }
#if DEBUG 
      if(gIsDebugEnabled)
      {
         std::cout<<"Template File : "<<tplFile<<"\n"<<tpl<<"###### End of TemplateFile #######\n";
      }
#endif
      if (tpl.size() == 0){
         return 1;
      }

      //compile tempplate with context to create a soap message
      Plustache::template_t t;
      xml = t.render(tpl, o);

      return 0;
    }

    string getXml(){return xml;}
    void logRequest(){
#if DEBUG 
      if(gIsDebugEnabled)
      {
      std::cout<<"XML Request : "<<xml<<"#######################"<<std::endl;
      }
#endif
}
    std::shared_ptr<Result> send(){
#if DEBUG 
      if(gIsDebugEnabled)
      {
      std::cout<<"XML Request : "<<xml<<"#######################"<<std::endl;
      }
#endif
      std::shared_ptr<Result> result(new Result());
      return result;
}

    std::shared_ptr<Result> sendAndReceive(){

      //send the compiled soap message to dest
      HttpClient client; 

#if 0
      //std::string tpl="rsp.txt";
      //read entire template file 
      std::ifstream file("rst.txt");//tplFile.c_str()); //, std::ifstream::in);
      std::string response((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      auto retVal = 10;

#endif
#if 1
      //int SendAndReceive(std::string host, int port, std::string uri, std::string method, std::string content, std::string &response)
      std::string response="";
      int  contentType=0;
      auto retVal = client.SendAndReceive(host, port, querypath, method, header, xml, response, contentType);
#endif
      //get the result
      //std::cout <<"Response : "<<response<<"\n\n";
      stringstream ss;
      ss << response;
      
      //populate http response
      std::shared_ptr<string> res(new string(ss.str()));
      std::shared_ptr<string> sta(new string(toString(retVal))); //r2->status_code));

      //convert the ostream to xml
      std::shared_ptr<ptree> pt(new ptree());
      try {
         if(contentType == CONTENT_TYPE_JSON)
         {
            read_json(ss, *pt);
         }
         else
         {
            read_xml(ss, *pt);
         }
      }
      catch(exception& e){
         cout << e.what() << endl;
         cout << ss.str() << endl;
      }
      //prepare return result
      std::shared_ptr<Result> result(new Result());

      //r2->content.seekg(0, r2->content.beg);
      //result->httpResponse = r2;
      result->httpResponse = res;
      result->httpStatus = sta;
      result->xmlTree = pt;
      return result;
    }

  private:
    
  };

}

/**
@reference
https://akrzemi1.wordpress.com/examples/parsing-xml/
// Copyright 2014 Andrzej Krzemienski.
**/

class DateTranslator {
  typedef boost::date_time::date_facet<Date, char> tOFacet;
  typedef boost::date_time::date_input_facet<Date, char> tIFacet;
  std::locale locale_;

  static std::locale isoDateLocale() {
    std::locale loc;
    loc = std::locale(loc, new tIFacet("%Y-%m-%d"));
    loc = std::locale(loc, new tOFacet("%Y-%m-%d"));
    return loc;
  }

public:
  typedef std::string internal_type;
  typedef Date external_type;

  DateTranslator() : locale_(isoDateLocale()) {}

  boost::optional<external_type> get_value(internal_type const& v) {
    std::istringstream stream(v);
    stream.imbue(locale_);
    external_type vAns;
    if (stream >> vAns) {
      return vAns;
    }
    else {
      return boost::none;
    }
  }

  boost::optional<internal_type> put_value(external_type const& v) {
    std::ostringstream ans;
    ans.imbue(locale_);
    ans << v;
    return ans.str();
  }
};

#endif
