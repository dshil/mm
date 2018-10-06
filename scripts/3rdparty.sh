#! /bin/bash

set -xe

build_dir=3rdparty/deps
catch=catch2

function setup_catch () {
    catch_url=https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp

    mkdir -p $build_dir/$catch
    wget $catch_url -P $build_dir/$catch
}

if [ ! -d "$build_dir/$catch" ]; then
    setup_catch
fi
