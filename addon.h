#ifndef _ADDON_H
#define _ADDON_H

#include <iostream>
#include <vector>

bool checkInteger( const std::string &str, int &data );
std::vector<std::string> split( const std::string &str, char delim );

/* Auxillary structure to parse time */
class Time
{
public:
  int hour;
  int minute;

  Time( int h = 0, int m = 0 ) : hour(h), minute(m)
  {
  }

  Time( const std::string &timeStr );

  bool operator <( const Time &other ) const;
  bool operator >( const Time &other ) const;
  bool operator ==( const Time &other ) const;
  bool operator <=( const Time &other ) const;
  bool operator >=( const Time &other ) const;

  // Elapsed time in minutes
  int operator -( const Time &other ) const;
};

/* Class to manage events */
class Event
{
public:

  Time time;             // Time of an event
  int id;                // Identificator of and event
  std::string strParam;  // Body of an event - first parameter
  int iParam;            // Second parameter - optional (-1 if no any)

  Event( void ) = default;

  Event( int hour_, int minute_, int id_, const std::string &param1, int param2 = -1 ) : 
    time(hour_, minute_), id(id_), strParam(param1), iParam(param2)
  {
  }

  std::string toString( void ) const;
};

std::istream & operator >>( std::istream &stream, Event &e );
std::ostream & operator <<( std::ostream &stream, const Event &e );
std::ostream & operator <<( std::ostream &stream, const Time &time );

#endif // _ADDON_H
