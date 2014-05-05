#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# ProtoScale Build File
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

SET(CMAKE_CXX_COMPILER clang++)
SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_FLAGS "-g -std=c++11 -Wall -Wextra -fcolor-diagnostics")

FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

IF(BISON_FOUND)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/MetaParser.cpp 
           ${CMAKE_SOURCE_DIR}/MetaParser.hpp
    COMMAND ${BISON_EXECUTABLE} 
      -p metayy 
      -d 
      -o ${CMAKE_SOURCE_DIR}/MetaParser.cpp 
      ${CMAKE_SOURCE_DIR}/MetaParser.y
    COMMENT "Generating Meta Language Parser"
  )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/ModelParser.cpp 
           ${CMAKE_SOURCE_DIR}/ModelParser.hpp
    COMMAND ${BISON_EXECUTABLE} 
      -p modelyy 
      -d 
      -o ${CMAKE_SOURCE_DIR}/ModelParser.cpp 
      ${CMAKE_SOURCE_DIR}/ModelParser.y
    COMMENT "Generating Model Language Parser"
  )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/CommandParser.cpp 
           ${CMAKE_SOURCE_DIR}/CommandParser.hpp
    COMMAND ${BISON_EXECUTABLE} 
      -p cmdyy 
      -d 
      -o ${CMAKE_SOURCE_DIR}/CommandParser.cpp 
      ${CMAKE_SOURCE_DIR}/CommandParser.y
    COMMENT "Generating Command Language Parser"
  )
ENDIF()

IF(FLEX_FOUND)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/MetaScanner.cpp 
           ${CMAKE_SOURCE_DIR}/MetaScanner.hpp
    COMMAND ${FLEX_EXECUTABLE} 
      -P metayy 
      --header-file=${CMAKE_SOURCE_DIR}/MetaScanner.hpp 
      -o ${CMAKE_SOURCE_DIR}/MetaScanner.cpp 
      ${CMAKE_SOURCE_DIR}/MetaScanner.l
    COMMENT "Generating Meta Language Scanner"
  )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/ModelScanner.cpp 
           ${CMAKE_SOURCE_DIR}/ModelScanner.hpp
    COMMAND ${FLEX_EXECUTABLE} 
      -P modelyy 
      --header-file=${CMAKE_SOURCE_DIR}/ModelScanner.hpp 
      -o ${CMAKE_SOURCE_DIR}/ModelScanner.cpp 
      ${CMAKE_SOURCE_DIR}/ModelScanner.l
    COMMENT "Generating Model Language Scanner"
  )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/CommandScanner.cpp 
           ${CMAKE_SOURCE_DIR}/CommandScanner.hpp
    COMMAND ${FLEX_EXECUTABLE} 
      -P cmdyy 
      --header-file=${CMAKE_SOURCE_DIR}/CommandScanner.hpp 
      -o ${CMAKE_SOURCE_DIR}/CommandScanner.cpp 
      ${CMAKE_SOURCE_DIR}/CommandScanner.l
    COMMENT "Generating Meta Language Scanner"
  )
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(meta_lang_sources 
  MetaScanner.cpp MetaScanner.hpp MetaParser.cpp MetaParser.hpp
  MetaAST.cpp MetaAST.hpp
)

SET(model_lang_sources
  ModelScanner.cpp ModelScanner.hpp ModelParser.cpp ModelParser.hpp
  ModelAST.cpp ModelAST.hpp
)

set(command_lang_sources
  CommandScanner.cpp CommandScanner.hpp CommandParser.cpp CommandParser.hpp
  CommandAST.cpp CommandAST.hpp
)

set(protoc_core_sources
  FileUtil.cpp FileUtil.hpp
  SemaVisitor.cpp SemaVisitor.hpp
  CppGen.cpp CppGen.hpp
)

SET_SOURCE_FILES_PROPERTIES(
  MetaScanner.cpp PROPERTIES COMPILE_FLAGS 
  "-Wno-deprecated-register -Wno-sign-compare -Wno-unused-function"
)
SET_SOURCE_FILES_PROPERTIES(
  ModelScanner.cpp PROPERTIES COMPILE_FLAGS 
  "-Wno-deprecated-register -Wno-sign-compare -Wno-unused-function"
)
SET_SOURCE_FILES_PROPERTIES(
  CommandScanner.cpp PROPERTIES COMPILE_FLAGS 
  "-Wno-deprecated-register -Wno-sign-compare -Wno-unused-function"
)
 
SET(lib_protoc_sources ${meta_lang_sources}
                       ${model_lang_sources}
                       ${command_lang_sources}
                       ${protoc_core_sources})

ADD_LIBRARY(protoc SHARED ${lib_protoc_sources})

ADD_EXECUTABLE(proto Driver.cpp Driver.hpp Diagnostics.hpp)
TARGET_LINK_LIBRARIES(proto protoc)

