
//----------------------------------------------------------------------------------------
/**
 * \file       KDTree.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains KDTree class and structures
 *
 * File contains KDTree class for standard tree, structure for compact tree (used for GPU) and methods to chage KDTree to compact one.
 *
*/
//----------------------------------------------------------------------------------------


#ifndef KDTREE
#define KDTREE

#include <Vector>
#include "Collidable.h"
#include "Sphere.h"
#include "Triangle.h"

#define SPLIT_THRESHOLD 1.8f

#define SPLIT_X 0
#define SPLIT_Y 1
#define SPLIT_Z 2

#define LIM_SIZE 2
#define LIM_DEPTH 10

//#define PLANE_EXTRA_NAIVE	// cut in half
#define PLANE_NAIVE			// average of objects

int DEBUG_DEPTH;
bool BUILD_KDTREE = true;

struct KDTreeNodeCompact
{
	int id;
	bool isLeaf;
	short axis;
	float split;
	float bounds[6];
	int idLeft, idRight;

	int countTri;
	int countSph;
	int offsetTri;
	int offsetSph;

	int neighbors[6];
};

struct CompactTree
{
	int countNode;
	KDTreeNodeCompact *nodes;
	int countSph;
	SphereCompact *sphere;
	int countTri;
	TriangleCompact *tri;
	int countMat;
	MaterialCompact *mat;
	int countPLi;
	PointLightCompact *pLi;

	int sphereIndicesSize;
	int *sphereIndices;
	int triangleIndicesSize;
	int *trinagleIndices;

};

CompactTree  GLOBAL_COMPACT;

void printCompact()
{
	for(int i = 0; i < GLOBAL_COMPACT.countNode; i++)
	{
		printf("id %d neighbors %d %d | %d %d | %d %d \n",GLOBAL_COMPACT.nodes[i].id,GLOBAL_COMPACT.nodes[i].neighbors[0],GLOBAL_COMPACT.nodes[i].neighbors[1],GLOBAL_COMPACT.nodes[i].neighbors[2],GLOBAL_COMPACT.nodes[i].neighbors[3],GLOBAL_COMPACT.nodes[i].neighbors[4],GLOBAL_COMPACT.nodes[i].neighbors[5]);
	}
}


static int KDCounter = 0;

class KDTree
{
public:

	int depth;
	bool isLeaf;
	int axis;
	float split;
	float bounds[6];
	KDTree *neighbors[6];

	int id;
	
	KDTree *left;
	KDTree *right;
	std::vector<Collidable *> *items;
	
	KDTree::KDTree()
	{
		depth = 0;
		isLeaf = false;
		id = -1;
	}

