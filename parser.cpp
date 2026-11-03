#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>
using namespace std;

int space = 0;

class Parser {
    vector<string> tokens;
    vector<string> values;
    int index = 0;
    bool debug = true;

public:
    void enableDebug(bool val) {
        debug = val;
    }

    void readTokens() {
        ifstream file("tokens.txt");
        string line;
        while (getline(file, line)) {
            size_t commaPos = line.find(',');
            string token = line.substr(commaPos + 1);
            string value = line.substr(1, commaPos - 1);
            token = token.substr(0, token.size() - 1);
            token.erase(remove(token.begin(), token.end(), ' '), token.end());
            tokens.push_back(token);
            values.push_back(value);
        }
        trimTrailingSpaces(values);
    }

    void trimTrailingSpaces(vector<string>& arr) {
        for (string& str : arr) {
            while (!str.empty() && str.back() == ' ') {
                str.pop_back();
            }
        }
    }

    string getNextToken() {
        if (index < tokens.size()) return tokens[index];
        return "NULL";
    }

    string getNextCharacter() {
        if (index < values.size()) return values[index];
        return "";
    }

    void match(string expected) {
        string current = getNextToken();
        if (current == expected) {
            index++;
        }
        else {
            cout << "[Error] Expected: " << expected << ", but got: " << current << endl;
            exit(1);
        }
    }

    void matchCharacter(string expected) {
        string current = getNextCharacter();
        if (current == expected) {
            index++;
        }
        else {
            cerr << "[Error] Expected: " << expected << ", but got: " << current << endl;
            exit(1);
        }
    }

    void indentPrintHorizonatl() {
        for (int i = 0; i < space; i++) cout << "|" << endl;
    }

    void indentPrint(const string& name) {
        for (int i = 0; i < space; i++) cout << "   ";
        cout << "|-- " << name << endl;
    }

    void Function() {
        indentPrint("Function");
        space++;
        Type();
        indentPrint("identifier"); match("identifier");
        indentPrint("("); matchCharacter("(");
        ArgList();
        indentPrint(")"); matchCharacter(")");
        CompStmt();
        space--;
        cout << "Parsing Successful!" << endl;
    }

    void ArgList() {
        indentPrint("ArgList");
        space++;
        if (getNextToken() == "keyword") {
            Arg();
            A();
        }
        else {
            indentPrint("^");
        }
        space--;
    }

    void A() {
        indentPrint("A");
        space++;
        if (getNextCharacter() == ",") {
            matchCharacter(",");
            Arg();
            A();
        }
        space--;
    }

    void Arg() {
        indentPrint("Arg");
        space++;
        Type();
        indentPrint("identifier"); match("identifier");
        space--;
    }

    void Declaration() {
        indentPrint("Declaration");
        space++;
        Type();
        IdentList();
        indentPrint("::"); matchCharacter("::");
        space--;
    }

    void IdentList() {
        indentPrint("IdentList");
        space++;
        indentPrint("identifier"); match("identifier");
        B();
        space--;
    }

    void B() {
        indentPrint("B");
        space++;
        if (getNextCharacter() == ",") {
            indentPrint(","); matchCharacter(",");
            indentPrint("identifier"); match("identifier");
            B();
        }
        space--;
    }

    void Stmt() {
        indentPrint("Stmt");
        space++;
        if (getNextToken() == "keyword") {
            if (getNextCharacter() == "for") ForStmt();
            else if (getNextCharacter() == "while") WhileStmt();
            else if (getNextCharacter() == "Agar") IfStmt();
            else Type();
        }
        else if (getNextToken() == "identifier") {
            Expr();
            indentPrint("::"); matchCharacter("::");
        }
        else if (getNextCharacter() == "::") {
            indentPrint("::"); matchCharacter("::");
        }
        else {
            cout << "error" << endl;
        }
        space--;
    }

    void ForStmt() {
        indentPrint("ForStmt");
        space++;
        matchCharacter("for");
        matchCharacter("(");
        Expr();
        matchCharacter("::");
        OptExpr();
        matchCharacter("::");
        OptExpr();
        matchCharacter(")");
        Stmt();
        space--;
    }

    void OptExpr() {
        indentPrint("OptExpr");
        space++;
        if (getNextToken() == "identifier") {
            Expr();
        }
        space--;
    }

