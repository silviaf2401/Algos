/*
 * File: Trailblazer.cpp
 * --------------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * This file implements 4 functions for finding paths in a graph:
 * - Breadth First Search
 * - Dijkstra's algorithm
 * - A*
 * - Alternative Route.
 * Dijkstra's algorithm, alternative route and A* rely on the same function, dijkstraorAstar,
 * which takes in parameters to specify whether we are using a heuristic in to under-estimate the
 * cost of travelling between 2 vertices (for A*) or whether or not we are ignoring any edges (for alternative
 * route).
 * Please refer to the function header comments for details on algorithm implementation and details.
 * This file contains the following extension functionality: path found is printed in the console as a list
 * of edges with cost for BFS, Dijkstra or A* algorithms ONLY (console printing is not supported for Alt path algorithm).
 * Please refer to submission #5 in paperless for core functionality.
 *
 */
#include "Trailblazer.h"
#include "queue.h"
#include "RoadGraph.h"
#include "pqueue.h"
#include "basicgraph.h"
using namespace std;
 
static const double SUFFICIENT_DIFFERENCE = 0.2;
/* Function prototypes */
Path dijkstrasorAstar(const RoadGraph& graph, RoadNode* start, RoadNode* end, bool withheuristic, RoadEdge* toignore, double &priority);
bool nodeInAltPath(RoadNode* node, Path altpath);
double calculatePercentageDifference (Path bestPath, Path candidatePath);
void markPointVisited(RoadNode *node, Set<RoadNode*> &visitedPoints);
void exploreNeighbor(const Set<RoadNode*> &visitedPoints, RoadNode* node, RoadEdge* toignore, bool withheurisic,
                     PriorityQueue<Path> &queueofPaths, RoadNode* lastnode, const RoadGraph &graph, Path temp,
                     double currentPriority, RoadNode* end);
void  printPathInfo(Path temp);
 
/* Function: breadthFirstSearch()
 * Usage: breadthFirstSearch(graph, start, end)
 * --------------------------------------------
 * Searches the given graph for a path from the given start vertex to the given end vertex. If a
 * path is found, it is returned as a list of vertexes along that path, with the starting vertex
 * first and the endex vertex last. If no path is found, it returns an empty path. If the start and
 * end vertexes are the same, a one-element vector is returned containing that edge only.
 * Uses Breadth First Search algorithm which explores node neighbors one level away to look for target
 * node before moving to bext level neighbors.
 * Assumptions: graph passed in is not corrupt. Graphs can be directed or undirected.
 * @param: graph type RoadGraph - graph where we'll be searching the path from the given start
 * vertex to the given end vertex.
 * @param: start, type RoadNode* starting vertex for the path we're looking for
 * @param: end, type RoadNode* ending vertex for the path we're looking for
 * @return: pathFound, type Path. Lists the edges (RoadNode*) between the start and end vertex for
 * the path found if a path was found, empty if not path was found.
 */
Path breadthFirstSearch(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    Path pathfound;
    if (start == end) {
        pathfound.add(start);
        printPathInfo(pathfound);
        return pathfound;
    } else {
        Queue<Path> queueofPaths;
        Path startPath;
        startPath.add(start);
        queueofPaths.enqueue(startPath);
        Set<RoadNode*> visitedPoints;
        markPointVisited(start, visitedPoints);
        while (!queueofPaths.isEmpty()) {
            Path temp = queueofPaths.dequeue();
            RoadNode* lastnode = temp[temp.size() - 1];
            markPointVisited(lastnode, visitedPoints);
            if (lastnode == end) {
                printPathInfo(temp);
                return temp;
            }
            Set<RoadNode *> neighbors = graph.neighborsOf(lastnode);
            for (RoadNode* node: neighbors) {
                if (!visitedPoints.contains(node)) {
                    Path newPath = temp;
                    newPath.add(node);
                    node->setColor(Color::YELLOW);
                    queueofPaths.enqueue(newPath);
                }
            }
        }
    }
    printPathInfo(pathfound);
    return pathfound;
}
 
