
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// 100-Fix-Section.cpp

// Catch has two ways to express fixtures:
// - Sections (this file)
// - Traditional class-based fixtures

// main() provided by linkage to Catch2WithMain

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

// std::string constants::resoucesPath = (EXECUTABLE_PATH / ".." / "resources" / "").string();

TEST_CASE("Test", "[test]")
{
    CHECK(true);
}

// TEST_CASE("Initializing entities", "[init]")
// {
//     UNSCOPED_INFO("resoucesPath: " << constants::resoucesPath + "wall.csv");

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
//             utilities::wallhelper::createWall(a_wall, (constants::resoucesPath + "wall.csv").c_str());
//         }());
//     }
// }

// TEST_CASE("Bechmark", "[!benchmark]")
// {
//     BENCHMARK("Bechmark creating a wall with benchmark.csv")
//     {
//         wall a_wall;
//         return utilities::wallhelper::createWall(a_wall, (constants::resoucesPath + "benchmark.csv").c_str());
//     };
// }
