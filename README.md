# Playful Palette
## Building

The assets are store using git lfs.
https://git-lfs.github.com/


This project requires Qt5 sdk.
For windows, once QT5 sdk have been installed you need to add it to your path.
"Edit the system environment variables" -> "Environment Variables..." -> Select Path -> Edit -> New 
Then write the path to the bin directory of your Qt5 install : 
For example : 
"C:\Qt\5.12.3\msvc2017_64\bin"

Other dependencies are managed using git submodule.
To initialize the submodules and fetch the appropriate commit use :

```
    $ git submodule init
    $ git submodule update
```

Or directly clone the repository with the submodule using :

```
    git clone --recurse-submodules git@gitlab.com:tlambert/playful-palette.git
```

Then to build the application
```
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
	$ ./viewer
```

On Windows with visual studio 2017, File->Open->CMake and then select the CMakeLists.txt at the root of the project.
Once cmake configuration done choose viewer.exe as startup item.