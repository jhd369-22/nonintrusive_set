# define options for enabling the Address Sanitizer(ASan) and
# the Undefined Behavior Sanitizer(UBSan).
# Note: To enable the option in CLI,
# use "-D ENABLE_ASAN=true -D ENABLE_UBSAN=true" when generate the native build files
# e.g. "cmake -S . -B tmp -D ENABLE_ASAN=true -D ENABLE_UBSAN=true -D ENABLE_MEMORY=true -D ENABLE_THREAD=true"
option(ENABLE_ASAN false)
option(ENABLE_UBSAN false)
option(ENABLE_MEMORY false)
option(ENABLE_THREAD false)

if(ENABLE_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_UBSAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_MEMORY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fno-omit-frame-pointer")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=memory")
endif()

if(ENABLE_THREAD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
endif()