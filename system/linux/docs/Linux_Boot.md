# Linux基本启动过程

Linux的引导启动过程过程非常复杂，可以分为以下几个典型阶段：

1. BIOS stage。
2. Bootloader stage。
3. Kernel stage。
4. Systemd/Init stage。

## BIOS stage

​	当执行了Power On的操作，Linux系统开始启动。当处理器苏醒后会从固定位置（0xFFFF0）开始执行BIOS程序来启动系统。BIOS（Basic Input/Output System）是一段固件程序（firmware），它被存储在主板上的闪存存储器（Flash Memory），一般就是CMOS。系统启动过程中，我们可以进入BIOS设置页面，比如系统日期时间和启动设备的顺序等。因为BIOS是ROM，所以这些用户设置实际上也被存储在CMOS里，叫做CMOS settings。

> 关于BIOS和CMOS的关系和区别，可以参考[Differences and Relationships between BIOS and CMOS](https://www.4winkey.com/windows-tips/differences-and-relations-between-bios-and-cmos.html)。

<img src="https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/%E6%88%AA%E5%9B%BE.png" alt="img" style="zoom:50%;" />

BIOS的运行分为两个步骤：第一步是加电自检（POST, Power On Self Test），主要是对硬件进行检查；第二步是枚举并初始化本地设备。因此，BIOS程序也包含两个部分：POST代码和运行时服务（Runtime Services），在POST执行完成后POST代码就从内存中清除了，只有运行时服务驻留在内存中。

为了启动操作系统，BIOS runtime按照CMOS 设置里的顺序搜索是激活的并且可启动的设备，这个启动设备可以是硬盘、CD-ROM或者网络设备（NFS）等。

一般地，Linux从硬盘启动，硬盘的`MBR`包含了主引导加载程序（Primary Bootloader）。MBR是硬盘上的第一个扇区（sector 1 of cylinder 0, head 0），当MBR被加载到内存中后，BIOS将控制权交给MBR，系统启动进入Bootloader stage。

## Bootloader stage

一般地，Bootlader阶段也分为两个子阶段。

### Stage 1 Bootloader

存储在启动硬盘的MBR中的主引导加载程序是一个512字节的映像，其中包含程序代码和较小的分区表。 前446个字节是主引导加载程序，其中包含可执行代码和错误消息文本。 接下来的64个字节是分区表，其中包含四个分区中的每个分区的记录（每个分区16个字节）。 MBR的结尾是两个字节的魔法数字，这个数字用作MBR的验证检查。下图是MBR的布局：

![MBR的整体布局](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/fig2.gif)

主引导加载程序的工作是查找并加载第二阶段引导加载程序（Secondary Bootloader）。 它通过在分区表中查找活动分区来做到这一点，找到活动分区后，它将扫描表中的其余分区，以确保它们均处于非活动状态。 验证完成后，活动分区的动记录将从设备读取到RAM中并执行。

### Stage 2 Bootloader

Stage 2 Bootloader的工作是加载Linux kernel和可选的`initrd`（initial RAM disk）。在X86 PC环境中，两个阶段的Bootloader被统称为GRUB（GRand Unified Bootloader），这是当前主流的BootLoader。

GRUB的优点在于它了解Linux文件系统，因此它可以从ext2或者ext3文件系统中加载Linux内核。为了实现这一点，GRUB的做法是在Stage 1 BootLoader后启动了`Stage 1.5 BootLoader`，这个Stage 1.5 BootLoader可以理解包含Linux kernel的特定文件系统。当Stage 1.5 BootLoader被加载并运行后，就可以继续加载Stage 2 BootLoader。当Stage 2 BootLoader被加载后，我们就可以看到可用的内核映像的列表，这个列表来自于我们的grub配置文件`/etc/grub2.conf`（通过软链接指向boot分区下的配置文件），每个内核映像都是grub2.conf中的类似于下面这样的menuentry，除了加载启动默认的Linux内核映像，我们也可以手动选择加载其中一个内核映像，甚至可以输入Tab键后来修改内核启动参数。

```bash
### BEGIN /etc/grub.d/10_linux ###
menuentry 'Red Hat Enterprise Linux Server (3.10.0-862.11.6.el7.x86_64) 7.5 (Maipo)' --class red --class gnu-linux --class gnu --class os --unrestricted $menuentry_id_option 'gnulinux-3.10.0-862.11.6.el7.x86_64-advanced-37c98685-63d6-4c1e-a5e2-19158b744319' {
        load_video
        set gfxpayload=keep
        insmod gzio
        insmod part_msdos
        insmod ext2
        set root='hd0,msdos1'
        if [ x$feature_platform_search_hint = xy ]; then
          search --no-floppy --fs-uuid --set=root --hint-bios=hd0,msdos1 --hint-efi=hd0,msdos1 --hint-baremetal=ahci0,msdos1 --hint='hd0,msdos1'  e70dff6a-7c2b-4d2b-9b6b-54e4af258339
        else
          search --no-floppy --fs-uuid --set=root e70dff6a-7c2b-4d2b-9b6b-54e4af258339
        fi
        linux16 /vmlinuz-3.10.0-862.11.6.el7.x86_64 root=/dev/mapper/system-root ro nomodeset modprobe.blacklist=qla2xxx,ahci,isci rd.lvm.lv=system/root rd.lvm.lv=system/swap nodmraid LANG=en_US.UTF-8 crashkernel=256M loglevel=1 pcie_aspm=off nokaslr
        initrd16 /initramfs-3.10.0-862.11.6.el7.x86_64.img
}
```

/dev/mapper/system-root是根文件系统，e70dff6a-7c2b-4d2b-9b6b-54e4af258339是boot分区的uuid，grub的配置、内核映像和initrd都存储在boot分区，grub2.conf里配置的内核映像和initrd的路径也是以boot分区为根目录。先看一下系统启动完成后boot分区里的文件（已经被挂载到/boot目录）：

```bash
# ls -al /boot/ | grep 3.10.0-862.11
-rw-r--r--.  1 root root   147859 Aug 10  2018 config-3.10.0-862.11.6.el7.x86_64
-rw-r--r--   1 root root 28636614 Oct 23  2018 initramfs-3.10.0-862.11.6.el7.x86_64.img
-rw-------   1 root root 23314553 Oct 25  2018 initramfs-3.10.0-862.11.6.el7.x86_64kdump.img
-rw-r--r--.  1 root root   305158 Aug 10  2018 symvers-3.10.0-862.11.6.el7.x86_64.gz
-rw-------.  1 root root  3414344 Aug 10  2018 System.map-3.10.0-862.11.6.el7.x86_64
-rwxr-xr-x.  1 root root  6398256 Aug 10  2018 vmlinuz-3.10.0-862.11.6.el7.x86_64
-rw-r--r--.  1 root root      171 Aug 10  2018 .vmlinuz-3.10.0-862.11.6.el7.x86_64.hmac
```

但是在Bootloader引导阶段，根文件系统还没有被加载，boot分区（sda1）也还没有被挂载到/boot目录下，而是grub直接访问该分区里的文件系统，所以boot分区的文件系统不能是任意的格式，而必须是GRUB可以识别的文件系统格式。

```bash
$ ls -al /dev/disk/by-uuid/e70dff6a-7c2b-4d2b-9b6b-54e4af258339
lrwxrwxrwx 1 root root 10 May 20 02:24 /dev/disk/by-uuid/e70dff6a-7c2b-4d2b-9b6b-54e4af258339 -> ../../sda1
$ fdisk /dev/sda
Welcome to fdisk (util-linux 2.23.2).

Changes will remain in memory only, until you decide to write them.
Be careful before using the write command.
Command (m for help): p
Disk /dev/sda: 526.1 GB, 526133493760 bytes, 1027604480 sectors
Units = sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disk label type: dos
Disk identifier: 0x00089227

   Device Boot      Start         End      Blocks   Id  System
/dev/sda1   *        2048     1026047      512000   83  Linux #大约500MB
/dev/sda2         1026048  1027604479   513289216   8e  Linux LVM

$ cat /boot/grub2/device.map 
# this device map was generated by anaconda
(hd0)      /dev/sda
(hd1)      /dev/sdb
(hd2)      /dev/sdc
(hd3)      /dev/sdd
```

Linux内核代码量是很庞大的，考虑到各个发行版本可能运行在不同的硬件环境上，那么内核所需要的各种驱动和模块也是不同的，如果把所有的驱动和模块都装进内核里，那么内核必然非常臃肿，且加载启动速度很慢。 因此，内核的大部分代码都以可加载模块的形式被组织起来，而内核本身始终保持非常精简。内核启动过程需要通过不同的驱动模块访问硬件设备和文件系统，一般地，这些驱动模块就被放进了`initrd`，它可以在根文件系统被加载之前提供虚拟的根文件系统。

>  关于什么是initrd可以参考：https://github.com/torvalds/linux/blob/master/Documentation/admin-guide/initrd.rst， 以及为什么需要initrd可以参考下面这个讨论帖：[[Why don't we include File System drivers in the kernel itself instead of using Initrd/Initramfs](https://unix.stackexchange.com/questions/332817/why-dont-we-include-file-system-drivers-in-the-kernel-itself-instead-of-using-i)

总之，Stage 2 Bootloader（包含了前面的Stage 1.5）会查询启动分区的文件系统，并将内核映像和initrd映像加载到内存中。 准备好映像后，Stage 2 Bootloader将调用内核映像。

## Kernel stage

在Stage2 Bootloader将内核映像加载到内存中并进行控制的情况下，内核阶段开始了。 但是，从boot分区加载的内核映像并不是可执行的内核，而是压缩的内核映像。 通常，这是zImage（压缩后的图像，小于512KB）或bzImage（大压缩后的图像，大于512KB），之前已使用zlib对其进行了压缩。 内核映像的顶部是一个例程（routine），该例程执行一些最少的硬件设置，然后解压缩包含在内核映像中的内核，并将其放入高地址内存中。 如果存在initrd，则此例程会将其移至内存中并记下以备后用。 然后，例程调用内核，内核启动开始。

参考下面的流程图，调用bzImage（用于i386映像）时，从执行./arch/i386/boot/head.S中的汇编例程`start()`开始，该例程执行一些基本的硬件设置，并在./arch/i386/boot/compressed/head.S中调用startup_32()例程。此例程设置基本环境（堆栈等），并清除符号开头的块（BSS，Block Started by Symbol ），然后通过调用称为`decompress_kernel`的C函数（位于./arch/i386/boot/compressed/misc.c中）对内核进行解压缩。当内核被解压缩到内存中时，另一个startup_32函数被调用，此函数在./arch/i386/kernel/head.S中。

![Major Functions in Linux Kernel i386 Boot Process](https://hunk-pic-store.oss-cn-beijing.aliyuncs.com/img/fig3.gif)

在新的`startup_32()`函数（也称为交换器或进程0）中，将初始化页表并启用内存分页。将检测CPU的类型以及任何可选的浮点单元（FPU），并将其存储起来以备后用。然后，调用`start_kernel`函数（init/main.c），该函数将切换到非体系结构特定的Linux内核。从本质上讲，这是Linux内核的主要功能。

通过调用`start_kernel()`，将调用一长串初始化函数来设置中断，执行进一步的内存配置并加载initrd。最后，调用kernel_thread（在arch/i386/kernel/process.c中）以启动init函数，这是第一个用户空间进程。最后，启动空闲任务，并且调度程序现在可以控制（在调用cpu_idle之后）。启用中断后，抢占式调度程序会定期进行控制以提供多任务处理。

在内核引导期间，将由Stage2 Bootloader加载到内存中的initrd复制到RAM中并进行安装。此initrd用作RAM中的临时根文件系统，并允许内核完全引导而不必装入任何物理磁盘。由于与外围设备接口所需的必要内核模块可能是initrd的一部分，因此内核可能很小，但仍支持大量可能的硬件配置，可以用`lsinitrd`命令来查看initrd包含了哪些内核模块。引导内核后，将根文件系统旋转（通过pivot_root），在其中卸载initrd根文件系统，并安装实际的根文件系统。

initrd功能允许我们使用编译为可加载模块的驱动程序来创建小型Linux内核，这些可加载模块为内核提供了访问磁盘和这些磁盘上的文件系统以及其他硬件资产的驱动程序的方法。 因为根文件系统是磁盘上的文件系统，所以initrd函数提供了一种引导方式来获取对磁盘的访问权并安装实际的根文件系统。 在没有硬盘的嵌入式目标中，initrd可以是最终的根文件系统，也可以通过网络文件系统（NFS）挂载最终的根文件系统。

## Systemd/Init stage

在Kernel stage末尾，Systemd（System Manger Daemon）或者更老的Init进程被启动，作为用户空间的第一个进程，负责按既定规则启动其他的服务或者进程。

关于systemd可以参考：

1. [Systemd Boot Process a Close Look in Linux](https://linoxide.com/systemd-boot-process/)

## References

[Inside the Linux boot process](https://developer.ibm.com/technologies/linux/articles/l-linuxboot/)

[Linux Boot Process Explained Step by Step in Detail](https://www.golinuxcloud.com/linux-boot-process-explained-step-detail/)

[Linux系统的启动过程](https://zhuanlan.zhihu.com/p/108084783)