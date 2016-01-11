#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that these additional
# terms apply under section 7:
#
#   No right, title or interest in or to any trade mark, service mark, logo or
#   trade name of of National ICT Australia Limited, ABN 62 102 206 173
#   ("NICTA") or its licensors is granted. Modified versions of the Program
#   must be plainly marked as such, and must not be distributed using
#   "eChronos" as a trade mark or product name, or misrepresented as being the
#   original Program.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# @TAG(NICTA_AGPL)
#

from prj import execute, SystemBuildError
import os
import pickle


def run(system, configuration=None):
    return system_build(system)


def system_build(system):
    print("== In TI build function! ==")

    ti_lib_dir = os.getcwd() + "/packages/machine-tm4c1294-launchpad/ti_libs/"
    
    system.add_include_path(ti_lib_dir)
    system.add_include_path(ti_lib_dir + "inc")
    system.add_include_path(ti_lib_dir + "stdlib_hook")
    system.add_include_path(ti_lib_dir + "examples/boards/ek-tm4c1294xl")
    system.add_include_path(ti_lib_dir + "third_party/lwip-1.4.1/src/include")
    system.add_include_path(ti_lib_dir + "third_party/lwip-1.4.1/apps")
    system.add_include_path(ti_lib_dir + "third_party/lwip-1.4.1/ports/tiva-tm4c129/include")
    system.add_include_path(ti_lib_dir + "third_party/lwip-1.4.1/src/include/ipv4")
    system.add_include_path(ti_lib_dir + "third_party")
    system.add_include_path(ti_lib_dir + "../projects/util")

    cpu_fpu = [ '-mfpu=fpv4-sp-d16', '-mfloat-abi=softfp']

    inc_path_args = ['-I%s' % i for i in system.include_paths]

    # All this code is to pickle include paths to a file for autocompletion tags
    ycmIncludes = []
    for path in system.include_paths:
        if path[0] != '/':
            ycmIncludes.append( os.getcwd() + '/' + path )
        else:
            ycmIncludes.append( path )

    ycm_path_args = ['-I%s' % i for i in ycmIncludes]

    print("== DUMPING YCM INCLUDES ==")
    print(ycm_path_args)

    pickle.dump( ycm_path_args, open( 'ycm_prj_include_dump.p', 'wb'), protocol=2 )

    common_flags = ['-mthumb', '-march=armv7-m', '-g3'] + cpu_fpu
    a_flags = common_flags
    c_flags = common_flags + ['-Os', '-DTARGET_IS_TM4C129_RA0', '-DPART_TM4C1294NCPDT', '-Dgcc', '-std=gnu99', '-g', '-DDEBUG' ]


    # Compile all C files.
    c_obj_files = [os.path.join(system.output, os.path.basename(c.replace('.c', '.o'))) for c in system.c_files]

    for c, o in zip(system.c_files, c_obj_files):
        os.makedirs(os.path.dirname(o), exist_ok=True)
        execute(['arm-none-eabi-gcc', '-ffreestanding', '-c', c, '-o', o, '-Wall'] +
                c_flags + inc_path_args)

    # Assemble all asm files.
    asm_obj_files = [os.path.join(system.output, os.path.basename(s.replace('.s', '.o'))) for s in system.asm_files]
    for s, o in zip(system.asm_files, asm_obj_files):
        os.makedirs(os.path.dirname(o), exist_ok=True)
        execute(['arm-none-eabi-as', '-o', o, s] + a_flags + inc_path_args)

    #HACKY SOLUTION
    #(Before modules written)
    #c_obj_files.append( ti_lib_dir + "driverlib/gcc/libdriver.a" )
    #c_obj_files.append( ti_lib_dir + "examples/boards/ek-tm4c1294xl/hello/gcc/uartstdio.o" )
    #c_obj_files.append( ti_lib_dir + "examples/boards/ek-tm4c1294xl/hello/gcc/pinout.o" )

    # Perform final link
    obj_files = asm_obj_files + c_obj_files
    execute(['arm-none-eabi-ld', '-T', system.linker_script, '-o', system.output_file ] + obj_files)
