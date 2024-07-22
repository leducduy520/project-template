//cmake -S. -Bcmakebuild -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX="D:\mongo-c-driver" -DENABLE_MONGOC=ON
//cmake .. --fresh -G "Visual Studio 17 2022" -A x64 -DBOOST_ROOT="C:\Boost" -DCMAKE_PREFIX_PATH="D:\mongo-c-driver" -DCMAKE_CXX_STANDARD=17
#include "dbclientGame.hpp"

DBClient *DBClient::m_instance;
std::once_flag DBClient::m_flag;

DBClient::DBClient() {
    // Connect to the MongoDB server
    const auto uri = mongocxx::uri{ "mongodb+srv://duyleduc123:vfmFqkgYRXOzKaDI@cluster0.24ewqox.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0" };
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
    client_options.server_api_opts(api);
    m_dbclient = std::move(mongocxx::client { uri, client_options });
    m_dbdatabase = m_dbclient[m_dbclient.list_database_names().front()];
    m_dbcollection = m_dbdatabase[m_dbdatabase.list_collection_names().front()];
}

DBClient *DBClient::GetInstance()
{
    std::call_once(m_flag, []() {
        m_instance = new DBClient();
    });
    return m_instance;
}

void DBClient::destroyInstance()
{
    delete m_instance;
    m_instance = nullptr;
}

const mongocxx::database *DBClient::GetDatabase(const char *name)
{
    m_dbdatabase = m_dbclient[name];
    return &m_dbdatabase;
}

const mongocxx::collection *DBClient::GetCollection(const char *name)
{
    m_dbcollection = m_dbdatabase[name];
    return &m_dbcollection;
}

void DBClient::createCollection(const std::string &collectionName, mongocxx::database *db)
{
    if(!db)
    {
        m_dbdatabase.create_collection(collectionName);
        return;
    }
    auto databases = m_dbclient.list_database_names();
    if(std::find(databases.begin(), databases.end(), db->name())!= databases.end())
    {
        db->create_collection(collectionName);
    }
}

bsoncxx::document::value DBClient::createDocument(const vector<pair<string, string>> &keyValues)
{
    bsoncxx::builder::stream::document document{};
	for (auto& keyValue : keyValues)
	{
		document << keyValue.first << keyValue.second;
	}
	return document << bsoncxx::builder::stream::finalize;
}

void DBClient::insertDocument(const bsoncxx::document::value &document, mongocxx::collection *collection)
{
    if(!collection)
    {
        m_dbcollection.insert_one(document.view());
        return;
    }
    if(m_dbdatabase.has_collection(collection->name()))
    {
        collection->insert_one(document.view());
    }
}
