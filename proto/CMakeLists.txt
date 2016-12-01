INCLUDE(FindProtobuf)
FIND_PACKAGE(Protobuf REQUIRED)
FIND_PACKAGE(LIBZIP REQUIRED)
FIND_PACKAGE(ZipIos REQUIRED)
#ZIPIOSCC_LIBRARY     
#ZIPIOSCC_DEFINITIONS 
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${ZIPIOSCC_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR} . ${CMAKE_CURRENT_BINARY_DIR} ${ZIPIOSCC_INCLUDE_DIR})

SET(ProMC_FILES
  ProMCBook
  )

ADD_SUBDIRECTORY(promc)
ADD_SUBDIRECTORY(pronlo)
ADD_SUBDIRECTORY(proreco)
ADD_SUBDIRECTORY(proeic)

#message("PROMC_BUILD_EIC = ${PROMC_BUILD_EIC}")
#message("PROMC_BUILD_NLO = ${PROMC_BUILD_NLO}")
#
#if( PROMC_BUILD_PROMC )
#
#  FILE(GLOB ProMC_ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/promc/*.proto")
#
#  PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${ProMC_ProtoFiles})
#  PROTOBUF_GENERATE_PYTHON(PROTO_PY ${ProMC_ProtoFiles})
#
#  SET(lib_VERSION "${${PROJECT_NAME}_VERSION}")
#  SET(lib_MAJOR_VERSION "${${PROJECT_NAME}_MAJOR_VERSION}")
#  SET(lib_LIBRARY_PROPERTIES 
#    VERSION "${lib_VERSION}"
#    SOVERSION "${lib_MAJOR_VERSION}"
#    SUFFIX ".so")
#  SET(libname promc)
#
#  SET(ProMC_FILES_SOURCE)
#  SET(ProMC_FILES_HEADER)
#  FOREACH(f ${ProMC_FILES})
#    #MESSAGE(" file : ${f}")
#    SET(ProMC_FILES_SOURCE "${ProMC_FILES_SOURCE} ${f}.cc")
#    SET(ProMC_FILES_HEADER "${ProMC_FILES_HEADER} ${f}.h")
#  ENDFOREACH()
#
#  ADD_LIBRARY(promc SHARED ${PROTO_HEADER} ${PROTO_SRC} ${ProMC_FILES})
#  TARGET_LINK_LIBRARIES(promc ${PROTOBUF_LIBRARIES})
#  SET_TARGET_PROPERTIES(${libname} PROPERTIES ${lib_LIBRARY_PROPERTIES})
#  #ADD_DEPENDENCIES(promc ${PROTO_HEADER} ${PROTO_SRC})
#
#  INSTALL( TARGETS ${libname} DESTINATION lib )
#  INSTALL( FILES ${PROTO_HEADER} DESTINATION include/${PROJECT_NAME} )
#
#  FIND_PROGRAM(PYTHON "python")
#  IF (PYTHON)
#    SET(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
#    SET(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
#    SET(DEPS        "${CMAKE_CURRENT_SOURCE_DIR}/promc/__init__.py" ${PROTO_PY})
#    SET(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")
#
#    CONFIGURE_FILE(${SETUP_PY_IN} ${SETUP_PY})
#
#    ADD_CUSTOM_COMMAND(OUTPUT ${OUTPUT}
#      COMMAND ${PYTHON} ${SETUP_PY} build
#      COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
#      DEPENDS ${DEPS})
#
#    ADD_CUSTOM_TARGET(target ALL DEPENDS ${OUTPUT})
#
#    INSTALL(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install --prefix=${CMAKE_INSTALL_PREFIX})")
#  ENDIF()
#ENDIF()

#target_link_libraries(${libname} ${LINK_LIBRARIES} ${needs_libs})
#set_target_properties(${libname} PROPERTIES ${lib_LIBRARY_PROPERTIES})

#
#ADD_CUSTOM_TARGET(${aname}_ROOTDICTS DEPENDS ${lib_SRCS} ${lib_HEADERS} ${lib_DICTIONARY_SRC} ${lib_DICTIONARY_HEADER})
#
#add_library(${libname} SHARED ${lib_SRCS})
#target_link_libraries(${libname} ${LINK_LIBRARIES} ${needs_libs})
#set_target_properties(${libname} PROPERTIES ${lib_LIBRARY_PROPERTIES})
#add_dependencies(${libname} ${needs_libs} ${aname}_ROOTDICTS)

#install(
#   TARGETS ${libname} 
#   #   EXPORT ${PROJECT_NAME}Targets
#   DESTINATION lib )
#
#if(PROJECT_USE_ROOT6)
#   install(
#   FILES ${lib_PCM_FILE} 
#   DESTINATION lib )
#endif(PROJECT_USE_ROOT6)
#
#file(GLOB template_headers ${PROJECT_SOURCE_DIR}/src/${aname}/include/*.hxx)
#SET(lib_HEADERS ${lib_HEADERS} ${template_headers})
#
#install(
#   FILES ${lib_HEADERS} 
#   DESTINATION include/${PROJECT_NAME} )
#
