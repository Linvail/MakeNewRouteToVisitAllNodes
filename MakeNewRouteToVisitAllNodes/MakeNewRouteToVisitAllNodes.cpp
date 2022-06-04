// MakeNewRouteToVisitAllNodes.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <deque>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

//---------------------------------------------------------------------------------------
// Inspired by Youtube video: https://www.youtube.com/watch?v=qz9tKlF431k
// (Mock Google Coding Interview)
//
// It's said that the similar question was asked in a Airbnb interview.
//
// Idea:
// 1. Find strongly connected component.
// 2. Compress into several component with different Id (or pick a node as representative
//    node and use its Id?).
// 3. Do topological sort on the components. Count the number of components that has
//    0 in-degree.
//
// Input:
// airports = {"BGI", "CDG", "DEL", "DOH", "DSM", "EWR", "EYW", "HND", "ICN",
// "JFK", "LGA", "LHR", "ORD", "SAN", "SFO", "SIN", "TLV", "BUD" };
// routes =
// {
// {"DSM", "ORD"}, {"ORD", "BGI"}, {"BGI", "LGA"}, {"SIN", "CDG"},
// {"CDG", "SIN"}, {"CDG", "BUD"}, {"DEL", "DOH"}, {"DEL", "CDG"}, {"TLV", "DEL"},
// {"EWR", "HND"}, {"HND", "ICN"}, {"HND", "JFK"}, {"ICN", "JFK"}, {"JFK", "LGA"},
// {"EYW", "LHR"}, {"LHR", "SFO"}, {"SFO", "SAN"}, {"SFO", "DSM"}, {"SAN", "EYW"}
// };
// startingAirport = "LGA";
//
// Find out how many routes you will need to create to ensure you can travel all airports.
// Answer: 3
//
// The following code will also give what routes they are:
// Add lines from LGA to [LHR,TLV,EWR].
//---------------------------------------------------------------------------------------

using RouteType = pair<string, string>;
using RoutesMapType = unordered_map<string, unordered_set<string>>;

void towardDfs(const string& node, const RoutesMapType& graph, deque<string>& order, unordered_set<string>& visited)
{
    if (!visited.count(node))
    {
        visited.insert(node);
        for (const auto& n : graph.at(node))
        {
            towardDfs(n, graph, order, visited);
        }
        order.push_front(node);
    }
}

void reversedDfs(const string& node, const string& root, const RoutesMapType& graph, unordered_map<string, string>& components)
{
    if (!components.count(node))
    {
        components[node] = root;
        for (const auto& n : graph.at(node))
        {
            reversedDfs(n, root, graph, components);
        }
    }
}

//! This function implements the Kosaraju's algorithm. It is not fastest, but it is the simplest.
unordered_map<string, string> findAllSsc(const RoutesMapType& graph, const RoutesMapType& reversedGraph, const unordered_set<string>& nodes)
{
    // Helper to detect if a node has been visited during DFS.
    unordered_set<string> visited;
    // Adjacent list to represent the component.
    // <node's name, root's name of the component that this node belongs to>
    unordered_map<string, string> components;
    // Store the visited node in chronologically order.
    // First visited node is put in the end.
    deque<string> order;

    for (const auto& node : nodes)
    {
        towardDfs(node, graph, order, visited);
    }

    for (const auto& node : order)
    {
        // Call DFS on this node, every node visited by this DFS call will be added into components.
        // The 2nd 'node' parameter means this 'node' is the representative node of that SSC.
        reversedDfs(node, node, reversedGraph, components);
    }

    return components;
}

vector<string> makeNewRoutes(const vector<string>& airports, const vector<RouteType>& routes, const string& startingAirport)
{
    RoutesMapType graph;
    RoutesMapType reversedGraph;
    unordered_set<string> nodes;

    // Scan the routes, build adjacent lists in graph/reversedGraph.
    // Also push every airports we ever meet into nodes.
    for (const auto& edge : routes)
    {
        graph[edge.first].insert(edge.second);
        // Some airports may have 0 outbound, but we still need to create it in the graph.
        // Note that emplace() won't take place if the key already exists.
        graph.emplace(edge.second, unordered_set<string>());
        reversedGraph[edge.second].insert(edge.first);
        // Some airports may have 0 inbound, but we still need to create it in the reversedGraph.
        reversedGraph.emplace(edge.first, unordered_set<string>());
        nodes.insert(edge.first);
        nodes.insert(edge.second);
    }

    // This components stores the component's representative node's name of every node.
    // For example, ("CDG", "CDG"), ("SIN", "CDG") means SIN and CDG belong to the same component - CDG.
    unordered_map<string, string> components = findAllSsc(graph, reversedGraph, nodes);

    // If a SSC's in-degree is 0, we are unable to reach it.
    // That means we need to a line to connect to it.
    // Scan edges and construct the graph of SSCs.
    unordered_map<string, int> inDegreeSsc;
    for (const auto& edge : routes)
    {
        const string& sscOfStart = components[edge.first];
        const string& sscOfdest = components[edge.second];
        // Note that we are dealing with SSC, not airport.
        // If two airports are in the same SSC, skip.
        if (sscOfStart != sscOfdest)
        {
            inDegreeSsc[sscOfdest]++;
        }
        // Ensure every SSC would exist in the inDegreeSsc.
        inDegreeSsc.emplace(sscOfStart, 0);
    }

    // Collect all nodes whose in-degree is 0.
    vector<string> result;
    for (const auto& pr : inDegreeSsc)
    {
        if (pr.second == 0)
        {
            result.push_back(pr.first);
        }
    }

    return result;
}


//! Helper function to print a vector (1D array)
template<typename T>
void PrintVector
	(
	const vector<T>& aInput
	)
{
	cout << "[";
	for (int i = 0; i < aInput.size(); ++i)
	{
		cout << aInput[i];
		if (i != aInput.size() - 1)
		{
			cout << ",";
		}
	}
	cout << "]" << endl;
}

int main()
{
    std::cout << "What routes should we create to make us able to travel all airports?\n";

    // We have 18 airports.
    vector<string> airports =
    { "BGI", "CDG", "DEL", "DOH", "DSM", "EWR", "EYW", "HND", "ICN",
      "JFK", "LGA", "LHR", "ORD", "SAN", "SFO", "SIN", "TLV", "BUD" };
    vector<pair<string, string>> routes =
    { {"DSM", "ORD"}, {"ORD", "BGI"}, {"BGI", "LGA"}, {"SIN", "CDG"},
      {"CDG", "SIN"}, {"CDG", "BUD"}, {"DEL", "DOH"}, {"DEL", "CDG"}, {"TLV", "DEL"},
      {"EWR", "HND"}, {"HND", "ICN"}, {"HND", "JFK"}, {"ICN", "JFK"}, {"JFK", "LGA"},
      {"EYW", "LHR"}, {"LHR", "SFO"}, {"SFO", "SAN"}, {"SFO", "DSM"}, {"SAN", "EYW"} };
    string startingAirport = "LGA";

    auto resultVS = makeNewRoutes(airports, routes, startingAirport);
    cout << "\nMake new routes from " << startingAirport << " to ";
    PrintVector(resultVS);
    cout << "Expected: [LHR,TLV,EWR]\n";

}
