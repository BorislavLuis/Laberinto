#include "octree.h"

void Octree::calculateBounds(BoundingRegion& out, Octant octant, BoundingRegion parentRegion)
{
	glm::vec3 center = parentRegion.calculateCenter();
	if (octant == Octant::O1) {
		out = BoundingRegion(center, parentRegion.max);
	}
	else if (octant == Octant::O2) {
		out = BoundingRegion(glm::vec3(parentRegion.min.x, center.y, center.z), glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z));
	}
	else if (octant == Octant::O3) {
		out = BoundingRegion(glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z), glm::vec3(center.x, center.y, parentRegion.max.z));
	}
	else if (octant == Octant::O4) {
		out = BoundingRegion(glm::vec3(center.x, parentRegion.min.y, center.z), glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z));
	}
	else if (octant == Octant::O5) {
		out = BoundingRegion(glm::vec3(center.x, center.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z));
	}
	else if (octant == Octant::O6) {
		out = BoundingRegion(glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z), glm::vec3(center.x, parentRegion.max.y, center.z));
	}
	else if (octant == Octant::O7) {
		out = BoundingRegion(parentRegion.min, center);
	}
	else if (octant == Octant::O8) {
		out = BoundingRegion(glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, center.y, center.z));
	}

}

Octree::node::node()
	:region(BoundTypes::AABB){}

Octree::node::node(BoundingRegion bounds)
	:region(bounds){}

Octree::node::node(BoundingRegion bounds, std::vector<BoundingRegion> objectList)
	:region(bounds)
{
	objects.insert(objects.end(), objectList.begin(), objectList.end());
}

void Octree::node::addToPending(RigidBody* instance, trie::Trie<Model*> models)
{
	for (BoundingRegion br : models[instance->modelId]->boundingRegions)
	{
		br.instance = instance;
		br.transform();
		queue.push(br);
	}
}

void Octree::node::build()
{
	if (objects.size() <= 1)
	{
		treeBuild = true;
		treeReady = true;
		return;
	}

	glm::vec3 dimensions = region.calculateDimensions();
	for (int i = 0; i < 3; i++)
	{
		if (dimensions[i] < MIN_BOUNDS)
		{
			treeBuild = true;
			treeReady = true;
			return;
		}
	}

	BoundingRegion octants[NO_CHILDREN];
	for (int i = 0; i < NO_CHILDREN; i++)
	{
		calculateBounds(octants[i], (Octant)(1 << i), region);
	}

	std::vector<BoundingRegion> octLists[NO_CHILDREN];
	std::stack<int> delList;

	for (int i = 0,length = objects.size(); i < length;i++)
	{
		BoundingRegion br = objects[i];
		for (int j = 0; j < NO_CHILDREN; j++)
		{
			if (octants[j].containsRegion(br))
			{
				octLists[j].push_back(br);
				delList.push(i);
				break;
			}
		}
	}

	while (delList.size() != 0)
	{
		objects.erase(objects.begin() + delList.top());
		delList.pop();
	}

	for (int i = 0; i < NO_CHILDREN; i++)
	{
		if (octLists[i].size() != 0)
		{
			children[i] = new node(octants[i], octLists[i]);
			States::activateIndex(&activeOctants, i);
			children[i]->parent = this; // borislav 
			children[i]->build();
	
			hasChildren = true;
		}
	}
	treeBuild = true;
	treeReady = true;
}

