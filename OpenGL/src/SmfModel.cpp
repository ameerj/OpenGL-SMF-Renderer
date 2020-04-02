#include "SmfModel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

std::vector<std::string> SmfModel::split_string (std::string line) {
	std::istringstream iss(line);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
							  std::istream_iterator<std::string>{} };
	return tokens;
}

SmfModel::SmfModel(const std::string& filepath) : m_FilePath(filepath) {
	std::ifstream stream(filepath);
	std::string line;

	while (getline(stream, line)) {
		if (line.find("v") != std::string::npos)
			m_SMFPositions.push_back(parsePosition(line));
		else if (line.find("f") != std::string::npos)
			m_SMFFaces.push_back(parseFace(line));
	}
}

SmfModel::~SmfModel(){

}

Positions SmfModel::GetPositions(){
	Positions positions;
	positions.size = m_SMFPositions.size() * 3; // * 3 for x,y,z
	positions.positions = new float[positions.size];

	int i = 0;
	for (auto p : m_SMFPositions) {
		positions.positions[i++] = p.x;
		positions.positions[i++] = p.y;
		positions.positions[i++] = p.z;
	}
	return positions;
}
Faces SmfModel::GetFaces(){
	Faces faces;
	faces.count = m_SMFFaces.size() * 3; // * 3 for v0,v1,v2
	faces.faces = new unsigned int[faces.count];

	int i = 0;
	for (auto f : m_SMFFaces) {
		faces.faces[i++] = f.v0;
		faces.faces[i++] = f.v1;
		faces.faces[i++] = f.v2;
	}
	return faces;

}

// Parse a line containing a vector
SMFPosition SmfModel::parsePosition(const std::string line) {
	SMFPosition smfpositions;
	std::vector<std::string> tokens = split_string(line);

	smfpositions.x = std::stof(tokens[1]);
	smfpositions.y = std::stof(tokens[2]);
	smfpositions.z = std::stof(tokens[3]);

	return smfpositions;
}

// Parse a line containing a face
SMFFace SmfModel::parseFace(std::string line) {
	SMFFace smfface;
	std::vector<std::string> tokens = split_string(line);

	smfface.v0 = std::stoi(tokens[1]) - 1; // Subtract one to get proper index within vertex vector
	smfface.v1 = std::stoi(tokens[2]) - 1;
	smfface.v2 = std::stoi(tokens[3]) - 1;

	return smfface;
}