	KDTree::KDTree( float xStart, float xEnd, float yStart, float yEnd, float zStart, float zEnd, short ax, int d, bool l)
	{
		bounds[0] = xStart;
		bounds[1] = xEnd;
		bounds[2] = yStart;
		bounds[3] = yEnd;
		bounds[4] = zStart;
		bounds[5] = zEnd;

		depth = d;
		axis = ax;
		isLeaf = l;
		id = -1;
	}

	
	float findCuttingPlane(std::vector<Collidable *>& vec, int& ax)
	{
	#ifdef PLANE_EXTRA_NAIVE
		if(axis = SPLIT_X){
			return (xMin + xMax) / 2;
		}else if(axis = SPLIT_Y){
			return (yMin + yMax) / 2;
		}else{
			return (zMin + zMax) / 2;
		}
	#elif defined(PLANE_NAIVE)
		
		float min = std::numeric_limits<float>::max();
		float max = -std::numeric_limits<float>::max();
		std::vector<Collidable *>::iterator it;
		Collidable *c;
		it = vec.begin();
		switch(ax)
		{
		case SPLIT_X:
			for(;it < vec.end();it++)
			{
				c = (*it);

				if(c->xMax > max){ max = c->xMax; }
				if(c->xMin < min){ min = c->xMin; }
			}
			break;
		case SPLIT_Y:
			for(;it < vec.end();it++)
			{
				c = (*it);

				if(c->yMax > max){ max = c->yMax; }
				if(c->yMin < min){ min = c->yMin; }
			}
			break;
		case SPLIT_Z:
			for(;it < vec.end();it++)
			{
				c = (*it);

				if(c->zMax > max){ max = c->zMax; }
				if(c->zMin < min){ min = c->zMin; }
			}
			break;
		}
		return (min + max)/2.0f;
	#else

	#endif
	}
	/**
	recursive part of tree building
	*/
	void buildTree(std::vector<Collidable *>& vec, float xStart, float xEnd, float yStart, float yEnd, float zStart, float zEnd, short axisPrevious, int d, int threshold)
	{
		Collidable *c;
		bounds[0] = xStart;
		bounds[1] = xEnd;
		bounds[2] = yStart;
		bounds[3] = yEnd;
		bounds[4] = zStart;
		bounds[5] = zEnd;
		depth = d;
		if(BUILD_KDTREE && threshold != 3 && depth < LIM_DEPTH && vec.size() > LIM_SIZE)
		{
			switch(axisPrevious){
			case SPLIT_X:
				axis = SPLIT_Y;
				break;
			case SPLIT_Y:
				axis = SPLIT_Z;
				break;
			case SPLIT_Z:
				axis = SPLIT_X;
				break;
			}
			
			split = findCuttingPlane(vec, axis);
			std::vector<Collidable *>::iterator it = vec.begin();
			std::vector<Collidable *> *low = new std::vector<Collidable *>();
			std::vector<Collidable *> *high = new std::vector<Collidable *>();
			
			int counterA = 0;
			int counterL = 0;
			int counterR = 0;
			
			switch(axis)
			{
			case SPLIT_X:
				for(;it < vec.end();it++)
				{
					c = (*it);
					
					counterA++;
					if(c->xMax <= split)
					{
						counterL++;
						low->push_back(c);
						continue;
					}
					if(c->xMin >= split)
					{
						counterR++;
						high->push_back(c);
						continue;
					}
					
					counterL++;
					counterR++;
					low->push_back(c);
					high->push_back(c);
				}
				
				if((counterL + counterR < counterA * SPLIT_THRESHOLD) && (counterL < counterA && counterR < counterA))
				{
					left = new KDTree();
					right = new KDTree();
					
					id = KDCounter;
					KDCounter++;
					left->buildTree(*low, bounds[0], split, bounds[2], bounds[3], bounds[4], bounds[5], axis, depth + 1, 0);
					right->buildTree(*high, split, bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], axis, depth + 1, 0);
				}else{
					delete low;
					delete high;
					this->buildTree(vec, bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], axis, depth, threshold + 1);
				}
				return;
				break;
			case SPLIT_Y:
				for(;it < vec.end();it++)
				{
					c = (*it);
					
					counterA++;
					if(c->yMax <= split)
					{
						counterL++;
						low->push_back(c);
						continue;
					}
					if(c->yMin >= split)
					{
						counterR++;
						high->push_back(c);
						continue;
					}
					counterL++;
					counterR++;
					low->push_back(c);
					high->push_back(c);
				}
				
				if((counterL + counterR < counterA * SPLIT_THRESHOLD) && (counterL < counterA && counterR < counterA))
				{
					left = new KDTree();
					right = new KDTree();
					
					id = KDCounter;
					KDCounter++;
					left->buildTree(*low, bounds[0], bounds[1], bounds[2], split, bounds[4], bounds[5], axis, depth + 1, 0);
					right->buildTree(*high, bounds[0], bounds[1], split, bounds[3], bounds[4], bounds[5], axis, depth + 1, 0);
				}else{
					delete low;
					delete high;
					this->buildTree(vec, bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], axis, depth, threshold + 1);
				}
				return;
				break;
			case SPLIT_Z:
				for(;it < vec.end();it++)
				{
					c = (*it);
					
					counterA++;
					if(c->zMax <= split)
					{
						//to the low
						counterL++;
						low->push_back(c);
						continue;
					}
					if(c->zMin >= split)
					{
						//to the high
						counterR++;
						high->push_back(c);
						continue;
					}
					counterL++;
					counterR++;
					low->push_back(c);
					high->push_back(c);
				}
				
