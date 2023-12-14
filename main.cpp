
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <queue> 
using namespace std;

enum NodeType { DIRECTORY, FILE_TYPE };

class TreeNode {
public:
    string name;
    NodeType type;
    TreeNode* parent;
    vector<unique_ptr<TreeNode>> children;
    string data;

    TreeNode(string n, NodeType t) : name(n), type(t), parent(nullptr) {}

    TreeNode* findChild(const string& childName, NodeType childType) {
        auto it = find_if(children.begin(), children.end(),
                          [&](const auto& child) { return child->name == childName && child->type == childType; });
        return (it != children.end()) ? it->get() : nullptr;
    }
};

class FileSystem {
public:
    unique_ptr<TreeNode> root;
    TreeNode* currentDir;
    string path;
    vector<TreeNode*> allNodes;

    FileSystem() : root(make_unique<TreeNode>("/", DIRECTORY)), currentDir(root.get()), path("/") {
        allNodes.push_back(root.get());
    }

    void mkdir(const string& name) {
        if (currentDir->findChild(name, DIRECTORY) != nullptr) {
            cout << "mkdir: cannot create directory '" << name << "': File exists\n";
            return;
        }

        auto newDir = make_unique<TreeNode>(name, DIRECTORY);
        newDir->parent = currentDir;
        currentDir->children.push_back(move(newDir));
        allNodes.push_back(currentDir->children.back().get());
    }

    void cd(const string& dest) {
        if (dest == "..") {
            if (currentDir->parent) {
                currentDir = currentDir->parent;
                path = getPath(currentDir);
            }
            return;
        }

        queue<string> q;
        string temp = "";

        for (auto it : dest) {
            if (it == '/') {
                q.push(temp);
                temp = "";
            }
            temp += it;
        }
        q.push(temp);

        while (!q.empty()) {
            string curr = q.front();
            q.pop();

            auto child = currentDir->findChild(curr, DIRECTORY);
            if (child) {
                currentDir = child;
                path += "/" + currentDir->name;
            } else {
                cout << "The system cannot find the path specified.\n";
                break;
            }
        }
    }

    void ls(const string& dest) {
        TreeNode* curr;
        if (dest == "") {
            curr = currentDir;
        } else {
            auto it = find_if(allNodes.begin(), allNodes.end(), [&](const auto& node) { return node->name == dest; });
            curr = (it != allNodes.end()) ? *it : nullptr;
        }

        if (curr) {
            for (auto& child : curr->children) {
                cout << child->name << " " << (child->type == DIRECTORY ? 'd' : '-') << "\n";
            }
        } else {
            cout << "ls: cannot access '" << dest << "': No such file or directory\n";
        }
    }

    void touch(const string& file) {
        if (currentDir->findChild(file, FILE_TYPE) != nullptr) {
            cout << "touch: cannot create file '" << file << "': File exists\n";
            return;
        }

        auto newFile = make_unique<TreeNode>(file, FILE_TYPE);
        currentDir->children.push_back(move(newFile));
        newFile->parent = currentDir;
        allNodes.push_back(currentDir->children.back().get());
    }

    void cat(const string& file) {
        auto fileNode = findNode(file);

        if (!fileNode) {
            cout << "cat: " << file << ": No such file\n";
            return;
        }

        if (fileNode->type != FILE_TYPE) {
            cout << "cat: " << file << ": Is a directory\n";
            return;
        }

        ifstream fileStream(file);
        if (fileStream.is_open()) {
            string line;
            while (getline(fileStream, line)) {
                cout << line << '\n';
            }
            fileStream.close();
        } else {
            cout << "cat: " << file << ": Unable to open file\n";
        }
    }

    void mv(const string& source, const string& destination) {
        auto sourceNode = findNode(source);
        auto destinationNode = findNode(destination);

        if (!sourceNode) {
            cout << "mv: cannot move '" << source << "': No such file or directory\n";
            return;
        }

        if (!destinationNode || destinationNode->type != DIRECTORY) {
            cout << "mv: cannot move to '" << destination << "': No such directory\n";
            return;
        }

        // Move the source node to the destination directory
        auto it = find_if(sourceNode->parent->children.begin(), sourceNode->parent->children.end(),
                          [&](const auto& child) { return child.get() == sourceNode; });

        if (it != sourceNode->parent->children.end()) {
            auto movedNode = move(*it);
            movedNode->parent = destinationNode;
            destinationNode->children.push_back(move(movedNode));

            sourceNode->parent->children.erase(it);
        }

        // Update path if the current directory is moved
        if (currentDir == sourceNode) {
            currentDir = destinationNode;
            path = getPath(destinationNode);
        }
    }

