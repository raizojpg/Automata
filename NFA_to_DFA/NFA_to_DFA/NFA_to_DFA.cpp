#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <algorithm>

std::ifstream in("inputnfa.txt");
std::ofstream out("outputnfa.txt");


class automata {
	std::vector<char> states_name;
	std::vector<char> alphabet;
	std::set<char> final_states_nfa;
	std::set<std::string> final_states_dfa;

	std::string start;

	int n, m, a, f, nr;
	char x, y, letter;
	std::string str;

	std::map<char, std::map<char, std::vector<char>>> states_nfa;
	std::map<std::string, std::map<char, std::string>> states_dfa;

	std::map<char, std::vector<bool>> visited;
	// This data structure helps us not repeating a search 
	//	that was already computed on a state at a specific point in time

public:

	automata() {
		in >> n;
		for (int i = 0; i < n; i++) {
			in >> x;
			in.get();
			states_name.push_back(x);
		}
		in >> a;
		for (int i = 0; i < a; i++) {
			in >> x;
			in.get();
			alphabet.push_back(x);
		}
		in >> m;
		for (int i = 0; i < m; i++) {
			in >> x >> y;
			in >> letter;
			in.get();
			states_nfa[x][letter].push_back(y);
		}
		in >> start;
		in >> f;
		for (int i = 0; i < f; i++) {
			in >> x;
			final_states_nfa.insert(x);
		}
		nr = 0;
	}

	void NFA_to_DFA() {
		std::stack<std::string> states_to_process;
		states_to_process.push(start);
		while (!states_to_process.empty()) {
			std::string current_state = states_to_process.top();
			states_to_process.pop();
			// check to see if we already have processed this state
			if (states_dfa[current_state].empty()) {
				for (int i = 0; i < alphabet.size(); i++) {

					bool final_state_in_dfa = false;
					std::set<char> reachable_states_set;

					// current state is a string with letter states
					for (char letter_state : current_state) {
						for (char state : states_nfa[letter_state][alphabet[i]]) {
							if (final_states_nfa.find(state) != final_states_nfa.end()) { final_state_in_dfa = true; }
							reachable_states_set.insert(state);
						}

					}

					//we want the letter states in lexicographic order
					std::vector<char> reachable_states_vector;
					for (char state : reachable_states_set) {
						reachable_states_vector.push_back(state);
					}
					std::sort(reachable_states_vector.begin(), reachable_states_vector.end());

					std::string state_for_dfa = "";
					for (char state : reachable_states_vector) {
						state_for_dfa += state;
					}

					if (state_for_dfa != "") {
						if (final_state_in_dfa) { final_states_dfa.insert(state_for_dfa); }
						states_dfa[current_state][alphabet[i]] = state_for_dfa;
						states_to_process.push(state_for_dfa);
					}
					else {
						// # will be the sink state
						states_dfa[current_state][alphabet[i]] = '#';
					}

				}
			}
		}


	}

	bool DFA(std::string& str) {
		std::string state = start;
		for (int i = 0; i < str.length(); i++) {
			state = states_dfa[state][str[i]]; // only one state is possible for each letter
		}
		if (final_states_dfa.find(state) != final_states_dfa.end()) { return true; }
		else { return false; }
	}

	bool NFA(std::string& str) {

		std::stack<std::pair<char, int>> possible_states; //the second element will be the string index  
		possible_states.push({ start[0],0 });
		while (!possible_states.empty()) {
			std::pair<char, int> state_index = possible_states.top();
			possible_states.pop();
			int state = state_index.first;
			int index = state_index.second;
			if (index < str.length()) {
				if (states_nfa[state][str[index]].empty()) {
					// std::cout << "State does not exist" << std::endl;
					continue;
				}
				// multiple states are possible for each letter
				for (int possible_state : states_nfa[state][str[index]]) {
					if (visited[possible_state][index] == 0) {
						visited[possible_state][index] = 1;
						possible_states.push({ possible_state,index + 1 });
					}
				}
			}
			else if (index == str.length()) {
				if (final_states_nfa.find(state) != final_states_nfa.end()) { return true; }
			}
		}
		return false;
	}

	void check_equlity() {
		bool error = false;
		bool dfa_response;
		bool nfa_response;
		in >> nr;
		for (int i = 0; i < nr; i++) {
			in >> str;
			for (int state : states_name) {
				visited[state] = std::vector<bool>(str.length(), 0);
			}
			dfa_response = DFA(str);
			nfa_response = NFA(str);
			if (nfa_response) { out << "YES" << std::endl; }
			else { out << "NO" << std::endl; }
			if (nfa_response != dfa_response) {
				error = true;
				out << "ERROR" << std::endl;
			}
		}
		if (error == false) {
			out << "The conversion was successful !" << std::endl;
		}

	}
};

int main() {

	automata nfa;

	nfa.NFA_to_DFA();

	nfa.check_equlity();
	
	return 0;
}
