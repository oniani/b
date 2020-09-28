#include "b.hpp"

CommandVertex::CommandVertex(std::string specifier,
                             std::string exec,
                             std::vector<std::string> dependencies)
    : specifier(specifier)
    , exec(exec)
    , dependencies(dependencies)
{};
    
void CommandVertex::run()
{
    // Takes a C string as an argument
    system(std::string(this->exec).c_str());
}

bool proper(std::string line)
{
    if (line.empty() || line[0] == '#')
        return false;

    for (int i = 1; i < line.size(); i++) {
        if (line[i] != ' ') {
            if (line[i] == '#' || line[i] == '\t') {
                return false;
            }

            else if (isalpha(line[i])) {
                return true;
            }
        }
    }

    return false;
}

std::vector<std::string> get_proper_lines(std::string filepath)
{
    std::vector<std::string> lines;
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open())
        perror("Error while opening the file");

    // Get only if not a comment
    while (std::getline(file, line))
        if (proper(line))
            lines.push_back(line);

    if (file.bad())
        perror("Error while reading the file");

    return lines;
}

std::vector<std::string> split(std::string str, char delimiter)
{
    std::vector<std::string> internal;

    // Turn the string into a stream
    std::stringstream ss(str);
    std::string token;

    while(getline(ss, token, delimiter)) {
        std::string_view ref = token;
        ref.remove_prefix(std::min(ref.find_first_not_of(" "), ref.size()));
        ref.remove_suffix(std::min(ref.find_first_not_of(" "), ref.size()));
        internal.push_back(std::string(ref));
    }

    return internal;
}

Graph build_graph(std::vector<std::string> lines, char delimiter)
{
    Graph cmds;

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].find(DELIMITER) != std::string::npos) {
            std::vector<std::string> all = split(lines[i], delimiter);

            // std::string
            std::string specifier = all[0];

            // Exec
            std::string_view sv = lines[i + 1];
            sv.remove_prefix(std::min(sv.find_first_not_of(" "), sv.size()));
            std::string exec = std::string(sv);

            // Dependencies
            std::vector<std::string> dependencies;
            for (std::string item : split(all[1], ' ')) {
                std::string ref = item;
                dependencies.push_back(ref);
            }

            // Create the command and push it onto the cmds
            CommandVertex cmd = CommandVertex(specifier, exec, dependencies);
            cmd.indegree = dependencies.size();
            cmds.push_back(cmd);
        }
        else {
            continue;
        }
    }

    return cmds;
}

CommandVertex get_vertex(Graph g, std::string specifier)
{
    for (int i = 0; i < g.size(); i++) {
        if (g[i].specifier == specifier) {
            return g[i];
        }
    }

    throw("The vertex does not exist in the graph");
}

std::vector<std::string> get_all_dependencies(Graph g, std::string specifier)
{
    std::vector<std::string> subgraph;
    std::set<std::string> visited;
    std::vector<std::string> vertices;

    vertices.push_back(specifier);

    while (vertices.size() != 0) {
        std::string vertex = vertices.back();
        vertices.pop_back();

        if (visited.find(vertex) == visited.end()) {
            visited.insert(vertex);
            subgraph.push_back(vertex);

            // TODO: Should be optimized, `std::map`?
            // NOTE: Though, `std::map` uses the built-in hasher that has
            //       a bad performance due to backward compatibility
            std::vector<std::string> deps = get_vertex(g, vertex).dependencies;
            for (std::string dep_name : deps) {
                vertices.push_back(dep_name);
            }
        }

    }

    return subgraph;
}

std::vector<CommandVertex> topological_sort(Graph g)
{
    std::vector<CommandVertex> no_incoming;
    
    // Keep track of the vertices with no incoming edges
    for (CommandVertex vertex : g) {
        if (vertex.indegree == 0) {
            no_incoming.push_back(vertex);
        }
    }

    // A topological ordering
    // Should populate this vector
    std::vector<CommandVertex> topological_ordering;

    // As long as there exists a vertex with no incoming edges
    while (no_incoming.size() > 0) {
        // Add the vertex to the ordering and remove it from the vector
        CommandVertex vertex = no_incoming.back();
        no_incoming.pop_back();
        topological_ordering.push_back(vertex);
        
        // Decrement the indegree of the neighbors of the removed vertex
        for (int i = 0; i < g.size(); i++) {
            // Get all dependencies
            std::vector<std::string> deps = g[i].dependencies;

            // `std::find` is more efficient than `std::count` since it stops
            // the search once the element is found
            if (std::find(deps.begin(), deps.end(), vertex.specifier)
                    != deps.end()) {
                g[i].indegree -= 1;
                if (g[i].indegree == 0) {
                    no_incoming.push_back(g[i]);
                }
            }
        }
    }

    // If we have all vertices, we are done
    if (topological_ordering.size() == g.size()) {
        return topological_ordering;
    }
   
    // Otherwise, we have a cycle
    throw("Circular dependencies -> the dependency graph has a cycle");
}

int main(int argc, char **argv)
{
    // The system looks for the file `b.txt`
    std::vector<std::string> lines = get_proper_lines("b.txt");
    // Build a dependency graph
    Graph dependency_graph = build_graph(lines, DELIMITER);

    // Validate the command line arguments
    if (argc == 1) {
        return 0;
    }

    if (argc != 2) {
        throw("Redundancy in the argument list");
    }

    // Get the specifier
    std::string specifier = argv[1];

    // Make sure the command exists
    // If it does, construct the subgraph, construct the topological ordering,
    // and execute the commands
    bool exists = false;
    for (int i = 0; i < dependency_graph.size(); i++) {
        if (dependency_graph[i].specifier == std::string(specifier)) {
            // It exists
            exists = true;

            // If there are no dependencies, the associated command
            // gets executed
            if (dependency_graph[i].dependencies.empty()) {
                dependency_graph[i].run();
            }
            else {
                // If there are some dependencies, a subgraph gets constructed
                // and the topological ordering is created
                // Finally, we iterate over all dependencies and run them
                std::vector<std::string> dependencies =
                    get_all_dependencies(dependency_graph, specifier);
                

                // Create a subgraph of the command and all associated
                // dependencies
                Graph subgraph;
                for (std::string dep : dependencies) {
                    subgraph.push_back(get_vertex(dependency_graph, dep));
                }

                // Return the topological ordering of the graph
                subgraph = topological_sort(subgraph);
                subgraph.pop_back();

                // Run the commands in a topological order
                for (CommandVertex vertex : subgraph) {
                    std::cout << vertex.specifier << std::endl;
                    vertex.run();
                }
            }
        }
    }

    if (!exists) {
        std::cout << "The specifier does not exist" << std::endl;
    }
}
