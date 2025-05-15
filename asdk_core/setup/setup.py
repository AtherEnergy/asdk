# imports

from __future__ import print_function
import os
import platform
import sys
import logging
import argparse
import re
import tempfile
import stat

# global variables

# initialzed variables

g_os_type = platform.system().lower()
g_script_path = os.path.dirname(os.path.abspath(sys.argv[0]))
g_script_name = os.path.splitext(sys.argv[0])[0]
g_req_filepath = os.path.join(g_script_path, "requirements.txt")
g_config_filepath = os.path.join(g_script_path, "setup.yaml")

g_unix_user_home = os.path.expanduser("~")
g_users_destination = os.path.join(g_unix_user_home, "asdk_toolchain")
g_asdk_env_file = os.path.join(g_unix_user_home, ".asdk_environment")
g_linux_env_file = os.path.join(g_unix_user_home, ".bashrc")
g_darwin_env_file = os.path.join(g_unix_user_home, ".bash_profile")

g_zip_tar_pattern = re.compile("(.zip$)|(.tar$)|(.tar.gz$)|(.tar.bz2$)")

g_bashrc_text = """\n# source asdk environment variables
if [ -f $HOME/.asdk_environment ]; then
    . $HOME/.asdk_environment
fi\n"""

g_platform_choices = ["arm", "c2000"]
g_arm_choices = ['gcc-arm-none-eabi-7-2018-q2-update']
g_c2000_choices = ["None"]

g_env_asdk_cmake_root = "ASDK_CMAKE_ROOT"
g_env_asdk_ninja_root = "ASDK_NINJA_ROOT"
g_env_target_toolchain_root_template = "ASDK_<ARCH>_TOOLCHAIN_ROOT"
g_env_target_toolchain_version_template = "ASDK_<ARCH>_TOOLCHAIN_VERSION"
g_os_env = os.environ

# default variables

g_logger = object()
g_downloaded_filepath = str()
g_parsed_args = object()
g_env_variables = dict()
g_cmake_installed = False
g_ninja_installed = False
g_toolchain_installed = False

# function definitions

# helper functions


def setup_logging():
    """
    Setup in-built loggin module for logging.
    Two loggers are enabled.s
    1. Console - prints on console.
    2. File - logs to a file.

    The log file is created in the CWD and
    the filename is sames script name with
    '.log' extenstion.
    """

    global g_logger

    g_logger = logging.getLogger(g_script_name)

    g_logger.setLevel(logging.DEBUG)

    # console stream, prints only on console
    c_handler = logging.StreamHandler()
    c_format = logging.Formatter('%(levelname)s: %(message)s')
    c_handler.setFormatter(c_format)
    c_handler.setLevel(logging.INFO)
    g_logger.addHandler(c_handler)

    # file stream, doesn't print on console
    f_handler = logging.FileHandler(g_script_name + '.log')
    f_format = logging.Formatter('%(asctime)s, %(levelname)s, %(message)s')
    f_handler.setFormatter(f_format)
    f_handler.setLevel(logging.DEBUG)
    g_logger.addHandler(f_handler)

    g_logger.propogate = False


def log(level, *msg, **args):
    """
    Wrapper for logging user messages.
    """

    level = str(level).lower()

    if level == "debug":
        g_logger.debug(*msg, **args)

    elif level == "info":
        g_logger.info(*msg, **args)

    elif level == "warning":
        g_logger.warning(*msg, **args)

    elif level == "error":
        g_logger.error(*msg, **args)

    else:  # critical error
        g_logger.critical(*msg, **args)


def log_fun_marker(fun, end=False):
    # log("debug", fun.__name__ + ": %s", "completed" if end else "started")
    pass


def import_modules():
    global getpass
    global zipfile
    global tarfile
    global requests
    global yaml
    global SafeLoader

    import getpass
    import zipfile
    import tarfile

    try:
        # other non-default import
        import requests
        import yaml
        from yaml.loader import SafeLoader
    except Exception as error:
        if (platform.sys.version_info.major == 3):
            log("error", "Import exception, %s\nRun 'pip3 install -r %s' to resolve it & retry.", error.msg, g_req_filepath)
        else:
            log("error", "Import exception, %s\nRun 'pip2 install -r %s' to resolve it & retry.", error.message, g_req_filepath)
        sys.exit(1)


# functions


def sys_supported():
    """
    Check if the user's OS is supported.
    If un-supported exit.
    """

    supported_status = True

    log_fun_marker(sys_supported)

    if ((g_os_type != "linux") and (g_os_type != "windows") and (g_os_type != "darwin")):
        log("error", "Your OS is not supported!")
        supported_status = False

    log("debug", "OS info = '%s'", platform.platform())
    log("debug", "User = '%s'", getpass.getuser())
    log("debug", "CWD = '%s'", os.getcwd())
    log("debug", "Python = %s", str(platform.sys.version))

    log_fun_marker(sys_supported, True)

    return supported_status


