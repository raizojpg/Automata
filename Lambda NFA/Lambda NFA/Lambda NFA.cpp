#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>

std::ifstream in("input.txt");
std::ofstream out("output.txt");

int x, y, n, m, S, nrF, nrCuv;
char l;
std::string str;
std::vector<int> states_name;
std::set<int> final_states;
std::map<int, std::map<char, std::vector<int>>> states;

std::map<int, std::vector<std::pair<bool,bool>>> visited;
// This data strcuture helps us not repeating a search 
//   that was already computed on a state at a specific point in time
// The second bool is for visiting the state with lambda


bool Lambda_NFA(std::string& str) {
	// It is cicleproof because we will go back to states that were already visited
	//	   only after we advanced the iteration
	// The lambda states transitions are noted with '*' 
	// The lambda letters that may appear in our string are noted with '*'
	char lambda = '*';

	//check for lambdas in our string
	str.erase(remove(str.begin(), str.end(), lambda), str.end());

	// We will use a stack, because once we found one possible path, 
	//    we are no longer interested in following the search 
	// If we would have used a queue, we will have computed all the possible searches 

	std::stack<std::pair<int, int>> possible_states; //the second element will be the string index 
	possible_states.push({ S,0 });
	while (!possible_states.empty()) {
		std::pair<int, int> state_index = possible_states.top();
		possible_states.pop();
		int state = state_index.first;
		int index = state_index.second;
		if (index < str.length()) {
			if (states[state][str[index]].empty()) {
				// std::cout << "State does not exist" << std::endl;
			}
			else {
				// multiple states are possible for each letter
				for (int possible_state : states[state][str[index]]) {
					if (visited[possible_state][index].first == 0) {
						visited[possible_state][index].first = 1;
						possible_states.push({ possible_state,index + 1 });
					}
				}
			}
			// check for lambda states
			for (int possible_state : states[state][lambda]) {
				if (visited[possible_state][index].second == 0) {
					visited[possible_state][index].second = 1;
					possible_states.push({ possible_state,index });
				}
			}
		}
		else if (index == str.length()) {
			if (final_states.find(state) != final_states.end()) { return true; }
			// check for lambda states
			for (int possible_state : states[state][lambda]) {
				if (visited[possible_state][index].second == 0) {
					visited[possible_state][index].second = 1;
					possible_states.push({ possible_state,index });
				}
			}
		}
	}
	return false;
}

int main() {

	in >> n;
	for (int i = 0; i < n; i++) {
		in >> x;
		states_name.push_back(x);
	}
	in >> m;
	for (int i = 0; i < m; i++) {
		in >> x >> y;
		in >> l;
		in.get();
		states[x][l].push_back(y);
	}
	in >> S;
	in >> nrF;
	for (int i = 0; i < nrF; i++) {
		in >> x;
		final_states.insert(x);
	}
	in >> nrCuv;
	for (int i = 0; i < nrCuv; i++) {
		in >> str;
		for (int state : states_name) {
			visited[state] = std::vector<std::pair<bool,bool>>(str.length() + 1, { 0,0 });
			// The +1 is because we can still search with lambda, even though the string has ended
		}
		if (Lambda_NFA(str)) { out << "DA" << std::endl; }
		else { out << "NU" << std::endl; }
	}
	return 0;
}