				if((counterL + counterR < counterA * SPLIT_THRESHOLD) && (counterL < counterA && counterR < counterA))
				{
					left = new KDTree();
					right = new KDTree();
					
					id = KDCounter;
					KDCounter++;
					left->buildTree(*low, bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], split, axis, depth + 1, 0);
					right->buildTree(*high, bounds[0], bounds[1], bounds[2], bounds[3], split, bounds[5], axis, depth + 1, 0);
				}else{
					delete low;
					delete high;
					this->buildTree(vec, bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], axis, depth, threshold + 1);
				}
				return;
				break;
			}
		}
		
			isLeaf = true;
			left = right = NULL;
			items = &vec;
			
					
			id = KDCounter;
			KDCounter++;
			return;
	}
	/**
	Start building tree
	*/
	void buildTreeStart(std::vector<Collidable *>& vec)
	{
		resetCounter();
		//id = KDCounter;
		//KDCounter++;
		std::vector<Collidable *>::iterator it;
		bounds[0] = bounds[2] = bounds[4] = std::numeric_limits<float>::max();
		bounds[1] = bounds[3] = bounds[5] = -std::numeric_limits<float>::max();
		depth = 0;
		axis = SPLIT_X;
		Collidable *c;

		it = vec.begin();
		for(;it < vec.end();it++)
		{
			c = (*it);
			
			if(c->xMin < this->bounds[0]){ this->bounds[0] = c->xMin; }
			if(c->xMax > this->bounds[1]){ this->bounds[1] = c->xMax; }
			
			if(c->yMin < this->bounds[2]){ this->bounds[2] = c->yMin; }
			if(c->yMax > this->bounds[3]){ this->bounds[3] = c->yMax; }

			if(c->zMin < this->bounds[4]){ this->bounds[4] = c->zMin; }
			if(c->zMax > this->bounds[5]){ this->bounds[5] = c->zMax; }
		}
		this->buildTree(vec, bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], SPLIT_Z, 0, 0);
		this->setNeighborsStart();
	}
	/**
	recursive part of neighbor setting
	*/
	void setNeighbors()
	{
		if(isLeaf){return;}
		
		left->neighbors[0] = neighbors[0];
		left->neighbors[1] = neighbors[1];
		left->neighbors[2] = neighbors[2];
		left->neighbors[3] = neighbors[3];
		left->neighbors[4] = neighbors[4];
		left->neighbors[5] = neighbors[5];

		right->neighbors[0] = neighbors[0];
		right->neighbors[1] = neighbors[1];
		right->neighbors[2] = neighbors[2];
		right->neighbors[3] = neighbors[3];
		right->neighbors[4] = neighbors[4];
		right->neighbors[5] = neighbors[5];

		switch(axis)
		{
		case SPLIT_X:
			left->neighbors[1] = right;
			right->neighbors[0] = left;
			break;
		case SPLIT_Y:
			left->neighbors[3] = right;
			right->neighbors[2] = left;
			break;
		case SPLIT_Z:
			left->neighbors[5] = right;
			right->neighbors[4] = left;
			break;
		}
		
		left->setNeighbors();
		right->setNeighbors();
	}
	/**
	Set neighbors for nodes in tree
	*/
	void setNeighborsStart()
	{
		neighbors[0] = neighbors[1] = neighbors[2] = neighbors[3] = neighbors[4] = neighbors[5] = NULL; 
		this->setNeighbors();
	}

	bool isPointInTree(Vec3& point)
	{
		if(point.x > bounds[1] || point.x < bounds[0]){return false;}
		if(point.y > bounds[3] || point.y < bounds[2]){return false;}
		if(point.z > bounds[5] || point.z < bounds[4]){return false;}
		return true;
	}
	
	void printTree()
	{
		if(isLeaf)
		{
			printf(" leaf: %d \n", depth);
			std::vector<Collidable *>::iterator it;
			printf("addr %d \n",items);
			it = items->begin();
			for(;it < items->end();it++)
			{
				printf(" (%f, %f, %f) ",(*it)->xMin,(*it)->yMin,(*it)->zMin);
			}
			printf("\n");
		}else{
			printf(" splited in: %f \n", split);
			printf(" going to left in: %d \n", depth);
			left->printTree();
			printf(" going to right in: %d \n", depth);
			right->printTree();
			printf(" leaving: %d \n", depth);
		}
	}

	void printTreeMod()
	{
		if(isLeaf)
		{
			//printf("id: %d depth: %d x: %d <-> %d y: %d <-> %d z: %d <-> %d \n", id, depth, (int) xMin, (int) xMax, (int) yMin, (int) yMax, (int) zMin, (int) zMax);
		}else{
			//printf("id: %d depth: %d x: %d <-> %d y: %d <-> %d z: %d <-> %d \n", id, depth, (int) xMin, (int) xMax, (int) yMin, (int) yMax, (int) zMin, (int) zMax);
			left->printTreeMod();
			right->printTreeMod();
		}
	}

	KDTree *findNodeByPoint(Vec3 &point)
	{
		DEBUG_DEPTH++;
		if(isLeaf)
		{
			//printf("found %d \n",id);
			//printf("node %d \n",this->id);
			//printf("depth %d \n\n", DEBUG_DEPTH);
			return this;
		}
		KDTree *next;
		switch(axis)
		{
		case SPLIT_X:
			if(point.x < split)
			{
				next = left;
			}else{
				next = right;
			}
			break;
		case SPLIT_Y:
			if(point.y < split)
			{
				next = left;
			}else{
				next = right;
			}
			break;
		case SPLIT_Z:
			if(point.z < split)
			{
				next = left;
			}else{
				next = right;
			}
			break;
		}
		
		return next->findNodeByPoint(point);
	}
	int findEnterPoint(Vec3& start, Vec3& dir, Vec3& point)
	{
		Vec3 corner = Vec3();
		float nX, nY, nZ;
		
		//choose corner point
		if(dir.x < 0){
			corner.x = bounds[1];
			nX = 1;
		}else{
			corner.x = bounds[0];
			nX = -1;
		}
		if(dir.y < 0){
			corner.y = bounds[3];
			nY = 1;
		}else{
			corner.y = bounds[2];
			nY = -1;
		}
		if(dir.z < 0){
			corner.z = bounds[5];
			nZ = 1;
		}else{
			corner.z = bounds[4];
			nZ = -1;
		}

		
		Vec3 n;
		float d, u, div, t;

		//X plane
		n = Vec3(nX, 0.0f, 0.0f);
		d = n.dot(corner);
		u = n.dot(start);
		div = n.dot(dir);
		if(div != 0){
			t = (d-u)/div;

			point.set(start);
			point.addMultipliedLoc(dir,t);
			if(point.y >= bounds[2] && point.y <= bounds[3] && point.z >= bounds[4] && point.z <= bounds[5])
			{
				return (nX < 0)?0:1;
			}
		}

		//Y plane
		n = Vec3(0.0f, nY, 0.0f);
		d = n.dot(corner);
		u = n.dot(start);
		div = n.dot(dir);
		if(div != 0){
			t = (d-u)/div;

			point.set(start);
			point.addMultipliedLoc(dir,t);
			if(point.x >= bounds[0] && point.x <= bounds[1] && point.z >= bounds[4] && point.z <= bounds[5])
			{
				return (nY < 0)?2:3;
			}
		}

		//Z plane
		n = Vec3(0.0f, 0.0f, nZ);
		d = n.dot(corner);
		u = n.dot(start);
		div = n.dot(dir);
		if(div != 0){
			t = (d-u)/div;

			point.set(start);
			point.addMultipliedLoc(dir,t);
			if(point.y >= bounds[2] && point.y <= bounds[3] && point.x >= bounds[0] && point.x <= bounds[1])
			{
				return (nZ < 0)?4:5;
			}
		}
		point.clear();
		return -1;
	}

	int findExitPoint(Vec3& start, Vec3& dir, Vec3& point)
	{
		Vec3 corner = Vec3();
		float nX, nY, nZ;
		
		//choose corner point
		if(dir.x < 0){
			corner.x = bounds[0];
			nX = 1;
		}else{
			corner.x = bounds[1];
			nX = -1;
		}
		if(dir.y < 0){
			corner.y = bounds[2];
			nY = 1;
		}else{
			corner.y = bounds[3];
			nY = -1;
		}
		if(dir.z < 0){
			corner.z = bounds[4];
			nZ = 1;
		}else{
			corner.z = bounds[5];
			nZ = -1;
		}
		
		Vec3 n;
		float d, u, div, t;
		//X plane
		n = Vec3(nX, 0.0f, 0.0f);
		d = n.dot(corner);
		u = n.dot(start);
		div = n.dot(dir);
		if(div != 0){
			t = (d-u)/div;

			point.set(start);
			point.addMultipliedLoc(dir,t);
			if(point.y >= bounds[2] && point.y <= bounds[3] && point.z >= bounds[4] && point.z <= bounds[5])
			{
				return (nX > 0)?0:1;
			}
		}
		//Y plane
		n = Vec3(0.0f, nY, 0.0f);
		d = n.dot(corner);
		u = n.dot(start);
		div = n.dot(dir);
		if(div != 0){
			t = (d-u)/div;

			point.set(start);
			point.addMultipliedLoc(dir,t);
			if(point.x >= bounds[0] && point.x <= bounds[1] && point.z >= bounds[4] && point.z <= bounds[5])
			{
				return (nY > 0)?2:3;
			}

		}
		//Z plane
		n = Vec3(0.0f, 0.0f, nZ);
		d = n.dot(corner);
		u = n.dot(start);
		div = n.dot(dir);
		if(div != 0){
			t = (d-u)/div;
		
			point.set(start);
			point.addMultipliedLoc(dir,t);
			if(point.y >= bounds[2] && point.y <= bounds[3] && point.x >= bounds[0] && point.x <= bounds[1])
			{
				return (nZ > 0)?4:5;
			}
		}
		point.clear();
	return -1;
	}

	void resetCounter()
	{
		KDCounter = 0;
	}

	void printNeighbors()
	{
		printf("%d = %d <---> %d | %d <---> %d | %d <---> %d | \n ",id, (neighbors[0] != NULL)?neighbors[0]->id:-1,(neighbors[1] != NULL)?neighbors[1]->id:-1,(neighbors[2] != NULL)?neighbors[2]->id:-1,(neighbors[3] != NULL)?neighbors[3]->id:-1,(neighbors[4] != NULL)?neighbors[4]->id:-1,(neighbors[5] != NULL)?neighbors[5]->id:-1);
		if(!isLeaf)
		{
			left->printNeighbors();
			right->printNeighbors();
		}
	}
};

