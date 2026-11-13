cmake_minimum_required(VERSION 3.2)

project(nn_postprocess)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER "gcc" )
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99 -O3")

set(viplite "VIPLiteDrv_2.0.0")
set (prj_root "${CMAKE_CURRENT_SOURCE_DIR}")

list(
	APPEND src
	${prj_root}/src/main.c
	${prj_root}/src/model_yolo_sim.c
	${prj_root}/src/model_scrfd_sim.c
	${prj_root}/src/img_render.c
	${prj_root}/src/nbg_reader.c

	${prj_root}/../nn_utils/class_name.c
	${prj_root}/../nn_utils/iou.c
	${prj_root}/../nn_utils/nms.c
	${prj_root}/../nn_utils/quantize.c
	${prj_root}/../nn_utils/sigmoid.c
	${prj_root}/../nn_utils/tensor.c
)

list(
	APPEND inc_path
    ${prj_root}/..
    ${prj_root}/../nn_utils
	${prj_root}/../nn_mmf

	${prj_root}/../image/3rdparty/stb

	${prj_root}/lib
	${prj_root}/lib/NBGParser
)

list(
	APPEND flags
	PC_SIMULATION=1
	__fp16=float #TBD
)

ADD_LIBRARY (libNBGParser STATIC IMPORTED )
SET_PROPERTY ( TARGET libNBGParser PROPERTY IMPORTED_LOCATION ${prj_root}/lib/NBGParser/libNBGParser.a )

add_executable(nn_postprocess ${src})
target_compile_definitions(nn_postprocess PRIVATE ${flags})
target_include_directories(nn_postprocess PUBLIC ${inc_path})
target_link_libraries(nn_postprocess PUBLIC libNBGParser m c gcc)