def parse_user_args():
    global g_parsed_args

    log_fun_marker(parse_user_args)

    # add cli arguments
    arg_parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    # --platform - MCU selection
    arg_parser.add_argument('-p', '--platform',
                            help="Install platform-specific toolchain.\nAllowed values:\n" +
                            ", ".join(g_platform_choices),
                            type=str,
                            default="arm",
                            choices=g_platform_choices,
                            required=False,
                            metavar=""
                            )

    # --version - toolchain version selection
    version_group_parser = arg_parser.add_mutually_exclusive_group()

    version_group_parser.add_argument("--arm-version",
                                      help="ARM toolchain version. Allowed values:\n[" + ", ".join(
                                          g_arm_choices) + "]",
                                      type=str,
                                      default="gcc-arm-none-eabi-7-2018-q2-update",
                                      choices=g_arm_choices,
                                      required=False,
                                      metavar="")

    version_group_parser.add_argument("--c2000-version",
                                      help="TI C2000 toolchain version. Allowed values:\n[" + ", ".join(
                                          g_c2000_choices) + "]",
                                      type=str,
                                      default="None",
                                      choices=g_c2000_choices,
                                      required=False,
                                      metavar="")

    # --force
    arg_parser.add_argument("-f", "--force",
                            help="Force install if the specified tools already installed.",
                            required=False,
                            action="store_true",
                            )

    # parse arguemnts
    g_parsed_args = arg_parser.parse_args()

    log("debug", "force = '%s'", g_parsed_args.force)
    log("debug", "platform = '%s'", g_parsed_args.platform)
    if (g_parsed_args.platform == "arm"):
        log("debug", "arm_version = '%s'", g_parsed_args.arm_version)
    else:
        log("debug", "c2000_version = '%s'", g_parsed_args.c2000_version)

    log_fun_marker(parse_user_args, True)


def get_config():
    # config_file = os.path.join(os.getcwd(), "setup.yaml")

    if os.path.exists(g_config_filepath):
        log("debug", "setup.yaml file found!")
    else:
        log("error", "cannot find '%s' file.", g_config_filepath)
        sys.exit(1)

    try:
        with open(g_config_filepath) as yaml_cfg:
            cfg = yaml.load(yaml_cfg, Loader=SafeLoader)
    except Exception as error:
        if (platform.sys.version_info.major == 3):
            log("error", "YAML - %s", error.msg)
        else:
            log("error", "YAML - %s", error.message)
        sys.exit(1)

    return cfg


def _progress_bar(count, block_size, total):
    progress = int((count*block_size*100) / total)

    if (progress >= 100):
        print('downloading {:d}%'.format(progress), end='\n')
    else:
        print('downloading {:d}%'.format(progress), end='\r')
        sys.stdout.flush()


def _execute_download(user_url, timeout_s):
    download_status = False
    dest_or_error = os.path.join(
        tempfile.gettempdir(), os.path.basename(user_url))

    # get request to file
    try:
        requests.packages.urllib3.disable_warnings()
        resp = requests.get(user_url, timeout=timeout_s, stream=True, verify=False)
        resp.raise_for_status()
        download_status = True
    except requests.exceptions.RequestException as error:
        dest_or_error = error
    
    # downloading file
    if (download_status):
        try:
            filesize = resp.headers['Content-length']
            chunk_counter = 0
            with open(dest_or_error, "wb") as f:
                for chunk_counter, chunk_data in enumerate(resp.iter_content(chunk_size=8192)):
                    if chunk_data:
                        f.write(chunk_data)
                        _progress_bar(int(chunk_counter), 8192, int(filesize))
        except requests.exceptions.RequestException as error:
            download_status = False
            dest_or_error = error

    return (download_status, dest_or_error)


def _download(url, retry=3, read_timeout=30):
    """
    Download the specified utilitiy for the OS based on the
    configuration in setup.yaml file. 

    Returns the filepath to the downloaded file.
    """

    dwld_status = False
    dwld_file_or_error = None

    while (retry > 0):
        dwld_status, dwld_file_or_error = _execute_download(url, read_timeout)

        if (dwld_status):
            log("debug", "Downloaded to '%s'", dwld_file_or_error)
            log("info", "downloaded successfully")
            break
        else:
            log("debug", dwld_file_or_error)

        retry = retry - 1

    # exit if download fails
    if (dwld_status == False):
        log("error", "Could not download the file. See the log file for more information.")
        sys.exit(1)

    return dwld_file_or_error


