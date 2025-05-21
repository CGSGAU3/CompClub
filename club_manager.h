#ifndef _CLUB_MANAGER_H
#define _CLUB_MANAGER_H

#include <string>
#include <vector>

#include "addon.h"

/* Struct to represent table */
struct Table
{
  std::string client; // If no client then it's ""
  int profit = 0;     // Daily profit
  int timeBusy = 0;   // Time in minutes when the table was busy
};

/* Main class */
class ClubManager
{
private:

  Time start, end;           // Work hours
  int hourPrice;             // Hour price like in config
  std::vector<Table> tables; // Tables' state
  std::vector<Event> events; // List of all events

  void sendMessage( const Time &time, int id, const std::string &strParam, int iParam ) const;

public:

  ClubManager( const std::string &fileName );

  void work( void );
};

#endif // _CLUB_MANAGER_H

