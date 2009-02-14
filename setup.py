
from distutils import sysconfig
from sys import argv, version_info


def version():
    print "%d.%d" % version_info[0:2]


def libs():
    print sysconfig.get_python_lib(standard_lib=1)


def include():
    print sysconfig.get_python_inc()


def prefix():
    print sysconfig.PREFIX


if argv[1] == "version": version()
if argv[1] == "libs": libs()
if argv[1] == "include": include()
if argv[1] == "prefix": prefix()


