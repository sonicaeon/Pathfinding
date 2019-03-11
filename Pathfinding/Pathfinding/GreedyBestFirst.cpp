#include "pch.h"
#include "GreedyBestFirst.h"
#include <queue>
#include <functional>

// calculates the Manhatten distance between a starting node and an ending node
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

namespace Pathfinding
{
	// comparision for priority queue
	struct heuristic_cmp
	{
		bool operator()(std::shared_ptr<Library::Node> a, std::shared_ptr<Library::Node> b) const
		{
			return a->Heuristic() > b->Heuristic();
		}
	};

	std::deque<std::shared_ptr<Library::Node>> GreedyBestFirst::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet)
	{
		UNREFERENCED_PARAMETER(start);
		UNREFERENCED_PARAMETER(end);
		UNREFERENCED_PARAMETER(closedSet);
		return std::deque<std::shared_ptr<Library::Node>>();
	}

	std::deque<std::shared_ptr<Library::Node>> GreedyBestFirst::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet, int& heuristics)
	{
		// priority queue for the heuristics of nodes sorting by lower heuristic/path cost > higher heuristic
		std::priority_queue<std::shared_ptr<Library::Node>, std::vector<std::shared_ptr<Library::Node>>, heuristic_cmp> frontier;
		std::set<std::shared_ptr<Library::Node>> openSet;
		std::shared_ptr<Library::Node> currentNode;
		std::deque<std::shared_ptr<Library::Node>> path;
		currentNode = start;
		closedSet.insert(currentNode);

		// free function to calculate the heuristics, TODO: using UI selection change b/w manhatten or euclidian from the user
		std::function<float(std::shared_ptr<Library::Node>, std::shared_ptr<Library::Node>)> heuristic;
		// default to manhattan
		if (heuristics == 1)
			heuristic = Euclidean;
		else
			heuristic = Manhattan;

		// do greedy best first search
		do {
			for (auto& weakNeighbor : currentNode->Neighbors())
			{
				auto neighbor = weakNeighbor.lock();

				// if the neighbor has already been visited skip it
				if (closedSet.count(neighbor))
					continue;
				else
				{
					neighbor->SetParent(currentNode);
					if (!(openSet.count(neighbor)))
					{
						// if frontier doesn't contain the neighbor compute the heuristic
						neighbor->SetHeuristic(heuristic(neighbor, end));
						
						// add node to the openSet(frontier) and priority queue for sorting heuristics
						frontier.push(neighbor);
						openSet.insert(neighbor);
					}
				}
			}
			if (frontier.empty())
				break;
			// go to the node with the lowest heuristic/cost, remove it from the queue, add it to the closedSet 
			currentNode = frontier.top();
			frontier.pop();
			closedSet.insert(currentNode);
		} while (currentNode != end);

		// if the closed set is empty or the end node isn't in it, exit early there is no path
		if (!(closedSet.count(end)) || closedSet.empty())
			return std::deque<std::shared_ptr<Library::Node>>();

		// Construct the path
		currentNode = end;
		while (currentNode != start)
		{
			path.push_front(currentNode);
			currentNode = currentNode->Parent().lock();
		}

		return path;
	}
}