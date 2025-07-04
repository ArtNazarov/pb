#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

// Data structures
unordered_map<string, unordered_map<string, string>> WebSite;  // page_id -> {attribute -> value}
unordered_map<string, string> Build;                          // filename -> html_content
mutex website_mutex, build_mutex, output_mutex;

// Function to read a file's contents
string read_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

// Function to process a single page's attributes
void process_page_attributes(const string& page_id, const vector<string>& props) {
    unordered_map<string, string> page_attributes;

    for (const auto& prop : props) {
        string filename = page_id + "-" + prop + ".txt";
        string content = read_file(filename);

        if (!content.empty()) {
            page_attributes[prop] = content;
        }
    }

    // Safely add to the global WebSite structure
    lock_guard<mutex> lock(website_mutex);
    WebSite[page_id] = move(page_attributes);
}

// Function to generate HTML for a single page
void generate_html(const string& page_id, const string& template_str) {
    string html = template_str;

    // Get the page attributes (read-only)
    unordered_map<string, string> page_attributes;
    {
        lock_guard<mutex> lock(website_mutex);
        if (WebSite.find(page_id) == WebSite.end()) return;
        page_attributes = WebSite[page_id];
    }

    // Replace all placeholders in the template
    for (const auto& [prop, value] : page_attributes) {
        string placeholder = "{" + prop + "}";
        size_t pos = html.find(placeholder);
        while (pos != string::npos) {
            html.replace(pos, placeholder.length(), value);
            pos = html.find(placeholder, pos + value.length());
        }
    }

    // Safely add to the Build structure
    string filename = page_id + ".html";
    lock_guard<mutex> lock(build_mutex);
    Build[filename] = move(html);
}

// Function to write a single HTML file to build directory
void write_html_file(const string& filename, const string& content) {
    // Create build directory if it doesn't exist
    fs::create_directory("./build");

    string output_path = "./build/" + filename;
    ofstream file(output_path);
    if (file.is_open()) {
        file << content;

        // Thread-safe output (commented out as requested)
        // lock_guard<mutex> lock(output_mutex);
        // cout << "Generated: " << output_path << endl;
    }
}

int main() {
    // Start timing
    auto start_time = chrono::high_resolution_clock::now();

    // Read configuration files
    vector<string> page_ids;
    ifstream ids_file("ids.txt");
    string line;
    while (getline(ids_file, line)) {
        if (!line.empty()) {
            page_ids.push_back(line);
        }
    }

    vector<string> props;
    ifstream props_file("props.txt");
    while (getline(props_file, line)) {
        if (!line.empty()) {
            props.push_back(line);
        }
    }

    string template_str = read_file("template.txt");
    if (template_str.empty()) {
        cerr << "Error: template.txt is empty or not found" << endl;
        return 1;
    }

    // Phase 1: Parallel reading of all attributes
    vector<thread> attribute_threads;
    for (const auto& page_id : page_ids) {
        attribute_threads.emplace_back(process_page_attributes, page_id, props);
    }

    for (auto& t : attribute_threads) {
        t.join();
    }

    // Phase 2: Parallel HTML generation
    vector<thread> html_threads;
    for (const auto& page_id : page_ids) {
        html_threads.emplace_back(generate_html, page_id, template_str);
    }

    for (auto& t : html_threads) {
        t.join();
    }

    // Phase 3: Parallel file writing
    vector<thread> write_threads;
    for (const auto& [filename, content] : Build) {
        write_threads.emplace_back(write_html_file, filename, content);
    }

    for (auto& t : write_threads) {
        t.join();
    }

    // Calculate and print execution time
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    cout << "\nBuild completed in " << duration.count() << " milliseconds" << endl;
    cout << "Generated " << Build.size() << " files in ./build directory" << endl;

    return 0;
}
