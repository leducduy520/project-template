#include <PingPongGame.hpp>
#include <background.hpp>
#include <ball.hpp>
#include <brick.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <constants.hpp>
#include <filesystem>
#include <helper.hpp>
#include <memory>
#include <paddle.hpp>
#include <vector>
#include <DBClientGame.hpp>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif

// static std::filesystem::path getExecutablePath()
// {
// #ifdef _WIN32
//     char path[MAX_PATH];
//     GetModuleFileNameA(nullptr, path, MAX_PATH);
//     return std::filesystem::path(path).parent_path();
// #else
//     char path[PATH_MAX];
//     ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
//     return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path();
// #endif
// }

// #define EXECUTABLE_PATH getExecutablePath()

// std::string constants::resouces_path = (EXECUTABLE_PATH / ".." / "resources" / "").string();

extern bsoncxx::stdx::optional<string> get_database_uri();
extern bsoncxx::stdx::optional<string> get_database_name();
extern bsoncxx::stdx::optional<string> get_coll_name();

TEST_CASE("Test mongo db connection", "[mongo-connnection]")
{
    SECTION("s1")
    {
        REQUIRE(get_database_name());
    }
    SECTION("s2")
    {
        REQUIRE(get_coll_name());
    }
    SECTION("s3")
    {
        CHECK_NOTHROW(DBClient::GetInstance());
    }
    SECTION("s4")
    {
        CHECK_NOTHROW(DBClient::GetInstance()->GetDatabase(get_database_name().value().c_str()));
    }
    SECTION("s5")
    {
        CHECK_NOTHROW(DBClient::GetInstance()->GetDatabase(get_coll_name().value().c_str()));
    }
}

// TEST_CASE("Initializing entities", "[init]")
// {
//     UNSCOPED_INFO("resouces_path: " << constants::resouces_path + "wall.csv");

//     SECTION("i1")
//     {
//         std::unique_ptr<ball> a_ball;
//         CHECK_NOTHROW(a_ball = std::make_unique<ball>());
//     }

//     SECTION("i2")
//     {
//         std::unique_ptr<paddle> a_paddle;
//         CHECK_NOTHROW(a_paddle = std::make_unique<paddle>());
//     }

//     SECTION("i3")
//     {
//         std::unique_ptr<background> a_background;
//         CHECK_NOTHROW(a_background = std::make_unique<background>());
//     }

//     SECTION("i4")
//     {
//         CHECK_NOTHROW([&]() {
//             wall a_wall;
//             utilities::wallhelper::build_wall(a_wall, (constants::resouces_path + "wall.csv").c_str());
//         }());
//     }
// }

// TEST_CASE("Bechmark", "[!benchmark]")
// {
//     BENCHMARK("Bechmark creating a wall with benchmark.csv")
//     {
//         wall a_wall;
//         return utilities::wallhelper::build_wall(a_wall, (constants::resouces_path + "benchmark.csv").c_str());
//     };
// }