// Prints directions taken between each node
void  printPathInfo(Path temp) {
    if (temp.size() == 0) {
        cout << "The start and end points chosen are not connected! Please choose different locations and try again." << endl << endl;
    } else if (temp.size() == 1) {
        cout << "You did not move from your starting point. Go places! Choose different starting and end points."<< endl << endl;
    } else {
        cout << "Here is your path info:" << endl;
        cout << "Start at node " << temp[0]->nodeName() << endl;
 
        for (int i  = 1 ; i < temp.size() -1; i++) {
                string direction ="";
                if (temp[i]-> location().getX() > temp[i-1] -> location().getX()) {
                    direction += "E";
                } else if  (temp[i]-> location().getX() < temp[i-1]-> location().getX()) {
                    direction += "W";
                } else {
                    direction = direction;
                }
                if (temp[i]-> location().getY() > temp[i-1] -> location().getY()) {
                    direction += "S";
                } else if  (temp[i]-> location().getY() < temp[i-1]-> location().getY()) {
                    direction += "N";
                }  else {
                direction = direction;
                }
                cout << "Then go " + direction + " towards " << temp[i]->nodeName() << endl;
 
 
        }
        string direction ="";
        if (temp[temp.size() -1]-> location().getX() > temp[temp.size() -2] -> location().getX()) {
            direction += "E";
        } else if  (temp[temp.size() -1]->location().getX() < temp[temp.size() -2] ->location().getX()) {
            direction += "W";
        }
        if (temp[temp.size() -1]-> location().getY() > temp[temp.size() -2] -> location().getY()) {
            direction += "S";
        } else if  (temp[temp.size() -1]-> location().getY() < temp[temp.size() -2] -> location().getY()) {
            direction += "N";
        }
        cout << "Then go " + direction + " towards " << temp[temp.size() -1]->nodeName() << endl;
        cout << "End at node " << temp[temp.size() -1] ->nodeName() << endl;
    }
}
 
/* Function: markPointVisited()
 * Usage: markPointVisited(node, visitedPoints)
 * --------------------------------------------
 * Marks the RoadNode* passed as parameter to this method as visited by changing its color to green,
 * and adding it to the set of RoadNode* also passed as parameter to this function.
 * @param: node, type RoadNode*: RoadNode* we're marking visited
 * @parame: visitedPoints: type Set<RoadNode*>, set of visited RoadNode*.
 */
void markPointVisited(RoadNode *node, Set<RoadNode*> &visitedPoints) {
    visitedPoints.add(node);
    node->setColor(Color::GREEN);
}
 
/* Function: dijkstrasorAstar()
 * Usage: dijstrasorAstar(graph, start, end, withheuristic, toignore, priority)
 * -----------------------------------------------------------------------------
 * Searches the given graph for the shortest path from the given start vertex to the given end vertex. If a
 * path is found, it is returned as a list of vertexes along that path, with the starting vertex
 * first and the endex vertex last. If no path is found, it returns an empty path. If the start and
 * end vertexes are the same, a one-element vector is returned containing that edge only.
 * Uses Dijkstra's algorithm or a* and can be used for an alternative route calculation,
 * the algorithm used is determined by the parameters passed to this method: if withheuristic is true,
 * a* will be used, if withheuristic is false, Dijkstra's will be used, if toignore is not null, the
 * node passed as parameter to the method is ignored (used for alternative path caculation).
 * Dijkstra's algorithm, from the start vertex, explores the neighbor nodes first, before moving to
 * the next level neighbors, in priority order. It returns the shortest path between start and end point,
 * if path exists, and uses a priority queue to do so.
 * A* algorithm is a variation of Dijkstra's algorithm using a heuristic to fine-tune the order of elements
 * explored, so that more likely desirable elements are explored first. The heuristic used is the time
 * it would take to get between nodes assuming there is a super highway between the 2 nodes. The heuristic
 * implemented under-estimates the cost of traveling between 2 vertices. A* returns a path with the same
 * length and cost as Dijkstra.
 * Assumptions: graph passed in is not corrupt. Graph can be directed or undirected.
 * @param: graph type RoadGraph - graph where we'll be searching the path from the given start
 * vertex to the given end vertex.
 * @param: start type RoadNode* starting vertex for the path we're looking for.
 * @param: end, type RoadNode* ending vertex for the path we're looking for.
 * @param: withheuristic, type boolean. If true, a* algorithm used to search for a path,
 * if false, Dijkstra's algorithm will be used to search for a path.
 * @param: toignore, type RoadEdge* - edge to ignore while calculating an alternative route
 * @param: priority - type double - priority of the path being returned by this function (if path exists).
 * @return: pathFound, type Path. Lists the edges (RoadNode*) between the start and end vertex for
 * the path found if a path was found, empty if not path was found.
 */