def _extract(src_path, dest_path):
    ext_match = re.search(g_zip_tar_pattern, src_path)

    if (ext_match != None):
        try:
            file_ext = ext_match.group()

            # zip file
            if (file_ext == ".zip"):
                with zipfile.ZipFile(src_path, 'r') as zip_ref:
                    zip_ref.extractall(dest_path)
            # tar file
            else:
                tar = tarfile.open(src_path)
                tar.extractall(dest_path)
                tar.close()
        except Exception as error:
            log("error", "Error occured while extracing file.\nException: {0}".format(error.__str__()))
            sys.exit(1)

        log("info", "extracted successfully")
    else:
        log("error", "Unsupported file extension '%s'", src_path)
        sys.exit(1)


def _get_zip_filename(filepath):
    filename = os.path.basename(filepath)

    if (re.search(g_zip_tar_pattern, filename) != None):
        filename = re.split(g_zip_tar_pattern, filename)[0]
        return filename

    return None


def check_requirements():
    global g_cmake_installed
    global g_ninja_installed
    global g_toolchain_installed

    log_fun_marker(check_requirements)

    if g_parsed_args.force:
        g_cmake_installed = False
        g_ninja_installed = False
        g_toolchain_installed = False
        log("info", "forced install...")
    
    else:
        # check if cmake is available
        if g_env_asdk_cmake_root.upper() in g_os_env:
            if (g_os_type == "windows"):
                cmake_filepath = os.path.join(g_os_env.get(g_env_asdk_cmake_root), "bin", "cmake.exe")
            else:
                cmake_filepath = os.path.join(g_os_env.get(g_env_asdk_cmake_root), "bin", "cmake")
            
            if (os.path.isfile(cmake_filepath)):
                g_cmake_installed = True
                log("info", "CMake is already installed.")
                log("debug", cmake_filepath)
        
        # check if ninja is available
        if g_env_asdk_ninja_root.upper() in g_os_env:
            if (g_os_type == "windows"):
                ninja_filepath = os.path.join(g_os_env.get(g_env_asdk_ninja_root), "ninja.exe")
            else:
                ninja_filepath = os.path.join(g_os_env.get(g_env_asdk_ninja_root), "ninja")

            if (os.path.isfile(ninja_filepath)):
                g_ninja_installed = True
                log("info", "Ninja is already installed.")
                log("debug", ninja_filepath)
        
        # check if target toolchain is available
        target_toolchain_root = g_env_target_toolchain_root_template.replace("<ARCH>", g_parsed_args.platform.upper())
        target_toolchain_version = g_env_target_toolchain_version_template.replace("<ARCH>", g_parsed_args.platform.upper())

        if g_parsed_args.platform.lower() in g_platform_choices:
            if (target_toolchain_root in g_os_env) and (target_toolchain_version in g_os_env):
                toolchain_filepath = os.path.join(g_os_env.get(target_toolchain_root), g_os_env.get(target_toolchain_version))

                target_platform = g_parsed_args.platform.lower()

                # for ARM toolchain
                if (target_platform == "arm"):
                    if (g_os_type == "windows"):
                        toolchain_filepath = os.path.join(toolchain_filepath, "bin", "arm-none-eabi-gcc.exe")
                    else:
                        toolchain_filepath = os.path.join(toolchain_filepath, "bin", "arm-none-eabi-gcc")
                
                if (os.path.isfile(toolchain_filepath)):
                    g_toolchain_installed = True
                    log("info", "Toolchain is already installed.")
                    log("debug", toolchain_filepath)
                    
    
    log_fun_marker(check_requirements, True)


def install_asdk_requirements(cfg):
    global g_env_variables

    log_fun_marker(install_asdk_requirements)

    g_env_variables["ASDK_TOOLCHAIN_ROOT"] = g_users_destination

    # extract requirements info from setup.yaml
    requirements = cfg.get("configuration").get("requirements")

    if not g_cmake_installed:
        # download
        log("info", "getting cmake...")
        cmake_url = requirements.get("cmake")[g_os_type]
        cmake_dwld_path = _download(cmake_url)

        cmake_dest = g_users_destination

        # extract
        cmake_dest = g_users_destination
        filename = _get_zip_filename(cmake_dwld_path)
        log("info", "extracting '%s' to '%s'...", filename, cmake_dest)
        _extract(cmake_dwld_path, cmake_dest)

        # assign env variables
        if (g_os_type == "darwin"):
            filename = os.path.join(filename, "CMake.app", "Contents")
        g_env_variables["ASDK_CMAKE_ROOT"] = os.path.join(cmake_dest, filename)

    if not g_ninja_installed:
        # download
        log("info", "getting ninja...")
        ninja_url = requirements.get("ninja")[g_os_type]
        ninja_dwld_path = _download(ninja_url)

        # extract
        ninja_dest = os.path.join(g_users_destination, "ninja")
        filename = os.path.basename(cmake_dwld_path)
        log("info", "extracting '%s' to '%s'...", filename, ninja_dest)
        _extract(ninja_dwld_path, ninja_dest)

        # add executable permissions for ninja on linux and mac
        ninja_bin = os.path.join(ninja_dest, "ninja")
        if (g_os_type != "windows"):
                st = os.stat(ninja_bin)
                os.chmod(ninja_bin, st.st_mode | stat.S_IEXEC)

        # assign env variables
        g_env_variables["ASDK_NINJA_ROOT"] = ninja_dest

    log_fun_marker(install_asdk_requirements, True)


