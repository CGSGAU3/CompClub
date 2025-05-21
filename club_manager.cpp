#include <cmath>
#include <cctype>
#include <fstream>
#include <queue>
#include <map>

#include "club_manager.h"

ClubManager::ClubManager( const std::string &fileName )
{
  std::ifstream file(fileName);
  std::string str;
  int tablesCnt;

  if (!file)
    throw std::runtime_error("Can't open the file " + fileName + "!");

  if (!file)
    throw std::runtime_error("Unexpected end of file!");

  try
  {
    // Parse num of tables
    std::getline(file, str);
    if (!checkInteger(str, tablesCnt))
      throw str;

    tables.resize(tablesCnt, {});

    if (!file)
      throw std::runtime_error("Unexpected end of file!");

    // Parse working hours
    std::getline(file, str);

    std::vector<std::string> workHours = split(str, ' ');

    if (workHours.size() != 2)
      throw str;

    start = Time(workHours[0]);
    end = Time(workHours[1]);

    if (!file)
      throw std::runtime_error("Unexpected end of file!");

    // Parse hour price
    std::getline(file, str);
    if (!checkInteger(str, hourPrice))
      throw str;

    if (!file)
      throw std::runtime_error("Unexpected end of file!");
  }
  catch ( const std::string &errStr )
  {
    throw errStr;
  }
  catch ( const std::invalid_argument & )
  {
    throw str;
  }
  catch ( const std::exception &err )
  {
    throw err;
  }

  // Parse events
  while (file)
  {
    try
    {
      Event e;

      file >> e;

      if (file)
      {
        // Check clients' name - must be only a..z, 0-9, - and _
        for (const auto &ch : e.strParam)
        {
          bool pass = islower((unsigned char)ch) ||
                      isdigit((unsigned char)ch) ||
                      ch == '-' || ch == '_';

          if (!pass)
            throw e.toString();
        }

        // Check id's - must be 1-4
        if (e.id < 1 || e.id > 4)
          throw e.toString();

        // Add event to "event queue"
        events.push_back(e);
      }
    }
    catch ( const std::string &errStr )
    {
      throw errStr;
    }
  }

  // Another format checks
  Event prev = events[0];

  // Check for the first event
  if (prev.iParam != -1)
  {
    // iParam is always number of a table
    if (prev.iParam > (int)tables.size())
      throw prev.toString();
  }

  for (int i = 1; i < (int)events.size(); i++)
  {
    // Check if events go after each other
    if (events[i].time < prev.time)
      throw events[i].toString();

    // Check if table numbers are legal
    if (events[i].iParam != -1)
    {
      if (events[i].iParam > (int)tables.size())
        throw prev.toString();
    }

    // Check again...
    prev = events[i];
  }
}

void ClubManager::sendMessage( const Time &time, int id, const std::string &strParam, int iParam = -1 ) const
{
  std::cout << Event(time.hour, time.minute, id, strParam, iParam) << std::endl;
}

