INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIR})
add_executable (nsldfttest NSLDFTTest.cpp ../NSLTest.cpp ../../CommonTest.cpp)

target_link_libraries(nsldfttest Qt5::Test)
target_link_libraries(nsldfttest ${GSL_LIBRARIES} ${GSL_CBLAS_LIBRARIES})

IF (FFTW3_FOUND)
	target_link_libraries(nsldfttest ${FFTW3_LIBRARIES})
ENDIF ()

target_link_libraries(nsldfttest labplot2lib)

add_test(NAME nsldfttest COMMAND nsldfttest)
