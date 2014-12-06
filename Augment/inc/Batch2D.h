#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "ShaderProgram.h"


class Batch2D
{
public:

	static unsigned int batchSize;

	Batch2D();
	~Batch2D();

	//Performs necessary batch initialization.
	void Begin();

	//Sets the texture for the following Draw calls
	//texture: the Texture2D object to draw
	//sourceRect: a rectangle(x,y,width,height) specifying where to sample from the texture
	void SetTexture(Texture2D& texture, glm::vec4* sourceRect = nullptr);

	//Adds an instance of the set texture to be rendered
	//position: the location to draw the texture, in screen space coordinates. If nullptr default to (0,0).
	//rotation: (aroundXAxis, aroundYAxis, aroundZAxis) the rotations to apply to the texture.  If nullptr default to (0,0,0).
	//scale: (x,y) the scale to apply along each axis.  If nullptr default to (1,1).
	//origin: the point relative to the texture where transforms are applied (ie rotated around, translated to).  If nullptr defaults to (0,0).
	//tint: the color to tint the sprite
	void Draw(
		const glm::vec2& position = glm::vec2(0, 0),
		const glm::vec3& rotation = glm::vec3(0, 0, 0),
		const glm::vec2& scale = glm::vec2(1, 1),
		const glm::vec2& origin = glm::vec2(0, 0),
		const glm::vec4& tint = glm::vec4(1,1,1,1));

	//Draws all textures and resets state
	void End();

private:

	struct Vertex {
		glm::vec2 position;
		glm::vec2 texCoords;
		glm::vec4 color;
	};

	//vertex array object, vertex buffer
	UniqueVertexArray vao;
	UniqueBuffer vertexBuff, indexBuff;

	ShaderProgram shaderProgram;

	Texture2D* texture;

	unsigned int instanceCount;

	Vertex* mappedVertexBuff;
	GLushort* mappedIndexBuff;

	const static unsigned int vertexPerSprite = 4;
	const static unsigned int indexPerSprite = 6;

	glm::vec2 sourceDim;
	glm::vec2 sourceTexCoords[4];

	void Flush();
	void MapBuffers();
	void UnmapBuffers();
};