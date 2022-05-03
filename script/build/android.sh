#!/usr/bin/env bash

# setting
BUILD=(armeabi-v7a arm64-v8a x86 x86_64)

THREADS=$(nproc 2>/dev/null || sysctl -n hw.ncpu)

export CMAKE=cmake

# check args
if [ $# -lt 1 ]; then
    echo "Usage: $0 install"
    exit 1
fi

# make path absolute
mkdir -p $1
INSTALL=$(cd "$1"; pwd)

# setup vars

PWD=`pwd`
HOME=$(cd `dirname $0`; pwd)
PROJECT="$HOME/../.."

SCRIPT_ROOT="$PROJECT/script/cmake"

echo "[INFO] Using $THREADS thread(s)"

echo "========[ Building jar"

mkdir -p "$PWD/android-jar"
pushd "$PWD/android-jar" > /dev/null

$SHELL "${SCRIPT_ROOT}/android-${BUILD[0]}.sh"
if [ $? -ne 0 ]; then
    echo "[ERROR] cmake build porjcet filed in ${BUILD[0]}"
    exit $?
fi
$CMAKE --build . --target jar -- -j $THREADS

mkdir -p "$INSTALL/lib"
cp "$PROJECT"/lib/*.jar "$INSTALL/lib"
mkdir -p "$INSTALL/src"
cp -r "$PROJECT"/java/src/seeta "$INSTALL/src"

popd > /dev/null

for arch in ${BUILD[@]}
do
    echo "========[ Building $arch"
    
    mkdir -p "$PWD/android-$arch"
    pushd "$PWD/android-$arch" > /dev/null

    $SHELL "${SCRIPT_ROOT}/android-$arch.sh"
    if [ $? -ne 0 ]; then
        echo "[ERROR] cmake build porjcet filed in $arch"
        exit $?
    fi
    $CMAKE --build . --target clean -- -j $THREADS
    $CMAKE --build . --target seeta_aip_java -- -j $THREADS
    $CMAKE --build . --target kernel32 -- -j $THREADS

    mkdir -p "$INSTALL/lib/$arch"
    cp "$PROJECT/lib"/*.so "$INSTALL/lib/$arch"
    
    popd > /dev/null
done

echo "[SUCCEED] $INSTALL"