KDTree GLOBAL_TREE;

void compactKD(KDTree& tree, KDTreeNodeCompact *nodeBufC, std::vector<Triangle *>& triInd, std::vector<Sphere *>& sphInd)
{
	KDTreeNodeCompact *actual = nodeBufC + tree.id;

	
	actual ->id			=	tree.id;
	actual ->isLeaf		=	tree.isLeaf;
	actual ->axis		=	tree.axis;
	actual ->split		=	tree.split;
	actual ->bounds[0]		=	tree.bounds[0];
	actual ->bounds[1]		=	tree.bounds[1];
	actual ->bounds[2]		=	tree.bounds[2];
	actual ->bounds[3]		=	tree.bounds[3];
	actual ->bounds[4]		=	tree.bounds[4];
	actual ->bounds[5]		=	tree.bounds[5];
	
	for(int i = 0; i < 6; i++)
	{
		if(tree.neighbors[i] == NULL)
		{
			actual ->neighbors[i]	=	-1;
		}else{
			actual ->neighbors[i]	=	tree.neighbors[i]->id;
		}
	}

	if(tree.isLeaf)
	{
		
		actual ->idLeft		=	-1;
		actual ->idRight	=	-1;
	
		actual ->countTri	=	0;
		actual ->countSph	=	0;
		actual ->offsetTri	=	triInd.size();
		actual ->offsetSph	=	sphInd.size();

		std::vector<Collidable *>::iterator itC;
		Collidable *c;

		itC = tree.items->begin();
		for(; itC != tree.items->end(); itC++)
		{
			c = *itC;
			switch(c->collidableType())
			{
			case COLLID_SPHERE:
				sphInd.push_back(static_cast<Sphere *>(c));
				actual ->countSph++;
				break;
			case COLLID_TRIANGLE:
				triInd.push_back(static_cast<Triangle *>(c));
				actual ->countTri++;
				break;
			case COLLID_POINT:
				break;
			}
		}
		return;
	}else{

		actual ->idLeft		=	tree.left->id;
		actual ->idRight	=	tree.right->id;
	
		actual ->countTri	=	-1;
		actual ->countSph	=	-1;
		actual ->offsetTri	=	-1;
		actual ->offsetSph	=	-1;
		
		compactKD(*tree.left, nodeBufC, triInd, sphInd);
		compactKD(*tree.right, nodeBufC, triInd, sphInd);
	}
}

