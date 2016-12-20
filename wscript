from os import listdir
from os.path import splitext

flags = ''

def options(opt):
    opt.load('compiler_c')

def configure(cnf):
    cnf.load('compiler_c')

def build(bld):
    bld.program(source='sh1.c', 
                target='sh1', 
                cflags=flags)
    sources = [f for f in listdir(".") if f.endswith('.c')]
    for f in sources:
        bld.program(source="/"+f,
                    target="/"+splitext(f)[0],
                    cflags=flags)