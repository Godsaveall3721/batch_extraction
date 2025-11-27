# MFS目录结构导及其导出方法
这个项目借助hfsutils工具在
在Linux实现对属于MFS/HFS文件系统的.iso/.img/.dsk镜像文件
打印目录文件树，以及导出整体目录文件结构。

---

## 第一步：安装hfsutils

    #Arch Linux
    sudo pacman -S hfsutils

    #Debain
    sudo apt install hfsutils

## 第二步：手动挂载一个镜像
    hmount xxx.img
    hmount xxx.iso
    hmount xxx.dsk

## 第三步：执行该程序
    #可以通过 --path "绝对路径" 来指定导出路径
    #如不指定则默认导出到当前目录
    ./batch_extraction --path "/home/x-pc/Desktop/c++TEMP/batch_extraction/bazel-bin/main/"
    提示： 可以通过 --path "绝对路径" 来指定导出路径
    如不指定则默认导出到当前目录
    
    接收到导出路径: "/home/x-pc/Desktop/c++TEMP/batch_extraction/bazel-bin/main/"
    [DIR] /
    [DIR] System Tools 1
        [DIR] System Folder
        [FILE] Clipboard File
        [FILE] Color
        [FILE] DA�Handler
        [FILE] Easy Access
        [FILE] Finder
        [FILE] General
        [FILE] Key Layout
        [FILE] Keyboard
        [FILE] Monitors
        [FILE] Mouse
        [FILE] MultiFinder
        [FILE] Scrapbook File
        [FILE] Sound
        [FILE] Startup Device
        [FILE] System
    [FILE] TeachText
    [DIR] Update Folder
        [FILE] Read Me
    [DIR] Utilities Folder
        [FILE] Apple HD SC Setup
        [FILE] Installer
        [DIR] Installer Scripts
            [FILE] Macintosh II Script
            [FILE] Macintosh Plus Script
            [FILE] Macintosh SE Script
    [DIR] System Tools 2
        [FILE] Installer
        [DIR] System Folder
            [FILE] AppleTalk ImageWriter
            [FILE] Background Printing
            [FILE] Backgrounder
            [FILE] Clipboard File
            [FILE] Color
            [FILE] DA�Handler
            [FILE] Easy Access
            [FILE] Finder
            [FILE] General
            [FILE] ImageWriter
            [FILE] Key Layout
            [FILE] Keyboard
            [FILE] Laser Prep
            [FILE] LaserWriter
            [FILE] Mouse
            [FILE] MultiFinder
            [FILE] PrintMonitor
            [FILE] Scrapbook File
            [FILE] Sound
            [DIR] Spool Folder
            [FILE] Startup Device
            [FILE] System
        [FILE] TeachText
        [DIR] Update Folder
            [FILE] Read Me
    [DIR] Utilities 1
        [FILE] Apple HD SC Setup
        [FILE] Disk First Aid
        [FILE] HDBackup
        [DIR] System Folder
            [FILE] Clipboard File
            [FILE] Color
            [FILE] DA�Handler
            [FILE] Easy Access
            [FILE] Finder
            [FILE] General
            [FILE] Key Layout
            [FILE] Keyboard
            [FILE] Monitors
            [FILE] Mouse
            [FILE] MultiFinder
            [FILE] Scrapbook File
            [FILE] Sound
            [FILE] Startup Device
            [FILE] System
        [FILE] TeachText
        [DIR] Update Folder
            [FILE] Read Me
    [DIR] Utilities 2
        [DIR] Apple File Exchange Folder
            [FILE] Apple File Exchange
            [FILE] DCA-RFT/MacWrite
        [DIR] Font/DA Mover Folder
            [FILE] Desk Accessories
            [FILE] Font/DA Mover
            [FILE] Fonts
    导出成功

## 第四步： 优化文件/目录名和消去非法符号

    #Arch Linux
    sudo pacman -S convmv detox

    #Debain
    sudo apt install convmv detox

    #消除旧MFS时代的非法字符
    convmv -r -f cp1252 -t utf-8 --notest ./

    #消除目录/文件的空格，格式化路径
    detox -r -v ./
