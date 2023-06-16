set(NVSDKDIR "${CMAKE_CURRENT_LIST_DIR}/nvidia_video_codec_sdk")

add_library(
  nvdecoder STATIC
  ${NVSDKDIR}/Samples/NvCodec/NvDecoder/NvDecoder.cpp
  ${NVSDKDIR}//Samples/Utils/ColorSpace.cu
)

ignore_third_party(nvdecoder)

target_include_directories(
  nvdecoder SYSTEM
  PUBLIC
    ${NVSDKDIR}/Samples/NvCodec
    ${NVSDKDIR}/Samples/Utils
    ${NVSDKDIR}/Samples
    ${NVSDKDIR}/
)

target_link_libraries(
  nvdecoder
  ffmpeg::libavcodec
  ffmpeg::libavformat
  CUDA::cuda_driver
  ${CUVID_LIB}
)

add_library(nvidia::nvdecoder ALIAS nvdecoder)

add_executable(
  appdecgl
  ${NVSDKDIR}/Samples/AppDecode/AppDecGL/AppDecGL.cpp
  ${NVSDKDIR}/Samples/AppDecode/AppDecGL/FramePresenterGLX.cpp
)

ignore_third_party(appdecgl)

target_link_libraries(appdecgl PRIVATE nvidia::nvdecoder GLEW ${GL_LIB})
