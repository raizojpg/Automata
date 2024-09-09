#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>

std::ifstream in("pda.txt");
std::ofstream out("output.txt");

struct edge {
	int state;
	char popped;
	std::string pushed;
};

struct node {
	int state;
	int index;
	std::stack<char> memory;
};


char lambda = '*';
char start_symbol = '$';
int x, y, n, m, S, nrF, nrCuv;
char l;
std::string str;
std::vector<int> states_name;
std::set<int> final_states;
std::map<int, std::map<char, std::vector<edge>>> states;
std::set<std::string> visited;


std::string stringify(node& current, edge& action) {
	std::string result = "";
	result += std::to_string(current.state);
	result += " ";
	result += std::to_string(current.index);
	result += " mem: ";
	std::stack<char> aux = current.memory;
	while (!aux.empty()) {
		result += aux.top();
		result += " ";
		aux.pop();
	}
	result += "| ";
	result += std::to_string(action.state);
	result += " ";
	result += action.popped;
	result += " | ";
	for (int i = 0; i < action.pushed.size(); i++) {
		result += action.pushed[i];
		result += " ";
	}
	return result;
}

node make_transition(node& current, edge& action) {
	node transition;
	transition.state = action.state;
	transition.index = current.index;
	transition.memory = current.memory;
	if (action.popped != lambda) {
		transition.memory.pop();
	}
	if (action.pushed[0] != lambda) {
		for (char val : action.pushed) {
			transition.memory.push(val);
		}
	}
	return transition;
}

bool PDA(std::string& str) {

	node start;
	start.state = S;
	start.index = 0;
	start.memory.push(start_symbol);

	std::stack<node> possible_states;
	possible_states.push(start);

	while (!possible_states.empty()) {
		node current = possible_states.top();
		possible_states.pop();
		if (current.index < str.length()) {
			if (states[current.state][str[current.index]].empty()) {
				// std::cout << "State does not exist" << std::endl;
			}
			else {
				// multiple actions are possible for each letter
				for (edge action : states[current.state][str[current.index]]) {
					std::string str = stringify(current, action);
					if (visited.find(str) == visited.end()) {
						visited.insert(str);
						if (action.popped == current.memory.top() || action.popped == lambda) {
							node transition = make_transition(current, action);
							transition.index++;
							possible_states.push(transition);
						}
					}
				}
			}
			// check for lambda states
			for (edge action : states[current.state][lambda]) {
				std::string str = stringify(current, action);
				if (visited.find(str) == visited.end()) {
					visited.insert(str);
					if (action.popped == current.memory.top() || action.popped == lambda) {
						node transition = make_transition(current, action);
						possible_states.push(transition);
					}
				}
			}
		}
		else if (current.index == str.length()) {
			if (final_states.find(current.state) != final_states.end() && current.memory.top() == start_symbol) { return true; }
			// check for lambda states
			for (edge action : states[current.state][lambda]) {
				std::string str = stringify(current, action);
				if (visited.find(str) == visited.end()) {
					visited.insert(str);
					if (action.popped == current.memory.top() || action.popped == lambda) {
						node transition = make_transition(current, action);
						possible_states.push(transition);
					}
				}
			}
		}
	}
	return false;
}

int main() {

	/* FORMAT:
		states : integers;
		alphabet : letters;
		stack alphabet : letters;
		transitions : state -> state with (aphabet_letter, stack_alphabet_letter | vector_stack_alphabet_letters)
	*/


	in >> S;

	in >> m;
	for (int i = 0; i < m; i++) {
		edge action;
		in >> x >> y;
		in >> l;
		in.get();
		in >> action.popped;
		in.get();
		in >> action.pushed;
		in.get();
		action.state = y;
		states[x][l].push_back(action);
	}

	in >> nrF;
	for (int i = 0; i < nrF; i++) {
		in >> x;
		final_states.insert(x);
	}
	in >> nrCuv;
	for (int i = 0; i < nrCuv; i++) {
		in >> str;
		visited.clear();
		if (PDA(str)) { out << "YES" << std::endl; }
		else { out << "NO" << std::endl; }
	}
	return 0;
}