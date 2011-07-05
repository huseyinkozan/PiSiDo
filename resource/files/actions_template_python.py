#!/usr/bin/python
# -*- coding: utf-8 -*-
# Licensed under the GNU General Public License, version 3.
# See the file http://www.gnu.org/copyleft/gpl.txt

# Use this as variables:
# Package Name : ___package_name___
# Version : ___version___
# Summary : ___summary___

from pisi.actionsapi import pythonmodules
from pisi.actionsapi import pisitools

# if archive have project in a sub directory:
# WorkDir="___package_name___-"+ get.srcVERSION() +"/sub_project_dir/"

def build():
    pythonmodules.compile()

def install():
    pythonmodules.install()

#    pisitools.dodoc("AUTHORS", "BUGS", "ChangeLog", "COPYING", "NEWS", "README")
#    pisitools.dobin("___package_name___")
