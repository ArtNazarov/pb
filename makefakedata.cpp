#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// Generate random string of given length
std::string random_string(size_t length) {
    static const std::string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);
    
    std::string result;
    std::generate_n(std::back_inserter(result), length, 
        []() { return chars[dist(rng)]; });
    return result;
}

// Generate random lorem ipsum content
std::string random_content(int paragraphs) {
    static const std::vector<std::string> words = {
        "lorem", "ipsum", "dolor", "sit", "amet", "consectetur",
        "adipiscing", "elit", "sed", "do", "eiusmod", "tempor",
        "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua",
        "ut", "enim", "ad", "minim", "veniam", "quis", "nostrud",
        "exercitation", "ullamco", "laboris", "nisi", "ut", "aliquip",
        "ex", "ea", "commodo", "consequat"
    };
    
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> word_dist(0, words.size() - 1);
    std::uniform_int_distribution<int> sentence_len(5, 15);
    std::uniform_int_distribution<int> para_len(3, 7);
    
    std::string result;
    
    for (int p = 0; p < paragraphs; ++p) {
        int sentences = para_len(rng);
        for (int s = 0; s < sentences; ++s) {
            int words_in_sentence = sentence_len(rng);
            
            // Capitalize first letter
            std::string first_word = words[word_dist(rng)];
            first_word[0] = toupper(first_word[0]);
            result += first_word;
            
            // Add rest of words
            for (int w = 1; w < words_in_sentence; ++w) {
                result += " " + words[word_dist(rng)];
            }
            
            result += ". ";
        }
        result += "\n\n";
    }
    
    return result;
}

// Generate random HTML tag
std::string random_html_tag(const std::string& content) {
    static const std::vector<std::string> tags = {
        "h1", "h2", "h3", "p", "div", "span", "strong", "em", "blockquote"
    };
    
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, tags.size() - 1);
    
    std::string tag = tags[dist(rng)];
    return "<" + tag + ">" + content + "</" + tag + ">";
}

void create_directory_if_not_exists(const std::string& dir) {
    if (!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}

void generate_fake_data(int page_count = 100000) {
    // Create output directory
    create_directory_if_not_exists("test_data");
    fs::current_path("test_data");
    
    // Generate page IDs
    std::ofstream ids_file("ids.txt");
    for (int i = 1; i <= page_count; ++i) {
        ids_file << "page" << i << "\n";
    }
    ids_file.close();
    
    // Generate properties
    std::ofstream props_file("props.txt");
    std::vector<std::string> properties = {
        "title", "header", "body", "footer", "author", 
        "date", "keywords", "description"
    };
    
    for (const auto& prop : properties) {
        props_file << prop << "\n";
    }
    props_file.close();
    
    // Generate template
    std::ofstream template_file("template.txt");
    template_file << "<!DOCTYPE html>\n"
                 << "<html>\n"
                 << "<head>\n"
                 << "<title>{title}</title>\n"
                 << "<meta name=\"description\" content=\"{description}\">\n"
                 << "<meta name=\"keywords\" content=\"{keywords}\">\n"
                 << "</head>\n"
                 << "<body>\n"
                 << "<header>{header}</header>\n"
                 << "<main>\n"
                 << "{body}\n"
                 << "</main>\n"
                 << "<footer>\n"
                 << "{footer}\n"
                 << "<p>Author: {author}</p>\n"
                 << "<p>Date: {date}</p>\n"
                 << "</footer>\n"
                 << "</body>\n"
                 << "</html>";
    template_file.close();
    
    // Generate property files for each page
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> content_len(1, 5);
    
    for (int i = 1; i <= page_count; ++i) {
        std::string id = "page" + std::to_string(i);
        
        for (const auto& prop : properties) {
            std::string filename = id + "-" + prop + ".txt";
            std::ofstream file(filename);
            
            if (prop == "title") {
                file << "Page " << i << " - " << random_string(10);
            }
            else if (prop == "header") {
                file << random_html_tag("Welcome to Page " + std::to_string(i));
            }
            else if (prop == "body") {
                file << random_html_tag(random_content(content_len(rng)));
            }
            else if (prop == "footer") {
                file << random_html_tag("Copyright " + std::to_string(1900 + i % 100));
            }
            else if (prop == "author") {
                file << "Author" << (i % 20 + 1);
            }
            else if (prop == "date") {
                file << 2000 + i % 20 << "-" 
                     << 1 + i % 12 << "-" 
                     << 1 + i % 28;
            }
            else if (prop == "keywords") {
                file << "keyword" << i % 10 + 1 << ", "
                     << "tag" << i % 20 + 1 << ", "
                     << "category" << i % 5 + 1;
            }
            else if (prop == "description") {
                file << "This is page " << i << " with random content about "
                     << random_string(5) << " and " << random_string(7);
            }
            
            file.close();
        }
        
        // Progress indicator
        if (i % 100 == 0) {
            std::cout << "Generated " << i << " of " << page_count << " pages\n";
        }
    }
    
    std::cout << "Successfully generated test data for " << page_count << " pages\n";
}

int main() {
    try {
        generate_fake_data();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
