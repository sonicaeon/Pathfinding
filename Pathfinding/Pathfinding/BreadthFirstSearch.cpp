#include "pch.h"
#include "BreadthFirstSearch.h"
#include <cassert>

namespace Pathfinding
{
	std::deque<std::shared_ptr<Library::Node>> Pathfinding::BreadthFirstSearch::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet)
	{
		std::shared_ptr<Library::Node> currentNode;
		// Construct the frontier starting from the start node
		std::deque<std::shared_ptr<Library::Node>> frontier;
		frontier.push_back(start);
		closedSet.insert(start);
		
		// visit each of it's neighbors and put them in the visited Set (closedSet)
		while (!(frontier.empty()))
		{
			// the frontier is a queue of neighbors to visit
			currentNode = frontier.front();
			frontier.pop_front();
			// If node from frontier hasn't been visited yet, and it's not a wall, add it to visited set
			if (!(closedSet.count(currentNode)) && (currentNode->Type() == Library::NodeType::Normal))
			{
				closedSet.insert(currentNode);
				// Early exit/out, if we found the end stop exploring the frontier
				if (currentNode == end)
					break;
			}
			for (const auto& weakNeighbor : currentNode->Neighbors())
			{
				auto neighbor = weakNeighbor.lock();
				assert(neighbor != nullptr);

				// if neighbor is a wall, skip it
				if (neighbor->Type() == Library::NodeType::Wall)
					continue;

				// add all new neighbor's to the frontier if they haven't already been visited/exist in the frontier & set their parent node as the current node
				if (std::find(frontier.begin(), frontier.end(), neighbor) == frontier.end() && !(closedSet.count(neighbor)))
				{
					frontier.push_back(neighbor);
					neighbor->SetParent(currentNode);
				}
			}
		}

		// if end node is not in the closed set or the set is empty, exit early, no path exists
		if(!(closedSet.count(end)) || closedSet.empty())
			return std::deque<std::shared_ptr<Library::Node>>();

		// Otherwise, construct the path starting from the end node retracing back to start node
		std::deque<std::shared_ptr<Library::Node>> path;
		currentNode = end;
		while(currentNode != start)
		{
			path.push_front(currentNode);
			currentNode = currentNode->Parent().lock();
		}
		
		// return the BFS generated path
		return path;
	}
}

