#ifndef __DBCLIENT_GAME__
#define __DBCLIENT_GAME__

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mutex>
#include <vector>
#include <string>
#include <bsoncxx/builder/stream/document.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using namespace std;

class DBClient
{
    mongocxx::client m_dbclient;
    mongocxx::instance m_dbinstance;
    mongocxx::database m_dbdatabase;
    mongocxx::collection m_dbcollection;
    static DBClient *m_instance;
    static std::once_flag m_flag;
    DBClient();

public:
    static DBClient *GetInstance();
    static void destroyInstance();
    const mongocxx::database* GetDatabase(const char* name);
    const mongocxx::collection* GetCollection(const char* name);
    void createCollection(const std::string& collectionName, mongocxx::database* db = nullptr);
    bsoncxx::document::value createDocument(const vector<pair<string, string>>& keyValues);
    void insertDocument(const bsoncxx::document::value& document, mongocxx::collection* collection = nullptr);
};

#endif  /*  __DBCLIENT_GAME__ */
