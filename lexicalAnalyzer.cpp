#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <sstream>
#include <string>
#include <unordered_map>
using namespace std;

const int BUFFER_SIZE = 1000;
const int STATES = 47;
const int SYMBOLS = 33;
int TABLE_PTR = 1;
int LIT_PTR = 0;


string keywords[] = {
       "asm", "Wagarna", "new", "this", "auto", "enum", "operator", "throw", "Mantiqi",
       "explicit", "private", "True", "break", "export", "protected", "try", "case",
       "extern", "public", "typedef", "catch", "False", "register", "typeid", "Harf",
       "Ashriya", "typename", "Adadi", "class", "for", "Wapas", "union", "const",
       "dost", "short", "unsigned", "goto", "signed", "using", "continue", "Agar",
       "sizeof", "virtual", "default", "inline", "static", "Khali", "delete", "volatile",
       "do", "long", "struct", "double", "mutable", "switch", "while", "namespace",
       "template", "Marqazi", "Matn", "input->", "output<-"
};



static string getLexemeClass(int stateNo = 0) {
    static const unordered_map<int, string> charMap = {

        //identifiers
        {3, "identifier"},


        //punctuation
        {4, "punctuation"},
        {5, "punctuation"},
        {6, "punctuation"},
        {7, "punctuation"},
        {8, "punctuation"},
        {9, "punctuation"},
        {10, "punctuation"},
        {36, "punctuation"},

        //numbers
        {43, "number"},
        {44, "number" },
        {45, "number"},


        //operatorfinalstates
        {12, "operator"},
        {18, "operator"},
        {16, "operator"},
        {17, "operator"},
        {21, "operator"},
        {13, "operator"},
        {20, "operator"},
        {22, "operator"},
        {24, "operator"},
        {25, "operator"},
        {26, "operator"},
        {28, "operator"},
        {24, "operator"},
        {30, "operator"},
        {14, "operator"},
        {31, "operator" },
        {32, "operator" },
        {34, "operator" }
    };

    auto it = charMap.find(stateNo);
    if (it != charMap.end()) {
        return it->second;
    }
    return "unknown";
}

static bool checkIfKeyword(string lexeme) {
    int len = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < len; i++) {
        if (lexeme == keywords[i]) {
            return 1;
        }
    }
    return 0;
}



static void Meow(const string& inputFile, const string& outputFile) {
    ifstream read(inputFile);
    ofstream write(outputFile);
    if (!read || !write) {
        cerr << "error file nahi opne" << endl;
        return;
    }

    string line;
    bool multilineComment = false;
    while (getline(read, line)) {
        int i = 0;
        string parsedLine = "";
        bool spaceFlag = false;

        while (i < line.length()) {
            if (!multilineComment && i + 1 < line.length() && line[i] == '/' && line[i + 1] == '*') {
                multilineComment = true;
                i += 2;
            }
            else if (multilineComment && i + 1 < line.length() && line[i] == '*' && line[i + 1] == '/') {
                multilineComment = false;
                i += 2;
            }
            else if (!multilineComment && i + 1 < line.length() && line[i] == '/' && line[i + 1] == '/') {
                break;
            }
            else if (!multilineComment) {
                if (isspace(line[i])) {
                    if (!spaceFlag && !parsedLine.empty() && (isalnum(parsedLine.back()) || ispunct(parsedLine.back()))) {
                        parsedLine += ' ';
                    }
                    spaceFlag = true;
                }
                else {
                    parsedLine += line[i];
                    spaceFlag = false;
                }
            }
            i++;
        }

        if (!multilineComment && !parsedLine.empty()) {
            write << parsedLine << '\n';
        }
    }

    read.close();
    write.close();
}


static bool CheckSymbolTable(const string& symboltable, const string& check, const string& classLex) {
    ifstream read(symboltable);
    if (!read) {
        cerr << "File open nahi ho rahi" << endl;
        return false;
    }

    vector<int> positions;
    vector<string> values;
    vector<string> classLexemes;

    string line;
    while (getline(read, line)) {
        stringstream ss(line);
        int pos;
        string value, lexeme;

        ss >> pos;
        ss.ignore();
        getline(ss >> ws, value, '\t');
        getline(ss >> ws, lexeme);

        positions.push_back(pos);
        values.push_back(value);
        classLexemes.push_back(lexeme);
    }
    read.close();

    for (const auto& val : values) {
        if (val == check) {
            return false;
        }
    }

    // Adding new entry
    int newPos = positions.size() + 1;
    positions.push_back(newPos);
    values.push_back(check);
    classLexemes.push_back(classLex);

    // Writing updated contents back to the file
    ofstream file(symboltable, ios::trunc);
    if (!file) {
        cerr << "File write nahi ho rahi" << endl;
        return false;
    }

    for (size_t i = 0; i < positions.size(); i++) {
        file << positions[i] << "\t" << values[i] << "\t" << classLexemes[i] << endl;
    }

    file.close();
    return true;
}



