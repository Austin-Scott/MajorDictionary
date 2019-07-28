#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

unordered_map<string, string> phoneticDictionary;

struct VisualizableNoun {
	string word = "";
	int score = INT_MIN;
};

unordered_map<string, VisualizableNoun> mnemonicDictionary;

optional<char> phoneticToDigit(string p) {
	if (p == "B") return '9';
	if (p == "CH") return '6';
	if (p == "D") return '1';
	if (p == "DH") return '1';
	if (p == "ER") return '4';
	if (p == "ER0") return '4';
	if (p == "ER1") return '4';
	if (p == "ER2") return '4';
	if (p == "F") return '8';
	if (p == "G") return '7';
	if (p == "JH") return '6';
	if (p == "K") return '7';
	if (p == "L") return '5';
	if (p == "M") return '3';
	if (p == "N") return '2';
//	if (p == "NG") return '2'; <- Nobody can seem to agree how to assign this sound, skipped
	if (p == "P") return '9';
	if (p == "R") return '4';
	if (p == "S") return '0';
	if (p == "SH") return '6';
	if (p == "T") return '1';
	if (p == "V") return '8';
	if (p == "Z") return '0';
	if (p == "ZH") return '6';

	return {};
}

int main() {
	/*
	1. Read contents of cmudict.txt
	2. Parse words into hash dictionary of (word -> major mnemonic)
	3. Read contents of ReillyDataset.csv
	4. Create second hash table of (major mnemonic -> (visualizable noun, score) discarding redundant mnemonics of lower scores
	5. Create C++ file containing contents of second hash table
	6. Save that file
	*/

	cout << "Constructing phonetic dictionary..." << endl;

	ifstream cmudict("cmudict.txt");
	
	if (!cmudict) { return -1; }

	while (!cmudict.eof()) {
		string line = "";
		getline(cmudict, line);
		if (line.length() > 0 && line[0] >= 'A' && line[0] <= 'Z') {
			stringstream buffer(line);
			string word = "";
			string mnemonic = "";
			
			getline(buffer, word, ' ');
			while (!buffer.eof()) {
				string phonetic = "";
				buffer >> phonetic;
				if (auto c = phoneticToDigit(phonetic)) {
					mnemonic.push_back(*c);
				}
			}

			phoneticDictionary[word] = mnemonic;
		}
	}
	cmudict.close();

	cout << "Translating visualizable nouns into mnemonics..." << endl;

	ifstream reillyDataset("ReillyDataset.csv");

	if (!reillyDataset) { return -2; }

	while (!reillyDataset.eof()) {
		string word = "";
		string scoreString = "";
		int score = 0;
		string mnemonic = "";

		getline(reillyDataset, word, ',');
		getline(reillyDataset, scoreString);
		score = stoi(scoreString);

		mnemonic = phoneticDictionary[word];

		if (mnemonic != "") {
			if (mnemonicDictionary[mnemonic].score < score) {
				mnemonicDictionary[mnemonic] = { word, score };
			}
		}
	}
	reillyDataset.close();

	cout << "Printing source file..." << endl;

	ofstream result("dictionary.cpp");

	result << "#include \"dictionary.h\"\n\n#include <unordered_map>\n#include <string>\n\nusing namespace std;\n\n";
	result << "struct VisualizableNoun { string word = \"\"; int score = INT_MIN; };\n\n";
	result << "const unordered_map<string, VisualizableNoun> dictionary = {\n";

	for (auto mnemonic : mnemonicDictionary) {
		result << "\t{ \"" << mnemonic.first << "\", { \"" << mnemonic.second.word << "\", " << mnemonic.second.score << " } },\n";
	}

	result << "};\n";

	result.close();

	cout << "...done!" << endl;

	system("pause");

}