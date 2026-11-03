#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>
using namespace std;

class parser {
    vector<string> tokens;
    vector<string> values;
    int index = 0;
    bool debug = true;
    int tempCounter = 0;  

public:
    void enabledebug(bool val) {
        debug = val;
    }

    void readtokens() {
        ifstream file("tokens.txt");
        string line;
        while (getline(file, line)) {
            size_t commapos = line.find(',');
            string token = line.substr(commapos + 1);
            string value = line.substr(1, commapos - 1);
            token = token.substr(0, token.size() - 1);
            token.erase(remove(token.begin(), token.end(), ' '), token.end());
            tokens.push_back(token);
            values.push_back(value);
        }
        trimtrailingspaces(values);
    }

    void trimtrailingspaces(vector<string>& arr) {
        for (string& str : arr) {
            while (!str.empty() && str.back() == ' ') {
                str.pop_back();
            }
        }
    }

    string getnexttoken() {
        if (index < tokens.size()) return tokens[index];
        return "null";
    }

    string getnextcharacter() {
        if (index < values.size()) return values[index];
        return "";
    }

    void match(string expected) {
        string current = getnexttoken();
        if (current == expected) {
            index++;
        }
        else {
            cout << "Expected " << expected << " but got " << current << endl;
            exit(1);
        }
    }

    void matchcharacter(string expected) {
        string current = getnextcharacter();
        if (current == expected) {
            index++;
        }
        else {
            cout << "Expected " << expected << " but got " << current << endl;
            exit(1);
        }
    }

    void stmt() {
        if (getnexttoken() == "keyword") {
            if (getnextcharacter() == "while") whilestmt();
            else if (getnextcharacter() == "agar") ifstmt();
            else type();
        }
        else if (getnexttoken() == "identifier") {
            expr();
            matchcharacter("::");
        }
        else if (getnextcharacter() == "::") {
            matchcharacter("::");
        }
        else {
            exit(1);
        }
    }

    void whilestmt() {
        matchcharacter("while");
        matchcharacter("(");
        expr();
        matchcharacter(")");
        stmt();
    }

    void ifstmt() {
        matchcharacter("agar");
        matchcharacter("(");
        expr();
        matchcharacter(")");
        stmt();
    }

    void compstmt() {
        matchcharacter("{");
        stmt();
        matchcharacter("}");
    }

    string expr() {  
        if (getnexttoken() == "identifier" && values[index + 1] == "=") {
            string identifier = getnextcharacter();
            match("identifier");
            matchcharacter("=");
            Result magRes = Mag();
            Gen(identifier + " = " + magRes.addr); 
            return magRes.addr; 
        }
        else {
            Rvalue();
            return ""; 
        }
    }

    struct Result {
        string addr;
        string left;
        string sym;
    };

    struct RvalueResult {
        string op;
        string addr;
        string trueLabel;
        string falseLabel;
    };

    RvalueResult Rvalue() {
        Result magRes = Mag();
        Result eRes = E();

        RvalueResult rv;
        rv.op = eRes.left;
        rv.addr = magRes.addr + " " + rv.op + " " + eRes.addr;

        static int labelCounter = 0;
        rv.trueLabel = "L" + to_string(labelCounter++);
        rv.falseLabel = "L" + to_string(labelCounter++);

        Gen("if " + magRes.addr + " " + rv.op + " " + eRes.addr + " goto " + rv.trueLabel);
        Gen("goto " + rv.falseLabel);

        return rv;
    }

    Result E() {
        if (getnexttoken() == "punctuation") {
            string relop_val = relop();
            Result magRes = Mag();
            Result e1Res = E();

            Result eRes;
            eRes.left = relop_val;
            eRes.addr = magRes.addr + " " + relop_val + " " + e1Res.addr;
            return eRes;
        }
        else {
            Result eRes;
            eRes.left = "";
            eRes.addr = "";
            return eRes;
        }
    }

    string relop() {
        if (getnexttoken() == "punctuation") {
            string relop_op = getnextcharacter();
            matchcharacter(relop_op);
            return relop_op;
        }
        else {
            cout << "Error: Invalid relop" << endl;
            exit(1);
        }
    }

    Result Mag() {
        Result termRes = Term();
        Result fRes = F(termRes.addr);
        Result magRes;
        magRes.addr = newTemp();
        magRes.sym = fRes.left;
        Gen(magRes.addr + " = " + termRes.addr + " " + magRes.sym + " " + fRes.addr); 
        return magRes;
    }

    Result Term() {
        Result factorRes;
        factorRes.addr=factor();
        Result gRes = G(factorRes.addr);
        Result termRes;
        termRes.addr = newTemp();
        termRes.sym = gRes.left;
        Gen(termRes.addr + " = " + factorRes.addr + " " + termRes.sym + " " + gRes.addr);  
        return termRes;
    }

    Result G(const string& factorAddr) {
        if (getnextcharacter() == "*") {
            matchcharacter("*");
            Result factorRes;
            factorRes.addr=factor();
            Result G1 = G(factorRes.addr);
            Result Gres;
            Gres.addr = newTemp();
            Gres.left = "*";
            Gen(Gres.addr + " = " + factorAddr + " * " + G1.addr); 
            return Gres;
        }
        else if (getnextcharacter() == "/") {
            matchcharacter("/");
            Result factorRes;
            factorRes.addr= factor();
            Result G1 = G(factorRes.addr);
            Result Gres;
            Gres.addr = newTemp();
            Gres.left = "/";
            Gen(Gres.addr + " = " + factorAddr + " / " + G1.addr); 
            return Gres;
        }
        else {
            Result Gres;
            Gres.addr = factorAddr;
            Gres.left = "";
            return Gres;
        }
    }

    string factor() {
        string factor_tok;
        if (getnextcharacter() == "(") {
            matchcharacter("(");
            factor_tok = expr();
            matchcharacter(")");
            return factor_tok;
        }
        else if (getnexttoken() == "identifier") {
            factor_tok = getnextcharacter();
            match("identifier");
            return factor_tok;
        }
        else if (getnexttoken() == "number") {
            factor_tok = getnextcharacter();
            match("number");
            return factor_tok;
        }
        else {
            cout << "Error: Invalid factor" << endl;
            exit(1);
        }
    }

    void Gen(const string& code) {
        if (debug) {
            cout  << code << endl;
        }
    }

    string newTemp() {
        return "t" + to_string(tempCounter++);
    }

    Result F(const string& inheritedAddr) {
        Result res;
        if (getnextcharacter() == "+") {
            matchcharacter("+");
            Result t = Term();
            Result f1 = F(t.addr);
            res.addr = newTemp();
            res.left = "+";
            Gen(res.addr + " = " + inheritedAddr + " + " + f1.addr);
        }
        else if (getnextcharacter() == "-") {
            matchcharacter("-");
            Result t = Term();
            Result f1 = F(t.addr);
            res.addr = newTemp();
            res.left = "-";
            Gen(res.addr + " = " + inheritedAddr + " - " + f1.addr);
        }
        else {
            res.addr = inheritedAddr;
        }
        return res;
    }

    void type() {
        if (getnexttoken() == "keyword") {
            string type_tok = getnextcharacter();
            match("keyword");
            Gen("Type = " + type_tok);  
        }
        else {
            cout << "Error: Invalid type" << endl;
            exit(1);
        }
    }
};

int main() {
    parser p;
    p.readtokens();
    p.enabledebug(true);
    try {
        p.stmt();
    }
    catch (const runtime_error& e) {
        cout << e.what() << endl;
    }
    return 0;
}
