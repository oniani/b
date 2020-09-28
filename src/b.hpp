#pragma once

#ifndef B_HPP
#define B_HPP

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

// The command class that allows for running the commands
class CommandVertex {
    public:
        // The specifier
        std::string specifier;
        // Line that specifies the additional instructions to be executed
        std::string exec;
        // Neighbors
        std::vector<std::string> dependencies;
        // Indegree
        int indegree;
        // Create a command to execute
        CommandVertex(std::string,
                      std::string,
                      std::vector<std::string>);
        // Run the command
        void run();
};

// Graph is represented as an adjacency list
// NOTE: this be a DAG (Directed Acyclic Graph)
typedef std::vector<CommandVertex> Graph;

// Extract dependencies and commands
Graph build_graph(std::vector<std::string>, const char);
// Get a vertex from the graph
CommandVertex get_vertex(Graph, std::string);
// Build a subgraph out of all dependencies for the command
std::vector<std::string> get_all_dependencies(Graph, std::string);
// Run a topological sort
std::vector<CommandVertex> topological_sort(Graph);

// Delimiter
const char DELIMITER = ':';

// Determines the proper string
bool proper(std::string);
// Get the lines
std::vector<std::string> get_proper_lines(std::string);
// Split on the delimiter
std::vector<std::string> split(std::string, char);

#endif
