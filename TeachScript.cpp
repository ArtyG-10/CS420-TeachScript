#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <limits>


struct Token {
	std::string type;
	std::string value;
	Token(const std::string type_, const std::string value_) : type(type_), value(value_) {}
};

class Lexer {
	std::string input;
	int position;
public:
	Lexer(std::string& input_) {
		input = input_;
		position = 0;
	};

	std::vector<Token> tokenize() {
		std::vector<Token> tokens;
		std::istringstream sstream(input);
		std::string word;
		while (sstream >> word) {
			if (word[0] == '$') {
				tokens.emplace_back("IDENTIFIER", word);
			}
			else if (word.find("declare") != std::string::npos || word.find("assign") != std::string::npos || word == "print" || word == "remainder") {
				tokens.emplace_back("OPERATOR", word);
			}
			else if (word.find("type_") != std::string::npos) {
				tokens.emplace_back("TYPE", word);
			}
			else if (word == "if" || word == "for") {
				tokens.emplace_back("KEYWORD", word);
			}
			else if (word == ";" || word == "|") {
				tokens.emplace_back("PUNCTUATION", word);
			}
			else if (word == "is_less_than" || word == "is_greater_than" || word == "is_equal_to") {
				tokens.emplace_back("EVALUATION", word);
			}
			else {
				tokens.emplace_back("UNKNOWN", word);
			}
		}
		return tokens;
	}
};


class Parser {
	std::vector<Token> tokens;
	std::vector<std::vector<std::string>> statements;
public:

	Parser(const std::vector<Token>& tokens_) : tokens(tokens_) {}
	std::vector<std::vector<std::string>> parse() {
		int index = 0;
		while (index < tokens.size()) {
			if (tokens[index].type == "OPERATOR") {
				std::vector<std::string> statement;
				do {
					statement.push_back(tokens[index].value);
					++index;
				} while (tokens[index].value != ";");
				++index;
				statements.push_back(statement);
			}
			
			if (index < tokens.size() && tokens[index].type == "KEYWORD") {
				std::vector<std::string> statement;
				do {
					statement.push_back(tokens[index].value);
					++index;
				} while (tokens[index].value != "|");
				++index;
				statements.push_back(statement);
			}
			
		} 
		return statements;
	}

};

class Interpreter {
public:

	std::map<std::string, int> int_vars;    //map to hold state of integer variables
	std::map<std::string, bool> bool_vars;    //map to hold state of bool variables

	//ex. declare_int $num
	void declare_int(std::vector<std::string> statement) {
		std::string name = statement[1];
		int_vars[name] = std::numeric_limits<int>::min();
	}
	//ex. declare_bool $num
	void declare_bool(std::vector<std::string> statement) {
		std::string name = statement[1];
		bool_vars[name] = false;
	}
	//ex. assign_int 7 to $num
	void assign_int(std::vector<std::string> statement) {
		int_vars[statement[3]] = std::stoi(statement[1]);
	}
	//ex. remainder $num by 3 save to $rem
	void remainder(std::vector<std::string> statement) {
		int num = int_vars[statement[1]];
		int divisor = std::stoi(statement[3]);
		int_vars[statement[6]] = num % divisor;
	}
	//ex. print $var
	void print(std::vector<std::string> statement) {
		if (statement[1].find('$') == 0) {
			std::cout << int_vars[statement[1]] << std::endl;
		}
		else {
			std::cout << statement[1] << std::endl;
		}

	}
	//ex. if $rem is_equal_to 0 print equal_to_zero ; |
	void conditional(std::vector<std::string> statement) {
		int value1 = int_vars[statement[1]];
		int value2 = std::stoi(statement[3]);
		if (statement[2] == "is_equal_to") {
			if (value1 == value2) {
				std::cout << statement[5] << std::endl;
			}
		}
	}
};

int main(int argc, char* argv[]) {
	if (argc == 2) {
		std::ifstream file(argv[1]);
		if (file.is_open()) {
			//Load input file in a buffer
			std::stringstream buff;
			buff << file.rdbuf();
			std::string input = buff.str();

			Lexer lexer(input);    //tokenize words
			std::vector<Token> tokens = lexer.tokenize();

			Parser parser(tokens);   //parse into statements
			std::vector<std::vector<std::string>> statements = parser.parse();

			Interpreter interpreter;  //interpret statements

			//debug
			for (Token token : tokens) {
				std::cout << token.type << " : " << token.value << std::endl;
			} std::cout << std::endl << "Begin Program Output:" << std::endl << std::endl;


			for (std::vector<std::string> statement : statements) {
				if (statement[0] == "declare_int") {
					interpreter.declare_int(statement);
				}
				else if (statement[0] == "assign_int") {
					interpreter.assign_int(statement);
				}
				else if(statement[0] == "remainder") {
					interpreter.remainder(statement);
				}
				else if (statement[0] == "print") {
					interpreter.print(statement);
				}
				else if (statement[0] == "if") {
					interpreter.conditional(statement);
				}
			}
			
			return 0;

		}
		else {
			std::cerr << "error file not found";
			return 1;
		}   
	}
	else {
		std::cerr << "error please type input file on function call";
		return 1;
	}
}
