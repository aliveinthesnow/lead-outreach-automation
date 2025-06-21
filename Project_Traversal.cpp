#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <set>
#include <queue>
#include <map>
#include <limits>
#include "json.hpp"
using json = nlohmann::json;

using namespace std;

struct Lead {
    int id;
    string name, email, title, industry, stage, company, details;
};

vector<Lead> readCSV(const string& filename) {
    vector<Lead> leads;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return leads;
    }
    string line;
    getline(file, line);  // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        Lead lead;
        string idStr;
        getline(ss, idStr, ','); lead.id = stoi(idStr);
        getline(ss, lead.name, ',');
        getline(ss, lead.email, ',');
        getline(ss, lead.title, ',');
        getline(ss, lead.industry, ',');
        getline(ss, lead.stage, ',');
        getline(ss, lead.company, ',');
        getline(ss, lead.details, ',');
        leads.push_back(lead);
    }
    file.close();
    return leads;
}

class HashTable {
private:
    static const int hashGroups = 10;
    list<pair<string, vector<Lead>>> table[hashGroups];
    list<pair<int, int>> ID_table[hashGroups];  //maps ID to the index

    int hashFunction(const string& key) const {
        int sum = 0;
        for (char c : key) sum += c;
        return sum % hashGroups;
    }

public:
    void insertItem(const string& key, const Lead& lead) {
        int hashValue = hashFunction(key);
        auto& cell = table[hashValue];
        for (auto& pair : cell) {
            if (pair.first == key) {
                pair.second.push_back(lead);
                return;
            }
        }
        cell.emplace_back(key, vector<Lead>{lead});
    }

    void ID_to_lead(const int& key, int index){
        int hashValue = key%hashGroups;
        auto& cell = ID_table[hashValue];
        for (auto& pair : cell){
            if (pair.first == key){
                pair.second = index;
                return;
            }
        }
        cell.emplace_back(key, index);
    }

    int getIndexByID(const int& key) const {
    int hashValue = key % hashGroups;
    for (const auto& pair : ID_table[hashValue]) {
        if (pair.first == key) {
            return pair.second;  // the saved index
        }
    }
    return -1;  // not found
    }

    vector<Lead> getLeadsByKey(const string& key) const {
        int hashValue = hashFunction(key);
        for (const auto& pair : table[hashValue]) {
            if (pair.first == key) {
                return pair.second;
            }
        }
        return {};
    }
};

class Graph {
private:
    vector<vector<int>> adj;  // size = leads.size()

public:
    Graph(const vector<Lead>& leads) {
        int n = leads.size();
        adj.assign(n, {});
        // Build edges: fully connect leads with same industry
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (leads[i].industry == leads[j].industry) {
                    adj[i].push_back(j);
                    adj[j].push_back(i);
                }
            }
        }
    }

    const vector<vector<int>>& getAdj() const {
        return adj;
    }
};

