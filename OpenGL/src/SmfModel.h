#pragma once
#include <string>
#include <vector>

struct SMFPosition {
	float x;
	float y;
	float z;
};
struct SMFFace {
	int v0;
	int v1;
	int v2;
};

struct Positions {
	unsigned int size;
	float* positions;
};

struct Faces {
	unsigned int count;
	unsigned int* faces;
};

class SmfModel {
private:
	std::string m_FilePath;
	std::string m_CurrentLine;
	std::vector<SMFPosition> m_SMFPositions;
	std::vector<SMFFace> m_SMFFaces;
public:
	SmfModel(const std::string& filepath);
	~SmfModel();

	Positions GetPositions();
	Faces GetFaces();
private:
	// Parse a line containing a vector
	SMFPosition parsePosition(std::string line);

	// Parse a line containing a face
	SMFFace parseFace(std::string line);

	// Split string on space delimiter
	std::vector<std::string> split_string(std::string line);

};