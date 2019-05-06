# Python script used to replace the name of the plugin
# whereever applicable to simplify using the template.
# The script can be run with:
#
# python3 update_name.py -n NAME
#
# where NAME is the name of the plugin you are developing
#

import os
import argparse
import shutil

def walk_through_dirs(driverNameLowerShort, driverNameStandard):
    for dirpath, dirnames, filenames in os.walk(".."):
        for dirname in dirnames:
            if ".git" not in os.path.join(dirpath, dirname):
                newName = update_dir_file_name(os.path.join(dirpath, dirname), driverNameLowerShort, driverNameStandard)
                os.rename(os.path.join(dirpath, dirname), newName)


def walk_through_files(driverNameLower, driverNameLowerShort, driverNameStandard):
    for dirpath, dirnames, filenames in os.walk(".."):
        for filename in filenames:
            if ".git" not in os.path.join(dirpath, filename):
                newName = update_dir_file_name(os.path.join(dirpath, filename), driverNameLowerShort, driverNameStandard)
                os.rename(os.path.join(dirpath, filename), newName)
                update_sources(newName, driverNameLowerShort, driverNameLower, driverNameLower.upper(), driverNameStandard)



# Updates the filenames of all of the plugin specific files. Also updates the contents of all files that are not in the src dir
def update_dir_file_name(path, driverNameLowerShort, driverNameStandard):
    if "DRIVERNAMESTANDARD" in path:
        return path.replace("DRIVERNAMESTANDARD", driverNameStandard)
    elif "DRIVERNAMELOWERSHORT" in path:
        return path.replace("DRIVERNAMELOWERSHORT", driverNameLowerShort)


# Reads file line by line and updates specific locations with correct plugin name
def update_sources(path, lower_short, all_lowercase, all_uppercase, standard_name):
    os.rename(path, path+"_OLD")
    oldFile = open(path+"_OLD", "r")
    newFile = open(path, "w")

    line = oldFile.readline()
    while(line):
        if "DRIVERNAMESTANDARD" in line:
            line = line.replace("DRIVERNAMESTANDARD", standard_name)
        if "DRIVERNAMEUPPER" in line:
            line = line.replace("DRIVERNAMEUPPER", all_uppercase)
        if "DRIVERNAMELOWER" in line:
            line = line.replace("DRIVERNAMELOWER", all_lowercase)
        if "DRIVERNAMELOWERSHORT" in line:
            line = line.replace("DRIVERNAMELOWERSHORT", lower_short)
        newFile.write(line)
        line = oldFile.readline()

    oldFile.close()
    newFile.close()
    os.remove(path+"_OLD")



# Renames the plugin root directory to reflect the new plugin name
def update_root_dir(standard_name):
    for directory_name in os.listdir("../.."):
        if "ADDriverTemplate" in directory_name:
            shutil.move("../../"+directory_name, "../../AD"+standard_name)




# Parses name from user args in command line
def parse_args():
    parser = argparse.ArgumentParser(description="Update driver names in template")
    parser.add_argument('-n', '--name', help = 'Name of the driver without AD, ex. EmergentVision')
    parser.add_argument('-s', '--short', help = 'Shorthand name for the driver. Ex. EmergentVision shorthand could be evt. If not used, full name will be shorthand')
    arguments = vars(parser.parse_args())

    if arguments["name"] is not None:
        name = arguments["name"]
        all_lowercase = name.lower()
        standard_name = name
        lower_short = ""

        if arguments["short"] is not None:
            lower_short = arguments[short].lower()
        else:
            lower_short = all_lowercase

        update_root_dir(standard_name)
        walk_through_dirs(lower_short, standard_name)
        walk_through_files(all_lowercase, lower_short, standard_name)

    else:
        print("Error, no plugin name specified")


# calls other functions
parse_args()
