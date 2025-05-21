#include <string>

#include "addon.h"

/* 
 * AUXILLARY FUNCTIONS
 */

bool checkInteger( const std::string &str, int &data )
{
  char *end;

  data = strtoul(str.c_str(), &end, 10);
  return *end == 0 || (*end == '\r' && strlen(end) == 1);
}

std::vector<std::string> split( const std::string &str, char delim )
{
  std::vector<std::string> res;
  std::string cur = "";

  for (const char &ch : str)
  {
    if (delim == ch)
    {
      res.push_back(cur);
      cur = "";
    }
    else
    {
      cur += ch;
    }
  }

  res.push_back(cur);
  return res;
}

/*
 * FUNCTIONS TO WORK WITH TIME
 */

Time::Time( const std::string &timeStr )
{
  std::vector<std::string> hm = split(timeStr, ':');

  if (hm.size() != 2 || hm[0].length() != 2 || hm[1].length() != 2)
    throw std::invalid_argument("Error in delimiters!");

  if (!checkInteger(hm[0], hour) || !checkInteger(hm[1], minute))
    throw std::invalid_argument("Time is not integer!");

  if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
    throw std::invalid_argument("Impossible time!");}

bool Time::operator <( const Time &other ) const
{
  if (hour == other.hour)
    return minute < other.minute;
  return hour < other.hour;
}

bool Time::operator >( const Time &other ) const
{
  if (hour == other.hour)
    return minute > other.minute;
  return hour > other.hour;
}

bool Time::operator ==( const Time &other ) const
{
  return hour == other.hour && minute == other.minute;
}

bool Time::operator <=( const Time &other ) const
{
  if (hour == other.hour)
    return minute <= other.minute;
  return hour < other.hour;
}

bool Time::operator >=( const Time &other ) const
{
  if (hour == other.hour)
    return minute >= other.minute;
  return hour > other.hour;
}

int Time::operator -( const Time &other ) const
{
  int allMinutes = hour * 60 + minute;
  int otherAllMinutes = other.hour * 60 + other.minute;

  return allMinutes - otherAllMinutes;
}

/*
 * OPERATORS TO PARSE & OUTPUT EVENTS
 */

std::istream & operator >>( std::istream &stream, Event &e )
{
  std::istream::sentry sentry(stream);

  if (!sentry)
    return stream;

  std::string eventStr;

  std::getline(stream, eventStr);
  std::vector<std::string> params = split(eventStr, ' ');

  try
  {
    for (const auto &str : params)
    {
      if (str == "")
        throw std::exception();
    }

    if (params.size() != 3 && params.size() != 4)
      throw std::exception();

    e.time = Time(params[0]);      // first argument in event is always time
    checkInteger(params[1], e.id); // second is always ID
    e.strParam = params[2];        // first parameter

    if (params.size() == 4)        // optional parameter
      checkInteger(params[3], e.iParam);
    else
      e.iParam = -1;
  }
  catch (...)
  {
    throw eventStr;
  }

  return stream;
}

std::string Event::toString( void ) const
{
  std::string res = "";
  char timeBuf[6];

  sprintf(timeBuf, "%02d:%02d\0", time.hour, time.minute);

  res += timeBuf;
  res += " " + std::to_string(id) + " " + strParam;

  if (iParam != -1)
    res += " " + std::to_string(iParam);

  return res;
}

std::ostream & operator <<( std::ostream &stream, const Event &e )
{
  std::ostream::sentry sentry(stream);

  if (!sentry)
    return stream;

  char timeBuf[6];

  sprintf(timeBuf, "%02d:%02d\0", e.time.hour, e.time.minute);

  stream << timeBuf << ' ' << e.id << ' ' << e.strParam;

  if (e.iParam != -1)
    stream << ' ' << e.iParam;

  return stream;
}

std::ostream & operator <<( std::ostream &stream, const Time &time )
{
  std::ostream::sentry sentry(stream);

  if (!sentry)
    return stream;

  char timeBuf[6];

  sprintf(timeBuf, "%02d:%02d\0", time.hour, time.minute);
  stream << timeBuf;
  return stream;
}
