# This is a generated build-system-configuration file for 4diac forte
#
# To use the current directory-name as basis for the module-name use

forte_add_directory_module(gpiocontroller OFF "custom module")

forte_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

forte_add_sourcefile_hcpp(gpiocontroller_fbt gpio_1)

# If you want to set your own name independent of storage location you can use
# forte_add_module("YOUR_MODULE_NAME_HERE" <ON/OFF> "short description of your module")
# ON/OFF to specify if the module shall be added by default or not
# Additional parameters can be added to specify dependencies on other modules

# To specify the source-files to be included in this module you can use

# forte_add_all_sourcefiles_recursive()

# This will add all files named *.h *.cpp *.c in the current directory
# The order the files are added in is dependent on your file system.
#
# If for any reason you need more control you can use
# forte_add_sourcefile_hcpp, forte_add_sourcefile_cpp, forte_add_sourcefile_c
# forte_add_sourcefile_hc, ...
#
# For more information look at buildsupport/forte.cmake of the 4diac forte sourcecode
