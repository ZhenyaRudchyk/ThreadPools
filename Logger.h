#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#define LOG
class CLogger
{
public:
  CLogger();
  ~CLogger(); 
  bool Initialize(const std::string & csFileName);
  bool OpenFile();
  bool WriteToFileWithNewLine(const std::string & csMessage);
  void CloseFile();
private:
std::mutex m_Locker;
std::string m_sFilePath;
std::ofstream m_File;
bool m_bInitialized;
};
///////////////////////////////////////////////////////////////////////////////////////////////
extern CLogger g_Logger;
///////////////////////////////////////////////////////////////////////////////////////////////
bool CLogger::Initialize(const std::string & csFileName)
{
  if(csFileName.empty())
  {
    return m_bInitialized;
  }
  m_sFilePath.assign(csFileName);
  m_bInitialized = true;
  return m_bInitialized;
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool CLogger::OpenFile()
{
  if(!m_bInitialized)
  {
    return false;
  }

  m_File.open(m_sFilePath, std::ofstream::out | std::ofstream::app);
  
  if(!m_File.fail())
  {
	std::strerror(errno); 
  }
  return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
 bool CLogger::WriteToFileWithNewLine(const std::string & csMessage)
 {
  std::scoped_lock<std::mutex> guard(m_Locker);
   if(!m_File.is_open())
   {
     return false;
   }

    m_File << csMessage << '\n';

    if(!m_File.good())
    {
      std::cout <<"Write failed!!!"<< '\n';
      std::cout << "Error: " << std::strerror(errno);
      return false;
    }

   return true;
 }
///////////////////////////////////////////////////////////////////////////////////////////////
 void CLogger::CloseFile()
 {
   m_File.close();

   if(m_File.good())
   {
      std::cout <<"File succesfully closed!!!"<< '\n';
   }
   else
   {
      std::cout <<"Error on close !!!"<< '\n';
      std::cout << "Error: " << std::strerror(errno) << '\n';
   }
 }
///////////////////////////////////////////////////////////////////////////////////////////////
CLogger::CLogger():
m_bInitialized(false)
{}
///////////////////////////////////////////////////////////////////////////////////////////////
  CLogger::~CLogger() 
  {
    CloseFile();
  }

  std::string ThreadIDToString(std::thread::id id)
  {
	  std::ostringstream ss;
	  ss << id;
	  return ss.str();
  }