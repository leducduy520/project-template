FetchContent_Declare(
    SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    rapidcsv GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
    GIT_TAG v8.82
)
FetchContent_MakeAvailable(rapidcsv)

FetchContent_Declare(
    magic_enum GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.6
)
FetchContent_MakeAvailable(magic_enum)

# FetchContent_Declare(
# SQLiteCpp GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
# GIT_TAG 3.3.1
# )
# FetchContent_MakeAvailable(SQLiteCpp)
