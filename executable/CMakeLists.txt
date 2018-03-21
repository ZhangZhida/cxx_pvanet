include_directories(/usr/include/freetype2)
add_executable(pvademo pvademo.cpp ../src/OpenCVputTextChinese/OpencvText.cpp)
target_link_libraries(pvademo myfUtils ${OpenCV_LIBS} freetype pva tools caffe glog boost_system )

add_executable(saveIntermediateResult saveIntermediateResult.cpp)
target_link_libraries(saveIntermediateResult myfUtils ${OpenCV_LIBS} pva tools caffe glog boost_system )

add_executable(detectPipeLine detectPipeLine.cpp)
target_link_libraries(detectPipeLine myfUtils ${OpenCV_LIBS} pva tools caffe glog boost_system )

add_executable(pvademo_full pvademo_full.cpp)
target_link_libraries(pvademo_full myfUtils ${OpenCV_LIBS} pva tools caffe glog boost_system )