Path dijkstrasorAstar(const RoadGraph& graph, RoadNode* start, RoadNode* end, bool withheuristic,
                      RoadEdge* toignore, double &priority) {
    Path pathFound;
    if (start == end) {
        pathFound.add(start);
        if (toignore == NULL) {
            printPathInfo(pathFound);
        }
        return pathFound;
    } else {
        PriorityQueue<Path> queueofPaths;
        Path startPath;
        startPath.add(start);
        queueofPaths.enqueue(startPath, 0);
        Set<RoadNode*> visitedPoints;
        markPointVisited(start, visitedPoints);
        while (!queueofPaths.isEmpty()) {
            double currentPriority = queueofPaths.peekPriority();
            Path temp = queueofPaths.dequeue();
            RoadNode* lastnode = temp[temp.size() - 1];
            markPointVisited(lastnode, visitedPoints);
            if (lastnode == end) {
                priority = currentPriority;
                if (toignore == NULL) {
                    printPathInfo(temp);
                }
                return temp;
            }
            Set<RoadNode *> neighbors = graph.neighborsOf(lastnode);
            for (RoadNode* node: neighbors) {
                exploreNeighbor(visitedPoints, node, toignore, withheuristic, queueofPaths, lastnode, graph,
                                temp, currentPriority, end);
            }
        }
    }
    if (toignore == NULL) {
        printPathInfo(pathFound);
    }
    return pathFound;
}
 
 
/* Function: exploreNeighbor()
 * Usage: exploreNeighbor(visitedPoints, node, toignore, withheuristic, queueofPaths, lastnode, graph, temp, currentPriority, end);
 * ----------------------------------------------------------------------------
 * Examines node (lastnode neighbor) being passed as parameter, creates a new path with it if unvisited,
 * and enqueues the new path with the corresponding priority. Priority calculation is determined by parameter
 * withheuristic which specifies whether or not we are using a heuristic for it (distinguish
 * between Dijkstra and a*), the edge passed as toignore parameter will be ignored (used for alternative
 * path caculation).
 * @param visitedPoints: type Set<RoadNode*> - RoadNode* that have already been visited in our graph
 * @param node: type RoadNode*, node we're considering to create a new path with.
 * @param toignore: type RoadEdge* - edge to ignore (if not NULL). Used in alternative path
 * calculation only.
 * @param withheuristic: bool type, if true, priority calculation is that of a* algorithm,
 * if false priorty calculation is that of dijkstra's algorithm.
 * @param queueofPaths: type PriorityQueue<Path>, contains paths being explored to find shortest path
 * between start and end nodes in the calling function.
 * @param lastnode: type RoadNode* - last node in temp, path we're using as a starting point to create
 * a new path with one of lastnode neighbors, node (if node not already visited).
 * @param: graph type RoadGraph - graph where we're searching the path from the given start
 * vertex to the given end vertex in calling function.
 * @param: temp, type Path, path being used as starting point to create a new path that includes node
 * (if node not visited).
 * @param: end, type RoadNode*, destination node for our path in calling function.
 * Note: this decomposition was suggested by Chris.
 */
