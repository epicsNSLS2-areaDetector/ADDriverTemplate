"""
Python script used to replace the name of the plugin
whereever applicable to simplify using the template.
The script can be run with:

python3 update_name.py -n NAME -s SHORTHAND

where NAME is the name of the plugin you are developing
"""

import os
import argparse
import shutil
from sys import platform


def walk_through_dirs(driverNameLowerShort, driverNameStandard):
    """ Function that walks through directories, and replaces their names """

    paths = []
    for dirpath, dirnames, filenames in os.walk(".."):
        for dirname in dirnames:
            if ".git" not in os.path.join(dirpath, dirname):
                newName = update_dir_file_name(dirpath, dirname, driverNameLowerShort, driverNameStandard)
                paths.append([os.path.join(dirpath, dirname), newName])

    # Need to reverse to not break paths
    paths.reverse()
    for path in paths:
        os.rename(path[0], path[1])
        #print("Replacing {}".format(path))

def walk_through_files(driverNameLower, driverNameLowerShort, driverNameStandard):
    """ Function that walks through files and calls the update sources on them """

    for dirpath, dirnames, filenames in os.walk(".."):
        for filename in filenames:
            if ".git" not in os.path.join(dirpath, filename):
                newName = update_dir_file_name(dirpath, filename, driverNameLowerShort, driverNameStandard)
                os.rename(os.path.join(dirpath, filename), newName)
                update_sources(newName, driverNameLowerShort, driverNameLower, driverNameLower.upper(), driverNameStandard)


def update_dir_file_name(path, name, driverNameLowerShort, driverNameStandard):
    """
    Updates the filenames of all of the driver specific files. Also updates the contents of all files that are not in the src dir
    """

    if "DRIVERNAMESTANDARD" in name:
        return os.path.join(path, name.replace("DRIVERNAMESTANDARD", driverNameStandard))
    elif "DRIVERNAMELOWERSHORT" in name:
        return os.path.join(path, name.replace("DRIVERNAMELOWERSHORT", driverNameLowerShort))
    else:
        return os.path.join(path, name)


def update_sources(path, lower_short, all_lowercase, all_uppercase, standard_name):
    """
    Reads file line by line and updates specific locations with correct driver name
    """

    if path != "update_names.py":
        os.rename(path, path+"_OLD")
        oldFile = open(path+"_OLD", "r")
        newFile = open(path, "w")

        line = oldFile.readline()
        while(line):
            if "DRIVERNAMESTANDARD" in line:
                line = line.replace("DRIVERNAMESTANDARD", standard_name)
            if "DRIVERNAMEUPPER" in line:
                line = line.replace("DRIVERNAMEUPPER", all_uppercase)
            if "DRIVERNAMELOWERSHORT" in line:
                line = line.replace("DRIVERNAMELOWERSHORT", lower_short)
            if "DRIVERNAMELOWER" in line:
                line = line.replace("DRIVERNAMELOWER", all_lowercase)
            newFile.write(line)
            line = oldFile.readline()

        oldFile.close()
        newFile.close()
        os.remove(path+"_OLD")


def update_root_dir(standard_name):
    """ Renames the driver root directory to reflect the new driver name """

    for directory_name in os.listdir("../.."):
        if "ADDriverTemplate" in directory_name:
            shutil.move("../../"+directory_name, "../../AD"+standard_name)


def parse_args():
    """ Parses name from user args in command line. No longer used, must be called from initDriver.py """

    parser = argparse.ArgumentParser(description="Update driver names in template")
    parser.add_argument('-n', '--name', help = 'Name of the driver without AD, ex. EmergentVision')
    parser.add_argument('-s', '--short', help = 'Shorthand name for the driver. Ex. EmergentVision shorthand could be evt. If not used, full name will be shorthand')
    arguments = vars(parser.parse_args())

    if arguments["name"] is not None:
        run_all(arguments['name'], arguments['short'])

    else:
        print("Error, no plugin name specified")


def run_all(in_name, in_shorthand):
    """ Executes the update process """

    name = in_name
    all_lowercase = name.lower()
    standard_name = name
    if in_shorthand is not None:
        lower_short = in_shorthand.lower()
    else:
        lower_short = all_lowercase
    #print('Updating name references with standard name {}, and shorthand name {}'.format(name, lower_short))
    if platform == 'linux':
        print('Renaming root directory')
        update_root_dir(standard_name)
    else:
        print('Windows arch detected. Cannot update root dir due to permission issue.')
        print('Please remember to rename ADDriverTemplate to {}'.format('AD' + standard_name))
    print('Renaming directories...')
    walk_through_dirs(lower_short, standard_name)
    print('Renaming files...')
    walk_through_files(all_lowercase, lower_short, standard_name)



# calls other functions
# parse_args()
# walk_through_dirs("uvc", "UVC")
