<!-- LC_NOTICE_BEGIN
===============================================================================
|                        Copyright (C) 2021 Luca Ciucci                       |
|-----------------------------------------------------------------------------|
| Important notices:                                                          |
|  - This work is distributed under the MIT license, feel free to use this    |
|   work as you wish.                                                         |
|  - Read the license file for further info.                                  |
| Written by Luca Ciucci <luca.ciucci99@gmail.com>, 2021                      |
===============================================================================
LC_NOTICE_END -->

# *Visual Studio* Tutorial

Using *VS* with *CMake* is a bit different story, please read this tutorial if you are going to use *VS*.

## Installing *Visual Studio*
Go to the [VS download page](https://visualstudio.microsoft.com/it/downloads/) and download the installer.

Follow the instructions and select (at least) the `C++ desktop development tools`, this includes CMake support:

![C++ desktop development tools](img/VS_guide/cpp_desktop_component.png)

## Opening the *Gnuplot++* project

Open VS and select *"Continue Without Code"*
![](img/VS_guide/continue_without_code.png)

Then select `File->Open->CMake...` and open the *Gnuplot++* folder:

![](img/VS_guide/open_cmake.png)

## Manage configurations

VS support multiple configurations, you can change them:

![](img/VS_guide/manage_configurations.png)

You will see something like this:

![](img/VS_guide/manage_configurations2.png)

You can change settings using the cmake variables table.

Now we need to set the install folder. Click on *Show advanced settings*:

![](img/VS_guide/show_advanced_settings.png)

Now set your favourite install location for libraries:

![](img/VS_guide/install_dir.png)

## Add the install folder to PATH

Add the following entries to the system PATH:
```
your_libraries_install_folder/lib
your_libraries_install_folder/lib/cmake
your_libraries_install_folder/bin
```

## Build the library:

To build the library, select `Build->Build All`:

![](img/VS_guide/build_all.png)

## Install the library:

Select `Build->Install gnuplotpp`.

![](img/VS_guide/install.png)

## Using the library from an external project

Create a new CMake project as described in [BuildAndInstall](BuildAndInstall.md), use the above steps to open the new project and you are ready to go!

Note that to run an executable target you can click the Run button, for example:

![](img/VS_guide/run.png)