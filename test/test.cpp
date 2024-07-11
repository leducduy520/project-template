
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

#include <ball.hpp>
#include <paddle.hpp>
#include <background.hpp>
#include <brick.hpp>
#include <wallHelper.hpp>
#include <constants.hpp>
#include <PingPongGame.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <memory>
#include <filesystem>
#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <linux/limits.h>
#endif

static std::filesystem::path getExecutablePath() {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path();
#endif
}

#define EXECUTABLE_PATH getExecutablePath()

std::string constants::resoucesPath;

TEST_CASE("Initializing entities", "[single-file]")
{
    constants::resoucesPath = (EXECUTABLE_PATH / ".." / "resources" / "").string();
    UNSCOPED_INFO("LOG" << constants::resoucesPath + "wall.csv");

    CHECK_NOTHROW([&](){
        auto entity = std::make_unique<ball>();
    }());

    CHECK_NOTHROW([&](){
        auto entity = std::make_unique<paddle>();
    }());

    CHECK_NOTHROW([&](){
        auto entity = std::make_unique<background>();
    }());

    CHECK_NOTHROW([&](){
        auto entity = std::make_unique<brick>();
    }());

    CHECK_NOTHROW([&](){
        wall a_wall;
        wall_utils::createWall(a_wall, (constants::resoucesPath + "wall.csv").c_str());
    }());
}
