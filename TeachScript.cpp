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
			else if (word.find("declare") != std::string::npos || word.find("assign") != std::string::npos || word == "print" || word == "remainder" || word == "add" || word == "subtract") {
				tokens.emplace_back("OPERATOR", word);
			}
			else if (word.find("type_") != std::string::npos) {
				tokens.emplace_back("TYPE", word);
			}
			else if (word == "if") {
				tokens.emplace_back("CONDITIONAL", word);
			}
			else if (word == "loop") {
				tokens.emplace_back("LOOP", word);
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

	Parser(const std::vector<Token>& tokens_) : tokens(tokens_), statements({}) {}
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
			if (index < tokens.size() && tokens[index].type == "CONDITIONAL") {
				std::vector<std::string> statement;
				do {
					statement.push_back(tokens[index].value);
					++index;
				} while (tokens[index].value != ";");
				++index;
				statements.push_back(statement);
			}
			if (index < tokens.size() && tokens[index].type == "LOOP") {
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
	std::vector<std::vector<std::string>> statements;

	Interpreter(std::vector<std::vector<std::string>> statements_) : statements(statements_) {}
	//ex. declare_int $num
	void declare_int(std::vector<std::string> statement) {
		std::string name = statement[1];
		int_vars[name] = std::numeric_limits<int>::min();
	}
	//ex. declare_bool $flag
	void declare_bool(std::vector<std::string> statement) {
		bool_vars[statement[1]] = false;
	}
	//ex. assign_int 7 to $num
	void assign_int(std::vector<std::string> statement) {
		int_vars[statement[3]] = std::stoi(statement[1]);
	}
	// assign_bool true to $flag
	void assign_bool(std::vector<std::string> statement) {
		if (statement[1] == "true") {
			bool_vars[statement[3]] = true;
		}
		else {
			bool_vars[statement[3]] = false;
		}
	}
	//ex. remainder $num by 3 save to $rem
	void remainder(std::vector<std::string> statement) {
		int num = int_vars[statement[1]];
		int divisor = std::stoi(statement[3]);
		int_vars[statement[6]] = num % divisor;
	}
	//ex. add $num to 3 save to $total
	void add(std::vector<std::string> statement) {
		int num = int_vars[statement[1]];
		int num2 = std::stoi(statement[3]);
		int_vars[statement[6]] = num + num2;
	}
	//ex. subtract 3 from $num save to $total
	void subtract(std::vector<std::string> statement) {
		int num = std::stoi(statement[1]);
		int num2 = int_vars[statement[3]];
		int_vars[statement[6]] = num2 - num;
	}
	//ex. print $var
	void print(std::vector<std::string> statement) {
		if (statement[1].find('$') == 0) {
			if (int_vars.find(statement[1]) != int_vars.end()) {
				std::cout << int_vars[statement[1]] << std::endl;
			}
			else {
				std::cout << bool_vars[statement[1]] << std::endl;
			}
		}
		else {
			std::cout << statement[1] << std::endl;
		}

	}
	
	//ex. if $rem is_equal_to 0 print equal_to_zero ;
	//ex. if $rem is_equal_to 0 assign_bool false to $flag ;
	//ex. if $flag is_equal_to true print^ fizzbuzz ;
	void conditional(std::vector<std::string> statement) {
		if (int_vars.find(statement[1]) != int_vars.end()) {
			int value1;
			if (statement[1][0] == '$') {
				value1 = int_vars[statement[1]];
			}
			else {
				value1 = std::stoi(statement[1]);
			}
			int value2 = std::stoi(statement[3]);
			if (statement[2] == "is_equal_to") {
				if (value1 == value2 && statement[4] == "print") {
					std::cout << statement[5] << std::endl;
				} else if (value1 == value2 && statement[4] == "assign_bool"){
					bool_vars[statement[7]] = true;
				}
			}
			if (statement[2] == "is_not_equal_to") {
				if (value1 != value2 && statement[4] == "print") {
					std::cout << statement[5] << std::endl;
				} else if (value1 != value2 && statement[4] == "assign_bool"){
					bool_vars[statement[7]] = false;
				}
			}
		}
		else {
			bool value1;
			value1 = bool_vars[statement[1]];
			bool value2; 
			if (statement[3] == "true") value2 = true;
			else value2 = false;
			if (statement[2] == "is_equal_to") {
				if (value1 == value2) {
					std::cout << statement[5] << std::endl;
				}
			}
			if (statement[2] == "is_not_equal_to") {
				if (value1 != value2) {
					std::cout << statement[5] << std::endl;
				}
			}
		}
	}

	//ex. loop 20 times over i : 
	//		if $rem is_equal_to 0 print i ;
	//		if $rem is_not_equal_to 0 print no ;	 
	//		| 
	std::vector<std::vector<std::string>> parse2(std::vector<std::string> tokens2) {
		int index = 0;
		std::vector<std::vector<std::string>> statements;
		while (index < tokens2.size()) {
			if (tokens2[index] == "print" || tokens2[index] == "add" || tokens2[index] == "remainder" || tokens2[index] == "assign_bool") {
				std::vector<std::string> statement;
				do {
					statement.push_back(tokens2[index]);
					++index;
				} while (tokens2[index] != ";");
				++index;
				statements.push_back(statement);
			}
			if (index < tokens2.size() && tokens2[index] == "if") {
				std::vector<std::string> statement;
				do {
					statement.push_back(tokens2[index]);
					++index;
				} while (tokens2[index] != ";");
				++index;
				statements.push_back(statement);
			}
		}
		return statements;
	}

	//loop 20 times :
	//	if $num is_equal_to 0 print $num ;
	//	add $num to 1 save to $num ;
	//	print $num ;
	//|
	void loop(std::vector<std::string> statement) {

		int loops = std::stoi(statement[1]);
		std::vector<std::string> statements_unparsed = std::vector<std::string>(statement.begin() + 4, statement.end());
		std::vector<std::vector<std::string>> statements = parse2(statements_unparsed);
		for (int i = 0; i < loops; ++i) {
			interpret(statements);
		}
	}
	void interpret(std::vector<std::vector<std::string>> statements) {
		for (std::vector<std::string> statement : statements) {
			if (statement[0] == "declare_int") {
				declare_int(statement);
			}
			else if (statement[0] == "assign_int") {
				assign_int(statement);
			}
			else if (statement[0] == "declare_bool") {
				declare_bool(statement);
			}
			else if (statement[0] == "assign_bool") {
				assign_bool(statement);
			}
			else if (statement[0] == "remainder") {
				remainder(statement);
			}
			else if (statement[0] == "print") {
				print(statement);
			}
			else if (statement[0] == "if") {
				conditional(statement);
			}
			else if (statement[0] == "add") {
				add(statement);
			}
			else if (statement[0] == "subtract") {
				subtract(statement);
			}
			else if (statement[0] == "loop") {
				loop(statement);
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

			//debug tokens
			
			for (Token token : tokens) {
				std::cout << token.type << " : " << token.value << std::endl;
			} std::cout << std::endl << "Begin Program Output:" << std::endl << std::endl;
			

			Interpreter interpreter(statements);  //interpret statements
			interpreter.interpret(interpreter.statements);


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
