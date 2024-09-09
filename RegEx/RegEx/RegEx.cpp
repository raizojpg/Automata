#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>

std::ifstream in("inputregexp.txt");
std::ofstream out("outputregexp.txt");

class automata {

	int start;
	std::vector<int> states_name;
	std::set<int> final_states;
	std::map<int, std::map<char, std::vector<int>>> states;

	std::map<int, std::vector<std::pair<bool, bool>>> visited;
	// This data structure helps us not repeating a search 
	//   that was already computed on a state at a specific point in time
	// The second bool is for visiting the state with lambda

public:
	automata() { 
		start = -1; 
	}

	automata(std::string exp) {
		if (check_just_letters(exp)) {
			// we construct an automata based of a string with concatenated letters
			int size = exp.size() + 1;
			start = 1;
			for (int i = 1; i < size; i++) {
				states_name.push_back(i);
				states[i][exp[i - 1]] = { i + 1 };
			}
			states_name.push_back(size);
			final_states.insert(size);
		}
		else {
			// the string contains other symbols that need to be proccesed
			std::vector<std::string> subexp = split_plus(exp); // (exp,+,exp,+,exp..)
			if (subexp.size() > 1) {
				automata x(subexp[0]);
				for (int i = 2; i < subexp.size(); i += 2) {
					automata y(subexp[i]);
					plus(x, y);
				}
				*this = x;
			}
			else {
				std::vector<std::string> subexp = split_star(exp); // (exp,exp,*,exp,*,exp..)
				if (subexp.size() > 1) {
					automata x(subexp[0]);
					for (int i = 1; i < subexp.size(); i++) {
						if (subexp[i][0] == '*') { star(x); }
						else {
							automata y(subexp[i]);
							if (i + 1 < subexp.size() && subexp[i+1][0]=='*') {
								star(y);
								i++;
							}
							concat(x, y);
						}
					}
					*this = x;
				}
				else {
					std::vector<std::string> subexp = split_parenthesis(exp);;
					automata x(subexp[0]);
					for (int i = 1; i < subexp.size(); i++) {
						automata y(subexp[i]);
						concat(x, y);
					}
					*this = x;
				}

			}
		}
	}

	bool check_just_letters(std::string exp) {
		for (char letter : exp) {
			if (!((letter >= 'a' && letter <= 'z') ||
				(letter >= 'A' && letter <= 'Z') ||
				(letter >= '0' && letter <= '9'))) {
				return false;
			}
		}
		return true;
	}

	std::vector<int> increment(std::vector<int>& vec, int val) {
		std::vector<int> aux;
		for (int v : vec) {
			aux.push_back(v + val);
		}
		return aux;
	}

	void add_states(automata& x, automata& y) {
		int offset = x.states_name.size();
		for (int state_name : y.states_name) {
			x.states_name.push_back(state_name + offset);
			//iterate through all the letters for that state
			//states[state_name] -> a map with (key,value) = (letter,vector with states)
			for (auto i = y.states[state_name].begin(); i != y.states[state_name].end(); i++) {
				x.states[state_name + offset][i->first] = increment(i->second, offset);
			}
		}
	}

	void concat(automata& x, automata& y) {
		int offset = x.states_name.size();
		for (int state : x.final_states) {
			x.states[state]['*'].push_back(y.start+offset);
		}
		add_states(x, y);
		x.final_states.clear();
		for (int state : y.final_states) {
			x.final_states.insert(state+offset);
		}
	}

	void plus(automata& x, automata& y) {
		int offset;
		automata aux;
		aux.start = 1;
		aux.states_name.push_back(aux.start);

		//copying the states from x in aux
		offset = aux.states_name.size();
		add_states(aux, x);
		for (int state : x.final_states) {
			aux.final_states.insert(state + offset);
		}
		aux.states[aux.start]['*'].push_back(x.start + offset);

		//copying the states from y in aux
		offset = aux.states_name.size();
		add_states(aux, y);
		for (int state : y.final_states) {
			aux.final_states.insert(state + offset);
		}
		aux.states[aux.start]['*'].push_back(y.start + offset);

		x = aux;
	}

	void star(automata& x) {
		int new_state = x.states_name.size();
		x.states_name.push_back(new_state);
		for (int state : x.final_states) {
			x.states[state]['*'].push_back(new_state);
		}
		x.final_states.insert(new_state);
		x.states[new_state]['*'].push_back(x.start);
		x.start = new_state;
	}
	
