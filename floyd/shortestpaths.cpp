#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <iomanip>

using namespace std;

int len(long num) {
    //returns the digits required to represent a number
    if (!num) {
        return 1; //if 0 return 1
    }
    int res = 0;
    while (num) {
        num = num / 10; //keep shifting until 0
        res++;
    }
    return res;
}

void display_table(long** const matrix, const string &label, int num_vertices, long INF, const bool use_letters = false) {
    //Displays the matrix on the screen formatted as a table.
    cout << label << endl;
    long max_val = 0;

    // Find the maximum value in the matrix
    for (int i = 0; i < num_vertices; i++) {
        for (int j = 0; j < num_vertices; j++) {
            long cell = matrix[i][j];
            if (cell < INF && cell > max_val) {
                max_val = matrix[i][j];
            }
        }
    }

    // Calculate the maximum cell width
    int max_cell_width = use_letters ? len(max_val) : len(max(static_cast<long>(num_vertices), max_val));

    // Print column headers
    cout << ' ';
    for (int j = 0; j < num_vertices; j++) {
        cout << setw(max_cell_width + 1) << static_cast<char>(j + 'A');
    }
    cout << endl;

    // Print each row of the table
    for (int i = 0; i < num_vertices; i++) {
        cout << static_cast<char>(i + 'A');
        for (int j = 0; j < num_vertices; j++) {
            cout << " " << setw(max_cell_width);
            if (matrix[i][j] == INF) {
                cout << "-";
            } else if (use_letters) {
                cout << static_cast<char>(matrix[i][j] + 'A');
            } else {
                cout << matrix[i][j];
            }
        }
        cout << endl;
    }
    cout << endl;
}

void construct_path(long** const inter, int i, int j, long INF) {
    //constructs and prints the path between i and j recursively using intermediate matrix
    if (inter[i][j] == INF) { //no intermediate so just print the vertex (only destination to avoid duplicates)
        cout << " -> " << (char)(j+'A');
    } else {
        //split the path construction based off of the intermediate vertex
        construct_path(inter, i, inter[i][j], INF);
        construct_path(inter, inter[i][j], j, INF);
    }
}

void display_paths(long** const path, long** const inter, int num_vertices, long INF) {
    //displays all of the paths using intermediate vertices and lengths of shortest path matrices
    for (int i=0; i < num_vertices; i++) {
        for (int j=0; j < num_vertices; j++) { //iterate over all paths
            if (path[i][j] != INF) {
                cout << (char)(i+'A') << " -> " << (char)(j+'A') << ", distance: " << path[i][j] << ", path: " << (char)(i+'A'); //print beginning statement
                if (i != j) { //if same letter than path is just itself (already printed)
                    construct_path(inter, i, j, INF); //call function to print the path between i and j (not inclusive of i)
                }
            } else {
                cout << (char)(i+'A') << " -> " << (char)(j+'A') << ", distance: infinity, path: none"; //print infinity statement
            }
            cout << endl;
        }
    }

}

void delete_matrix(long** matrix, int n) {
    //deallocates a matrix on heap
    for (int i=0; i < n; i++) {
        delete[] matrix[i]; //deallocate sub arrays
    }
    delete[] matrix; //deallocate outer array
}

vector<string> parse_components(const string& str) {
    //returns vector of components in a line of input
    size_t index = 0;
    vector<string> result;
    for (size_t i=0; i < str.size(); i++) {
        if (str[i] == ' ') { //if space we push index of next component forward
            index = i+1;
        } else {
            if (i == str.size() - 1 || str[i+1] == ' ') { //end of string or component
                result.push_back(str.substr(index, i-index+1)); //add component to result
            }
        }
    }
    return result;
}

void floyd(long** matrix, const int n, const long INF, long** inter) {
    //runs floyd algorithm on a matrix and fills intermediate values into the last argument
    for (int k=0; k < n; k++) {
        for (int i=0; i < n; i++) {
            for (int j=0; j < n; j++) {
                if (matrix[i][k] != INF && matrix[k][j] != INF) { //iterate over all elements for each k numbered intermediate vertex
                    if (matrix[i][k] + matrix[k][j] < matrix[i][j]) {
                        matrix[i][j] = matrix[i][k] + matrix[k][j]; //update since found smaller path using intermediate
                        inter[i][j] = k; //k is an intermediate for this path
                    }
                }
            }
        }
    }
}

