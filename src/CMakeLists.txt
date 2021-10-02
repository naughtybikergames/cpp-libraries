project(hello)

target_sources(${PROJECT_NAME}
    PRIVATE
        helloworld.cpp
)

#Subdirectories
#add_subdirectory(<some-other-subdirectory>)

#Deeper subdirectories for sources. Used for submodules.
#Subdirectories inherit the parent's variables and lists.

#Include more subdirectories if needed.