void Octree::node::update(Box& box)
{
	if (treeBuild && treeReady)
	{
		box.positions.push_back(region.calculateCenter());
		box.sizes.push_back(region.calculateDimensions());

		if (objects.size() == 0)
		{
			if (!hasChildren)
			{
				if (currentLifespan == -1)
				{
					currentLifespan = maxLifespan;
				}
				else if (currentLifespan > 0)
				{
					currentLifespan--;
				}
			}
		}
		else
		{
			if (currentLifespan != -1)
			{
				if (maxLifespan <= 64)
				{
					maxLifespan <<= 2;
				}
			}
		}
		
		for (int i = 0, listSize = objects.size(); i < listSize; i++)
		{
			if (States::isActive(&objects[i].instance->state,INSTANCE_DEAD))
			{
				objects.erase(objects.begin() + i);
				i--;
				listSize--;
			}
		}
		std::stack<std::pair<int,BoundingRegion>> movedObjects;

		for (int i = 0, listSize = objects.size(); i < listSize; i++)
		{
			if (States::isActive(&objects[i].instance->state, INSTANCE_MOVED))
			{
				objects[i].transform();
				movedObjects.push({ i,objects[i] });
			}
			box.positions.push_back(objects[i].calculateCenter());
			box.sizes.push_back(objects[i].calculateDimensions());
		}

		unsigned char flags = activeOctants;
		for (int i = 0; flags > 0; flags >>= 1, i++)
		{
			if (States::isIndexActive(&flags, 0) && children[i]->currentLifespan == 0)
			{
				if (children[i]->objects.size() > 0)
				{
					children[i]->currentLifespan = -1;
				}
				else
				{
					delete children[i];
					children[i] = nullptr;
					States::deactivateIndex(&activeOctants, i);
				}
			}
		}

		if (children != nullptr)
		{
			for (unsigned char flags = activeOctants, i = 0; flags > 0; flags >>= 1, i++)
			{
				if (States::isIndexActive(&flags, 0))
				{
					if (children[i] != nullptr)
					{
						children[i]->update(box);
					}
				}
			}
		}

		BoundingRegion movedObj;
		while (movedObjects.size() != 0)
		{
			movedObj = movedObjects.top().second;
			node* current = this;
			while (!current->region.containsRegion(movedObj))
			{
				if (current->parent != nullptr)
				{
					current = current->parent;
				}
				else
				{
					break;
				}
			}
			objects.erase(objects.begin() + movedObjects.top().first);
			movedObjects.pop();
			current->insert(movedObj);
		}
	}
	else
	{
		if (queue.size() > 0)
		{
			processPending();
		}
	}
}

void Octree::node::processPending()
{
	if (!treeBuild)
	{
		while (queue.size() != 0)
		{
			objects.push_back(queue.front());
			queue.pop();
		}
		build();
	}
	else
	{
		while (queue.size() != 0)
		{
			insert(queue.front());
			queue.pop();
		}
	}
}

bool Octree::node::insert(BoundingRegion obj)
{
	glm::vec3 dimensions = region.calculateDimensions();
	if (objects.size() == 0 || 
		dimensions.x < MIN_BOUNDS ||
		dimensions.y < MIN_BOUNDS ||
		dimensions.z < MIN_BOUNDS)
	{
		objects.push_back(obj);
		return true;
	}

	if (!region.containsRegion(obj))
	{
		return parent == nullptr ? false : parent->insert(obj);
	}

	BoundingRegion octants[NO_CHILDREN];
	for (int i = 0; i < NO_CHILDREN; i++)
	{
		if (children[i] != nullptr)
		{
			octants[i] = children[i]->region;
		}
		else
		{
			calculateBounds(octants[i], (Octant)(1 << i), region);
		}
	}

	std::vector<BoundingRegion> octList[NO_CHILDREN];
	objects.push_back(obj);

	for (int i = objects.size() - 1; i >= 0; i--)
	{
		for (int j = 0; j < NO_CHILDREN; j++)
		{
			if (octants[j].containsRegion(objects[i]))
			{
				octList[j].push_back(objects[i]);
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}

	//for (int i = 0; i < NO_CHILDREN; i++)
	//{
	//	if (octList[i].size() != 0)
	//	{
	//		if (children[i])
	//		{
	//			for (int j = 0, lenght = octList[i].size(); j < lenght; j++)
	//			{
	//				children[i]->insert(octList[i][j]);
	//			}
	//		}
	//		else
	//		{
	//			children[i] = new node(octants[i], octList[i]);
	//			States::activateIndex(&activeOctants, i);
	//			children[i]->parent = this;
	//			children[i]->build();
	//			hasChildren = true;
	//		}
	//	}
	//}

	for (int i = 0; i < NO_CHILDREN; i++)
	{
		if (octants[i].containsRegion(obj))
		{
			if (children[i] != nullptr)
			{
				return children[i]->insert(obj);
			}
			else
			{
				children[i] = new node(octants[i], { obj });
				States::activateIndex(&activeOctants, i);
				children[i]->parent = this;
				children[i]->build();
				hasChildren = true;
				return true;
			}
		}
	}

	//objects.push_back(obj);
	return true;
}

void Octree::node::destroy()
{
	if (children != nullptr)
	{
		for (int flags = activeOctants, i = 0; flags > 0; flags >> 1, i++)
		{
			if (States::isIndexActive(&flags, 0))
			{
				if (children[i] != nullptr)
				{
					children[i]->destroy();
					delete children[i];
					children[i] = nullptr;
				}
			}
		}
	}

	objects.clear();
	while (queue.size() != 0)
	{
		queue.pop();
	}
}
