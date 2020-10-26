#!/bin/bash
# Script to generate PlantUML file for C++ projects
# Recursively looks through the current directory

echo "Trying to build UML diagrams from source folder .. "

FILES=$(find -name "*.hpp" | grep ".hpp")
NUM_FILES=$(ls -R | grep -c ".hpp")

echo "Found $NUM_FILES files: "
echo $FILES

for FILE in $FILES
do
	PUML_STRING+=' -i '
	PUML_STRING+=$FILE
done

echo "Running hpp2plantuml with above files as input arguments .. "
hpp2plantuml $PUML_STRING -o plantuml_output.puml