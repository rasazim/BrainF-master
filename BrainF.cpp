#include <iostream>
#include<vector>
#include<map>
#include<string>
#include<fstream>
using namespace std;


enum inst {
    shift, change, startloop, endloop, read, print
};

class instruction {
public:
    inst type;
    int param;
    instruction(inst type) {
        this->type = type;
        this->param = 0;
    }
    instruction(inst type, int param) {
        this->type = type;
        this->param = param;
    }
};


vector<instruction> compile(string code) {
    vector<instruction> instruction_set;
    int len = code.length();
    int inc_dec = 0;
    int move_ = 0;
    vector<int> loop_starts;
    for (int i = 0; i < len; i++) {
        char c = code[i];
        if (c == '+') {
            if (move_ != 0) {
                instruction_set.push_back(instruction(shift, move_));
                move_ = 0;
            }
            inc_dec++;
        }
        else if (c == '-') {
            if (move_ != 0) {
                instruction_set.push_back(instruction(shift, move_));
                move_ = 0;
            }
            inc_dec--;
        }
        else if (c == '<') {
            if (inc_dec != 0) {
                instruction_set.push_back(instruction(change, inc_dec));
                inc_dec = 0;
            }
            move_--;
        }
        else if (c == '>') {
            if (inc_dec != 0) {
                instruction_set.push_back(instruction(change, inc_dec));
                inc_dec = 0;
            }
            move_++;
        }
        else if (c == ',') {
            if (move_ != 0) {
                instruction_set.push_back(instruction(shift, move_));
                move_ = 0;
            }
            else if (inc_dec != 0) {
                instruction_set.push_back(instruction(change, inc_dec));
                inc_dec = 0;
            }
            instruction_set.push_back(instruction(read));
        }
        else if (c == '.') {
            if (move_ != 0) {
                instruction_set.push_back(instruction(shift, move_));
                move_ = 0;
            }
            else if (inc_dec != 0) {
                instruction_set.push_back(instruction(change, inc_dec));
                inc_dec = 0;
            }
            instruction_set.push_back(instruction(print));
        }
        else if (c == '[') {
            if (move_ != 0) {
                instruction_set.push_back(instruction(shift, move_));
                move_ = 0;
            }
            else if (inc_dec != 0) {
                instruction_set.push_back(instruction(change, inc_dec));
                inc_dec = 0;
            }
            loop_starts.push_back(instruction_set.size());
        }
        else if (c == ']') {
            if (move_ != 0) {
                instruction_set.push_back(instruction(shift, move_));
                move_ = 0;
            }
            else if (inc_dec != 0) {
                instruction_set.push_back(instruction(change, inc_dec));
                inc_dec = 0;
            }
            int last;
            try {
                last = loop_starts.back();
            }
            catch (exception e) {
                throw exception("Syntax Error: Loop closed without opening. Instruction pointer: " + i);
            }
            loop_starts.pop_back();
            instruction_set.insert(instruction_set.begin() + last, instruction(startloop, instruction_set.size() + 1 - last));
            instruction_set.push_back(instruction(endloop, last - instruction_set.size()));
        }
    }
    if (loop_starts.size() > 0) {
        throw exception("Syntax Error: Non terminated loop.");
    }
    return instruction_set;
}

int run(vector<instruction> instruction_set, string input) {
    int cell[30000];
    for (int& a : cell) { a = 0; }
    int memory_pointer = 0;
    int len = instruction_set.size();
    int input_pointer = 0;
    for (int i = 0; i < len; i++) {
        instruction next = instruction_set[i];
        if (next.type == change) {
            cell[memory_pointer] += next.param;
        }
        else if (next.type == shift) {
            memory_pointer += next.param;
            memory_pointer %= 30000;
        }
        else if (next.type == print) {
            cout << char(cell[memory_pointer]);
        }
        else if (next.type == read) {
            cell[memory_pointer] = input[input_pointer++];
        }
        else if (next.type == startloop) {
            if (cell[memory_pointer] == 0) {
                i += next.param;
            }
        }
        else if (next.type == endloop) {
            if (cell[memory_pointer] != 0) {
                i += next.param;
            }
        }
    }
    return 0;
}
int main(int argc, char** args)
{
    ifstream file;
    string file_name = "";
    for (int i = 1; i < argc; i++) { file_name += string(args[i]) + ((i < argc - 1) ? " " : ""); }
    try {
        file.open(file_name);
    }
    catch (exception e) {
        cout << e.what();
    }
    string input;
    cout << "Enter all input in a single line.";
    getline(cin, input);

    string code = "";
    while (file) {
        string line;
        getline(file, line);
        code += line;
    }
    vector<instruction> instruction_set;
    try {
        instruction_set = compile(code);
    }
    catch (exception e) {
        cout << e.what() << endl;
        return -1;
    }
    cout << "Compiled successfully. Instruction_set length: " << instruction_set.size() << endl;
    try {
        run(instruction_set,input);
    }
    catch (exception e) {
        cout << e.what();
        return -1;
    }
    cout << "\nSuccess" << endl;
}