def install_asdk_toolchain(cfg):
    global g_env_variables

    log_fun_marker(install_asdk_toolchain)

    env_toolchain_root_var = "ASDK_" + g_parsed_args.platform.upper() + \
        "_TOOLCHAIN_ROOT"
    env_toolchain_ver_var = "ASDK_" + g_parsed_args.platform.upper() + \
        "_TOOLCHAIN_VERSION"

    # extract toolchain info from setup.yaml
    toolchain_cfg = cfg.get("configuration").get("toolchains")
    platform_toolchain = toolchain_cfg.get(g_parsed_args.platform)
    if (g_parsed_args.platform == "arm"):
        toolchain_version = g_parsed_args.arm_version
    else:
        toolchain_version = g_parsed_args.c2000_version
    
    if not g_toolchain_installed:
        # download
        toolchain_url = platform_toolchain.get(toolchain_version)[g_os_type]
        log("info", "getting '%s' toolchain (version: '%s')...",
            g_parsed_args.platform, toolchain_version)
        toolchain_dwld_path = _download(toolchain_url)

        # extract
        toolchain_root = os.path.join(
            g_users_destination, g_parsed_args.platform.lower())
        filename = os.path.basename(toolchain_dwld_path)
        if (g_os_type == "windows") and \
                (toolchain_version == "gcc-arm-none-eabi-7-2018-q2-update"):
            # keeping it unifrom across OS
            toolchain_dest = os.path.join(toolchain_root, toolchain_version)
        else:  # for linux and darwin
            toolchain_dest = toolchain_root
        log("info", "extracting '%s' to '%s'...", filename, toolchain_dest)
        _extract(toolchain_dwld_path, toolchain_dest)

        # assign env variables
        g_env_variables[env_toolchain_root_var] = toolchain_root
        g_env_variables[env_toolchain_ver_var] = toolchain_version

    log_fun_marker(install_asdk_toolchain, True)


def set_env_variables(var_kv_pairs):
    if not (g_cmake_installed and g_ninja_installed and g_toolchain_installed):
        log("info", "Setting envrionment variables...")
        if (g_os_type == "windows"):
            for var, val in var_kv_pairs.items():
                log("info", "setting '{0}' variable".format(var))
                if (os.system("SETX {0} {1}".format(var, val)) != 0):
                    log("error", "Failed to set variable.")
                    sys.exit(1)

            log("info", "Environment variables were added successfully.\nNote: Close this cmd-prompt and re-open it to take effect.")

        else:
            with open(g_asdk_env_file, "w+") as asdk_f:
                for var, val in var_kv_pairs.items():
                    log("info", "setting '{0}' variable".format(var))
                    asdk_f.write("export {0}={1}\n".format(var, val))
                asdk_f.close

            if (g_os_type == "linux"):
                bash_env_file = g_linux_env_file
            else:  # darwin
                bash_env_file = g_darwin_env_file

            # read ~/.bashrc or ~/.bash_profile
            bash_buffer = list()
            if os.path.exists(bash_env_file):
                with open(bash_env_file, "r") as bash_r:
                    bash_buffer = bash_r.read()
                    bash_r.close()

            # check if ~/.asdk_environment is already sourced
            # if not then source it
            if not (g_bashrc_text in bash_buffer):
                log("debug", "sourcing '%s' file from '%s' file.",
                    g_asdk_env_file, bash_env_file)
                with open(bash_env_file, "a+") as bash_w:
                    bash_w.write(g_bashrc_text)
                    bash_w.close()
            else:
                log("debug", "'%s' file is already sourced from '%s' file.",
                    g_asdk_env_file, bash_env_file)

            log("info", "ASDK environment setup completed successfully!\nRun 'source " + bash_env_file + "'")


def main():
    setup_logging()

    log("debug", "---- Running script ----")

    parse_user_args()
    import_modules()

    if sys_supported():                     # check OS support
        config = get_config()               # read setup.yaml file
        check_requirements()                # check requirements are already installed
        install_asdk_requirements(config)   # cmake, ninja
        install_asdk_toolchain(config)      # arm-none-eabi, c2000...etc
        set_env_variables(g_env_variables)

# entry point

if __name__ == "__main__":
    main()
