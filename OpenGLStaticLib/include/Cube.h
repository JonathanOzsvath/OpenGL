#pragma once

class Cube
{

private:
	unsigned int vaoHandle;

public:
	Cube(float, bool);

	void render();
};