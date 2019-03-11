#include "pch.h"
#include "AStar.h"
#include <queue>
#include <functional>

namespace Pathfinding
{
	// calculates the Manhattan distance between a starting node and an ending node
	float Manhattan(std::shared_ptr<Library::Node> current, std::shared_ptr<Library::Node> end)
	{
		float result = (float)abs(end->Location().X - current->Location().X) + (float)abs(end->Location().Y - current->Location().Y);
		return result;
	}

	// calculates the Euclidian distance b/w two nodes (Pythagorean theorem)
	float Euclidean(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end)
	{
		float a, b, c;

		a = (float)abs(end->Location().X - start->Location().X);
		b = (float)abs(end->Location().Y - start->Location().Y);
		c = sqrt((pow(a, (float)2.0)) + (pow(b, (float)2.0)));
		return c;
	}

	// comparision for sorting the open set
	struct totalCost_cmp
	{
		bool operator()(std::shared_ptr<Library::Node> a, std::shared_ptr<Library::Node> b) const
		{
			return a->TotalCost() < b->TotalCost();
		}
	};

	std::deque<std::shared_ptr<Library::Node>> AStar::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet)
	{
		UNREFERENCED_PARAMETER(start);
		UNREFERENCED_PARAMETER(end);
		UNREFERENCED_PARAMETER(closedSet);
		return std::deque<std::shared_ptr<Library::Node>>();
	}

	std::deque<std::shared_ptr<Library::Node>> AStar::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet, int& heuristics)
	{
		std::shared_ptr<Library::Node> currentNode;
		std::set<std::shared_ptr<Library::Node>, totalCost_cmp> frontier;
		std::set<std::shared_ptr<Library::Node>, totalCost_cmp>::iterator it;
		float pathCost;
		currentNode = start;
		closedSet.insert(currentNode);

		// free function to calculate the heuristics, TODO: using UI selection change b/w manhatten or euclidian from the user
		std::function<float(std::shared_ptr<Library::Node>, std::shared_ptr<Library::Node>)> heuristic;
		// default to manhatten for now
		if (heuristics == 1)
			heuristic = Euclidean;
		else
			heuristic = Manhattan;

		// visit each of it's neighbors and put them in the visited Set (closedSet)
		do
		{
			for (const auto& weakNeighbor : currentNode->Neighbors())
			{
				auto neighbor = weakNeighbor.lock();

				// if neighbor is a wall or already in the closedSet, skip it
				if (closedSet.count(neighbor))
					continue;

				// calculate the path cost to this neighbor
				pathCost = currentNode->PathCost() + 1;
				
				// if openSet contains neighbor and if the pathCost should be updated and it's new parent
				if (std::find(frontier.begin(), frontier.end(), neighbor) != frontier.end() && pathCost < neighbor->PathCost())
				{
					neighbor->SetParent(currentNode);
					neighbor->SetPathCost(pathCost);
				}
				else
				{
					// if node isn't in the set, add it and all of it's values
					neighbor->SetHeuristic(heuristic(neighbor, end));
					neighbor->SetParent(currentNode);
					neighbor->SetPathCost(pathCost);
					frontier.insert(neighbor);
				}
			}
			if (frontier.empty())
				break;

			// next node is the one w/ lowest total cost
			it = frontier.begin();
			currentNode = *it;
			frontier.erase(it);
			closedSet.insert(currentNode);
		} while (currentNode != end);

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