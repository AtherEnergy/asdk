#!/bin/bash


#Git branch
GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
#Git Commit
GIT_COMMIT=$(git rev-parse --short HEAD)
#hostame
HOST=$(uname -n)
#user
USER=$(whoami)

usage()
{
    echo "usage: build.sh [[-i install_dir ] [-b build_type] [-v version] [-p platform] [-n exe_name] [-g generated_dir] | [-h]]"
    echo "(e.g) ./build.sh -b Release -v 1.0.0 -p KW36 -n BMS -g ./generated -i ./binaries"
    exit 
}

while [ "$1" != "" ]; do
    case $1 in
        -i | --install_dir )    shift
				INSTALL=$(pwd)/$1
                                ;;
        -b | --build_type )     shift
                                BUILD_TYPE=$1
                                ;;
        -v | --version )        shift
                                VERSION=$1
                                ;;
        -p | --platform )       shift
                                PLATFORM=$1
                                ;;
        -n | --exe_name )       shift
                                EXE_NAME=$1
                                ;;
        -g | --generated_dir )  shift
				GENERATED=$(pwd)/$1
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage
                                exit 1
    esac
    shift
done

if [[ -z $EXE_NAME || -z $VERSION || -z $PLATFORM || -z $BUILD_TYPE ]]; then
	usage
fi

if [[ $VERSION: =~ ^([[:digit:]]{1}.){3}$ ]]; then
        MAJOR="$(echo $VERSION | cut -d'.' -f1)"
        MINOR="$(echo $VERSION | cut -d'.' -f2)"
        PATCH="$(echo $VERSION | cut -d'.' -f3)"
        #echo "$MAJOR $MINOR $PATCH"
else
	echo "version string must be x.y.z where x is major, y is minor, z is patch"
	exit
fi

if [ -v $INSTALL ]; then
       	echo "Use default install directory $(pwd)/install"
	INSTALL=$(pwd)/install
fi

SRC_DIR=$(pwd)

rm -rf build $INSTALL
mkdir build
mkdir $INSTALL
cd build
echo "$GENERATED"
cmake ../asdk -DCMAKE_TOOLCHAIN_FILE=cmake/devel_arm_eabi_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DTARGET_PLATFORM=$PLATFORM -DEXE_TYPE=External -DEXT_SRC=$SRC_DIR -DEXE_NAME=$EXE_NAME -DAPP_MAJOR_VERSION=$MAJOR -DAPP_MINOR_VERSION=$MINOR -DAPP_PATCH_VERSION=$PATCH -DAPP_GIT_BRANCH=$GIT_BRANCH -DAPP_GIT_COMMIT=$GIT_COMMIT -DHOSTNAME=$HOST -DUSERNAME=$USER -DINSTALL_DIR=$INSTALL -DGENERATED_DIR=$GENERATED

make install
