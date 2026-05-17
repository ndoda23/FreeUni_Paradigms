/**
 * File: rsg.cc
 * ------------
 * Random Sentence Generator
 */

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cassert>
#include "definition.h"
#include "production.h"
#include "random.h"
using namespace std;

/**
 * Reads the grammar from file and populates map<string, Definition>.
 */
static void readGrammar(ifstream& infile, map<string, Definition>& grammar) {
    while (true) {
        string uselessText;
        getline(infile, uselessText, '{');
        if (infile.eof()) return;  // no more definitions
        infile.putback('{');
        Definition def(infile);
        grammar[def.getNonterminal()] = def;
    }
}

// recursive method for generating sentences.
vector<string> makeSentence(string& nonterminal, map<string, Definition>& grammar) {
    vector<string> result;

    if (grammar.count(nonterminal) == 0) {
        cout << "Undefined non-terminal : " << nonterminal << endl; 
        assert(false);
    }

     Definition definition = grammar.at(nonterminal);
     Production production = definition.getRandomProduction(); 

    for ( string token : production) {
        
        if (token[0] != '<' || token[token.size()-1] != '>') {
            result.push_back(token);
        } else {
            
            vector<string> expansion = makeSentence(token, grammar);
        
            for (string newToken : expansion) {
               result.push_back(newToken);
             }
        }
    }
    
    return result;
}

// this method prints sentences.
static void printSentence(const vector<string>& sentence) {
  cout << "      " << endl;
  cout << sentence[0];
  int size = sentence.size();
    for (int i = 1; i < size; i++) {
        cout << " " << sentence[i];
    }
    cout << endl;
}

// main function 
int main(int argc, char *argv[]) {
    if (argc == 1) {
        cerr << "You need to specify the name of a grammar file." << endl;
        cerr << "Usage: rsg <path to grammar text file>" << endl;
        return 1;
    }

    ifstream grammarFile(argv[1]);
    if (grammarFile.fail()) {
        cerr << "Failed to open the file named \"" << argv[1]
             << "\". Check to ensure the file exists." << endl;
        return 2;
    }

    map<string, Definition> grammar;
    readGrammar(grammarFile, grammar);
 
    // main 
        int cnt = 1 ;
        string starter = "<start>";
        while(cnt<4){
          cout << "Version #" << cnt << ": ----------" << endl;
          vector<string> sentence = makeSentence(starter,grammar);
          printSentence(sentence); 
          cnt++;
        }
    return 0;
}