#include "SmfModel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

std::vector<std::string> SmfModel::split_string (std::string line, std::string delimiter) {
	std::vector<std::string> tokens;
	size_t pos = 0;
	while ((pos = line.find(delimiter)) != std::string::npos) {
		if (pos != 0)
			tokens.push_back(line.substr(0, pos));
		line.erase(0, pos + delimiter.length());
	}
	tokens.push_back(line.substr(0, pos));
	return tokens;
}

SmfModel::SmfModel(const std::string& filepath, float scale, glm::vec4 color, glm::vec3 translation, glm::vec3 rotation)
	: m_FilePath(filepath), scale(scale), color(color), translation(translation), rotation(rotation) {
	std::ifstream stream(filepath);
	std::string line;

	while (getline(stream, line)) {
		if (line.find("#") != std::string::npos)
			continue;
		if (line.find("v ") != std::string::npos)
			m_Positions.push_back(parsePosition(line));
		else if (line.find("f ") != std::string::npos)
			m_Faces.push_back(parseFace(line));
	}
	m_PositionsSize = m_Positions.size() * 3;
	m_FaceCount = m_Faces.size() * 3;
}

SmfModel::SmfModel() {

}

SmfModel::~SmfModel(){

}

float* SmfModel::GetPositions(){
	float* positions = new float[m_PositionsSize];

	unsigned int i = 0;
	for (auto p : m_Positions) {
		positions[i++] = p.x;
		positions[i++] = p.y;
		positions[i++] = p.z;
	}
	return positions;
}
unsigned int* SmfModel::GetFaces(){
	
	unsigned int* faces = new unsigned int[m_FaceCount];

	unsigned int i = 0;
	for (auto f : m_Faces) {
		faces[i++] = f.v0;
		faces[i++] = f.v1;
		faces[i++] = f.v2;
	}
	return faces;

}

// Parse a line containing a vector
Position SmfModel::parsePosition(const std::string line) {
	Position smfpositions;
	std::vector<std::string> tokens = split_string(line, " ");

	smfpositions.x = std::stof(tokens[1]);
	smfpositions.y = std::stof(tokens[2]);
	smfpositions.z = std::stof(tokens[3]);

	return smfpositions;
}

// Parse a line containing a face
Face SmfModel::parseFace(std::string line) {
	Face smfface;
	std::vector<std::string> tokens;
	std::size_t found = line.find("/");
	if (found != std::string::npos) {
		// .Obj file. Get faces. Disregard other info
		 tokens = split_string(line, " ");
		 for (int i = 0; i < tokens.size(); i++) {
			 tokens[i] = split_string(tokens[i], "/")[0];
		 }
	}
	else {
		tokens = split_string(line, " ");
	}
	smfface.v0 = std::stoul(tokens[1]) - 1; // Subtract one to get proper index within vertex vector
	smfface.v1 = std::stoul(tokens[2]) - 1;
	smfface.v2 = std::stoul(tokens[3]) - 1;

	return smfface;
}

unsigned int SmfModel::GetPositionSize() {
	return m_PositionsSize;
}

unsigned int SmfModel::GetFaceCount() {
	return m_FaceCount;
}

std::string SmfModel::GetModelName() {
	std::vector<std::string> tokens = split_string(m_FilePath, "/");
	std::vector<std::string> file = split_string(tokens[2], ".");
	return file[0];
}