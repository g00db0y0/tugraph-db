#!/bin/bash

# Standard Github-hosted runner is 2core currently.
# Larger runner will support soon(current in Beta).

# set $WORKSPACE to root dir
cd $WORKSPACE
bash ./cpplint/check_all.sh
cd deps
SKIP_WEB=1 bash ./build_deps.sh -j6
cd $WORKSPACE
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Coverage -DENABLE_INTERNAL_BUILD=1
make lgraph -j6
make lgraph_cypher_lib -j6
make -j6
cd $WORKSPACE

# unittest
cd build/output
./unit_test
rm -rf testdb* .import_tmp

# integrate tests
cp ../../src/client/python/TuGraphClient/TuGraphClient.py .
cp -r ../../test/integration/* ./
pytest ./ -k 'not TestJavaClient'

# codecov
cd $WORKSPACE
mkdir testresult
bash ./ci/codecov.sh build $WORKSPACE/testresult

python3  ./ci/lcov_cobertura.py  $WORKSPACE/testresult/coverage.info --output $WORKSPACE/testresult/coverage.xml --demangle
