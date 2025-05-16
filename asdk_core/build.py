#!/usr/bin/env python3

from __future__ import print_function
import os
import sys
import shutil
import platform
import argparse

# global variables

# environment variable names

g_env_asdk_cmake_root = "ASDK_CMAKE_ROOT"
g_env_asdk_ninja_root = "ASDK_NINJA_ROOT"

# initialized variables

g_os_type = platform.system().lower()
g_os_env = os.environ
g_current_src_path = os.path.dirname(os.path.abspath(sys.argv[0]))
g_asdk_install_script = os.path.join(
    g_current_src_path, "asdk_core", "setup", "setup.py")
g_arm_toolchain_dir = os.path.join(
    g_current_src_path, "cmake", "devel_arm_eabi_toolchain.cmake")
g_rtos = "uCOS3"
g_build_choices = ["release", "debug"]

# default variables, gets overriden later

g_cmake = str()
g_ninja = str()
g_users_build_dir = os.path.join(g_current_src_path, "build")

g_parsed_args = object()
g_arg_parser = object()
g_mcu_choices = [
    "cyt2b75_m0plus", 
    "cyt2b75_m4", 
    "stm32f4",
]

# build command templates

g_cmake_args = [
    "{0}",
    "-S .",
    "-B {1}",
    "-G Ninja",
    "-DCMAKE_MAKE_PROGRAM={2}",
    "-DCMAKE_TOOLCHAIN_FILE={3}",
    "-DTARGET_PLATFORM={4}",
    "-DCMAKE_BUILD_TYPE={5}",
    "-DTARGET_RTOS={6}",
]

def _parse_args():
    global g_parsed_args
    global g_arg_parser
    global g_users_build_dir

    # add cli arguments
    g_arg_parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter)

    # command: build.py --platform <mcu> [--build <build_dir>] [--clean] --type <build_type>
    # command: build.py -p <mcu> [-b <build_dir>] [--clean] -t <build_type>

    required_args = g_arg_parser.add_argument_group('required named arguments')

    # required arguments

    required_args.add_argument("-p", "--platform",
                              help="Target microcontroller\nAllowed values: [" +
                              ", ".join(g_mcu_choices) + "]",
                              metavar="",
                              choices=g_mcu_choices,
                              required=True)
    
    required_args.add_argument("-t", "--type",
                            help="Build type\nAllowed values: [" +
                            ", ".join(g_build_choices) + "]",
                            metavar="",
                            choices=g_build_choices,
                            required=True)
    
    # optional arguments

    g_arg_parser.add_argument("-b", "--build",
                              help="Output build artifacts within specified directory\nDefault: Current working directory",
                              metavar="",
                              required=False)

    g_arg_parser.add_argument("-c", "--clean",
                              help="Do a clean build",
                              action="store_true",
                              required=False,
                              default=False)

    if (len((sys.argv)) == 1):
        g_arg_parser.print_help()
        sys.exit(1)
    else:
        g_parsed_args = g_arg_parser.parse_args()

    if (g_parsed_args.build is not None):
        g_users_build_dir = os.path.join(g_parsed_args.build, g_parsed_args.type.lower())
    else:
        g_users_build_dir = os.path.join(g_users_build_dir, g_parsed_args.type.lower())



def _set_asdk_env():
    global g_cmake
    global g_ninja

    cmake_exists = False
    ninja_exists = False

    # check if cmake is available
    if (g_env_asdk_cmake_root in g_os_env):
        g_cmake = os.path.join(g_os_env[g_env_asdk_cmake_root], "bin", "cmake")
        if (g_os_type == "windows"):
            g_cmake = g_cmake + ".exe"
        if not os.path.exists(g_cmake):
            print("Error: ASDK-CMake is not available.")
            print("\nRun 'python {0}' to install it.".format(
                g_asdk_install_script))
            sys.exit(1)
        else:
            cmake_exists = True

    # check if ninja is available
    if (g_env_asdk_ninja_root in g_os_env):
        g_ninja = os.path.join(g_os_env[g_env_asdk_ninja_root], "ninja")
        if (g_os_type == "windows"):
            g_ninja = g_ninja + ".exe"
        if not os.path.exists(g_ninja):
            print("Error: ASDK-ninja is not available.")
            print("\nRun 'python {0}' to install it.".format(
                g_asdk_install_script))
            sys.exit(1)
        else:
            ninja_exists = True

    # ASDK build requirements are not met
    if ((ninja_exists and cmake_exists) == False):
        print("ASDK environment is not avaiable on your system.")
        print("\nRun 'python {0}' to set it up.".format(g_asdk_install_script))
        sys.exit(1)


def _clean():
    clean_cmd = "{0} --build {1} --target clean".format(
        g_cmake, g_users_build_dir)
    os.system(clean_cmd)



def _generate_build_cmd():
    build_cmd = " ".join(each for each in g_cmake_args)

    build_cmd = build_cmd.format(
        g_cmake,
        g_users_build_dir,
        g_ninja,
        g_arm_toolchain_dir,
        g_parsed_args.platform.upper(),
        g_parsed_args.type.upper(),
        g_rtos.upper()
    )
    
    return build_cmd



def _build():

    build_cmd = _generate_build_cmd()

    # print(build_cmd)
    if (os.system(build_cmd) == 0):
        build_cmd = "{0} --build {1}".format(g_cmake, g_users_build_dir)
        if (os.system(build_cmd) == 0):
            print("build completed successfully!")
        else:
            print("build failed.")
    else:
        print("build failed.")


def main():
    _parse_args()

    _set_asdk_env()

    if g_parsed_args.clean and os.path.exists(g_users_build_dir):
        _clean()

    if g_parsed_args.platform:
        _build()


# entry point

if __name__ == "__main__":
    main()
