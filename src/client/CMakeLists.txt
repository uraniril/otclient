# client options
add_definitions(-DCLIENT)
option(BOT_PROTECTION "Enable bot protection" ON)
if(BOT_PROTECTION)
    add_definitions(-DBOT_PROTECTION)
    message(STATUS "Bot protection: ON")
else(BOT_PROTECTION)
    message(STATUS "Bot protection: OFF")
endif(BOT_PROTECTION)

set(client_SOURCES ${client_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/thing/text/animatedtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/animator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/type/container.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/creature/creature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/manager/creatures.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/effect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/game.cpp
    ${CMAKE_CURRENT_LIST_DIR}/manager/houses.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/item.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/type/itemtype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/map/lightview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/creature/localplayer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lua/luafunctions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lua/luavaluecasts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/map/map.cpp
    ${CMAKE_CURRENT_LIST_DIR}/manager/mapio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/map/mapview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/map/minimap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/missile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/creature/outfit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/painter/creaturepainter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/painter/mapviewpainter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/painter/tilepainter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/painter/thingpainter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/creature/player.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol/protocolgame.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol/protocolgameparse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol/protocolgamesend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/manager/shadermanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/manager/spritemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/text/statictext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/thing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing/type/thingtype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/manager/thingtypemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/map/tile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/manager/towns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uicreature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiitem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimapanchorlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiminimap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiprogressrect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uisprite.cpp
)

if(NOT MSVC)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/lua/luafunctions.cpp
                                PROPERTIES LANGUAGE CXX COMPILE_FLAGS "-g0 -Os")
endif()
