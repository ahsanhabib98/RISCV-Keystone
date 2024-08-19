# Keystone Setup
From https://github.com/RamyaBoggaram/Keystone-with-Docker-configurations-as-non-docker

Follow these steps to build keystone as non-docker which can run cpp files and have the docker configurations.

## 1. Clone the Repo
```
git clone --recursive https://github.com/keystone-enclave/keystone.git
cd keystone
git checkout 4e96652
```
Without the checkout, latest version of keystone is cloned so make sure to use checkout to clone docker configuration keystone

## 2. Install Dependencies
```
sudo apt update
sudo apt install ninja-build
sudo apt install autoconf automake autotools-dev bc \
bison build-essential curl expat libexpat1-dev flex gawk gcc git \
gperf libgmp-dev libmpc-dev libmpfr-dev libtool texinfo tmux \
patchutils zlib1g-dev wget bzip2 patch vim-common lbzip2 python3 \
pkg-config libglib2.0-dev libpixman-1-dev libssl-dev screen \
device-tree-compiler expect makeself unzip cpio rsync cmake p7zip-full
```
## 3. Setup All Environment Variables 
```
./fast-setup.sh
source source.sh
mkdir build
cd build
cmake ..
make
make image
```
### Repeat Setup on Fresh Terminal
If you close the terminal and start a new one, make sure to set up the environment variables again using
```
source source.sh
```
## 4. Build and Run the Package
In your build directory:
```
make hello-package
cp examples/hello/hello.ke ./overlay/root
make image
./scripts/run-qemu.sh
```
Result should show lines: Welcome to Buildroot
```
buildroot login:
```
Login as `root` with the password `sifive`. You can exit QEMU by `ctrl-a + x` or using `poweroff` command.

## 5. Test by Running Hello World
```
insmod keystone-driver.ko
./hello.ke
```
