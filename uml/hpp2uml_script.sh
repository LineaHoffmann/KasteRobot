#!/bin/bash
# Script to generate PlantUML file for C++ projects
# Recursively looks through the current directory
# Written by Søren Pedersen

echo "Trying to build UML diagrams recursively from parent folder .. "

PARENT_DIR="$(dirname "$PWD")"

echo "Parent folder: $PARENT_DIR"

PLANTUML_SRC_PATH=$(find /usr -name "plantuml.jar")

if [ "$PLANTUML_SRC_PATH" == "" ]; then
	echo "PlantUML doesn't seem to be installed. It's available through apt."
	exit
fi

HPP2PLANTUML_SRC_PATH=$(find /usr -name "hpp2plantuml.py")

if [ "$HPP2PLANTUML_SRC_PATH" == "" ]; then
	echo "hpp2plantuml doesn't seem to be installed. It's available through pip."
	exit
fi

FILES=$(find $PARENT_DIR -name "*.hpp" | grep ".hpp")
NUM_FILES=$(ls -R $PARENT_DIR | grep -c ".hpp")

if [ $NUM_FILES -eq 0 ]; then
	echo "No files found, stopping .. "
	exit
fi

echo "Found $NUM_FILES files: "
echo "$FILES"

for FILE in $FILES
do
	PUML_STRING+=' -i '
	PUML_STRING+=$FILE
done

echo "Running hpp2plantuml with above files as input arguments .. "
hpp2plantuml $PUML_STRING -o plantuml_output.puml

echo "Running PlantUML on generated .puml file .."
cat plantuml_output.puml | java -jar $PLANTUML_SRC_PATH -pipe > uml_diagram.pdf

echo "Complete. I've made two files for you, a pdf and a puml."