#!bin/bash/

cd code/viewer/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/../imagelib
qtchooser --run-tool=qmake -qt=qt5

cd ../viewerLR/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/../imagelib
qtchooser --run-tool=qmake -qt=qt5
make
