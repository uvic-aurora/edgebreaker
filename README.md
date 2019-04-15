Edgebreaker Triangle-Mesh Coder Software Implementation
=======================================================

This code was developed by Yue (Freida) Tang as part of her Master's of
Engineering project.

The project report can be found in the directory report.  The source code
can be found in the directory src.  The design document for the software
can be found in the directory doc.  Some example triangle mesh data files
(in OFF format) can be found in the directory data.

Frieda can be reached at the following email address:
  tangfrieda@gmail.com

Installation
============

In what follows, let $TOP_DIR denote the top-level directory of the
Edgebreaker software distribution (i.e., the directory containing this
README file); let $BUILD_DIR denote a new directory to be created for
building the software; and let $INSTALL_DIR denote the directory in
which to install the Edgebreaker software.

To install the Edgebreaker software with CMake, use the command sequence:

  cmake -H$TOP_DIR -B$BUILD_DIR -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
  cmake --build $BUILD_DIR --target install

Using the Software
==================

Detailed information on the use of the software can be found in
Chapter 4 (titled "Software") of the MEng project report in the
report directory.

