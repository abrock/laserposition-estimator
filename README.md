# laserposition-estimator
A simple program for reading out a GigE-vision/USB-vision industrial camera, find one bright spot in the image and estimate its position.

Debendencies (Debian package names):

build-essential
cmake
libboost-filesystem-dev
libboost-iostreams-dev
libboost-system-dev
libceres-dev
libfmt-dev
libglib2.0-dev
libgoogle-glog-dev
libgtest-dev
libjsoncpp-dev
libtclap-dev
meson
ninja-build
qml-module-qt-labs-settings
qtbase5-dev
qtdeclarative5-dev
qtquickcontrols2-5-dev

### Aravis (use a 0.8 release) ###

https://github.com/AravisProject/aravis

pushd aravis
meson build
pushd build
ninja
sudo ninja install
popd
sudo cp src/aravis.rules /etc/udev/rules.d
popd


### RunningStats ###

git clone https://github.com/abrock/RunningStats
mkdir build-runningstats
cd build-runningstats
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../RunningStats
ninja
sudo ninja install

### OpenCV ###

You can use system libraries, but on Debian they are built without QT enabled, so for a better user interface you have to build it yourself:

git clone --recursive https://github.com/opencv/opencv
git clone --recursive https://github.com/opencv/opencv_contrib
mkdir -p build-opencv
pushd build-opencv
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv -DWITH_QT=True
ninja
sudo ninja install
popd

### Building and Installing ###

git clone https://github.com/abrock/laserposition-estimator
mkdir build-laserposition-estimator
cd build-laserposition-estimator
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../laserposition-estimator
ninja