void ClubManager::work( void )
{
  // Constants
  const int ID_LEFT = 11,
            ID_ATTABLE = 12,
            ID_ERROR = 13;

  // Clients queue in club but not at the table
  std::queue<std::string> clientsQueue;

  // Client state structure
  struct State
  {
    int tableNum = -1; // Table he/she currently at - if no table then it's -1
    Time startTime;   // Time when he/she start to sit at the table
  };

  // Client state: 0 if no at the table, tableNumber in another case
  std::map<std::string, State> clientState;

  std::cout << start << std::endl;
  for (const auto &e : events)
  {
    std::string clientName = e.strParam;
    int tableNum = e.iParam - 1;
    bool haveFree;

    std::cout << e << std::endl;

    switch (e.id)
    {
    case 1: // client has come

      // Client is already in
      if (clientState.find(clientName) != clientState.end())
      {
        sendMessage(e.time, ID_ERROR, "YouShallNotPass");
        break;
      }

      // Client has come not in working hours
      if (e.time < start || e.time > end)
      {
        sendMessage(e.time, ID_ERROR, "NotOpenYet");
        break;
      }

      clientState.insert({clientName, {}});
      break;
    case 2: // client has sit at the table

      // Client not in the club
      if (clientState.find(clientName) == clientState.end())
      {
        sendMessage(e.time, ID_ERROR, "ClientUnknown");
        break;
      }

      // Table is busy
      if (tables[tableNum].client != "")
      {
        sendMessage(e.time, ID_ERROR, "PlaceIsBusy");
        break;
      }

      // Take profit if there is change table
      if (clientState[clientName].tableNum != -1)
      {
        int timeBusy = e.time - clientState[clientName].startTime;
        int hoursSpent = (int)ceil(timeBusy / 60.0);

        tables[clientState[clientName].tableNum].profit += hoursSpent * hourPrice;
        tables[clientState[clientName].tableNum].timeBusy += timeBusy;
      }

      // Sit at the table
      tables[tableNum].client = clientName;
      clientState[clientName].startTime = e.time;
      clientState[clientName].tableNum = tableNum;
      break;
    case 3: // client is waiting

      // Check free tables
      haveFree = false;

      for (const auto &tbl : tables)
      {
        if (tbl.client == "")
        {
          haveFree = true;
          break;
        }
      }
      if (haveFree)
      {
        sendMessage(e.time, ID_ERROR, "ICanWaitNoLonger!");
        break;
      }

      // If queue is overflow then client is left
      if (clientsQueue.size() > tables.size())
      {
        sendMessage(e.time, ID_LEFT, clientName);
        break;
      }

      clientsQueue.push(clientName);
      break;
    case 4: // client has left

      tableNum = clientState[clientName].tableNum;

      // Client not in the club
      if (clientState.find(clientName) == clientState.end())
      {
        sendMessage(e.time, ID_ERROR, "ClientUnknown");
        break;
      }

      // Take profit if need
      if (clientState[clientName].tableNum != -1)
      {
        int timeBusy = e.time - clientState[clientName].startTime;
        int hoursSpent = (int)ceil(timeBusy / 60.0);

        tables[tableNum].profit += hoursSpent * hourPrice;
        tables[tableNum].timeBusy += timeBusy;
      }

      // Left the table
      tables[tableNum].client = "";
      clientState.erase(clientName);

      // Take client from the queue
      if (clientsQueue.size() != 0)
      {
        tables[tableNum].client = clientsQueue.front();
        clientState[clientsQueue.front()].tableNum = tableNum;
        clientState[clientsQueue.front()].startTime = e.time;

        sendMessage(e.time, ID_ATTABLE, clientsQueue.front(), tableNum + 1);
        clientsQueue.pop();
      }

      break;
    default: // no way that there is an error, but...
      throw std::runtime_error("Unknown event id!");
    }
  }

  // Talk estimated clients to go away
  for (const auto &client : clientState)
  {
    sendMessage(end, ID_LEFT, client.first);

    // Take profit if need
    if (clientState[client.first].tableNum != -1)
    {
      int timeBusy = end - clientState[client.first].startTime;
      int hoursSpent = (int)ceil(timeBusy / 60.0);

      tables[clientState[client.first].tableNum].profit += hoursSpent * hourPrice;
      tables[clientState[client.first].tableNum].timeBusy += timeBusy;

      // Clear the table
      tables[clientState[client.first].tableNum].client = "";
    }
  }
  clientState.clear();

  std::cout << end << std::endl;

  // Profit count
  for (int i = 0; i < (int)tables.size(); i++)
  {
    int hours = tables[i].timeBusy / 60;
    int minutes = tables[i].timeBusy % 60;

    printf("%d %d %02d:%02d\n", i + 1, tables[i].profit, hours, minutes);
  }
}
