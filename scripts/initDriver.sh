#!/bin/bash

# This script will initialzie the plugin template for development,
# and set up a version control system using git.

if [ "$#" -ne 2 ]; then
    echo "Please enter the following command line args"
    echo "Driver name: ex: Prosilica - Do not add AD to the front that will be done manually"
    echo "Driver name short: ex: EmergentVision -> EVT. If no shortened name, write NONE"
    exit
fi

echo "------------------------------------"
echo "Updating ADDriverTemplate with name:"
echo $1

python3 update_name.py -n $1 -s $2

cd ..

echo "" >> .gitignore
echo "scripts/" >> .gitignore
echo "TemplateReadme.md" >> .gitignore

mv README.md TemplateReadme.md

DriverName="${1,,}"
DriverName="${DriverName^}"

touch README.md
echo "# AD$DriverName" >> README.md
echo "This driver is currently under development." >> README.md

git init
git add -A

git commit -m "Initial Commit for AD$PluginName"