    void WhileStmt() {
        indentPrint("WhileStmt");
        space++;
        matchCharacter("while");
        matchCharacter("(");
        Expr();
        matchCharacter(")");
        Stmt();
        space--;
    }

    void IfStmt() {
        indentPrint("IfStmt");
        space++;
        matchCharacter("Agar");
        matchCharacter("(");
        Expr();
        matchCharacter(")");
        L();
        space--;
    }

    void L() {
        if (getNextCharacter() == "Wagarna") matched_stmt();
        else open_stmt();
    }

    void matched_stmt() {
        indentPrint("MatchedStmt");
        space++;
        elsepart_matched();
        M();
        space--;
    }

    void open_stmt() {
        indentPrint("OpenStmt");
        space++;
        C();
        space--;
    }

    void M() {
        indentPrint("M");
        space++;
        if (getNextCharacter() == "Agar") {
            matched_stmt();
            M();
        }
        space--;
    }

    void elsepart_matched() {
        indentPrint("ElsePartMatched");
        space++;
        if (getNextCharacter() == "Wagarna") {
            matchCharacter("Wagarna");
            matched_stmt();
        }
        space--;
    }

    void C() {
        indentPrint("C");
        space++;
        if (getNextCharacter() == "Wagarna") {
            matched_stmt();
            elsepart_closed();
        }
        else {
            Stmt();
        }
        space--;
    }

    void elsepart_closed() {
        indentPrint("ElsePartClosed");
        space++;
        if (getNextCharacter() == "Wagarna") {
            match("Wagarna");
            open_stmt();
        }
        space--;
    }

    void CompStmt() {
        indentPrint("CompStmt");
        space++;
        matchCharacter("{");
        StmtList();
        matchCharacter("}");
        space--;
    }

    void StmtList() {
        D();
    }

    void D() {
        if (getNextCharacter() != "}") {
            Stmt();
            D();
        }
    }

    void Expr() {
        indentPrint("Expr");
        space++;
        if (getNextToken() == "identifier" && values[index + 1] == "=") {

            indentPrint("identifier");match("identifier");
            indentPrint("="); matchCharacter("=");
            Expr();
        }
        else {
            Rvalue();
        }
        space--;
    }

    void Rvalue() {
        indentPrint("Rvalue");
        space++;
        Mag();
        E();
        space--;
    }

    void E() {
        indentPrint("E'");
        space++;
        if (getNextCharacter() == "+") {
            indentPrint("+"); matchCharacter("+");
            Mag();
            E();
        }
        else {
            indentPrint("^");
        }
        space--;
    }

    void Mag() {
        indentPrint("T");
        space++;
        Term();
        F();
        space--;
    }

    void F() {
        indentPrint("T'");
        space++;
        if (getNextCharacter() == "*") {
            indentPrint("*"); matchCharacter("*");
            Term();
            F();
        }
        else {
            indentPrint("^");
        }
        space--;
    }

    void Term() {
        indentPrint("F");
        space++;
        Factor();
        space--;
    }

    void Factor() {
        indentPrint("Factor");
        space++;
        if (getNextCharacter() == "(") {
            matchCharacter("(");
            Expr();
            matchCharacter(")");
        }
        else if (getNextToken() == "identifier") {
            indentPrint("id"); match("identifier");
        }
        else if (getNextToken() == "number") {
            indentPrint("num"); match("number");
        }
        else {
            cout << "error" << endl;
        }
        space--;
    }

    void printTokens() {
        cout << "===== Tokens Read from tokens.txt =====" << endl;
        for (int i = 0; i < tokens.size(); i++) {
            cout << i << ": " << tokens[i] << endl;
        }
        cout << "=======================================" << endl;
    }

    void printValues() {
        cout << "===== Values Read from tokens.txt =====" << endl;
        for (int i = 0; i < tokens.size(); i++) {
            cout << values[i] << endl;
        }
        cout << "=======================================" << endl;
    }

    void Type() {
        indentPrint("Type");
        if (getNextToken() == "keyword") {
            match("keyword");
        }
        else {
            cerr << "Type error" << endl;
            exit(1);
        }
    }
};

int main() {
    Parser p;
    p.readTokens();
    p.printTokens();
    p.enableDebug(true);
    p.Function();
    return 0;
}
