#!/usr/bin/python3

"""
This script will initialzie the plugin template for development,
and set up a version control system using git.
"""

__author__      = "Jakub Wlodek"
__copyright__   = "Copyright June 2019, Brookhaven Science Associates"
__license__     = "GPL"
__version__     = "R2-0"
__maintainer__  = "Jakub Wlodek"


import argparse
import subprocess
import update_names
import os


# ------------ Helper functions -------------

def parse_user_args():
    parser = argparse.ArgumentParser(description="Initialize ADDriver from template")
    parser.add_argument('-o', '--only-update', action='store_true', help='Flag that forces initDriver.py to only apply the update names script, not initialize version control.')
    parser.add_argument('-n', '--name', help="Flag representing new driver's name, without 'AD' at the start. ex. EmergentVision")
    parser.add_argument('-s', '--shorthand', help="Shorthand version of the new driver's name. ex. for EmergentVision it could be EVT. Optional arg.")
    arguments = vars(parser.parse_args())
    if arguments['name'] is None:
        print('ERROR - Please specifiy a driver name, run with -h for help.')
    else:
        return arguments['name'], arguments['shorthand'], arguments['only-update']



# -------------- Run script ------------------


name, shorthand, only_update = parse_user_args()
if name.startswith('AD'):
    print('Detected name starting with AD.')
    name = name[2:]


print("+-----------------------------------------------------------------")
print("+ ADDriverTemplate, version: {}                               +".format(__version__))
print("+ Author: Jakub Wlodek                                           +")
print("+ Copyright (c): Brookhaven National Laboratory 2018-2019        +")
print("+ This software comes with NO warranty!                          +")
print("+-----------------------------------------------------------------")
print()


print("Updating ADDriverTemplate with name: {}".format(name))
if shorthand is not None:
    print("Shorthand name: {}".format(shorthand))


update_names.run_all(name, shorthand)

if only_update:
    print('Only update option selected, exiting...')
    exit()

os.chdir('..')

gitignore_fp = open('gitignore', 'a')
gitignore_fp.write('scripts/\n')
gitignore_fp.close()

os.rename('README.md', 'TemplateReadme.md')

readme_fp = open('README.md', 'w')
readme_fp.write('# AD' + name + '\n')
readme_fp.write('This driver is currently under development\n')
readme_fp.close()

os.remove('LICENSE')
license_fp = open('LICENSE', 'w')
license_fp.close()

subprocess.call(['git', 'init'])
subprocess.call(['git', 'add', '-A'])
subprocess.call(['git', 'commit', '-m', 'Initial Commit for AD' + name])

print('AD' + name + ' driver initialized from ADDriverTemplate successfully.')