static bool LiteralTable(const string& symboltable, const string& check) {
    ifstream read(symboltable);
    if (!read) {
        cerr << "File open nahi ho rahi" << endl;
        return false;
    }

    vector<int> positions;
    vector<string> values;

    string line;
    while (getline(read, line)) {
        stringstream ss(line);
        int pos;
        string value;
        ss >> pos;
        ss.ignore();
        getline(ss >> ws, value);

        positions.push_back(pos);
        values.push_back(value);
    }
    read.close();

    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == check) {
            return false;
        }
    }

    int newPos = positions.empty() ? 0 : positions.back() + 1;
    positions.push_back(newPos);
    values.push_back(check);

    // Overwrite the file
    ofstream file(symboltable);
    if (!file) {
        cerr << "File write nahi ho rahi" << endl;
        return false;
    }

    for (size_t i = 0; i < positions.size(); i++) {
        if (values[i] != "::") {
            file << positions[i] << "\t" << values[i] << endl;
        }
    }

    file.close();
    return true;
}



static int getCharIndex(char ch, int state) {
    static const std::unordered_map<char, int> charMap = {
        {'_', 0}, {'(', 3}, {'{', 4}, {')', 5}, {'}', 6}, {'[', 7}, {']', 8}, {':', 9},
        {'<', 10}, {'>', 11}, {'=', 12}, {'+', 13}, {'%', 14}, {'/', 15}, {'&', 16},
        {'!', 17}, {'*', 18}, {'"', 19}, {'|', 20}, {'.', 21}, {'-', 22}, {'E', 23}
    };

    //otehrs 



    if (std::isalpha(ch)) return 1;
    if (std::isdigit(ch)) return 2;

    auto it = charMap.find(ch);
    int index = (it != charMap.end()) ? it->second : -1;

    if (state == 15 && index == -1) {//other1 ka index
        return 24;       //otehr 1 
    }

    if ((state == 1) && (index != 0 && index != 1 && index != 2)) {
        return 25;// state 2
    }


    if (state == 23 && index == -1) {
        return 26;//other 3
    }

    if (state == 37 && (index != 2 && index != 21)) {
        return 27;//other 3
    }


    if (state == 42 && index == -1) {
        return 28;//other 4
    }

    if (state == 19 && index != 12) {
        return 29;//other 5
        /*break;*/
    }

    if (state == 39 && index == -1) {
        return 30;//other 6
    }

    if (state == 12 && index != 10 && index != 11 && index != 12) {
        return 31;
    }
    if (state == 27 && index != 12) {
        return 32;
    }




    return index;
}


static void fillTransitions(int transitionTable[STATES][SYMBOLS], bool shouldAdvance[STATES][SYMBOLS]) {





    transitionTable[0][0] = 1;              // State 0, '_' → State 1   
    transitionTable[0][1] = 2;              // State 0, Letter → State 2
    transitionTable[0][3] = 7;              // State 0, '(' → State 7
    transitionTable[0][4] = 6;              // State 0, '{' → State 6
    transitionTable[0][5] = 5;              // State 0, ')' → State 5
    transitionTable[0][6] = 4;               // State 0, '}' → State 4
    transitionTable[0][7] = 5;              // State 0, '[' → State 5
    transitionTable[0][8] = 8;              // State 0, ']' → State 8
    transitionTable[0][9] = 9;              // State 0, ':' → State 9
    transitionTable[0][2] = 37;
    transitionTable[0][22] = 46;
    transitionTable[0][13] = 46;


    transitionTable[0][10] = 12;            //0    ,<  ->1
    transitionTable[0][11] = 15;            //0    ,>  ->4
    transitionTable[0][12] = 19;            //0    ,=  ->8
    transitionTable[0][14] = 21;            //0    ,%  ->10
    transitionTable[0][13] = 23;            //0    ,+ ->12
    transitionTable[0][17] = 27;            //0    ,!  ->16
    transitionTable[0][16] = 33;            //0    ,&  ->22
    transitionTable[0][18] = 32;            //0    ,*  ->18
    transitionTable[0][19] = 36;            //0    ,"  ->10
    transitionTable[0][20] = 37;            //0    ,|  ->10



    transitionTable[1][0] = 1;
    transitionTable[1][1] = 1;
    transitionTable[1][2] = 1;
    transitionTable[1][25] = 3;                 //other2


    transitionTable[2][0] = 1;
    transitionTable[2][1] = 2;
    transitionTable[2][2] = 2;



    transitionTable[9][9] = 10;



    transitionTable[12][10] = 14;       //<<
    transitionTable[12][11] = 13;       //<>
    transitionTable[12][12] = 18;       //<=
    transitionTable[12][31] = 18;       //<

    transitionTable[15][11] = 16;
    transitionTable[15][12] = 17;
    transitionTable[15][24] = 22;//other 1 >



    transitionTable[19][12] = 20;
    transitionTable[19][29] = 31;//other 5

    transitionTable[23][13] = 25;
    transitionTable[23][12] = 26;
    transitionTable[23][26] = 24;//other 3

    transitionTable[27][12] = 28;
    transitionTable[27][32] = 29;

    transitionTable[33][16] = 34;



    transitionTable[37][2] = 37;
    transitionTable[37][21] = 38;
    transitionTable[37][27] = 45;

    transitionTable[38][2] = 39;

    transitionTable[39][2] = 39;
    transitionTable[39][23] = 40;
    transitionTable[39][30] = 44;

    transitionTable[40][22] = 41;
    transitionTable[40][13] = 41;

    transitionTable[41][2] = 42;
    transitionTable[42][2] = 42;
    transitionTable[42][28] = 43;


    transitionTable[46][2] = 37;




}



