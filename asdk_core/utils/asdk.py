# imports

from __future__ import print_function
import sys
import os
import datetime
import zipfile
import argparse

# global variables

g_script_path = os.path.dirname(os.path.abspath(sys.argv[0]))
g_asdk_root = os.path.dirname(g_script_path)

g_dal_h_template_file = os.path.join(
    g_asdk_root, "utils", "file_templates", "dal_header.txt")

g_dal_c_template_file = os.path.join(
    g_asdk_root, "utils", "file_templates", "dal_source.txt")

g_mcu_choices = ["cyt2b75", "c2000"]

# uninitialized variables

g_parsed_args = object()
g_arg_parser = object()


def parse_args():
    global g_parsed_args
    global g_arg_parser

    # add cli arguments
    g_arg_parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    # add sub commands
    sub_cmds = g_arg_parser.add_subparsers()

    # command: asdk.py dal <dal_name> <author> <mcu>
    dal_cmd_parser = sub_cmds.add_parser("dal",
                                         help="Create a DAL module.",
                                         description="Creates the DAL header and source files.\
                                         Use the '--header' option to create header file alone.\
                                         Use the '--source' option to create the c file alone. \
                                         If both '--header' and '--source' are not specified \
                                         then both will be generated.",
                                         formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    dal_cmd_args = dal_cmd_parser.add_argument_group()
    dal_cmd_args.add_argument("name",
                              help="Name of the dal module without spaces.",
                              metavar="<dal_name>")
    dal_cmd_args.add_argument("author",
                              help="Author name. Provide git username.",
                              metavar="<username>")
    dal_cmd_args.add_argument("--mcu",
                              help="Target microcontroller. Mandatory argument when '--source' is specified. Allowed values: [" +
                              ", ".join(g_mcu_choices) + "]",
                              choices=g_mcu_choices,
                              required=("--source" in sys.argv),
                              metavar="<mcu>")

    # command: asdk.py dal [--source | --header] <dal_name> <author> <mcu>
    dal_cmd_options = dal_cmd_parser.add_mutually_exclusive_group(
        required=False)
    dal_cmd_options.add_argument("--header",
                                 help="Create DAL header file only.",
                                 action="store_true",
                                 required=False,
                                 default=False)
    dal_cmd_options.add_argument("--source",
                                 help="Create DAL source file only.",
                                 action="store_true",
                                 required=False,
                                 default=False)

    # command: asdk.py <app_name>
    app_cmd_parser = sub_cmds.add_parser("app",
                                         help="Create application from template.",
                                         description="Creates the application folder structure \
                                         in the parent directroy of 'asdk-gen2' submodule \
                                         by using a pre-defined application template.")
    app_cmd_args = app_cmd_parser.add_argument_group()
    app_cmd_args.add_argument("name",
                              help="Name of the application without spaces.",
                              metavar="<app_name>")

    if (len((sys.argv)) == 1):
        g_arg_parser.print_help()
        sys.exit(1)
    else:
        g_parsed_args = g_arg_parser.parse_args()


def _gen_dal_h_file(dal_name, dal_author):
    h_file_buffer = str()

    dal_h_dest = os.path.join(g_asdk_root, "inc")
    filename = "asdk_" + dal_name.lower() + ".h"
    filepath = os.path.join(dal_h_dest, filename)

    # timestamp
    now = datetime.datetime.now()
    str_time = now.strftime('%b %d, %Y')

    if os.path.exists(g_dal_h_template_file):
        try:
            with open(g_dal_h_template_file) as t_file:
                h_file_buffer = t_file.read()
                t_file.close()

            with open(filepath, "w+") as h_file:
                h_file.write(h_file_buffer.format(dal_name.lower(),
                                                  dal_name.upper(),
                                                  str_time,
                                                  dal_author.lower()))
                h_file.close()
        except Exception as error:
            print(
                "Error occured while creating dal header file.\nException:", error.__str__())
            sys.exit(1)
    else:
        print("Error: cannot find the template for DAL header file.")
        sys.exit(1)

    print("\n{0} - DAL header file created successfully!".format(filename))
    print("output: {0}".format(filepath))


def _gen_dal_c_file(dal_name, dal_author, dal_platform):
    c_file_buffer = str()

    dal_c_dest = os.path.join(g_asdk_root, "platform",
                              g_parsed_args.mcu, "dal", "src")
    filename = "asdk_" + dal_name.lower() + ".c"
    filepath = os.path.join(dal_c_dest, filename)

    # timestamp
    now = datetime.datetime.now()
    str_time = now.strftime('%b %d, %Y')

    if os.path.exists(g_dal_c_template_file):
        try:
            with open(g_dal_c_template_file) as t_file:
                c_file_buffer = t_file.read()
                t_file.close()

            with open(filepath, "w+") as c_file:
                c_file.write(c_file_buffer.format(dal_name.lower(),
                                                  dal_name.upper(),
                                                  str_time,
                                                  dal_author.lower(),
                                                  dal_platform.lower()))
                c_file.close()
        except Exception as error:
            print(
                "Error occured while creating dal source file.\nException:", error.__str__())
            sys.exit(1)
    else:
        print("Error: cannot find the template for DAL source file.")
        sys.exit(1)

    print("\n{0} - DAL source file created successfully!".format(filename))
    print("output: {0}".format(filepath))


def gen_dal_files():
    # generates asdk-gen2/inc/asdk_<dal_name>.h file
    if g_parsed_args.header:
        _gen_dal_h_file(g_parsed_args.name, g_parsed_args.author)

    # generates asdk-gen2/platform/<mcu>/dal/src/asdk_<dal_name>.c file
    elif g_parsed_args.source:
        _gen_dal_c_file(g_parsed_args.name,
                        g_parsed_args.author, g_parsed_args.mcu)

    else:  # generate both
        _gen_dal_h_file(g_parsed_args.name, g_parsed_args.author)
        _gen_dal_c_file(g_parsed_args.name,
                        g_parsed_args.author, g_parsed_args.mcu)


def _extract_zip(src_path, dest_path):
    file_ext = os.path.splitext(src_path)[1]

    # zip file
    if (file_ext == ".zip"):
        try:
            with zipfile.ZipFile(src_path, 'r') as zip_ref:
                zip_ref.extractall(dest_path)
        except Exception as error:
            print("Extract exception - {0}".format(error.__str__()))
            sys.exit(1)
    else:
        print("Unsupported file extension '%s'", src_path)
        sys.exit(1)


def _configure_app_cmake(app_cmake_t_file, app_name):
    error_msg = str()

    if os.path.exists(app_cmake_t_file):
        user_content = {"app_name": app_name}
        try:
            r_buffer = str()

            with open(app_cmake_t_file, "r") as r_file:
                r_buffer = r_file.read()
                r_file.close()

            with open(app_cmake_t_file, "w") as w_file:
                # w_buffer = r_buffer.format(**user_content)
                # print(w_buffer)
                w_file.write(r_buffer.format(**user_content))
                w_file.close()

            renamed_file = os.path.join(os.path.dirname(
                app_cmake_t_file), "app_config.cmake")
            os.rename(app_cmake_t_file, renamed_file)
        except Exception as error:
            error_msg = "Error occured while configuring app_config.cmake file.\nException: {0}".format(
                error)
    else:
        error_msg = "Error: File doesn't exist - '{0}'".format(
            app_cmake_t_file)

    if (len(error_msg) > 0):
        print(error_msg)
        sys.exit(1)


def gen_app():
    # python asdk.py app <app_name>

    app_temp_zip = os.path.join(g_asdk_root, "utils", "app_template.zip")
    app_dest = os.path.dirname(g_asdk_root)

    # extract the app_template zip archive to dest
    _extract_zip(app_temp_zip, app_dest)

    # set app_name in ./config/app_config.cmake
    app_cmake_path = os.path.join(
        app_dest, "app", "config", "app_config_cmake.txt")
    _configure_app_cmake(app_cmake_path, g_parsed_args.name)

    print("Generated '{0}' application successfully".format(
        g_parsed_args.name))


def main():
    parse_args()

    if "dal" in sys.argv:
        gen_dal_files()

    elif "app" in sys.argv:
        gen_app()

    else:
        g_arg_parser.print_help()

# entry point


if __name__ == "__main__":
    main()
