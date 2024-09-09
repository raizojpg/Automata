#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
using namespace std;

ifstream cin("cyk.txt");
ofstream cout("result.txt");

map<char, set<string>> grammer;
map<string, set<char>> grammer_reverse;

int n;
string rule;
string word;

void print(vector<vector<set<char>>>& table) {
	for (int i = 0; i < table.size(); i++) {
		for (int j = 0; j < table[i].size(); j++) {
			cout << table[i][j].size() << " ";
		}
		cout << endl;
	}
	cout << endl;
}

int main()
{
	cin >> n;
	cin.get();
	for (int i = 0; i < n; i++) {
		char left;
		string right = "";
		getline(cin,rule);
		left = rule[0];
		for (int i = 2; i < rule.size();i++) {
			if (rule[i] != '|') {
				right += rule[i];
			}
			else {
				grammer[left].insert(right);
				right = "";
			}
		}
		grammer[left].insert(right);
		for (string right : grammer[left]) {
			grammer_reverse[right].insert(left);
		}

	}
	
	getline(cin, word);
	set<char> s;
	vector<set<char>> v (word.length(),s);
	vector<vector<set<char>>> table(word.length(), v);

	for (int i = word.size() - 1; i >= 0; i--) {
		for (int j = i; j <= word.size() - 1; j++) {
			//cout << "(" << i << ";" << j << ") ";
			if (i == j) {
				string key = "";
				key += word[i];
				table[i][i] = grammer_reverse[key];
			}
			else {
				//cout<<"FOR"<< "(" << i << ";" << j << ")\n";
				for (int k = i + 1; k <= j; k++) {
					//cout <<"(" << i << ";" << k-1 << ") "<< " and " << "(" << k << ";" << j << ")\n";
					
					//product between the solutions of (i,k-1) and (k,j);
					set<string> productions;
					for (char x : table[i][k - 1]) {
						for (char y : table[k][j]) {
							string production = "";
							production += x;
							production += y;
							productions.insert(production);
						}
					}
					// check if the productions are in the grammer
					for (string prod : productions) {
						set<char> solutions = grammer_reverse[prod];
						for (char sol : solutions) {
							table[i][j].insert(sol);
						}
					}
				
				}
				
			}
		}
	}


	print(table);
	for (char sol : table[0][word.size() - 1]) {
		cout << sol << " ";
	}
	cout << endl;

	//check if S is in the solutions for the complete word
	if (table[0][word.size() - 1].find('S') != table[0][word.size() - 1].end()) { cout << "YES\n"; }
	else { cout << "NO\n"; }


}

