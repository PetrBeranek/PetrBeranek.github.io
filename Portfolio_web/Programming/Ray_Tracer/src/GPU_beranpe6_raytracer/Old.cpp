/*


void buildTree(std::vector<Collidable *>& vec, float xStart, float xEnd, float yStart, float yEnd, float zStart, float zEnd, short axisPrevious, int d)
	{
		Collidable *c;
		xMin = xStart;
		xMax = xEnd;
		yMin = yStart;
		yMax = yEnd;
		zMin = zStart;
		zMax = zEnd;
		depth = d;
		xP = xN = yP = yN = zP = zN = 0;
		
		id = KDCounter;
		KDCounter++;
		if(depth < LIM_DEPTH && vec.size() > LIM_SIZE)
		{
			
			switch(axisPrevious){
			case SPLIT_X:
				axis = SPLIT_Y;
				printf(" spliting Y (%d - %d) %f %f %f %f %f %f \n", id, depth, xMin, xMax, yMin, yMax, zMin, zMax);
				break;
			case SPLIT_Y:
				axis = SPLIT_Z;
				printf(" spliting Z (%d - %d) %f %f %f %f %f %f \n", id, depth, xMin, xMax, yMin, yMax, zMin, zMax);
				break;
			case SPLIT_Z:
				axis = SPLIT_X;
				printf(" spliting X (%d - %d) %f %f %f %f %f %f \n", id, depth, xMin, xMax, yMin, yMax, zMin, zMax);
				break;
			}
			
			axis = (axisPrevious + 1) % 3;
			printf("old axis %d \n", axisPrevious);
			printf("starting at axis %d \n", axis);
			
			
			//printf("spliting threshold %f \n", split);
			std::vector<Collidable *>::iterator it = vec.begin();
			std::vector<Collidable *> *low = new std::vector<Collidable *>();
			std::vector<Collidable *> *high = new std::vector<Collidable *>();
			
			int counterA = 0;
			int counterL = 0;
			int counterR = 0;

			int tmpAx = axis;
			float tmpSplit;
			for(int i = 0; i < 3; i++)
			{
				tmpAx = (tmpAx + i) % 3;
				tmpSplit = findCuttingPlane(vec,tmpAx);;
				for(;it < vec.end();it++)
				{
					c = (*it);
					
					counterA++;
					if(c->getMax(tmpAx) < split)
					{
						counterL++;
						low->push_back(c);
						continue;
					}
					if(c->getMin(tmpAx) > split)
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

				float value = ((float)counterL + (float)counterR) / ((float)counterA);
				printf("split value %f on axis %d \n",value,tmpAx);
				if(value > SPLIT_THRESHOLD){ continue; }

				axis = tmpAx;
				split = tmpSplit;
				
				left = new KDTree();
				right = new KDTree();
				printf("--- %d divided to %d and %d\n",counterA, counterL, counterR);
				switch(axis)
				{
				case SPLIT_X:
					left->buildTree(*low, xMin, split, yMin, yMax, zMin, zMax, axis, depth + 1);
					right->buildTree(*high, split, xMax, yMin, yMax, zMin, zMax, axis, depth + 1);
					break;
				case SPLIT_Y:
					left->buildTree(*low, xMin, xMax, yMin, split, zMin, zMax, axis, depth + 1);
					right->buildTree(*high, xMin, xMax, split, yMax, zMin, zMax, axis, depth + 1);
					break;
				case SPLIT_Z:
					left->buildTree(*low, xMin, xMax, yMin, yMax, zMin, split, axis, depth + 1);
					right->buildTree(*high, xMin, xMax, yMin, yMax, split, zMax, axis, depth + 1);
					break;
				}
				return;
			}

		}
		
		isLeaf = true;
		left = right = NULL;
		items = &vec;
			
		printf(" leaf (%d - %d) %f %f %f %f %f %f \n", id, depth, xMin, xMax, yMin, yMax, zMin, zMax);
		return;
	}


	*/