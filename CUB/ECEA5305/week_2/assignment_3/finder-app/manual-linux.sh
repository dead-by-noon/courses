#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

#OUTDIR defaults to /tmp/aeld
OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CURRENTDIR=`pwd`

#The cross compile binary should be pathed in the environment path so it is accessible
CROSS_COMPILE=aarch64-none-linux-gnu-

# OUTDIR contains all the build artifacts, and if an argument to the commandline executable is provided it instead uses the passed directory
if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

#Make the OUTDIR directory with parent directories if it does not exist
# Create the folder using mkdir with -p flag to handle missing parent directories. produce failure message if the command fails
if ! mkdir -p ${OUTDIR}; then
	echo "Error: Could not create folder ${OUTDIR}."
	return 1
fi


cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist. Using --depth 1 to prevent download of the entire history of the repo, limiting to the most recent commit
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # The patch to prevent the multiple symbol declaration of yylloc is applied in this step. It is first downloaded from the internet and then applied to the checked out version of the linux filesystem
    echo "Using wget to access the patch to prevent yylloc from causing an error in the build"
    wget -O e33a814e772cdc36436c8c188d8c42d019fda639.patch https://github.com/torvalds/linux/commit/e33a814e772cdc36436c8c188d8c42d019fda639.patch
    git apply e33a814e772cdc36436c8c188d8c42d019fda639.patch

    # TODO: Add your kernel build steps here
    # Removes all intermediate files, including the .config file. Use this target to return the source tree to the state it was in immediately after cloning or extracting the source code. If you are curious about the name, Mr. Proper is a cleaning product common in some parts of the world. The meaning of make mrproper is to give the kernel sources a really good scrub.
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    # With so many things to configure, it is unreasonable to start with a clean sheet each time you want to build a kernel, so there is a set of known working configuration files in arch/$ARCH/configs, each containing suitable configuration values for a single SoC or a group of SoCs.
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    # Build all of the artifacts required to run the kernel on an arm platform device
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
fi


echo "Adding the Image in outdir"
cp -r ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}/

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
# Allocating the root filesystem, switching to the rootfs folder, and generating all standard Linux folder directories required in a barebones Linux application
mkdir -p ${OUTDIR}/rootfs
cd "$OUTDIR/rootfs"

mkdir -p ${OUTDIR}/rootfs
# Noted that proc is a pseudo filesystem, and when it is mounted, it will not use a "real" device, instead it will use a placeholder name
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log

cd "$OUTDIR"
# If busybox is not installed, install busybox
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
# This utilizes the classic "make" to build the intermediate files and the "make install" command to install the files in the directory specified. Note that the compiling binary is the CROSS_COMPILE specified in the variable
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
#These commands will display the dependencies of busybox, which will be manually added to nacent the filesystem in the OUTDIR directory
cd $OUTDIR/rootfs
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
SYSROOT=`${CROSS_COMPILE}gcc -print-sysroot`
# Using cp with -a flag to copy the state and permissions of the libraries
cp -a $SYSROOT/lib/ld-linux-aarch64.so.1 $OUTDIR/rootfs/lib/
cp -a $SYSROOT/lib64/ld-2.33.so* $OUTDIR/rootfs/lib64/
cp -a $SYSROOT/lib64/libm.so.6 $OUTDIR/rootfs/lib64/
cp -a $SYSROOT/lib64/libm-2.33.so $OUTDIR/rootfs/lib64/
cp -a $SYSROOT/lib64/libresolv.so.2 $OUTDIR/rootfs/lib64/
cp -a $SYSROOT/lib64/libresolv-2.33.so $OUTDIR/rootfs/lib64/
cp -a $SYSROOT/lib64/libc.so.6 $OUTDIR/rootfs/lib64/
cp -a $SYSROOT/lib64/libc-2.33.so $OUTDIR/rootfs/lib64/

# TODO: Make device nodes
# In a really minimal root filesystem, you need just two nodes to boot with BusyBox: console and null. The console only needs to be accessible to root, the owner of the device node, so the access permissions are 600 (rw-------). The null device should be readable and writable by everyone, so the mode is 666 (rw-rw-rw-). You can use the -m option for mknod to set the mode when creating the node. You need to be root to create device nodes,
cd $OUTDIR/rootfs
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
# Change into the original running directory, and compile the writer program via the cross compiler. Copy the binary into the root filesystem of the new platform
cd $CURRENTDIR
make clean
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
#Copy the writer to the home directory (home directory was made earlier during the initial setup for the dirctory structure)
cp writer $OUTDIR/rootfs/home/

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
# Make the conf directory in the root filesystem's home folder
mkdir -p $OUTDIR/rootfs/home/conf
#Copy the pertinent files to the new location in the cross compiled filesystem
cp finder.sh finder-test.sh $OUTDIR/rootfs/home/
cp conf/username.txt conf/assignment.txt $OUTDIR/rootfs/home/conf/
cp autorun-qemu.sh $OUTDIR/rootfs/home/

# TODO: Chown the root directory
cd ${OUTDIR}/rootfs
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
#Changes to the rootfs directory, and uses a piped find and cpio command to convert the entire directory structure to an cpio image binary, which is then zipped into the initramfs.cpio.gz file. This initramfs.cpio.gz archive willl be passed to the kernel
cd "$OUTDIR/rootfs"
find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio
cd ..
gzip initramfs.cpio
