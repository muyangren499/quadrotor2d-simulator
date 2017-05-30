cmake_minimum_required (VERSION 2.8.1)
set(CMAKE_CXX_STANDARD 11)
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
cmake_policy(SET CMP0015 NEW) #for relative paths in link_directories

project (Quadrotor2DSimulator)

#set(BOX2D_BUILD_STATIC ON)
add_subdirectory("Box2D/Box2D")
include_directories(Box2D/Box2D)

#add_definitions("-DBUILD_CPP_EMPTY_TEST=OFF -DBUILD_CPP_TESTS=OFF -DBUILD_LUA_LIBS=OFF -DBUILD_JS_LIBS=OFF")
#set(BUILD_JS_LIBS OFF)
set(BUILD_CPP_EMPTY_TEST OFF CACHE BOOL "turn off cocos2d-x cpp empty test" FORCE)
set(BUILD_CPP_TESTS OFF CACHE BOOL "turn off cocos2d-x cpp tests" FORCE)
set(BUILD_LUA_LIBS OFF CACHE BOOL "turn off cocos2d-x lua libs" FORCE)
set(BUILD_JS_LIBS OFF CACHE BOOL "turn off cocos2d-x js libs" FORCE)
add_subdirectory("cocos2d-x")
include_directories(cocos2d-x/cocos)

add_subdirectory("uWebSockets")
include_directories(uWebSockets/src)

include_directories(rapidjson/include)

set(PLATFORM_SPECIFIC_LIBS)
if(UNIX AND NOT(MACOSX OR APPLE))
    set(LINUX true)
    add_definitions(-DLINUX)

    set(PLATFORM_SPECIFIC_LIBS
    glfw
    pthread
    X11
    GL
    GLEW
    fontconfig
    )

#  foreach(_pkg OPENGL GLEW GLFW3 FMOD FONTCONFIG THREADS GTK3)
#    cocos_use_pkg(cocos2dInternal ${_pkg})
#  endforeach()
elseif(MACOSX OR APPLE)
#  cocos_use_pkg(cocos2dInternal GLFW3)

 INCLUDE_DIRECTORIES ( /System/Library/Frameworks )

 link_directories (
  /usr/local/lib
 )

 FIND_LIBRARY(COCOA_LIBRARY Cocoa)
 FIND_LIBRARY(OPENGL_LIBRARY OpenGL )
 FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
 FIND_LIBRARY(OPENAL_LIBRARY OpenAL )
 FIND_LIBRARY(QUARTZCORE_LIBRARY QuartzCore )
 FIND_LIBRARY(APPLICATIONSERVICES_LIBRARY ApplicationServices)
 FIND_LIBRARY(IOKIT_LIBRARY IOKit)
 FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
 FIND_LIBRARY(ICONV_LIBRARY iconv)

  set(PLATFORM_SPECIFIC_LIBS
    ${COCOA_LIBRARY}
    ${OPENGL_LIBRARY}
    ${OPENAL_LIBRARY}
    ${AUDIOTOOLBOX_LIBRARY}
    ${QUARTZCORE_LIBRARY}
    ${APPLICATIONSERVICES_LIBRARY}
    ${IOKIT_LIBRARY}
    ${FOUNDATION_LIBRARY}
    ${ICONV_LIBRARY}
    glfw3
    iconv
  )

else()
  message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()


#link_directories (
#../../../installdir/lib
#../../../builddir/cocos2d-x/builddir/lib
#)

include_directories(
#../../../installdir/include
#../../../Quadrocopter-Box2D/Quadrocopter-Box2D/Quadrocopter-Box2D
#../../../Quadrocopter2DBrain/Quadrocopter2DBrain
#../../../Quadrocopter-Visualizer/Classes
#../../..//builddir/cocos2d-x/cocos
/usr/local/include/GLFW
/usr/include/GLFW
src/box2d-classes
src/brain-api
src/
)

add_executable(Quadrotor2DSimulator
src/box2d-classes/Obstacle1D.cpp
src/box2d-classes/Obstacle2D.cpp
src/box2d-classes/ObstacleModelIFace.cpp
src/box2d-classes/Quadrocopter1D.cpp
src/box2d-classes/Quadrocopter2D.cpp
src/box2d-classes/QuadrocopterModelIFace.cpp
src/box2d-classes/Simulation.cpp
src/box2d-classes/World.cpp
src/box2d-classes/WorldBase.cpp
src/box2d-classes/WorldObjectInfo.cpp
src/brain-api/Quadrocopter2DBrain.cpp
src/brain-api/RLServerClient.cpp
src/AppDelegate.cpp
src/ConsumerProducerSyncronizer.cpp
src/Lib.cpp
src/ObstacleDiscrete1D.cpp
src/ObstacleDiscrete2D.cpp
src/PID.cpp
src/Quadrocopter2DContActionCtrl.cpp
src/Quadrocopter2DContActionPIDCtrl.cpp
src/Quadrocopter2DContActionPIDLSTMCtrl.cpp
src/Quadrocopter2DContActionLSTMCtrl.cpp
src/Quadrocopter2DContActionLSTMWeakCtrl.cpp
src/Quadrocopter2DContActionMLPSeqCtrl.cpp
src/Quadrocopter2DCtrl.cpp
src/QuadrocopterContActionCtrl.cpp
src/QuadrocopterCtrl.cpp
src/QuadrocopterDiscrete1D.cpp
src/QuadrocopterDiscrete2D.cpp
src/QuadrocopterDiscrete2DCtrl.cpp
src/QuadrocopterDiscreteCtrl.cpp
src/QuadrocopterSimulator.cpp
src/Visualizer1DScene.cpp
src/Visualizer2DScene.cpp
src/WorldDiscrete.cpp
src/main.cpp
)

target_link_libraries(Quadrotor2DSimulator
uWS
Box2D
cocos2d
cocos2dInternal
bullet
flatbuffers
recast
tinyxml2
unzip
xxhash
freetype
tiff
Box2D
webp
z
png
jpeg
pthread
ssl
crypto
uv
${PLATFORM_SPECIFIC_LIBS}
)

install(TARGETS Quadrotor2DSimulator
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