void compactizeCurrentTree()
{
	MaterialCompact *matBufC;
	int countMat = GLOBAL_BUFFER_MATERIAL.size();
	matBufC = new MaterialCompact[countMat];

	SphereCompact *sphBufC;
	int countSph = GLOBAL_BUFFER_SPHERE.size();
	sphBufC = new SphereCompact[countSph];
	
	TriangleCompact *triBufC;
	int countTri = GLOBAL_BUFFER_TRIANGLE.size();
	triBufC = new TriangleCompact[countTri];

	PointLightCompact *pliBufC;
	int countPli = GLOBAL_BUFFER_LIGHT.count;
	pliBufC = new PointLightCompact[countPli];
	
	Material::fillCompactArray(matBufC);
	Sphere::fillCompactArray(sphBufC);
	Triangle::fillCompactArray(triBufC);
	GLOBAL_BUFFER_LIGHT.fillCompactArray(pliBufC);

	
	KDTreeNodeCompact *nodeBufC;
	int countNode = KDCounter;

	nodeBufC = new KDTreeNodeCompact[countNode];
	
	std::vector<Triangle *> triInd;
	std::vector<Sphere *> sphInd;

	compactKD(GLOBAL_TREE, nodeBufC, triInd, sphInd);
	
	GLOBAL_COMPACT.trinagleIndices	= new int[triInd.size()];
	GLOBAL_COMPACT.sphereIndices	= new int[sphInd.size()];
	GLOBAL_COMPACT.sphereIndicesSize	= sphInd.size();
	GLOBAL_COMPACT.triangleIndicesSize	= triInd.size();
	std::vector<Sphere *>::iterator itIS;
	std::vector<Triangle *>::iterator itIT;
	int i;
	
	bool test = true;

	i = 0;
	itIT = triInd.begin();
	while(itIT != triInd.end())
	{
		GLOBAL_COMPACT.trinagleIndices[i] = (*itIT)->id;
		i++;
		itIT++;
	}
	i = 0;
	itIS = sphInd.begin();
	while(itIS != sphInd.end())
	{
		GLOBAL_COMPACT.sphereIndices[i] = (*itIS)->id;
		i++;
		itIS++;
	}
	
	GLOBAL_COMPACT.mat			=	matBufC;
	GLOBAL_COMPACT.nodes		=	nodeBufC;
	GLOBAL_COMPACT.pLi			=	pliBufC;
	GLOBAL_COMPACT.sphere		=	sphBufC;
	GLOBAL_COMPACT.tri			=	triBufC;
	
	GLOBAL_COMPACT.countMat		=	countMat;
	GLOBAL_COMPACT.countNode	=	countNode;
	GLOBAL_COMPACT.countPLi		=	countPli;
	GLOBAL_COMPACT.countSph		=	countSph;
	GLOBAL_COMPACT.countTri		=	countTri;
}

#endif