#!/usr/bin/python
# -*- coding: utf-8 -*-
# Licensed under the GNU General Public License, version 3.
# See the file http://www.gnu.org/copyleft/gpl.txt

# Use this as variables:
# Package Name : ___package_name___
# Version : ___version___
# Summary : ___summary___

from pisi.actionsapi import pisitools
from pisi.actionsapi import qt4

# if archive have project in a sub directory:
# WorkDir="___package_name___-"+ get.srcVERSION() +"/sub_project_dir/"

def setup():
    qt4.configure()

def build():
    qt4.make()

def install():
    qt4.install()

#    pisitools.dodoc("AUTHORS", "BUGS", "ChangeLog", "COPYING", "NEWS", "README")
#    pisitools.dobin("___package_name___")