#include "bounds.h"

BoundingRegion::BoundingRegion(BoundTypes type)
	:type(type){}

BoundingRegion::BoundingRegion(glm::vec3 center, float radius)
	:type(BoundTypes::SPHERE),center(center),radius(radius){}

BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
	:type(BoundTypes::AABB),min(min),max(max){}

glm::vec3 BoundingRegion::calculateCenter()
{	
	return (type == BoundTypes::AABB)?(min+max)/2.0f:center;
}

glm::vec3 BoundingRegion::calculateDimensions()
{
	return (type == BoundTypes::AABB) ? (max - min):glm::vec3(2.0f*radius);
}

bool BoundingRegion::containsPoint(glm::vec3 pt)
{
	if (type == BoundTypes::AABB)
	{
		return (pt.x >= min.x) && (pt.x <= max.x)
			&& (pt.y >= min.y) && (pt.y <= max.y)
			&& (pt.z >= min.z) && (pt.z <= max.z);
	}
	else
	{
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			distSquared += (center[i] - pt[i]) * (center[i] - pt[i]);
		}
		return distSquared <= (radius*radius);
	}
	
}

bool BoundingRegion::containsRegion(BoundingRegion br)
{
	if (br.type == BoundTypes::AABB)
	{
		return containsPoint(br.min) && containsPoint(br.max);
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE)
	{
		return glm::length(center - br.center) + br.radius < radius;
	}
	else
	{
		if (!containsPoint(br.center))
		{
			return false;
		}
		for (int i = 0; i < 3; i++)
		{
			if (abs(max[i] - br.center[i] < br.radius ||
				abs(br.center[i] - min[i] < br.radius)))
			{
				return false;
			}
		}
	}
	return true;
}

bool BoundingRegion::intersectsWith(BoundingRegion br)
{
	if (type == BoundTypes::AABB && br.type == BoundTypes::AABB)
	{
		glm::vec3 rad = calculateDimensions() / 2.0f;
		glm::vec3 radBr = br.calculateDimensions() / 2.0f;
		glm::vec3 center = calculateCenter();
		glm::vec3 centerBr = br.calculateCenter();

		glm::vec3 dist = abs(center - centerBr);

		for (int i = 0; i < 3; i++)
		{
			if (dist[i] > rad[i] + radBr[i])
			{
				return false;
			}
		}
		return true;
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE)
	{
		return glm::length(center - br.center) < (radius + br.radius);
	}
	else if (type == BoundTypes::SPHERE)
	{
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			if (center[i] < br.min[i])
			{
				distSquared += (br.min[i] - center[i]) * (br.min[i] - center[i]);
			}
			else if (center[i] > br.max[i])
			{
				distSquared += (center[i] - br.max[i]) * (center[i] - br.max[i]);
			}
		}
		return distSquared < (radius* radius);
	}
	else
	{
		return br.intersectsWith(*this);
	}
	
}

bool BoundingRegion::operator==(BoundingRegion br)
{
	if (type != br.type)
	{
		return false;
	}
	if (type == BoundTypes::AABB)
	{
		return min == br.min && max == br.max;
	}
	else
	{
		return center == br.center && radius == br.radius;
	}
}
 