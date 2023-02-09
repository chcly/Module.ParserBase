set(TargetName ParserBase)
include (GroupSet)

set_group(Target_SRC
    Header:*.h
    Source:*.cpp
    )


include_directories(${ParserBase_INCLUDE} ${Utils_INCLUDE})

add_library(${TargetName}
            ${Target_SRC}
            )

target_link_libraries(${TargetName} ${Utils_LIBRARY})

set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetGroup}")
