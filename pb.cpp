#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
 

std::vector<std::string> readFileToVector(const std::string &fname) {
  std::vector<std::string> lines;
  std::ifstream infile(fname);

  if (!infile) {
    std::cerr << "Error opening file: " << fname << std::endl;
    return lines;
  }

  std::string line;
  while (std::getline(infile, line)) {
    lines.push_back(line);
  }

  infile.close();
  return lines;
}

std::vector<std::string> read_ids() { return readFileToVector("ids.txt"); }

std::vector<std::string> read_props() { return readFileToVector("props.txt"); }

std::map<std::string, std::map<std::string, std::string>>
getProperties(std::vector<std::string> ids, std::vector<std::string> props) {
  std::map<std::string, std::map<std::string, std::string>> result;

  for (auto e : ids) {
    for (auto p : props) {
      std::string filename = e + "-" + p + ".txt";
      std::ifstream file(filename);
      if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        result[e][p] = content;
        file.close();
      } else {
        std::cerr << "Unable to open file " << filename << std::endl;
      }
    }
  }

  return result;
}



void gen(std::string fname,
         std::map<std::string, std::map<std::string, std::string>> project) {
  std::ifstream fin(fname);
  std::string content((std::istreambuf_iterator<char>(fin)),
                      std::istreambuf_iterator<char>());
  fin.close();

  for (auto const &e : project) {
    std::string foutname = e.first + ".html";
    std::string fcontent = content;
    for (auto const &p : e.second) {
      std::string v = p.second;
      std::string substr = "{" + p.first + "}";
      size_t pos = fcontent.find(substr);
      while (pos != std::string::npos) {
        fcontent.replace(pos, substr.length(), v);
        pos = fcontent.find(substr, pos + v.length());
      }
    }
    std::ofstream fout(foutname);
    fout << fcontent;
    fout.close();
  }
}

int main() {

  std::vector<std::string> ids = read_ids();
  std::vector<std::string> props = read_props();
  std::map<std::string, std::map<std::string, std::string>> project =
      getProperties(ids, props);
  gen("template.txt", project);
}
