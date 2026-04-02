import sys
from SCons.Script import *

env = Environment()
project_full_name = "OpenGML 0.8.0"
env.Append(CPPDEFINES={"VERSION":project_full_name})
env.Program("test_version2.cpp")