    void cp(const string& source, const string& destination) {
        auto sourceNode = findNode(source);
        auto destinationNode = findNode(destination);

        if (!sourceNode) {
            cout << "cp: cannot copy '" << source << "': No such file or directory\n";
            return;
        }

        if (!destinationNode || destinationNode->type != DIRECTORY) {
            cout << "cp: cannot copy to '" << destination << "': No such directory\n";
            return;
        }

        // Create a copy of the source node and add it to the destination directory
        auto copyNode = make_unique<TreeNode>(sourceNode->name, sourceNode->type);
        destinationNode->children.push_back(move(copyNode));
        destinationNode->children.back()->parent = destinationNode;
        allNodes.push_back(destinationNode->children.back().get());
    }

    void rm(const string& target) {
        auto targetNode = findNode(target);

        if (!targetNode) {
            cout << "rm: cannot remove '" << target << "': No such file or directory\n";
            return;
        }

        // Remove the target node from its parent's children
        auto it = find_if(targetNode->parent->children.begin(), targetNode->parent->children.end(),
                          [&](const auto& child) { return child.get() == targetNode; });

        if (it != targetNode->parent->children.end()) {
            targetNode->parent->children.erase(it);

            // Update path if the current directory is removed
            if (currentDir == targetNode) {
                currentDir = targetNode->parent;
                path = getPath(currentDir);
            }

            // Delete the target node and its descendants
            deleteNode(targetNode);
        }
    }

private:
   

 void deleteNode(TreeNode* node) {
        for (auto& child : node->children) {
            deleteNode(child.get());
        }

        auto it = find_if(allNodes.begin(), allNodes.end(), [&](const auto& n) { return n == node; });
        if (it != allNodes.end()) {
            allNodes.erase(it);
        }
    }

    TreeNode* findNode(const string& path) {
        auto it = find_if(allNodes.begin(), allNodes.end(), [&](const auto& node) { return getPath(node) == path; });
        return (it != allNodes.end()) ? *it : nullptr;
    }

    string getPath(TreeNode* node) {
        string result = "";
        while (node != nullptr) {
            result = "/" + node->name + result;
            node = node->parent;
        }
        return result.empty() ? "/" : result;
    }
};

int main() {
    FileSystem fileSystem;

    while (true) {
        string command;
        cout << (fileSystem.currentDir->name != "/" ? fileSystem.path : fileSystem.currentDir->name) << "> ";

        getline(cin >> ws, command);

        vector<string> args = {};

        stringstream ss(command);
        string temp;

        while (ss >> temp) {
            args.push_back(temp);
        }

        if (args.empty()) {
            continue;
        }

        if (args[0] == "cd") {
            if (args.size() == 2) {
                fileSystem.cd(args[1]);
            } else {
                cout << "cd: missing destination operand\n";
            }
        } else if (args[0] == "mkdir") {
            if (args.size() > 1) {
                for (size_t i = 1; i < args.size(); i++) {
                    fileSystem.mkdir(args[i]);
                }
            } else {
                cout << "mkdir: missing directory name\n";
            }
        } else if (args[0] == "ls") {
            if (args.size() == 1) {
                fileSystem.ls("");
            } else if (args.size() == 2) {
                fileSystem.ls(args[1]);
            } else {
                cout << "ls: too many arguments\n";
            }
        } else if (args[0] == "touch") {
            if (args.size() == 2) {
                fileSystem.touch(args[1]);
            } else {
                cout << "touch: missing file operand\n";
            }
        } else if (args[0] == "cat") {
            if (args.size() == 2) {
                fileSystem.cat(args[1]);
            } else {
                cout << "cat: missing file operand\n";
            }
        } else if (args[0] == "mv") {
            if (args.size() == 3) {
                fileSystem.mv(args[1], args[2]);
            } else {
                cout << "mv: missing source or destination operand\n";
            }
        } else if (args[0] == "cp") {
            if (args.size() == 3) {
                fileSystem.cp(args[1], args[2]);
            } else {
                cout << "cp: missing source or destination operand\n";
            }
        } else if (args[0] == "rm") {
            if (args.size() == 2) {
                fileSystem.rm(args[1]);
            } else {
                cout << "rm: missing target operand\n";
            }
        } else if (args[0] == "exit") {
            break;
        } else {
            cout << "Unknown command: " << args[0] << "\n";
        }
    }

    return 0;
}


