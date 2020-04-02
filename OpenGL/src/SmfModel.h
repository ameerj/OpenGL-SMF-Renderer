#pragma once
#include <string>
#include <vector>

struct Position {
	float x;
	float y;
	float z;
};
struct Face {
	int v0;
	int v1;
	int v2;
};

class SmfModel {
private:
	std::string m_FilePath;
	std::string m_CurrentLine;
	std::vector<Position> m_Positions;
	unsigned int m_PositionsSize;
	std::vector<Face> m_Faces;
	unsigned int m_FaceCount;

public:
	SmfModel(const std::string& filepath);
	~SmfModel();

	float* GetPositions();
	unsigned int GetPositionSize();
	unsigned int* GetFaces();
	unsigned int GetFaceCount();
private:
	// Parse a line containing a vector
	Position parsePosition(std::string line);

	// Parse a line containing a face
	Face parseFace(std::string line);

	// Split string on space delimiter
	std::vector<std::string> split_string(std::string line);

};