#include "glm/glm.hpp"

class Table
{
	float barG, tableL, tableW, tableH, legH, legG;
	glm::vec4 tableClr;

public:
    Table( float wid, float len, int id );
    void render(int programID);
};