int main(int argc, const char *argv[]) {
    // Make sure the right number of command line arguments exist.
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    // Create an ifstream object.
    ifstream input_file(argv[1]);
    // If it does not exist, print an error message.
    if (!input_file) {
        cerr << "Error: Cannot open file '" << argv[1] << "'." << endl;
        return 1;
    }
    // Add read errors to the list of exceptions the ifstream will handle.
    input_file.exceptions(ifstream::badbit);

    //reading the file
    istringstream iss;
    string line;
    int num_vertices = 0;
    long** distance;
    const long INF = numeric_limits<long>::max();

    try {
        unsigned int line_number = 1;
        vector<string> components; //vector to store individual components read from file

        //get number of vertices
        if (getline(input_file, line)) {
            if (!line.empty() && line.back() == '\r') { //remove carraige return from windows
                line.pop_back();
            }
            //get components and check errors
            components = parse_components(line);
            if (components.size() != 1) {
                cerr << "Error: Invalid number of vertices '" << components.size() << "' on line " << line_number << "." << endl;
                return 1;
            } else {
                iss.str(components[0]); //convert to integer
                if (!(iss >> num_vertices) || (num_vertices < 1 || num_vertices > 26)) { //set number vertices and check for errors
                    cerr << "Error: Invalid number of vertices '" << components[0] << "' on line " << line_number << "." << endl;
                    return 1;
                }
            }
        }
        line_number++;
        char upper_limit = 'A'+(num_vertices-1); //final character that a vertex can be

        //know the amount of vertices so can intitialize distance matrix
        distance = new long*[num_vertices];
        for (int i=0; i < num_vertices; i++) {
            distance[i] = new long[num_vertices];
        }
        for (int i=0; i < num_vertices; i++) { //init all as infinity
            for (int j=0; j < num_vertices; j++) {
                distance[i][j] = INF;
            }
        }
        for (int i=0; i < num_vertices; i++) { //0 on diag
            distance[i][i] = 0;
        }

        //extract edges
        while (getline(input_file, line)) {
            if (!line.empty() && line.back() == '\r') { //remove carraige return from windows
                line.pop_back();
            }
            //get components and check errors
            components = parse_components(line);
            if (components.size() != 3) { //not 3 components
                cerr << "Error: Invalid edge data '" << line << "' on line " << line_number << "." << endl;
                delete_matrix(distance, num_vertices); //delete since we allocate and error check at same time
                return 1;
            }
            for (int i=0; i < 2; i++) { //check both vertices
                if ((components[i].size() != 1) || (components[i][0] - 'A' < 0 || components[i][0] - 'A' >= num_vertices)) { //not in range of vertices
                    if (i == 0) { //use the word starting vs ending
                        cerr << "Error: Starting vertex '" << components[i] << "' on line " << line_number << " is not among valid values A-" << upper_limit << "." << endl;
                        delete_matrix(distance, num_vertices);
                        return 1;
                    } else if (i == 1) {
                        cerr << "Error: Ending vertex '" << components[i] << "' on line " << line_number << " is not among valid values A-" << upper_limit << "." << endl;
                        delete_matrix(distance, num_vertices);
                        return 1;
                    }
                }
            }

            int weight = 0;
            iss.clear();
            iss.str(components[2]); //check weight
            if (!(iss >> weight) || weight <= 0) {
                cerr << "Error: Invalid edge weight '" << components[2] << "' on line " << line_number << "." << endl;
                delete_matrix(distance, num_vertices);
                return 1;
            }

            //passed all errors so now add the edge in distance matrix:
            distance[components[0][0] - 'A'][components[1][0] - 'A'] = weight; //index0 = A and so on (distance from 'A')
            line_number++;
        }

        input_file.close(); //close the file
    } catch (const ifstream::failure &f) {
        cerr << "Error: An I/O error occurred reading '" << argv[1] << "'.";
        if (num_vertices != 0) { //have started allocating distance matrix
            delete_matrix(distance, num_vertices);
        }
        return 1;
    }

    display_table(distance, "Distance matrix:", num_vertices, INF);
    //done reading, can now run algorithm

    //initialize other two matrices
    long** path;
    long** inter;
    path = new long*[num_vertices]; //outer
    inter = new long*[num_vertices];
    for (int i=0; i < num_vertices; i++) { //inner
        path[i] = new long[num_vertices];
        inter[i] = new long[num_vertices];
    }
    for (int i=0; i < num_vertices; i++) { //init all as infinity for intermediates
        for (int j=0; j < num_vertices; j++) {
            inter[i][j] = INF;
        }
    }
    for (int i=0; i < num_vertices; i++) { //copy distances to paths
        for (int j=0; j < num_vertices; j++) {
            path[i][j] = distance[i][j];
        }
    }
    floyd(path, num_vertices, INF, inter); //run the floyd algorithm on path (copy of distances to write over)

    //display path and intermediate matrices then the constructed paths
    display_table(path, "Path lengths:", num_vertices, INF);
    display_table(inter, "Intermediate vertices:", num_vertices, INF, true);
    display_paths(path, inter, num_vertices, INF); //detailed path data printed

    //delete heap alloc memory
    delete_matrix(distance, num_vertices);
    delete_matrix(inter, num_vertices);
    delete_matrix(path, num_vertices);

    return 0;
}
