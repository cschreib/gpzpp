# GPz++

[![Build Status](https://travis-ci.com/cschreib/gpzpp.svg?branch=master)](https://travis-ci.com/cschreib/gpzpp)

<!-- MarkdownTOC autolink=true levels=1,2 -->

- [Description](#description)
- [Install instructions](#install-instructions)
- [Acknowledgments](#acknowledgments)

<!-- /MarkdownTOC -->


# Description

This is a C++ version of the machine-learning photometric redshift code [GPZ](https://github.com/OxfordML/GPz) (Almosallam et al. 2061a,2016b,in prep.). Below is a list of the main selling points:

 - GPz++ is free software and does not require a MatLab license.
 - GPz++ runs up to 20 times faster than the MatLab version.
 - GPz++ implements GPz v2.0, with support for noisy and missing data.

If you use this code for your own work, please cite this repository, as well as Almosallam et al. (2016a,2016b,in prep.) where GPz was first introduced.


# Install instructions

To install GPz++, you first need to build it. For this, you will need a recent C++ compiler, [git](https://git-scm.com/) and [CMake](https://cmake.org/). All of these tools are available by default on most modern linux distributions, and for MacOS you may need to install them with MacPorts. From there installing GPz++ is very easy. Navigate to a directory of your choosing where the code will be downloaded (it will be placed in a subdirectory called ```gpzpp```), then execute the following commands:
```
# Download the code and dependencies
git clone https://github.com/cschreib/gpzpp.git
cd gpzpp

# Compile
mkdir build
cd build
cmake ../
make install
```

This will create an executable called ```gpz++``` in the ```gpzpp/bin``` directory, which you can use immediately.


# Acknowledgments

The C++ version of GPz was developed by Corentin Schreiber for the Euclid space mission, with funding from the UK Space Agency, under the supervision of Matt Jarvis.

Particular thanks go to the author of the original GPz implementation, Ibrahim Almosallam.
