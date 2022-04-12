#include "Mesh.h"

#include <glm/geometric.hpp>

Mesh::Mesh() :
	VAO(0),
	VBO(0),
	IBO(0),
	indexCount(0)
{
}

Mesh::~Mesh()
{
	ClearMesh();
}

void Mesh::CreateMesh(GLfloat* verts, size_t numVerts, unsigned* indices, size_t numIndices)
{
	indexCount = numIndices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts[0]) * numVerts, verts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(verts[0]) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(verts[0]) * 8, reinterpret_cast<void*>(sizeof(verts[0]) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(verts[0]) * 8, reinterpret_cast<void*>(sizeof(verts[0]) * 5));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::ClearMesh()
{
	if (0 != IBO)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (0 != VBO)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (0 != VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

void Mesh::CalculateAvgNormals(unsigned* indices, unsigned idxCount, GLfloat* verts, unsigned vertCount,
	unsigned vLength, unsigned nrmOffset)
{
	for (size_t i = 0; i < idxCount; i += 3)
	{
		unsigned idx0 = indices[i] * vLength;
		unsigned idx1 = indices[i + 1] * vLength;
		unsigned idx2 = indices[i + 2] * vLength;

		glm::vec3 v1(verts[idx1] - verts[idx0], verts[idx1 + 1] - verts[idx0 + 1], verts[idx1 + 2] - verts[idx0 + 2]);
		glm::vec3 v2(verts[idx2] - verts[idx0], verts[idx2 + 1] - verts[idx0 + 1], verts[idx2 + 2] - verts[idx0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		idx0 += nrmOffset; idx1 += nrmOffset; idx2 += nrmOffset;

		verts[idx0] += normal.x; verts[idx0 + 1] += normal.y; verts[idx0 + 2] += normal.z;
		verts[idx1] += normal.x; verts[idx1 + 1] += normal.y; verts[idx1 + 2] += normal.z;
		verts[idx2] += normal.x; verts[idx2 + 1] += normal.y; verts[idx2 + 2] += normal.z;
	}

	for (size_t i = 0; i < vertCount / vLength; ++i)
	{
		auto sectionOffset = i * vLength + nrmOffset;

		glm::vec3 v(verts[sectionOffset], verts[sectionOffset + 1], verts[sectionOffset + 2]);
		v = -glm::normalize(v);
		verts[sectionOffset] = v.x; verts[sectionOffset + 1] = v.y; verts[sectionOffset + 2] = v.z;
	}
}
