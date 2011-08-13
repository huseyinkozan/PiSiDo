#!/usr/bin/python
# -*- coding: utf-8 -*-
# Licensed under the GNU General Public License, version 3.
# See the file http://www.gnu.org/copyleft/gpl.txt

from pisi.actionsapi import kde4
#from pisi.actionsapi import pisitools

# You can use these as variables, they will replace GUI values before build.
# Package Name : __package_name__
# Version : __version__
# Summary : __summary__

# If the project that you are tying to compile is in a sub directory in the 
# source archive, than you can define working directory. For example; 
# WorkDir="__package_name__-"+ get.srcVERSION() +"/sub_project_dir/"

def setup():
    kde4.configure()

def build():
    kde4.make()

def install():
    kde4.install()

# Take a look at the source folder for these file as documentation.
#    pisitools.dodoc("AUTHORS", "BUGS", "ChangeLog", "COPYING", "README")
# If there is no install rule for a runnable binary, you can 
# install it to binary directory.
#    pisitools.dobin("__package_name__")
