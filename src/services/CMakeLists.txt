target_sources(app PRIVATE
    system_manager.cpp
    settings_storage.cpp)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