static void analyzeLexemes(const string& parsedFile, const string& lexemeFile, const string& errorsFile) {
    ifstream file(parsedFile);
    ofstream lexemeOut(lexemeFile);
    ofstream errorOut(errorsFile);
    if (!file) {
        cerr << "Error opening parsed file!" << endl;
        return;
    }
    if (!lexemeOut) {
        cerr << "Error creating lexeme file!" << endl;
        return;
    }
    if (!errorOut) {
        cerr << "Error creating errors file!" << endl;
        return;
    }
    //file IO








    //DataStructures
    char buffer1[BUFFER_SIZE + 1] = { 0 };
    bool useBuffer1 = true;
    int transition_table[STATES][SYMBOLS] = { -1 };
    bool shouldAdvance[STATES][SYMBOLS] = { false };
    bool accept[STATES] = {
                      0,0,0,1,1,1,1,1,1,0,1,
                      0,0,1,1,0,1,1,1,0,1,1,
                      1,0,1,1,1,0,1,1,0,1,1,
                      0,1,0,1,0,0,0,0,0,0,1,
                      1,1,0 };
    fillTransitions(transition_table, shouldAdvance);





    //Mian Loop
    while (!file.eof()) {
        char* buffer = buffer1;
        file.read(buffer, BUFFER_SIZE);
        buffer[file.gcount()] = '\0';

        int i = 0;



        while (i < file.gcount()) {
            int state = 0;
            int start = i;
            int handled_outer = 1;
            while (!accept[state]) {
                bool advanceFlag = true;
                string tempstr(buffer + start, i - start);
                int charIndex = getCharIndex(buffer[i], state);


                //debugging functon
                /*cout << "Current state: " << state << ", char: " << buffer[i] << ", index: " << charIndex << endl;*/


                if (checkIfKeyword(tempstr)) {
                    lexemeOut << "<" << tempstr << ",keyword>" << '\n';
                    handled_outer = false;
                    break;
                }


                if (buffer[i] == 'E' && state == 39) state = 40;
                else if (charIndex == -1 || transition_table[state][charIndex] == -1) break;
                else state = transition_table[state][charIndex];

                if ((state == 46 && buffer[i + 1] == ' ') || (state == 46 && getCharIndex(buffer[i + 1], -2) == '\0'))
                    state = 24;

                //handeling literals and errors
                if (state == 36) {
                    lexemeOut << '<' << "punctuation" << "," << "\"" << ">" << endl;
                    string literal;
                    i++;
                    while (buffer[i] != '"' && buffer[i] != '\0' && buffer[i] != '\n') {
                        literal = literal + buffer[i];
                        i++;
                    }
                    if (!literal.empty()) {
                        LiteralTable("literalTable.txt", literal);
                        ;                       lexemeOut << '<' << "literal" << "," << literal << ">" << endl;
                    }
                    handled_outer = false;
                    break;
                }


                //not needing a 2d Advnced array due to thisd funtion
                if (state == 45 || state == 29) {
                    advanceFlag = false;
                }


                if (advanceFlag) i++;
                else break;





                if (state == 36) {
                    handled_outer = false;
                    lexemeOut << "<\"" << ",punctuation>" << '\n';

                }

            }


            string lexeme(buffer + start, i - start);//making the token
            if (!lexeme.empty() && handled_outer) {
                if (accept[state]) {
                    string className = getLexemeClass(state);
                    CheckSymbolTable("symbolTable.txt", lexeme, className);
                    lexemeOut << "<" << lexeme << "," << className << ">" << '\n';
                }
                else {
                    errorOut << "Error: " << lexeme << endl;
                }
            }
            if (i == start) i++;


            /* cout <<"i:"<<i<<endl;*/

            while (i < file.gcount() && getCharIndex(buffer[i], state) == -1) i++;
        }
        useBuffer1 = !useBuffer1;
    }



    file.close();
    lexemeOut.close();
    errorOut.close();
}


int main() {
    string inputFile = "input.txt";
    string parsedFile = "parsed_code.txt";
    string lexemeFile = "22L_6669Lexemes.txt";
    string errorsFile = "errors.txt";
    Meow(inputFile, parsedFile);
    analyzeLexemes(parsedFile, lexemeFile, errorsFile);

    cout << "Lexical analysis completed. Valid lexemes written to: " << lexemeFile << endl;
    return 0;
}