void exploreNeighbor(const Set<RoadNode*> &visitedPoints, RoadNode* node, RoadEdge* toignore, bool withheuristic,
                     PriorityQueue<Path> &queueofPaths, RoadNode* lastnode, const RoadGraph &graph, Path temp,
                     double currentPriority, RoadNode* end) {
        if (!visitedPoints.contains(node)) {
            Path newPath = temp;
            newPath.add(node);
            node->setColor(Color::YELLOW);
            RoadEdge* edge = graph.edgeBetween(lastnode, node);
            if (edge != toignore) {
                double newpriority;
                if (!withheuristic) {
                    newpriority = currentPriority + edge->cost();
                } else {
                    double maxRoadSpeed = graph.maxRoadSpeed();
                    double newEstimate = graph.crowFlyDistanceBetween(node, end)/maxRoadSpeed;
                    double toRemove = graph.crowFlyDistanceBetween(lastnode,end)/maxRoadSpeed;
                    newpriority = currentPriority - toRemove + edge->cost() + newEstimate;
                }
                queueofPaths.enqueue(newPath, newpriority);
           }
        }
}
 
/* Function: dijkstrasAlgorithm()
 * Usage: dijkstrasAlgorithm(graph, start, end)
 * -----------------------------------------------------------------------------
 * Searches the given graph for the shortest path from the given start vertex to the given end vertex.
 * If a path is found, it is returned as a list of vertexes along that path, with the starting vertex
 * first and the endex vertex last. If no path is found, it returns an empty path. If the start and
 * end vertexes are the same, a one-element vector is returned containing that edge only.
 * Uses Dijkstra's algorithm to find shortest path; from the start vertex, explores the neighbor nodes first,
 * before moving to the next level neighbors, in priority order. Relies on helper function.
 * Assumptions: graph passed in is not corrupt. Graph can be directed or undirected.
 * @param: graph type RoadGraph - graph where we'll be searching the path from the given start
 * @param: start type RoadNode* starting vertex for the path we're looking for.
 * @param: end, type RoadNode* ending vertex for the path we're looking for.
 * @return: Path type, lists the edges (RoadNode*) between the start and end vertex for
 * the path found if a path was found, empty if not path was found.
 */
Path dijkstrasAlgorithm(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    /* Priority and edge to ignore are not needed for Dijkstra's algorithm but they are needed
     * for the helper function that dijkstrasAlgorithm, aStar and alternative route use.
     */
    double priority;
    RoadEdge* toignore = NULL;
    return dijkstrasorAstar(graph, start, end, false, toignore, priority);
}
 
 
/* Function: aStar()
 * Usage: aStar(graph, start, end)
 * -----------------------------------------------------------------------------
 * Searches the given graph for the shortest path from the given start vertex to the given end vertex.
 * If a path is found, it is returned as a list of vertexes along that path, with the starting vertex
 * first and the endex vertex last. If no path is found, it returns an empty path. If the start and
 * end vertexes are the same, a one-element vector is returned containing that edge only.
 * Uses A* algorithm to find shortest path, from the start vertex, explores the neighbor nodes first,
 * before moving to the next level neighbors, in priority order. Relies on helper function.
 * Uses a heuristic to fine-tune the order of elements explored, so that more likely desirable
 * elements are explored first. The heuristic used is the time it would take to get between nodes
 * assuming there is a super highway between the 2 nodes. The heuristic implemented under-estimates
 * the cost of traveling between 2 vertices.
 * Assumptions: graph passed in is not corrupt. Graph can be directed or undirected.
 * @param: graph type RoadGraph - graph where we'll be searching the path from the given start
 * @param: start type RoadNode* starting vertex for the path we're looking for.
 * @param: end, type RoadNode* ending vertex for the path we're looking for.
 * @return: Path type, lists the edges (RoadNode*) between the start and end vertex for
 * the path found if a path was found, empty if not path was found.
 */
Path aStar(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    /* Priority and edge to ignore are not needed for aStar algorithm but they are needed
     * for the helper function that dijkstrasAlgorithm, aStar and alternative route use.
     */
    double priority;
    RoadEdge* toignore = NULL;
    return dijkstrasorAstar(graph, start, end, true, toignore, priority);
}
 
