#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>


std::ifstream in("input.txt");
std::ofstream out("output.txt");

int x, y, n, m, S, nrF, nrCuv;
char l;
std::set<int> final_states;
std::map<int, std::map<char,int>> states;
std::string str;


bool DFA(std::string& str) {
	//I assumed that the lambda letters that may appear in our string are noted with '*'

	//check for lambdas in our string
	str.erase(remove(str.begin(), str.end(), '*'), str.end());
	
	int state = S;
	for (int i = 0; i < str.length(); i++) {
		state = states[state][str[i]]; // only one state is possible for each letter
	}
	if (final_states.find(state) != final_states.end()) { return true; }
	else { return false; }
}

int main() {

	in >> n;
	for (int i = 0; i < n; i++) {
		in >> x;
		states[x];
	}
	in >> m;
	for (int i = 0; i < m; i++) {
		in >> x >> y;
		in >> l;
		in.get();
		states[x][l] = y;
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
		if (DFA(str)) { out << "DA" << std::endl; }
		else { out << "NU" << std::endl; }
	}
	return 0;
}

