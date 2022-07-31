message("----- CONFIGURING LOOSE QUADTREE -----")
add_library(loose_quadtree INTERFACE)
target_include_directories(loose_quadtree INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../third_party/loose_quadtree/src/include)
add_library(loose_quadtree::loose_quadtree ALIAS loose_quadtree)