/* Function: alternativeRoute()
 * Usage: alternativeRoute(graph, start, end)
 * -----------------------------------------------------------------------------
 * Searches the given graph for an alternative route to the shortest path from the given start vertex
 * to the given end vertex.
 * If a path is found, it is returned as a list of vertexes along that path, with the starting vertex
 * first and the endex vertex last. If no path is found, it returns an empty path. If the start and
 * end vertexes are the same, a one-element vector is returned containing that edge only.
 * Uses A* algorithm to find shortest path, from the start vertex, then for each edge in the shortest path
 * path it calculates the shortest path from start to end that ignores that edge. Each search returns a
 * candidate alternate route, from all the candidate alternate routes, only those that have a percentage
 * difference from the original path > SUFFICIENT_DIFFERENCE are considered. Function returns the lowest
 * cost aternate route that is sufficiently different from the original best path.
 * Assumptions: graph passed in is not corrupt. Graph can be directed or undirected.
 * @param: graph type RoadGraph - graph where we'll be searching the path from the given start
 * @param: start type RoadNode* starting vertex for the path we're looking for.
 * @param: end, type RoadNode* ending vertex for the path we're looking for.
 * @return: Path type, lists the edges (RoadNode*) between the start and end vertex for
 * the path found if a path was found, empty if not path was found.
 */
Path alternativeRoute(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    // find best path using a*
    Path bestPath = aStar(graph, start, end);
    //cout << bestPath.size() << " this is my best path size " << endl;
    PriorityQueue<Path> alternativeBestPaths;
    Set<RoadEdge*> setofEdges;
    // find set of edges in best path
    for (int i = 0; i < bestPath.size() - 1; i++) {
        setofEdges.add(graph.edgeBetween(bestPath[i], bestPath[i + 1]));
    }
    double priority;
    // find candidate alternative routes to best path by ignoring each of the edges in best path
    for (RoadEdge* edge: setofEdges) {
        Path altPath = dijkstrasorAstar(graph, start, end, true, edge, priority);
        if (altPath.size() > 0) {
            // calculate the difference between bestPath and candidate alternative route found
            double difference = calculatePercentageDifference(bestPath, altPath);
            if (difference > SUFFICIENT_DIFFERENCE) {
            // only keep track of the alternative routes that are sufficiently different from best path
               alternativeBestPaths.enqueue(altPath, priority);
            }
        }
    }
    if (alternativeBestPaths.size() > 0) {
        // returns the lowest cost aternate route that is sufficiently different from the original best path
        printPathInfo(alternativeBestPaths.dequeue());
        return alternativeBestPaths.dequeue();
    } else {
        //printPathInfo({});
        return {};
    }
}
 
/* Method: calculatePercentageDifference()
 * Usage: calculatePercentageDifference(bestPath, candidatePath)
 * -------------------------------------------------------------
 * Calculates the percentage difference between best path and candidate path as the number of nodes
 * in the candidate path that are not in the best path, divided by the number of nodes in the best path.
 * Assumptions: best path has at least one element.
 * @param: bestPath, type Path. BestPath being used for calculation
 * @param: candidatePath, type Path. Candidate Path for which we're calculating the % difference
 * with the best path.
 * @return: type double, percentage difference between best path and candidate path.
 */
double calculatePercentageDifference (Path bestPath, Path candidatePath) {
    double counterNodesNotInB = 0;
    for (RoadNode* node: candidatePath) {
        bool contains = true;
        contains = nodeInAltPath(node, bestPath);
        if (!contains) {
            counterNodesNotInB++;
        }
        contains = true;
    }
    return counterNodesNotInB/bestPath.size();
}
 
/* Method: nodeInAltPath()
 * Usage: nodeInAltPath(node, altPath)
 * -------------------------------------------------------------
 * Checks if node is in altPath. Returns true if found, false otherwise.
 * @param: node, type RoadNode*. Node we're searching for in altPath.
 * @param: altpath, type Path. Path we're searching node in.
 * @return: boolean, true if altpath contains node, false otherwise
 */
bool nodeInAltPath(RoadNode* node, Path altpath) {
     for (RoadNode* nodeinPath: altpath) {
         if (node == nodeinPath) {
             return true;
         }
     }
     return false;
 }
 
 