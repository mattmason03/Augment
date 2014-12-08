#include "Batch2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Utils.h"

unsigned int Batch2D::batchSize = 1024;

Batch2D::Batch2D()
:vao(glGenVertexArray()),
vertexBuff(glGenBuffer()),
indexBuff(glGenBuffer())
{
	shaderProgram.LoadShader(R"(D:\git\Augment\Augment\res\shader\Batch2D.vert)", GL_VERTEX_SHADER);
	shaderProgram.LoadShader(R"(D:\git\Augment\Augment\res\shader\Batch2D.frag)", GL_FRAGMENT_SHADER);
	shaderProgram.Link();

	//bind vao to capture state
	glBindVertexArray(vao.get());

	//allocate vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuff.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* vertexPerSprite * batchSize, nullptr, GL_STREAM_DRAW);

	//set up vertex buffer format
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(2);

	//allocate index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)* indexPerSprite * batchSize, nullptr, GL_STREAM_DRAW);

	//unbind vao & buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Batch2D::~Batch2D(){
}

void Batch2D::Begin(){
	
	//bind the shader program
	shaderProgram.Bind();
	//bind the vao, automatically element buffer and attribute info
	glBindVertexArray(vao.get());
	//bind the vertex array, not automatically done by vao
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuff.get());

	//enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//no texture has been assigned
	texture = nullptr;

	//allow buffer loading
	MapBuffers();

	//number of instances defaults to 0
	instanceCount = 0;

	//set 2d projection matrix to screen size
	GLint viewportInfo[4];
	glGetIntegerv(GL_VIEWPORT, viewportInfo);
	glm::mat4 projMat = glm::ortho(0.f, (float)viewportInfo[2], (float)viewportInfo[3], 0.f);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projMat));

	glPointSize(40.0f);
}

void Batch2D::SetTexture(Texture2D& tex, glm::vec4* sourceRect){
	if (texture){
		//if texture is different from previous
		if (texture->GetHandle() != tex.GetHandle()){
			//draw all stored sprites
			Flush();
			//set texture handle
			texture = &tex;

			//bind texture to texture unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
		}
		
		//else: if texture is same as previous, do not bind

	}
	//no previous texture
	else{
		//set texture handle
		texture = &tex;

		//bind texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	}

	//set sourceDim and sourceTexCoords(normalized) based on sourceRect and texture
	if (sourceRect == nullptr){
		sourceDim = glm::vec2(texture->GetWidth(), texture->GetHeight());
		sourceTexCoords[0] = glm::vec2(0.f, 0.f);
		sourceTexCoords[1] = glm::vec2(1.f, 0.f);
		sourceTexCoords[2] = glm::vec2(0.f, 1.f);
		sourceTexCoords[3] = glm::vec2(1.f, 1.f);
	}
	else{
		sourceDim = glm::vec2(sourceRect->z, sourceRect->w);

		glm::float32 w = 1.f / texture->GetWidth();
		glm::float32 h = 1.f / texture->GetHeight();

		sourceTexCoords[0] = glm::vec2(sourceRect->x * w, sourceRect->y * h);
		sourceTexCoords[1] = glm::vec2(sourceRect->z * w, sourceRect->y * h);
		sourceTexCoords[2] = glm::vec2(sourceRect->x * w, sourceRect->w * h);
		sourceTexCoords[3] = glm::vec2(sourceRect->z * w, sourceRect->w * h);
	}
}

void Batch2D::Draw(const glm::vec2& position,
	const glm::vec3& rotation,
	const glm::vec2& scale,
	const glm::vec2& origin,
	const glm::vec4& tint){

	//if a texture is bound, add a sprite to the batch
	if (texture){
		//build basic coordinates, scaled
		glm::vec2 baseCoords[4] = {
			glm::vec2(0, 0),
			glm::vec2(sourceDim.x, 0),
			glm::vec2(0, sourceDim.y),
			glm::vec2(sourceDim.x, sourceDim.y)
		};

		for (int i = 0; i < vertexPerSprite; i++){

			//translate to origin
			baseCoords[i] -= origin;
			baseCoords[i].x *= scale.x;
			baseCoords[i].y *= scale.y;

			//rotate
			if (rotation.x != 0){
				baseCoords[i].y *= glm::cos(rotation.x);
			}
			if (rotation.y != 0){
				baseCoords[i].x *= glm::cos(rotation.y);
			}
			if (rotation.z != 0){
				baseCoords[i] = glm::vec2(
					baseCoords[i].x * glm::cos(rotation.z) - baseCoords[i].y * glm::sin(rotation.z),
					baseCoords[i].x * glm::sin(rotation.z) + baseCoords[i].y * glm::cos(rotation.z));
			}

			//translate to position
			baseCoords[i] += position;

			//load vertex data into buffer
			mappedVertexBuff[instanceCount * vertexPerSprite + i].position = baseCoords[i];
			mappedVertexBuff[instanceCount * vertexPerSprite + i].texCoords = sourceTexCoords[i];
			mappedVertexBuff[instanceCount * vertexPerSprite + i].color = tint;
		}

		//load index data into buffer
		unsigned int loc = instanceCount * indexPerSprite;
		mappedIndexBuff[loc] = instanceCount * vertexPerSprite;
		mappedIndexBuff[loc + 1] = instanceCount * vertexPerSprite + 1;
		mappedIndexBuff[loc + 2] = instanceCount * vertexPerSprite + 2;
		mappedIndexBuff[loc + 3] = instanceCount * vertexPerSprite + 1;
		mappedIndexBuff[loc + 4] = instanceCount * vertexPerSprite + 2;
		mappedIndexBuff[loc + 5] = instanceCount * vertexPerSprite + 3;

		//increment index count
		++instanceCount;

		//if batch is full, flush
		if (instanceCount == batchSize){
			Flush();
		}
	}
}

void Batch2D::End(){
	//as long as a texture is bound, draw sprites
	if (texture)
		Flush();
	texture = nullptr;
	UnmapBuffers();
	glBindVertexArray(0);
}

//draw sprites to framebuffer
void Batch2D::Flush(){
	UnmapBuffers();
	glDrawElements(GL_TRIANGLES, instanceCount * indexPerSprite, GL_UNSIGNED_SHORT, 0);
	instanceCount = 0;
	MapBuffers();
}

//enable buffer loading
void Batch2D::MapBuffers(){
	mappedVertexBuff = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	mappedIndexBuff = (GLushort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

//disable buffer loading
void Batch2D::UnmapBuffers(){
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}