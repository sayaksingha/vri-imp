/* ---------------------------------------------------------
 *
 * How to compile:
 * g++ HttpVxmlClient.cc  -I. -std=c++0x -I/usr/include -I/home/kkothari/soapTest/  /usr/local/lib/libplustache.a /usr/local/lib/libboost_system.a /usr/local/lib/libboost_regex.a  -lpthread -oHttpVxmlClient
 *
 * g++ HttpVxmlClient.cc  -I. -std=c++0x -I/usr/include /usr/local/lib/libplustache.a -lboost_system -lboost_regex -lpthread  -oHttpVxmlClient
 *
 * ./HttpVxmlClient 10.0.2.149 8080 "/test" request.ini response.ini response.xml
 *
 * ---------------------------------------------------------
 */
#include <iostream>
#include "plustache/plustache_types.hpp"
#include "http-soap-client.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <stdlib.h>
#include <math.h>
#if DEBUG
extern bool         gIsDebugEnabled;
#endif

#define TRUNC(x,y) \
((int)(x * pow(10, y))/pow(10, y))

using namespace SimpleSoap;

namespace HttpVxmlClient {
std::map <std::string, std::string> configParams;
std::map <std::string, std::string> requestParams;
std::map <std::string, std::string> jsonParams;
std::map <std::string, std::string> responseParam;
std::map <std::string, std::string> placeHolder;
boost::property_tree::ptree gMappedPt;
string getMappedVal(string section, string value);

string testHost = "";
int    testPort = 80;
string testUri = "";
string testMethod= "GET";
string testHeader = "";

string indent(int level) {
   string s;
   for (int i=0; i<level; i++) s += "  ";
   return s;
}

void printTree (ptree &pt, int level) {
   if (pt.empty()) {
      cerr << "\""<< pt.data()<< "\"";
   }

   else {
      if (level) cerr << endl;

      cerr << indent(level) << "{" << endl;

      for (ptree::iterator pos = pt.begin(); pos != pt.end();) {
         cerr << indent(level+1) << "\"" << pos->first << "\": ";

         printTree(pos->second, level + 1);
         ++pos;
         if (pos != pt.end()) {
            cerr << ",";
         }
         cerr << endl;
      }

      cerr << indent(level) << " }";
   }

   return;
}


void UpdateParamsDetail(std::string rKey, std::string rVal)
{
   if(rKey == "url")
   {
      testUri = rVal;
   }
   else if(rKey == "ip")
   {
      testHost = rVal;
   }
   else if(rKey == "port")
   {
      testPort = std::stol(rVal);
   }
   else if(rKey == "method")
   {
      testMethod = rVal;
   }
   else if(rKey.compare(0,6,"header") == 0)
   {
      testHeader.append(rVal);
      testHeader.append("\r\n");
   }
   return;
}

std::string Evaluate(std::string op, std::string val)
{
   std::string lResult=val;
   if(val.empty())
      val="0";
   if(op.compare(0,3,"MUL") == 0)
   {
      std::string newKey=op;
      newKey.erase(0,3);
      //std::cout<<"Multiplying "<<val<<" by " <<newKey<<std::endl;

      vector<string> strs;
      boost::split(strs, newKey, boost::is_any_of("F"));

      if(strs.size()>1)
      {
         double lTemp = std::stold(val) * std::stold(strs[0]);
         int lPrecision=2;
         try{
            lPrecision = std::stoi(strs[1]);
         }
         catch (std::exception &e)
         {
            lPrecision = 2;
         }
         int isRoundDown=1;
         if(strs.size()>2)
         {
            try{
               isRoundDown = (strs[2].compare("D"));
            }
            catch (std::exception &e)
            {
               isRoundDown = 1;
            }
         }

         char buff[100];
         //snprintf(buff, sizeof(buff), "%.*f", lPrecision, lTemp);
         if(isRoundDown)
            sprintf(buff, "%.*f", lPrecision, TRUNC(lTemp, lPrecision));
         else
            sprintf(buff, "%.*f", lPrecision, lTemp);
         lResult = buff;
         //std::cout<< "VAL:"<<val<<" MUL:"<<strs[0]<<" = "<<lTemp<< " Precision:"<<strs[1]<<"/"<<lPrecision <<" BUFF:"<<buff<<" Result:"<<lResult<<std::endl;
         //    std::string buffAsStdStr = buff;
         //lResult = std::to_string(lTemp); 
      }
      else{
         long long int lTemp = std::stoll(val) * std::stoll(strs[0]);

         lResult = std::to_string(lTemp); //long long int)(std::stol(val) * std::stol(newKey)));
      }
      //std::string lResult = std::string(lTemp);
   }
   else if(op.compare(0,3,"ABS") == 0)
   {
      long double lTemp = abs(std::stold(val));

      lResult = std::to_string(lTemp); //long long int)(std::stol(val) * std::stol(newKey)));
      //std::string lResult = std::string(lTemp);
   }
   else if(op.compare(0,4,"DATE") == 0)
   {
      struct tm   lDateTime;
      char lTimebuf[64];
      std::string newKey=op;
      newKey.erase(0,4);

      vector<string> strs;
      boost::split(strs, newKey, boost::is_any_of("@"));

      strptime(val.c_str(), strs[0].c_str(), &lDateTime);
      strftime(lTimebuf, 64, strs[1].c_str(),&lDateTime);

      lResult = lTimebuf;
      //std::cout<<"Date Format Conversion \""<<val.c_str() <<"\" to  \""<<lResult.c_str()<<"\"\n";
   }
   return lResult;
}

std::string getVal(std::string key)
{
   if (key.compare(0,7,"config.") == 0) 
   {
      std::string newKey=key;
      newKey.erase(0,7);
      //return configParams.find(newKey)->second;
      auto search = configParams.find(newKey);
      if(search != configParams.end()) {
         return search->second;
      }
   }
   else if(key.compare(0,8,"request.") == 0)
   {
      std::string newKey=key;
      newKey.erase(0,8);
      //return requestParams.find(newKey)->second;
      auto search = requestParams.find(newKey);
      if(search != requestParams.end()) {
         return search->second;
      }
   }
   else if(key.compare(0,9,"internal.") == 0)
   {
      std::string newKey=key;
      newKey.erase(0,9);
      //return requestParams.find(newKey)->second;
      auto search = requestParams.find(newKey);
      if(search != requestParams.end()) {
         return search->second;
      }
   }
   else if(key.compare(0,8,"reqjson.") == 0)
   {
      std::string newKey=key;
      newKey.erase(0,8);
      //std::cout <<" Finding RequestJSON key : "<<newKey<<std::endl;
      //return jsonParams.find(newKey)->second;
      auto search = jsonParams.find(newKey);
      if(search != jsonParams.end()) {
         return search->second;
      }
   }
   else if(key.compare(0,7,"rspxml.") == 0)
   {
      std::string newKey=key;
      newKey.erase(0,7);
      //std::cout <<" Finding ResponseXML key : "<<newKey<<std::endl;
      auto search = responseParam.find(newKey);
      if(search != responseParam.end()) {
         return search->second;
      }
   }
   else if(key.compare(0,4,"GRP;") == 0)
      return key;
   else if(key.compare(0,5,"JOIN;") == 0)
      return key;
   else if(key.compare(0,7,"GRPVAL;") == 0)
      return key;
   else if(key.compare(0,4,"CON;") == 0)
   {
      std::string newKey=key;
      newKey.erase(0,4);
      string lValue=getVal(newKey);
      if(lValue.size()> 0)
         return lValue;
      else
         return "";
   }
   else if(key.compare(0,3,"RE:") == 0)
   {
      std::string newKey=key;
      newKey.erase(0,3);
      string lValue=getVal(newKey);
      if(lValue.size()> 0)
         return lValue;
      else
         return newKey;
   }
   else if(key.compare(0,7,"IFNULL;") == 0)
   {
      // Syntax : val=IFNULL;value1?value2
      std::string newKey=key;
      newKey.erase(0,7);

      vector<string> strs;
      boost::split(strs,newKey,boost::is_any_of("?"));

      string lValue="";
      if(strs.size()>1)
      {
         lValue= getVal(strs[0]);

         if(lValue.size()> 0)
            return lValue;
         else
         {
            lValue= getVal(strs[1]); 
            return lValue;
         }
      }
      else
         lValue= getVal(newKey);
      return lValue;
   }
   else if(key.compare(0,4,"MAP;") == 0)
   {
      // Syntax : val=IFNULL;value1?value2
      std::string newKey=key;
      newKey.erase(0,4);

      vector<string> strs;
      boost::split(strs,newKey,boost::is_any_of("?"));

      string lValue="";
      if(strs.size()>1)
      {
         lValue= getVal(strs[0]);

         if(lValue.size()> 0)
         {
            lValue= getMappedVal(strs[1], lValue); 
         }
      }
      else
         lValue= getVal(newKey);
      return lValue;
   }
   else if(key.compare(0,5,"EMAP;") == 0)
   {
      // Syntax : val=EMAP;param1?value1@param2@MappedParam
      std::string newKey=key;
      newKey.erase(0,5);

      vector<string> strs;
      boost::split(strs,newKey,boost::is_any_of("?"));

      string lValue="";
      if(strs.size()>1)
      {

         vector<string> lTokens;
         boost::split(lTokens, strs[1], boost::is_any_of("@"));

         lValue= getVal(strs[0]); 
         
         //std::cout<<" TBR ############## EMAP "<<strs[0]<<" : "<<lValue <<std::endl; 
         if(lTokens.size() == 3)
         {
         //std::cout<<" TBR ############## EMAP Tokens ("<<lTokens[0]<<":"<<lTokens[1] <<":"<<lTokens[2]<<std::endl; 
            if(lValue.compare(lTokens[0]) != 0)
            {
               std::string lSecondValue=getVal(lTokens[1]);
               //std::cout<<" TBR ############## EMAP "<<lTokens[1]<<" : "<<lSecondValue <<std::endl; 
               lValue= getMappedVal(lTokens[2], lSecondValue);
               //std::cout<<" TBR ############## EMAP MappedValue "<<lTokens[2]<<"."<<lSecondValue<<" = "<<lValue <<std::endl; 
            }
         }
      }
      else
         lValue= getVal(newKey);
      return lValue;
   }
   else if(key.compare(0,5,"DATE;") == 0)
   {
      struct tm   lDateTime;
      char lTimebuf[64];
      std::string newKey=key;
      newKey.erase(0,5);

      vector<string> strs;
      boost::split(strs, newKey, boost::is_any_of("?"));

      string lValue="";
      if(strs.size()>1)
      {
         lValue = getVal(strs[0]);

         vector<string> formats;
         boost::split(formats, strs[1], boost::is_any_of("@"));

         strptime(lValue.c_str(), formats[0].c_str(), &lDateTime);
         strftime(lTimebuf, 64, formats[1].c_str(), &lDateTime);

         //std::cout<<"Date Format Conversion ("<<formats[0]<<") \""<<lValue.c_str() <<"\" to ("<<formats[1]<<") \""<<lTimebuf<<"\"\n";
         lValue = lTimebuf;
      }
      else
         lValue= getVal(newKey);

      //std::cout<<"Date Format Conversion Returning : "<<lValue<<"\n";
      return lValue;
   }
   else if(key.compare(0,3,"OP;") == 0)
   {
      struct tm   lDateTime;
      char lTimebuf[64];
      std::string newKey=key;
      newKey.erase(0,3);

      vector<string> strs;
      boost::split(strs, newKey, boost::is_any_of("?"));

      string lValue="";
      if(strs.size()>1)
      {
         lValue = getVal(strs[1]);
         lValue = Evaluate(strs[0], lValue);
         //std::cout<<"Operation Evaluated ("<<strs[0]<<") Returning : "<<lValue<<"\n";
      }
      else
         lValue= getVal(newKey);

      return lValue;
   }
   else if(key.compare(0,11,"DATEOFFSET;") == 0)
   {
      const time_t ONE_DAY = 24 * 60 * 60 ;
      struct tm   lDateTime;
      char lTimebuf[64];
      string lValue="";
      std::string newKey=key;
      newKey.erase(0,11);

      vector<string> strs;
      boost::split(strs, newKey, boost::is_any_of("?"));
      string lFormat = "%D";

      long lOffset = 0;

      if(strs.size()>1)
      {
         lValue = getVal(strs[0]);
         if(lValue.compare("")== 0)
            lOffset = 0;
         else
            lOffset = std::stol(lValue);
         lFormat = strs[1];

         // Seconds since start of epoch
         time_t date_seconds = time(0) + (lOffset * ONE_DAY) ;
         lDateTime = *localtime( &date_seconds ) ; ;
         strftime(lTimebuf, 64, lFormat.c_str(), &lDateTime);
      }
      else
      {
         lValue = getVal(newKey);
         if(lValue.compare("")== 0)
            lOffset = 0;
         else
            lOffset = std::stol(lValue);
         // Seconds since start of epoch
         time_t date_seconds = time(0) + (lOffset * ONE_DAY) ;
         lDateTime = *localtime( &date_seconds ) ; ;
         strftime(lTimebuf, 64, lFormat.c_str(), &lDateTime);

      }

      lValue = lTimebuf;

      //std::cout<<"Date Offset ("<<lOffset<<") \""<<lValue.c_str() <<"\" to ("<<lFormat<<") \"\n";
      return lValue;
   }
   else if(key.compare(0,7,"SHOWIF;") == 0)
   {
      // Syntax : val=SHOWIF;value1?value2  -- Compare Two Values to be equal
      // Syntax : val=SHOWIF;value1
      std::string newKey=key;
      newKey.erase(0,7);

      vector<string> strs;
      boost::split(strs, newKey, boost::is_any_of("?"));
      if(strs.size()<=1)
      {
         string lValue="false";
         lValue= getVal(newKey);

         if(lValue.size()> 0)
         {
            long lTemp= std::stol(lValue);
            if(lTemp)
               lValue= "true"; 
         }
         return lValue;
      }
      else
      {
         string lValue="false";

         string lValue1=getVal(strs[0]);
         string lValue2=getVal(strs[1]);
         boost::algorithm::to_lower(lValue1);
         boost::algorithm::to_lower(lValue2);

         if(lValue1 == lValue2)
         {
            lValue= "true"; 
         }
         return lValue;
      }
   }
   else if(key.compare(0,4,"val.") == 0)
   {
      std::string newKey=key;
      newKey.erase(0,4);
      return newKey;
   }
   else if(key.compare(0,5,"TMPL;") == 0)
   {
      return key;
   }
   else
      return "";
   return "";
}

std::string  GetTemplateVal(Plustache::Context &ctx, string templateStr )
{
   Plustache::template_t t;
   std::string lVal;
   lVal = t.render(templateStr, ctx);

   //std::cout<<"Template :"<<templateStr<<" After Substitution: "<<lVal<<"\n";
   return lVal;
}


string getMappedVal(string section, string value)
{
   static std::string defaultStr("default");
   string lValue="$$$"; //value;
   try{
   lValue = gMappedPt.get<std::string>(section+"."+value); //, value); // getMappedVal(section, defaultStr)) ;
   //std::cout<<"####################################### ["<<section<<"."<<value<<"] = "<<lValue<<std::endl;
   }
   catch (boost::property_tree::ptree_bad_path & e)
   {
      //std::cout << "Exception: " << e.what() << "\n";
      if(value.compare(defaultStr)!= 0) 
         lValue = gMappedPt.get<std::string>(section+"."+defaultStr, "-1");
      //std::cout<<"####################################### ["<<section<<"."<<defaultStr<<"] = "<<lValue<<std::endl;
   }
   catch (std::exception& e)
   {
      //std::cout << "Exception: " << e.what() << "\n";
   }
   return lValue;
}
void AddToContext(Plustache::Context &ctx, string key, string val)
{
   if(val.compare(0,4,"GRP;") == 0)
   {
      //Addinng Group Values to Context
      //std::cout << "Addinng Group Values to Context" << "\n";
      vector<string> strs;
      boost::split(strs,val,boost::is_any_of(";"));

      //std::cout << "* size of the vector: " << strs.size() << endl;
      //for (size_t i = 0; i < strs.size(); i++)
      //   cout << strs[i] << endl;

      if(strs.size()<3)
      {
         //std::cout <<"Error : none of details parameter configured for Grouped Data\n";
         ctx.add(key, "");
         return;
      }
      else
      {
         vector<string> keyPart, valuePart, regexPart, operationPart;
         for (int i=2, keyIndex = 0; i<strs.size(); i++, keyIndex++)
         {
            vector<string> lParts;
            boost::split(lParts, strs[i],boost::is_any_of(":"));
            keyPart.push_back(lParts[0]);

            vector<string> lKeyParts;
            boost::split(lKeyParts, lParts[1], boost::is_any_of("$$"));

            if(lKeyParts.size()>1)
            {
               //for(int k=0; k<lKeyParts.size(); k++)
               //   std::cout<<"Key Parts: "<<(k+1)<<" : "<<lKeyParts[k]<<std::endl;
               valuePart.push_back(lKeyParts[0]);
               regexPart.push_back(lKeyParts[1]);
               if(lKeyParts.size()>2)
                  operationPart.push_back(lKeyParts[2]);
               else
                  operationPart.push_back("");
            }
            else
            {
               valuePart.push_back(lParts[1]);
               regexPart.push_back("");
               operationPart.push_back("");
            }
         }

         string groupedElement = getVal(strs[1]);
         bool lIsIndexed = false;
         int noOfElements = 1;
         if(groupedElement.compare(0,7,"$count:") == 0)
         {
            lIsIndexed = true;
            std::string newKey=groupedElement;
            newKey.erase(0,7);
            noOfElements = atoi(newKey.c_str());
         }
         {
            //std::cout<<" Grouped Element from tree : "<<strs[1]<<" = " <<groupedElement<<" NoOfSubElements:"<<noOfElements<<std::endl;
            for(long long int i=0; i<noOfElements; i++)
            {
               PlustacheTypes::ObjectType obj;
               bool lRegexMatchResult = true;
               for(long long int j=0; j< keyPart.size(); j++)
               {
                  string lKey;
                  if(lIsIndexed)
                     lKey = strs[1]+"["+std::to_string(i)+"]."+valuePart[j];
                  else
                     lKey = strs[1]+"."+valuePart[j];

                  string value=getVal(lKey);

                  if(regexPart[j].size() > 0)
                  {
                     vector<string> lRegParts;
                     std::string lRegExprStr=regexPart[j];
                     boost::split(lRegParts, regexPart[j], boost::is_any_of("|"));
                     for(int k=0, first=1; k< lRegParts.size(); k++)
                     {
                        if(!first)
                           lRegExprStr = lRegExprStr + "|" + getVal("RE:"+lRegParts[k]);
                        else
                        {
                           lRegExprStr = getVal("RE:"+lRegParts[k]);
                           first = 0;
                        }

                     }

                     boost::regex lRegExpr(lRegExprStr); //regexPart[j]);
                     // Apply Regular Expression and validate
                     lRegexMatchResult = boost::regex_match(value, lRegExpr);
                     //std::cout<<"Apply Regular Expression (expr: "<<lRegExprStr<<") on Value: "<<value<< " and Result : "<<lRegexMatchResult<<std::endl;
                  }

                  //std::cout<<" Operation ["<<j<<"] ="<<operationPart[j]<<std::endl;
                  if(operationPart[j].size() > 0)
                     obj[keyPart[j]] = Evaluate(operationPart[j], value);
                  else
                     obj[keyPart[j]] = value;
                  //obj[keyPart[j]] = value;
                  //std::cout<<" Object Add -->" << keyPart[j] << " : " << value <<"\n";
               }
               //ctx.add(strs[1], obj);
               if(lRegexMatchResult)
                  ctx.add(key, obj);
               //std::cout<<"Context Add -> "<<strs[1]<<" : Object as Above"<<std::endl;
            }
         }
#if 0
         else
         {
            //std::cout<<" Grouped Element from tree : "<<strs[1]<<" = " <<groupedElement<<std::endl;
            ctx.add(key, groupedElement);
         }
#endif
      }
   }
   else if(val.compare(0,7,"GRPVAL;") == 0)
   {
      //Addinng Group Values to Context
      //std::cout << "Addinng Group Values to Context" << "\n";
      vector<string> strs;
      boost::split(strs,val,boost::is_any_of(";"));

      //std::cout << "* size of the vector: " << strs.size() << endl;
      //for (size_t i = 0; i < strs.size(); i++)
      //   cout << strs[i] << endl;

      if(strs.size()<4)
      {
         //std::cout <<"Error : none of details parameter configured for Grouped Data\n";
         ctx.add(key, "");
         return;
      }
      else
      {
         vector<string> keyPart, valuePart, regexPart, operationPart;
         for (int i=3, keyIndex = 0; i<strs.size(); i++, keyIndex++)
         {
            vector<string> lParts;
            boost::split(lParts, strs[i],boost::is_any_of(":"));
            keyPart.push_back(lParts[0]);

            vector<string> lKeyParts;
            boost::split(lKeyParts, lParts[1], boost::is_any_of("$$"));

            if(lKeyParts.size()>1)
            {
               for(int k=0; k<lKeyParts.size(); k++)
                  //std::cout<<"Key Parts: "<<(k+1)<<" : "<<lKeyParts[k]<<std::endl;
               valuePart.push_back(lKeyParts[0]);
               regexPart.push_back(lKeyParts[1]);
               if(lKeyParts.size()>2)
                  operationPart.push_back(lKeyParts[2]);
               else
                  operationPart.push_back("");
            }
            else
            {
               valuePart.push_back(lParts[1]);
               regexPart.push_back("");
               operationPart.push_back("");
            }
         }

         string groupedElement = getVal(strs[1]);
         bool lIsIndexed = false;
         int noOfElements = 1;
         if(groupedElement.compare(0,7,"$count:") == 0)
         {
            lIsIndexed = true;
            std::string newKey=groupedElement;
            newKey.erase(0,7);
            noOfElements = atoi(newKey.c_str());
         }
         {
            //std::cout<<" Grouped Element from tree : "<<strs[1]<<" = " <<groupedElement<<" NoOfSubElements:"<<noOfElements<<std::endl;
            for(long long int i=0; i<noOfElements; i++)
            {
               PlustacheTypes::ObjectType obj;
               bool lRegexMatchResult = true;
               for(long long int j=0; j< keyPart.size(); j++)
               {
                  string lKey;
                  if(lIsIndexed)
                     lKey = strs[1]+"["+std::to_string(i)+"]."+valuePart[j];
                  else
                     lKey = strs[1]+"."+valuePart[j];

                  string value=getVal(lKey);

                  if(regexPart[j].size() > 0)
                  {
                     vector<string> lRegParts;
                     std::string lRegExprStr=regexPart[j];
                     boost::split(lRegParts, regexPart[j], boost::is_any_of("|"));
                     for(int k=0, first=1; k< lRegParts.size(); k++)
                     {
                        if(!first)
                           lRegExprStr = lRegExprStr + "|" + getVal("RE:"+lRegParts[k]);
                        else
                        {
                           lRegExprStr = getVal("RE:"+lRegParts[k]);
                           first = 0;
                        }

                     }

                     boost::regex lRegExpr(lRegExprStr); //regexPart[j]);
                     // Apply Regular Expression and validate
                     lRegexMatchResult = boost::regex_match(value, lRegExpr);
                     //std::cout<<"Apply Regular Expression (expr: "<<lRegExprStr<<") on Value: "<<value<< " and Result : "<<lRegexMatchResult<<std::endl;
                  }

                  //std::cout<<" Operation ["<<j<<"] ="<<operationPart[j]<<std::endl;
                  if(operationPart[j].size() > 0)
                     obj[keyPart[j]] = Evaluate(operationPart[j], value);
                  else
                     obj[keyPart[j]] = value;
                  //std::cout<<" Object Add -->" << keyPart[j] << " : " << value <<"\n";
               }
               //ctx.add(strs[1], obj);
               if(lRegexMatchResult)
               {
                  ctx.add(key, obj[strs[2]]);
                  break;
               }
               //std::cout<<"Context Add -> "<<strs[1]<<" : Object as Above"<<std::endl;
            }
         }
#if 0
         else
         {
            //std::cout<<" Grouped Element from tree : "<<strs[1]<<" = " <<groupedElement<<std::endl;
            ctx.add(key, groupedElement);
         }
#endif
      }
   }
   else if(val.compare(0,5,"JOIN;") == 0)
   {
      //Addinng Joined Group Values to Context
      //std::cout << "Addinng Joined Group Values to Context" << "\n";
      vector<string> strs;
      boost::split(strs,val,boost::is_any_of(";"));

      //std::cout << "* size of the vector: " << strs.size() << endl;
      //for (size_t i = 0; i < strs.size(); i++)
      //   cout << strs[i] << endl;

      if(strs.size()<3)
      {
         //std::cout <<"Error : none of details parameter configured for Grouped Data\n";
         ctx.add(key, "");
         return;
      }
      else
      {
         vector<string> keyPart, valuePart, regexPart, operationPart;
         for (int i=2, keyIndex = 0; i<strs.size(); i++, keyIndex++)
         {
            vector<string> lParts;
            boost::split(lParts, strs[i],boost::is_any_of(":"));
            keyPart.push_back(lParts[0]);

            vector<string> lKeyParts;
            boost::split(lKeyParts, lParts[1], boost::is_any_of("$$"));

            if(lKeyParts.size()>1)
            {
               //for(int k=0; k<lKeyParts.size(); k++)
               //   std::cout<<"Key Parts: "<<(k+1)<<" : "<<lKeyParts[k]<<std::endl;
               valuePart.push_back(lKeyParts[0]);
               regexPart.push_back(lKeyParts[1]);
               if(lKeyParts.size()>2)
                  operationPart.push_back(lKeyParts[2]);
               else
                  operationPart.push_back("");
            }
            else
            {
               valuePart.push_back(lParts[1]);
               regexPart.push_back("");
               operationPart.push_back("");
            }
         }

         string groupedElement = getVal(strs[1]);
         bool lIsIndexed = false;
         int noOfElements = 1;
         if(groupedElement.compare(0,7,"$count:") == 0)
         {
            lIsIndexed = true;
            std::string newKey=groupedElement;
            newKey.erase(0,7);
            noOfElements = atoi(newKey.c_str());
         }
         {
            //std::cout<<" Grouped Element from tree : "<<strs[1]<<" = " <<groupedElement<<" NoOfSubElements:"<<noOfElements<<std::endl;
            for(long long int i=0; i<noOfElements; i++)
            {
               PlustacheTypes::ObjectType obj;
               bool lRegexMatchResult = true;
               string lJoinedValue = "";
               for(long long int j=0; j< keyPart.size(); j++)
               {
                  string lKey;
                  if(lIsIndexed)
                     lKey = strs[1]+"["+std::to_string(i)+"]."+valuePart[j];
                  else
                     lKey = strs[1]+"."+valuePart[j];

                  string value=getVal(lKey);

                  if(regexPart[j].size() > 0)
                  {
                     vector<string> lRegParts;
                     std::string lRegExprStr=regexPart[j];
                     boost::split(lRegParts, regexPart[j], boost::is_any_of("|"));
                     for(int k=0, first=1; k< lRegParts.size(); k++)
                     {
                        if(!first)
                           lRegExprStr = lRegExprStr + "|" + getVal("RE:"+lRegParts[k]);
                        else
                        {
                           lRegExprStr = getVal("RE:"+lRegParts[k]);
                           first = 0;
                        }

                     }

                     boost::regex lRegExpr(lRegExprStr); //regexPart[j]);
                     // Apply Regular Expression and validate
                     lRegexMatchResult = boost::regex_match(value, lRegExpr);
                     //std::cout<<"Apply Regular Expression (expr: "<<lRegExprStr<<") on Value: "<<value<< " and Result : "<<lRegexMatchResult<<std::endl;
                  }

                  //std::cout<<" Operation ["<<j<<"] ="<<operationPart[j]<<std::endl;
                  if(operationPart[j].size() > 0)
                  {
                     obj[keyPart[j]] = Evaluate(operationPart[j], value);
                     lJoinedValue += " "+Evaluate(operationPart[j], value);
                  }
                  else
                  {
                     obj[keyPart[j]] = value;
                     lJoinedValue += " "+value;
                  }
                  //obj[keyPart[j]] = value;
                  //std::cout<<" Object Add -->" << keyPart[j] << " : " << value <<"\n";
               }
               //ctx.add(strs[1], obj);
               if(lRegexMatchResult)
               {
                  //ctx.add(key, obj);
                  //std::cout<<" Adding Joined Value to Context: "<<lJoinedValue<<std::endl;
                  ctx.add(key, lJoinedValue);
               }
               //std::cout<<"Context Add -> "<<strs[1]<<" : Object as Above"<<std::endl;
            }
         }
#if 0
         else
         {
            //std::cout<<" Grouped Element from tree : "<<strs[1]<<" = " <<groupedElement<<std::endl;
            ctx.add(key, groupedElement);
         }
#endif
      }
   }
   else if(val.compare(0,5,"TMPL;") == 0)
   {
      std::string newVal=val;
      newVal.erase(0,5);
      //std::cout<<"Template to be substituted\n";
      std::string lVal=GetTemplateVal(ctx, newVal);
      //std::cout<< "ctx.add ("<<key<<", "<<lVal<<")\n";
      ctx.add(key, lVal);
      UpdateParamsDetail(key, lVal);
   }
   else
   {
      //std::cout<< "ctx.add ("<<key<<", "<<val<<")\n";
      ctx.add(key, val);
   }
}


Plustache::Context CreateContext(ptree &phTree)
{
   Plustache::Context ctx;
   for (ptree::iterator pos = phTree.begin(); pos != phTree.end();pos++) {
      if(pos->second.empty())
      {
         std::string lFirst = pos->first.data();
         //std::cout<<"Adding to context "<<pos->first.data()<<" : " << pos->second.data()<<std::endl;
         std::string lVal= getVal(pos->second.data());
         //std::cout<<"Context Add -> "<<" : "<<lFirst<<std::endl;
         //std::cout<<"Context Add -> "<<" : "<<lVal<<std::endl;
         //std::cout<<"Context Add -> "<<pos->first.data()<<" : "<<lVal<<std::endl;
         //std::cout<<"Context Add -> "<<pos->first.data()<<" : "<<lVal<<std::endl;
         AddToContext(ctx, lFirst, lVal);
         //AddToContext(ctx, pos->first.data(), lVal);
         //std::cout<<"Context Add -> "<<pos->first.data()<<" : "<<getVal(pos->second.data())<<std::endl;
         //AddToContext(ctx, pos->first.data(), getVal(pos->second.data()));
      }
      else
      {
         PlustacheTypes::ObjectType sobObject;
         ptree subTree = pos->second;

         for (ptree::iterator obj = subTree.begin(); obj != subTree.end();obj++) {
            sobObject[obj->first.data()] = getVal(obj->second.data());
            //std::cout<<"Object Add -> "<<obj->first.data()<<" : "<<getVal(obj->second.data())<<std::endl;
         }
         ctx.add(pos->first.data(), sobObject);
         //std::cout<<"Context Add -> "<<pos->first.data()<<" : Object as Above"<<std::endl;
      }
   }
   return ctx;
}

std::string  RemoveNameSpace( std::string key)
{
   std::string str=key;
   try{
      std::size_t pos = key.find(":");
      if(pos != std::string::npos){
         str = key.substr (pos+1);
      }
   }
   catch (std::exception& e)
   {
      //std::cout << "Exception: " << e.what() << "\n";
   }
   //std::cout<<"Remove Name Space : "<<key<<":"<<str<<std::endl;
   return str;
}
void InsertToMap(std::map <std::string, std::string> &rMap, std::string rPrefix, std::string key, std::string val)
{
   //   std::size_t pos = key.find(":");
   //   if(pos>0)
   //   key = key.substr (pos);

   boost::algorithm::trim(val);
   boost::algorithm::trim(key);
   rMap.insert (pair <std::string, std::string> (rPrefix+RemoveNameSpace(key), val));
   //string finalKey = rPrefix+RemoveNameSpace(key);
   //for (; finalKey[finalKey.size()-1]=='.';)
   //   finalKey.erase( finalKey.end()-1 );

   //rMap.insert (pair <std::string, std::string> (finalKey, val));
   return ;
}


void CreateMap(std::map <std::string, std::string> &rMap, std::string rPrefix, boost::property_tree::ptree &rTree, long long int j=0);
void CreateMap(std::map <std::string, std::string> &rMap, std::string rPrefix, boost::property_tree::ptree &rTree, long long int j)
{
   static const std::string XML_ATTR="<xmlattr>";
   static const std::string EMPTY_STR="";
   long long int i = 0;
   std::string lastNode="";
      rPrefix = RemoveNameSpace(rPrefix);

      for (boost::property_tree::ptree::iterator pos = rTree.begin(); pos != rTree.end();pos++) 
      {
         if(pos->second.empty())
         {
            if(rPrefix=="")
            {
            //rMap.insert (pair <std::string, std::string> (rPrefix +RemoveNameSpace(pos->first.data()), (pos->second.data())));
            InsertToMap(rMap,rPrefix,pos->first.data(), (pos->second.data()));
            }
            else
            {
            if(rTree.count(pos->first.data()) > 1)
            {
            //rMap.insert (pair <std::string, std::string> (rPrefix +pos->first.data()+"["+std::to_string(j)+"]", (pos->second.data())));
            InsertToMap(rMap,rPrefix, (EMPTY_STR+pos->first.data()+"["+std::to_string(j)+"]"), (pos->second.data()));
            j++;
            }
            else
            {
            //rMap.insert (pair <std::string, std::string> (rPrefix +pos->first.data(), (pos->second.data())));
            InsertToMap(rMap,rPrefix,pos->first.data(), (pos->second.data()));
            }
            }
         }
         else
         {
            if(XML_ATTR.compare(pos->first.data()) != 0)
            {

               //std::cout<<"################## LastNode:" <<lastNode<<"    CurrentNode:"<<pos->first.data()<<" J="<<j<<std::endl;
               if(lastNode != pos->first.data())
               {
                  //i=j=0;
                  j=0;
               }

               lastNode = pos->first.data();
               //std::cout<<"################## LastNode:" <<lastNode<<"    CurrentNode:"<<pos->first.data()<<" J="<<j<<std::endl;

               if(rTree.count(pos->first.data()) > 1)
               {
                  //rMap.insert (pair <std::string, std::string> (rPrefix +pos->first.data(), "$count:"+to_string((long long int)rTree.count(pos->first.data()))));
                  InsertToMap(rMap,rPrefix,(EMPTY_STR+pos->first.data()), "$count:"+to_string((long long int)rTree.count(pos->first.data())));
                  //CreateMap(rMap, rPrefix + RemoveNameSpace(EMPTY_STR+pos->first.data()+"["+std::to_string(i)+"]"+"."), pos->second, j);
                  CreateMap(rMap, rPrefix + RemoveNameSpace(EMPTY_STR+pos->first.data()+"["+std::to_string(j)+"]"+"."), pos->second, 0);
                  std::string str ((pos->second.data()));
                  boost::algorithm::trim(str);
                  if(str.length() >0)
                     InsertToMap(rMap,rPrefix,(EMPTY_STR+pos->first.data()+"["+std::to_string(j)+"]"), str);
                     //rMap.insert (pair <std::string, std::string> (rPrefix +pos->first.data()+"["+std::to_string(j)+"]", str));
                  j++;
               }
               else
               {
                  CreateMap(rMap, rPrefix + RemoveNameSpace(EMPTY_STR+pos->first.data()+"."), pos->second, 0);
                  std::string str ((pos->second.data()));
                  boost::algorithm::trim(str);
                  if(str.length() >0)
                     InsertToMap(rMap,rPrefix,pos->first.data(), str);
                     //rMap.insert (pair <std::string, std::string> (rPrefix +pos->first.data(), str));
               }
            }
         }
         //i++;
      }
      return ;
}


int ReadConfigFile(std::string cfgFile, Plustache::Context &ctx, std::string &tplFile, 
      std::string &url,
      std::string &ip,
      int &port,
      std::string &method,
      std::string &header
      )
{
   boost::property_tree::ptree pt;
   boost::property_tree::ptree phTree;
   boost::property_tree::ini_parser::read_ini(cfgFile, pt);
   //std::cout<<"################# ReadConfigFile ######### Tree #############\n";
   //printTree(pt, 0);
   //std::cout<<"################# ReadConfigFile ######### Tree #############\n";
   //tplFile = pt.get<std::string>("templatefile");
   //std::cout << "Template File : " <<tplFile<< std::endl;

   BOOST_FOREACH( boost::property_tree::ptree::value_type const& config, pt.get_child( "" ) )
   {
      if(config.first == "templatefile")
      {
         tplFile = config.second.data();
         //std::cout << "Template File : " <<tplFile<< std::endl;
      }
      else if(config.first == "url")
      {
         url = config.second.data();
         //std::cout << "URL : " <<url<< std::endl;
      }
      else if(config.first == "ip")
      {
         ip = config.second.data();
         //std::cout << "IP : " <<ip<< std::endl;
      }
      else if(config.first == "port")
      {
         port = std::stol(config.second.data());
         //std::cout << "Port : " <<port<< std::endl;
      }
      else if(config.first == "method")
      {
         method = config.second.data();
         //std::cout << "Method : " <<method<< std::endl;
      }
      else if(config.first.compare(0,6,"header") == 0)
      {
         if(config.second.data().compare(0,5,"TMPL;") != 0)
         {
            header.append(config.second.data());
            header.append("\r\n");
         }
      }
      //else

      {
         std::string label = config.first.data();
         std::string text = config.second.data();
         phTree.put(label, text);
      }

   }
   ctx = CreateContext(phTree);
   return 0;
}

int ReadConfigFile(std::string cfgFile, Plustache::Context &ctx, std::string &tplFile)
{
   boost::property_tree::ptree pt;
   boost::property_tree::ptree phTree;
   boost::property_tree::ini_parser::read_ini(cfgFile, pt);
   //std::cout<<"################# ReadConfigFile ######### Tree #############\n";
   //printTree(pt, 0);
   //std::cout<<"################# ReadConfigFile ######### Tree #############\n";
   //tplFile = pt.get<std::string>("templatefile");
   //std::cout << "Template File : " <<tplFile<< std::endl;

   BOOST_FOREACH( boost::property_tree::ptree::value_type const& config, pt.get_child( "" ) )
   {
      if(config.first == "templatefile")
      {
         tplFile = config.second.data();
         //std::cout << "Template File : " <<tplFile<< std::endl;
      }
      else
      {
         std::string label = config.first.data();
         std::string text = config.second.data();
         phTree.put(label, text);
      }

   }
   ctx = CreateContext(phTree);
   return 0;
}
bool ReadInputJsonFile(std::string jsonFile, std::map<std::string, std::string> &jsonMap, bool isFile=true)
{
      boost::property_tree::ptree jsonTree;
      if(isFile)
      {
      // Reading response XML file and buildng the response MAP 
      try {
         read_json(jsonFile, jsonTree); 
         //printTree(jsonTree, 0); 
      }catch(std::exception &jpe){
         //do error handling
         //std::cout<<"Execption occured while converting JSON file("<<jsonFile<<") to PTree\n";
         return false;
      }
      }
      else
      {
        std::stringstream ss;
        // send your JSON above to the parser below, but populate ss first

        ss << jsonFile;

        boost::property_tree::read_json(ss, jsonTree);
      }
      std::string lPrefix("");
      CreateMap(jsonMap, lPrefix, jsonTree);
#if DEBUG
      if(gIsDebugEnabled)
      {
      //std::cout<<"#################### JSON tree ####################\n";
      //printTree(jsonTree, 0);
      //std::cout<<"#################### JSON tree ####################\n";

      // printing 
      std::map <std::string, std::string> :: iterator it;
      std::cout<<"#################### JSON MAP ####################\n";
      for ( it= jsonMap.begin(); it!=jsonMap.end(); it++) {
        cout << it -> first; 
         cout << '\t';
         cout << it -> second;
         cout << endl;
      }
      std::cout<<"#################### JSON MAP ####################\n";
      }
      //std::cout<<" MapString: "<<jsonMap;
#endif
      return true;
}

void fillValueMap(std::string jsonFile)
{
   configParams["cfg1"]="value1";
   configParams["cfg2"]="value2";
   configParams["cfg3"]="value3";
   configParams["cfg4"]="value4";
   configParams["cfg5"]="Core";
   configParams["null"]="";
   configParams["num0"]="0";
   configParams["num1"]="1";
   configParams["num2"]="2";
   configParams["num3"]="3";
   configParams["num4"]="4";

   requestParams["host"]=testHost;
   requestParams["port"]=toString(testPort);
   requestParams["uri"]=testUri;

   ReadInputJsonFile(jsonFile, jsonParams);
   boost::property_tree::ini_parser::read_ini("/opt/ivr/www/scripts/mapping.ini", gMappedPt);
}

std::string ProcessHttpQuery(std::string rHost, int rPort, std::string rUri, std::string rQuery, int& status, std::string requestCfgFile, std::string responseCfgFile)
{
#if DEBUG
   if(gIsDebugEnabled)
   {
      std::cout<<" Inside ProcessHttpQuery : http://"<<rHost<<":"<<rPort<<"/"<<rUri<<"?"<<rQuery<<"\n";
      std::cout<<" Inside ProcessHttpQuery : RequestCfg:"<<requestCfgFile<<" Response Cfg:"<<responseCfgFile<<"\n";
   }
#endif
   //indicate what service to invoke
   std::string lRequestTmplFile, lResponseTmplFile;
   Plustache::Context lRequestContext, lResponseContext;
   
   try
   {
      // Filling up the Value Map from Configuurations & Request Structure
      ReadInputJsonFile(rQuery, jsonParams, false);
      boost::property_tree::ini_parser::read_ini("/opt/ivr/www/scripts/mapping.ini", gMappedPt);

      testHost=rHost;
      testPort=rPort;
      testUri=rUri;

      // Generating Internal Unique Txn Reference Id
      char lTxnId[20] = "";
      struct timeval  timePtr;

      gettimeofday(&timePtr, 0);

      snprintf(lTxnId, 20, "%010ld%06ld",
            timePtr.tv_sec,
            timePtr.tv_usec);
      
      std::string lUniqueRefNum (lTxnId);
      requestParams["uniqueRefNum"] = lUniqueRefNum;

#if DEBUG
      if(gIsDebugEnabled)
      {
         std::cout << "internal.uniqueRefNum :"<<lUniqueRefNum<<"\n";
      }
#endif

      // Creating the Context required for the Templates
      ReadConfigFile(requestCfgFile, lRequestContext, lRequestTmplFile, testUri, testHost, testPort, testMethod, testHeader);

      //create your client and pass your context
      auto client = Client(testHost, testPort, testUri, testMethod, testHeader);
      client.compile(lRequestTmplFile, lRequestContext, 1);

      //client.logRequest();
      //get the result
      auto result = client.sendAndReceive();  
#if DEBUG
      if(gIsDebugEnabled)
      {
      std::cout << "body" << *result->httpResponse << "\n";
      std::cout << "status => " << *result->httpStatus << "\n";
      }
#endif

      try{
         status = std::stol(*result->httpStatus);
      } catch (...)
      {
         status = -1; 
      }
      //traverse xml
      //auto responseTree = result->xmlTree;
      boost::property_tree::ptree responseTree = *(result->xmlTree);
      std::string lPrefix("");
      CreateMap(responseParam, lPrefix, responseTree);

#if DEBUG
      if(gIsDebugEnabled)
      {
      // printing 
      std::map <std::string, std::string> :: iterator it;
      //std::cout<<"#################### MAP ####################\n";
      for ( it= responseParam.begin(); it!=responseParam.end(); it++) {
         cout << it -> first; 
         cout << '\t';
         cout << it -> second;
         cout << endl;
      }
      //std::cout<<"#################### MAP ####################\n";
      }
#endif

      // Creating the Context required for the Templates
      ReadConfigFile(responseCfgFile, lResponseContext, lResponseTmplFile);

      client.compile(lResponseTmplFile, lResponseContext, 1);
      //client.logRequest();
      string xmlstr = client.getXml();
#if DEBUG
      if(gIsDebugEnabled)
      {
      std::cout<<"#################### Generating JSON from xml status: "<<status<<" ############\n";
      //string jsonstr = xml2json(client.getXml().c_str());
      //std::cout<<jsonstr<<std::endl;
      std::cout<<xmlstr<<std::endl;
      }
#endif      
      status = 0;
      return xmlstr;

   }
   catch (std::exception& e)
   {
      status = -1;
#if DEBUG
      if(gIsDebugEnabled)
      {
         std::cout << " Inside ProcessHttpQuery Exception: " << e.what() << "\n";
      }
#endif
      return e.what();
   }

   status = -1; 
   return "UNKNOWN_ERROR";

}
}
