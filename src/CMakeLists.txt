add_library(sirit
    ../include/sirit/sirit.h
    sirit.cpp
    op.cpp
    op.h
    stream.cpp
    stream.h
    operand.cpp
    operand.h
    literal_number.cpp
    literal_number.h
    literal_string.cpp
    literal_string.h
    common_types.h
    insts/type.cpp
    insts/constant.cpp
    insts/function.cpp
    insts/flow.cpp
    insts/debug.cpp
    insts/memory.cpp
    insts/annotation.cpp
    insts/misc.cpp
    insts/logical.cpp
    insts/conversion.cpp
    insts/bit.cpp
    insts/arithmetic.cpp
    insts/extension.cpp
)
target_include_directories(sirit
                           PUBLIC ../include
                           PRIVATE . ${SPIRV-Headers_SOURCE_DIR}/include
                           INTERFACE ${SPIRV-Headers_SOURCE_DIR}/include)
