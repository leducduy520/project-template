//cmake -S. -Bcmakebuild -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX="D:\mongo-c-driver" -DENABLE_MONGOC=ON
//cmake .. --fresh -G "Visual Studio 17 2022" -A x64 -DBOOST_ROOT="C:\Boost" -DCMAKE_PREFIX_PATH="D:\mongo-c-driver" -DCMAKE_CXX_STANDARD=17
#include "dbclientGame.hpp"

DBClient *DBClient::m_instance;
std::once_flag DBClient::m_flag;

DBClient::DBClient()
{
    // Connect to the MongoDB server
    const auto uri = mongocxx::uri{"mongodb+srv://duyleduc123:vfmFqkgYRXOzKaDI@cluster0.24ewqox.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"};
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);
    m_dbclient = std::move(mongocxx::client{uri, client_options});
    /*m_dbdatabase = m_dbclient[m_dbclient.list_database_names().front()];
    m_dbcollection = m_dbdatabase[m_dbdatabase.list_collection_names().front()];*/
}

DBClient *DBClient::GetInstance()
{
    std::call_once(m_flag, []() { m_instance = new DBClient(); });
    return m_instance;
}

void DBClient::DestroyInstance()
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
    if (m_dbdatabase.has_collection(name))
    {
        m_dbcollection = m_dbdatabase[name];
        return &m_dbcollection;
    }
    return nullptr;
}

bool DBClient::CreateCollection(const std::string &collectionName, mongocxx::database *db)
{
    if (!db)
    {
        m_dbcollection = m_dbdatabase.create_collection(collectionName);
        return true;
    }
    auto databases = m_dbclient.list_database_names();
    if (std::find(databases.begin(), databases.end(), db->name()) != databases.end())
    {
        db->create_collection(collectionName);
        m_dbcollection = db->collection(collectionName);
        return true;
    }
    return false;
}

bool DBClient::InsertDocument(const bsoncxx::document::value &document, mongocxx::collection *collection)
{
    if (!collection)
    {
        m_dbcollection.insert_one(document.view());
        return true;
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        collection->insert_one(document.view());
        return true;
    }
    return false;
}

bool DBClient::UpdateDocument(const bsoncxx::v_noabi::document::value &filter,
                              const bsoncxx::v_noabi::document::value &update,
                              mongocxx::collection *collection)
{
    if (!collection)
    {

        m_dbcollection.update_one(filter.view(), update.view());
        return true;
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        collection->update_one(filter.view(), update.view());
        return true;
    }
    return false;
}

optional<bsoncxx::v_noabi::document::value> DBClient::GetDocument(const bsoncxx::v_noabi::document::value &filter,
                                               mongocxx::collection *collection)
{
    if (!collection)
    {
        return m_dbcollection.find_one(filter.view());
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        return collection->find_one(filter.view());
    }
    return {};
}

bool DBClient::DeleteDocument(const bsoncxx::v_noabi::document::value& filter, mongocxx::collection *collection)
{
    if (!collection)
    {
        auto result = m_dbcollection.delete_one(filter.view());
        if (result)
            return true;
        return false;
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        auto result = collection->delete_one(filter.view());
        if (result)
            return true;
    }
    return false;
}

void DBClient::RunPipeLine(const mongocxx::pipeline pl, const mongocxx::options::aggregate opts, mongocxx::collection *collection)
{
    if (!collection)
    {
        auto cursor = m_dbcollection.aggregate(pl, opts);
        cout << "length:" << distance(cursor.begin(), cursor.end()) << endl;
        return;
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        auto cursor = collection->aggregate(pl, opts);
    }
}

void DBClient::testFunc()
{
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::close_document;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::open_document;
    
    mongocxx::v_noabi::pipeline pl;
    mongocxx::v_noabi::options::aggregate ag_opts;

    ag_opts.allow_disk_use(true).max_time(std::chrono::milliseconds{60000});

    pl
        .add_fields(make_document(kvp(
            "lastModified",
            (int64_t)chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count())))
        .add_fields(make_document(
            kvp("record",
                make_document(kvp("$slice",
                                  make_array(make_document(kvp("$sortArray",
                                                               make_document(kvp("input", "$history"),
                                                                             kvp("sortBy",
                                                                                 make_document(kvp("score", -1),
                                                                                               kvp("duration", 1),
                                                                                               kvp("live", -1),
                                                                                               kvp("id", 1)))))),
                                             5))))))
        .merge(make_document(kvp("into", make_document(kvp("db", "duyld"), kvp("coll", "pingpong_game")))));

}
