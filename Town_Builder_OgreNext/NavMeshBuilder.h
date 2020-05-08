#pragma once
#include "Recast.h"
#include "AI/Recast/Helpers/RecastInputGeom.h"

class CNavMeshBuilder
{
public:
	CNavMeshBuilder();
	~CNavMeshBuilder();

	void BuildNavMesh(InputGeom* geom);

private:
	rcContext* m_rcContext;
	rcConfig m_rcConfig;
	float m_cellSize; //size of a cell in world units
	float m_cellHeight; //height of a cell in world units
	
	int m_edgeMaxLenght;
	float m_egdeMaxError;

	int m_regionMinSize; //minimum number of cells to form an isolated island area
	int m_regionMergeSize; //Any regions with a span count smaller than this value will, if possible, be merged with larger regions
	int m_maxVertsPerPoly; //Maximum number of verts per polys generated during contour to poly conversation

	float m_detailSampleDistance; //Sets the sampling distance to use when generating the detail mesh
	float m_detailSampleMaxError; //The maximum distance the detail mesh surface should deviate from heightfield data
};

