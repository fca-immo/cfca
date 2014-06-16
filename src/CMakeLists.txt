
add_subdirectory(fca)

add_executable(../cfca cfca.c)
target_link_libraries(../cfca libcfca m)

add_executable(../gen-concepts gen-concepts.c)
target_link_libraries(../gen-concepts libcfca m pthread)

#add_executable(../termtrees-test test.c op_signature.c cli.c terms.c termtrees.c termtrees.h)

#target_link_libraries(../termtrees-test bstrlib confuse cprops hygra)
