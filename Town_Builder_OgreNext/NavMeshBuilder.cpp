#include "NavMeshBuilder.h"
#include <iostream>


CNavMeshBuilder::CNavMeshBuilder()
{
}


CNavMeshBuilder::~CNavMeshBuilder()
{
}

//TODO: Get agent information
void CNavMeshBuilder::BuildNavMesh(InputGeom* geom)
{
	std::cout << "Starting navmesh generation" << std::endl;
	if(geom->isEmpty())
	{
		std::cout << "ERROR: Input geometry is empty. Cancelling navmesh generation" << std::endl;
		return;
	}

	const float* bmin = geom->getMeshBoundsMin();
	const float* bmax = geom->getMeshBoundsMax();
	const float* verts = geom->getVerts();
	const int vertCount = geom->getVertCount();
	const int* tris = geom->getTris();
	const int triCount = geom->getTriCount();

	std::cout << "retrieved geometry information. VertCount: " << vertCount << ". TriCount: " << triCount << std::endl;

	//
	// Step 1. Initialize build config.
	//

	// Init build configuration
	memset(&m_rcConfig, 0, sizeof(m_rcConfig));
	m_rcConfig.cs = m_cellSize;
	m_rcConfig.ch = m_cellHeight;
	//m_rcConfig.walkableSlopeAngle = m_agentMaxSlope;
	//m_rcConfig.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
	//m_rcConfig.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
	//m_rcConfig.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
	m_rcConfig.maxEdgeLen = (int)(m_edgeMaxLenght / m_cellSize);
	m_rcConfig.maxSimplificationError = m_egdeMaxError;
	m_rcConfig.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
	m_rcConfig.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
	m_rcConfig.maxVertsPerPoly = (int)m_maxVertsPerPoly;
	m_rcConfig.detailSampleDist = m_detailSampleDistance < 0.9f ? 0 : m_cellSize * m_detailSampleDistance;
	m_rcConfig.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(m_rcConfig.bmin, bmin);
	rcVcopy(m_rcConfig.bmax, bmax);
	std::cout << "Calculating grid size" << std::endl;
	rcCalcGridSize(m_rcConfig.bmin, m_rcConfig.bmax, m_rcConfig.cs, &m_rcConfig.width, &m_rcConfig.height);
	std::cout << "Grid size: " << m_rcConfig.width << " x " << m_rcConfig.height << std::endl;

	std::cout << "Starting navmesh build process" << std::endl;
	std::cout << "Creating heightfield..." << std::endl;

	// Allocate voxel heightfield where we rasterize our input data to.
	//rcHeightfield* heightField = rcAllocHeightfield();
	rcHeightfield* heightfield = rcAllocHeightfield();

	if(!heightfield)
	{
		std::cout << "ERROR: Out of memory. Could not allocate memory for height field. Cancelling navmesh generation" << std::endl;
		return;
	}
	if(!rcCreateHeightfield(m_rcContext, *heightfield, m_rcConfig.width, m_rcConfig.height, m_rcConfig.bmin, m_rcConfig.bmax, m_rcConfig.cs, m_rcConfig.ch))
	{
		std::cout << "ERROR: Could not create solid heightfield. Cancelling navmesh generation" << std::endl;
		//m_rcContext->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return;
	}
	
	std::cout << "Created heightfield" << std::endl;
	std::cout << "Creating triangle areas..." << std::endl;


	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	unsigned char* triareas = new unsigned char[triCount];
	if(!triareas)
	{
		std::cout << "ERROR: Out of memory. Could not allocate memory for tri-areas. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		return;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(triareas, 0, triCount * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_rcContext, m_rcConfig.walkableSlopeAngle, verts, vertCount, tris, triCount, triareas); //WALKABLE SLOPE ANGLE IS MISSING
	if(!rcRasterizeTriangles(m_rcContext, verts, vertCount, tris, triareas, triCount, *heightfield, m_rcConfig.walkableClimb)) //WALKABLE CLIMB IS MISSING
	{
		std::cout << "ERROR: Could not rasterize triangles. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
		return;
	}
	delete[] triareas;
	triareas = nullptr;

	std::cout << "Created triangle areas" << std::endl;
	std::cout << "Filtering walkable sufaces..." << std::endl;
	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_rcContext, m_rcConfig.walkableClimb, *heightfield);
	rcFilterLedgeSpans(m_rcContext, m_rcConfig.walkableHeight, m_rcConfig.walkableClimb, *heightfield);
	rcFilterWalkableLowHeightSpans(m_rcContext, m_rcConfig.walkableHeight, *heightfield);

	std::cout << "Filtered walkable surfaces" << std::endl;
	std::cout << "Dividing walkable surfaces into simple regions..." << std::endl;
	std::cout << "Creating compact heightfield..." << std::endl;

	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	rcCompactHeightfield* compactHeightfield = rcAllocCompactHeightfield();
	if(!compactHeightfield)
	{
		std::cout << "ERROR: Out of memory. Could not allocate memory for compact heightfield. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return;
	}
	if(!rcBuildCompactHeightfield(m_rcContext, m_rcConfig.walkableHeight, m_rcConfig.walkableClimb, *heightfield, *compactHeightfield))
	{
		std::cout << "ERROR: Could not create compact heightfield. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return;
	}

	rcFreeHeightField(heightfield);
	heightfield = nullptr;

	std::cout << "Created compact heightfield" << std::endl;
	std::cout << "Eroding walkable area..." << std::endl;

	// Erode the walkable area by agent radius.
	if(!rcErodeWalkableArea(m_rcContext, m_rcConfig.walkableRadius, *compactHeightfield))
	{
		std::cout << "ERROR: Could not erode walkable areas. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return;
	}

	std::cout << "Eroded walkable area" << std::endl;
	std::cout << "Partitioning compact heightfield..." << std::endl;

	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// There are 3 martitioning methods, each with some pros and cons:
	// 1) Watershed partitioning
	//   - the classic Recast partitioning
	//   - creates the nicest tessellation
	//   - usually slowest
	//   - partitions the heightfield into nice regions without holes or overlaps
	//   - the are some corner cases where this method creates produces holes and overlaps
	//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
	//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
	//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
	// 2) Monotone partioning
	//   - fastest
	//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
	//   - creates long thin polygons, which sometimes causes paths with detours
	//   * use this if you want fast navmesh generation
	// 3) Layer partitoining
	//   - quite fast
	//   - partitions the heighfield into non-overlapping regions
	//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
	//   - produces better triangles than monotone partitioning
	//   - does not have the corner cases of watershed partitioning
	//   - can be slow and create a bit ugly tessellation (still better than monotone)
	//     if you have large open areas with small obstacles (not a problem if you use tiles)
	//   * good choice to use for tiled navmesh with medium and small sized tiles
	
	int partitioningMethod = 1;
	std::cout << "Partitioning method: ";

	switch(partitioningMethod)
	{
		case 1: //Watershed partitioning
			std::cout << "Watershed partitioning" << std::endl;
			std::cout << "Building distance field..." << std::endl;

			// Prepare for region partitioning, by calculating distance field along the walkable surface.
			if(!rcBuildDistanceField(m_rcContext, *compactHeightfield))
			{
				std::cout << "ERROR: Could not build distance field. Cancelling navmesh generation" << std::endl;
				//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
				return;
			}

			std::cout << "Built distance field" << std::endl;
			std::cout << "Building watershed regions..." << std::endl;

			// Partition the walkable surface into simple regions without holes.
			if(!rcBuildRegions(m_rcContext, *compactHeightfield, 0, m_rcConfig.minRegionArea, m_rcConfig.mergeRegionArea))
			{
				std::cout << "ERROR: Could not build watershed regions. Cancelling navmesh generation" << std::endl;
				//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
				return;
			}

			std::cout << "Built watershed regions" << std::endl;
			break;

		case 2: //Monotone partitioning
			std::cout << "Monotone partitioning" << std::endl;
			std::cout << "Building monotone regions..." << std::endl;

			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if(!rcBuildRegionsMonotone(m_rcContext, *compactHeightfield, 0, m_rcConfig.minRegionArea, m_rcConfig.mergeRegionArea))
			{
				std::cout << "Could not build monotone regions. Cancelling navmesh generation" << std::endl;
				//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
				return;
			}

			std::cout << "Built monotone regions" << std::endl;
			break;

		case 3: //layer partitioning
			std::cout << "Layer partitioning" << std::endl;
			std::cout << "Building layer regions..." << std::endl;

			// Partition the walkable surface into simple regions without holes.
			if(!rcBuildLayerRegions(m_rcContext, *compactHeightfield, 0, m_rcConfig.minRegionArea))
			{
				std::cout << "ERROR: Could not build layer regions. Cancelling navmesh generation" << std::endl;
				//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
				return;
			}

			std::cout << "Built layer regions" << std::endl;
			break;

		default:
			std::cout << partitioningMethod << std::endl;
			std::cout << "ERROR: Invalid partitioning method. Cancelling navmesh generation" << std::endl;
			return;
	}

	std::cout << "Partitioned compact heightfield..." << std::endl;
	std::cout << "Creating contours..." << std::endl;

	//
	// Step 5. Trace and simplify region contours.
	//
	// Create contours.
	rcContourSet* contourSet = rcAllocContourSet();
	if(!contourSet)
	{
		std::cout << "ERROR: Out of memory. Could not allocate memory for contour set. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return;
	}
	if(!rcBuildContours(m_rcContext, *compactHeightfield, m_rcConfig.maxSimplificationError, m_rcConfig.maxEdgeLen, *contourSet))
	{
		std::cout << "ERROR: Could not create contour set. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return;
	}

	std::cout << "Created contours" << std::endl;
	std::cout << "Building ploygon navmesh from contours" << std::endl;

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	rcPolyMesh* polyMesh = rcAllocPolyMesh();
	if(!polyMesh)
	{
		std::cout << "ERROR: Out of memory. Could not allocate memory for polygon mesh. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return;
	}
	if(!rcBuildPolyMesh(m_rcContext, *contourSet, m_rcConfig.maxVertsPerPoly, *polyMesh))
	{
		std::cout << "ERROR: could not build polygon mesh. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return;
	}

	std::cout << "Built polygon navmesh from contours" << std::endl;
	std::cout << "Building detail polygon navmesh..." << std::endl;

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	rcPolyMeshDetail* polyMeshDetail = rcAllocPolyMeshDetail();
	if(!polyMeshDetail)
	{
		std::cout << "ERROR: Out of memory. Could not allocate memory for detail polygon mesh. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		return;
	}

	if(!rcBuildPolyMeshDetail(m_rcContext, *polyMesh, *compactHeightfield, m_rcConfig.detailSampleDist, m_rcConfig.detailSampleMaxError, *polyMeshDetail))
	{
		std::cout << "ERROR: could not build detail polygon mesh. Cancelling navmesh generation" << std::endl;
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
		return;
	}

	rcFreeCompactHeightfield(compactHeightfield);
	compactHeightfield = nullptr;
	rcFreeContourSet(contourSet);
	contourSet = nullptr;
}
