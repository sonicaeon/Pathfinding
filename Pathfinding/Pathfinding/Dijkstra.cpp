#include "pch.h"
#include "Dijkstra.h"
#include <queue>

namespace Pathfinding
{
	// comparision for priority queue
	struct path_cmp
	{
		bool operator()(std::shared_ptr<Library::Node> a, std::shared_ptr<Library::Node> b) const
		{
			return a->PathCost() > b->PathCost();
		}
	};

	std::deque<std::shared_ptr<Library::Node>> Pathfinding::Dijkstra::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet)
	{
		std::shared_ptr<Library::Node> currentNode;
		// Construct the frontier starting from the start node
		std::priority_queue<std::shared_ptr<Library::Node>, std::vector<std::shared_ptr<Library::Node>>, path_cmp> frontier;
		std::deque<std::shared_ptr<Library::Node>> openSet;
		float pathCost;
		frontier.push(start);
		closedSet.insert(start);

		// visit each of it's neighbors and put them in the visited Set (closedSet)
		while (!(frontier.empty()))
		{
			// set current node to the one w/ the least path cost
			currentNode = frontier.top();
			frontier.pop();

			// Exit early test
			if (currentNode == end)
				break;
			
			for (const auto& weakNeighbor : currentNode->Neighbors())
			{
				auto neighbor = weakNeighbor.lock();

				// calculate the path cost to this neighbor
				pathCost = currentNode->PathCost() + 1;
				
				// add all new neighbor's to the frontier if they haven't already been visited/exist in the frontier & set their parent node as the current node
				// or if pathCost is less than the previous path cost, update the path cost for the neighbor
				if (!(closedSet.count(neighbor)) || pathCost < neighbor->PathCost())
				{
					neighbor->SetParent(currentNode);
					neighbor->SetPathCost(pathCost);
					frontier.push(neighbor);
					closedSet.insert(neighbor);
				}
			}
		}

		// if end node is not in the closed set or the set is empty, exit early, no path exists
		if (!(closedSet.count(end)) || closedSet.empty())
			return std::deque<std::shared_ptr<Library::Node>>();

		// Otherwise, construct the path starting from the end node retracing back to start node
		std::deque<std::shared_ptr<Library::Node>> path;
		currentNode = end;
		while (currentNode != start)
		{
			path.push_front(currentNode);
			currentNode = currentNode->Parent().lock();
		}

		// return the generated path
		return path;
	}
}