	/*
	std::vector<std::string> split(std::string exp) {
		std::vector<std::string> subexpressions;
		std::string subexpression = "";
		int open = 0;
		for (char x : exp) {
			if (x == ' ') { continue; }
			if (open) {
				if (x == ')' && open == 1) {
					open = false;
					subexpressions.push_back(subexpression);
					subexpression = "";
				}
				else {
					subexpression += x;
					if (x == '(') {
						open++;
					}
					else if (x == ')') {
						open--;
					}
				}
			}
			else if (x == '(') {
				open = 1;
			}
			else if (x != ' ') {
				subexpression += x;
				subexpressions.push_back(subexpression);
				subexpression = "";
			}
		}
		return subexpressions;
	}
	*/

	std::vector<std::string> split_parenthesis(std::string exp) {
		//split after parenthesis
		std::vector<std::string> subexpressions;
		std::string subexp = "";
		int open = 0;
		for (char x : exp) {
			if (x == ' ') { continue; }
			if (x == '(' && open == 0) {
				open = 1;
				if (subexp != "") { subexpressions.push_back(subexp); }
				subexp = "";
			}
			else if (x == ')' && open == 1) {
				subexpressions.push_back(subexp);
				subexp = "";
			}
			else {
				if (x == '(') { open++; }
				else if (x == ')') { open--; }
				subexp += x;
			}
		}
		if (subexp != "") { subexpressions.push_back(subexp); }
		return subexpressions;
	}

	std::vector<std::string> split_star(std::string exp) {
		//split after star
		std::vector<std::string> subexpressions;
		std::string subexp = "";
		int open = 0;
		for (char x : exp) {
			if (x == ' ') { continue; }
			if (x == '(') { open++; subexp += x; }
			else if (x == ')') { open--; subexp += x; }
			else if (x == '*' && open == 0) {
				if (subexp[subexp.size() - 1] == ')') {
					std::vector<std::string> sp = split_parenthesis(subexp);
					subexpressions.push_back(sp[0]);
					if (sp.size() == 2) { subexpressions.push_back(sp[1]); }
				}
				else {
					std::string sb1 = "";
					for (int i = 0; i < subexp.size() - 1; i++) {
						sb1 += subexp[i];
					}
					std::string sb2 = "";
					sb2 += subexp[subexp.size() - 1];
					if (sb1.size() > 0) { subexpressions.push_back(sb1); }
					subexpressions.push_back(sb2);
				}
				subexp = '*';
				subexpressions.push_back(subexp);
				subexp = "";

			}
			else {
				subexp += x;
			}
		}
		if (subexp != "") { subexpressions.push_back(subexp); }
		return subexpressions;
	}

	std::vector<std::string> split_plus(std::string exp) {
		//split after plus
		std::vector<std::string> subexpressions;
		std::string subexp = "";
		int open = 0;
		for (char x : exp) {
			if (x == ' ') { continue; }
			if (x == '(') { open++; subexp += x; }
			else if(x == ')') { open--; subexp += x;}
			else if (x == '+' && open == 0) {
				subexpressions.push_back(subexp);
				subexp = '+';
				subexpressions.push_back(subexp);
				subexp = "";
			}
			else {
				subexp += x;
			}
		}
		if (subexp!="") { subexpressions.push_back(subexp); }
		return subexpressions;
	}


	bool compute(std::string& str) {
		char lambda = '*';
		std::stack<std::pair<int, int>> possible_states; //the second element will be the string index 
		possible_states.push({ start,0 });
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

	void clear_visited(std::string str) {
		for (int state : states_name) {
			visited[state] = std::vector<std::pair<bool, bool>>(str.length() + 1, { 0,0 });
			// The +1 is because we can still search with lambda, even though the string has ended
		}
	}

	void print() {
		out << "Nr of states: " << states_name.size() << "\n";
	}
};



int main() {

	std::string exp;
	getline(in, exp);
	automata x(exp);
	
	out << "Conversion was successful\n";
	x.print();
	
	std::string str;
	int n;
	in >> n;
	for (int i = 0; i < n; i++) {
		in >> str;
		x.clear_visited(str);
		if (x.compute(str)) { out << "YES" << std::endl; }
		else { out << "NO" << std::endl; }
	}
	
	return 0;
}