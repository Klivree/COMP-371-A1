#ifndef WALL_BUILDER_HEADER
#define WALL_BUILDER_HEADER

#include <string>
#include <iostream>
#include <fstream>


using namespace std;

string getWallFilePath(string shapeFilePath) {
	/** converts a shapeFilePath into the wall file path that is used by the wall builder application **/
	string wallFilePath(shapeFilePath); // copy the shape file path
	wallFilePath.resize(wallFilePath.size() - 4); // remove ".csv"
	wallFilePath.append(" - WALL.csv");

	return wallFilePath;
}

string buildWall(string shapeFilePath) {
	/** builds the wall for the shape to go through. Certain criteria must be met for it to work properly
	* 1. shape must only be made of 1x1 cubes (reader is no sophisticated enough)
	* 2. the positions of the shape cubes are integers so that the array-based drawing can work
	* 
	* This returns the filepath of the created .csv to facilitate incoorperating this into one liners:
	* example usage while creating a wall model: Model(buildWall(shapeFilePath), ... , ...);
	**/
	string wallFilePath = getWallFilePath(shapeFilePath);


	const int width = 9, height = 9;
	bool wallcoords[height][width]; // tells which cubes to fill
	// initialize whole wall to fill
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			wallcoords[i][j] = true;
		}
	}

	// open the shape
	ifstream shapeStream(shapeFilePath, ios::in);
	if (!shapeStream.is_open()) {
		cerr << "Could not read file " << shapeFilePath << ". File does not exist." << endl;
		return "";
	}

	string line, value;
	int i, j;
	int xy[2];
	char curChar;
	while (!shapeStream.eof()) {
		getline(shapeStream, line);
		line.append("\n");

		value = "";
		i = 0; // i is the character index in the line
		j = 0; // j is the number of floats in the line

		while (j < 2) { // we only care about the x-y coordinates
			curChar = line[i++];

			if (curChar == ',' || (curChar == '\n' && j >= 5)) {//need to push the info when these chars appear
				xy[j++] = (int) stof(value); // push float value to the array
				value = "";
			}
			else if (curChar != ' ') // ignore spaces
				value.append(string(1, curChar)); // add the char to the current value

			 // line is finished
			if (curChar == '\n' || curChar == '/')
				break;
		}
		
		if (j == 2) {
			// set the section of wall to empty
			wallcoords[xy[1] + ((width / 2))][xy[0] + ((height / 2))] = false; // add wall offsets to work with the array coords
		}
	}

	// open wall file
	ofstream wallStream(wallFilePath);
	if (!wallStream.is_open()) {
		cout << "ERROR: could not open: " << wallFilePath << "." << endl;
		return "";
	}

	// write the wall to the file
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (wallcoords[i][j]) // flag that indicates whether we draw in the square or not
				wallStream << j - ((width / 2) ) << "," << i - ((height / 2)) << ",0,1,1,1,\n"; // write line to file and remove the wall offsets to get correct position
		}
	}

	return wallFilePath; // we return the filepath
};


#endif