void printBFSTraversal(const vector<vector<int>>& adjList, const vector<Lead>& leads, int startnode){
    vector<bool> visited(adjList.size(), false);
    queue<int> q;
    q.push(startnode);
    visited[startnode] = true;

    cout << "\nBFS Traversal starting from node: " << leads[startnode].name << "\n";

    while (!q.empty()) {
        int node = q.front(); q.pop();
        const Lead& lead = leads[node];
        cout << "  ID: " << lead.id
            << ", Name: " << lead.name
            << ", Email: " << lead.email
            << ", Title: " << lead.title
            << ", Industry: " << lead.industry
            << ", Stage: " << lead.stage 
            << ", Company: " << lead.company
            << ", Details: " << lead.details<< "\n";

        for (int neighbor : adjList[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
}

void insertBFSTraversal_tojson(const vector<vector<int>>& adjList, const vector<Lead>& leads, int startnode){
    vector<bool> visited(adjList.size(), false);
    queue<int> q;
    q.push(startnode);
    visited[startnode] = true;

    json j;
    j["lead"] = {
        {"id",leads[startnode].id},
        {"name",leads[startnode].name},
        {"email",leads[startnode].email},
        {"title",leads[startnode].title},
        {"industry",leads[startnode].industry},
        {"stage", leads[startnode].stage},
        {"company", leads[startnode].company},
        {"details", leads[startnode].details}
    };
    j["context"] = json::array();
    while (!q.empty()){
        int node = q.front();
        q.pop();
        if (node!=startnode){
            Lead lead = leads[node];
            j["context"].push_back(
                {
                    {"id", lead.id},
                    {"name", lead.name},
                    {"email", lead.email},
                    {"title", lead.title},
                    {"industry", lead.industry},
                    {"stage", lead.stage},
                    {"company", lead.company},
                    {"details", lead.details}
                }
            );
        }
        for (int neighbour : adjList[node]){
            if (!visited[neighbour]){
                visited[neighbour] = true;
                q.push(neighbour);
            }
        }
    }

    ofstream hey("industry_emails.json");
    hey<<j.dump(2);
    hey.close();
}

int main() {
    // 1. Read all leads from CSV
    vector<Lead> leads = readCSV("indian_leads_updated_stages.csv");
    map<string,int> industryFirstIndex;
    for (int i = 0; i < (int)leads.size(); ++i) {
        const string& ind = leads[i].industry;
        if (industryFirstIndex.find(ind) == industryFirstIndex.end()) {
            industryFirstIndex[ind] = i;
        }
    }
    Graph g(leads);
    HashTable ht;
    //for (const Lead& L : leads) {
    //    ht.insertItem(L.stage, L);
    //}
    //for (const Lead& L : leads){
    //    ht.ID_to_lead(L.id, L);
    //}
    for (int i = 0; i < (int)leads.size(); ++i){
    const Lead& L = leads[i];
    ht.insertItem(L.stage, L);
    ht.ID_to_lead(L.id, i);  // store index i   
    }
    // 4. MENU LOOP
    while (true) {
        cout << "\n--- Lead Management Menu ---\n";
        cout << "1. View all leads by Stage\n";
        cout << "2. View all leads by Industry\n";
        cout << "3. Send a personalized AI generated email to a lead\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            // Option 1: View all leads by Stage
            cout << "Enter Stage (New / Contacted / Engaged / Qualified / Won): ";
            string stage;
            getline(cin, stage);

            vector<Lead> bucket = ht.getLeadsByKey(stage);
            if (bucket.empty()) {
                cout << "No leads found for stage: " << stage << "\n";
            } else {
                cout << "\nLeads in stage '" << stage << "':\n";
                for (const Lead& L : bucket) {
                    cout << "  ID: " << L.id
                         << ", Name: " << L.name
                         << ", Email: " << L.email
                         << ", Title: " << L.title
                         << ", Industry: " << L.industry
                         << ", Stage: " << L.stage
                         << ", Company: " << L.company
                         << ", Details: " << L.details
                         << "\n";
                }
            }
        }
        else if (choice == 2) {
            cout << "Enter Industry for BFS: ";
            string industry;
            getline(cin, industry);
            auto it = industryFirstIndex.find(industry);
            if (it == industryFirstIndex.end()) {
                cout << "Invalid industry.\n";
            } else {
                int start = it->second;
                printBFSTraversal(g.getAdj(), leads, start);
            }
        }
        else if (choice == 3){
            cout<<"Enter the ID of lead: ";
            int ID;
            cin>>ID;
            int leadIndex = ht.getIndexByID(ID);
            if (leadIndex<0){
                cout<<"Lead not found\n";
                continue;
            }
            insertBFSTraversal_tojson(g.getAdj(), leads, leadIndex);
            string command = "cmd /C \"python MAIN_PYTHON_FILE.py\"";
            system(command.c_str());
            cout<<"Personalized AI generated mail has been sent\n\n";
        }
        else if (choice == 4) {
            cout << "Exiting...\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
