// Includes all standard C++ libraries (common in competitive programming, but not standard practice in production code).
#include <bits/stdc++.h>
// Uses the standard namespace to avoid typing 'std::' before cout, vector, etc.
using namespace std;


// Defines high-precision constants for Pi and Euler's number (e).
const double pi = 3.14159265358979323846;
const double e = 2.71828182845904523536;


// A map to store operator precedence. Higher numbers mean higher precedence.
// '#' is used for the unary minus.
map<string, int> precedence = {
    {"+", 1}, {"-", 1}, 
    {"*", 2}, {"/", 2}, 
    {"#", 3}, {"**", 4} // Power (**) has the highest precedence.
};

// A map to identify right-associative operators.
// Most operators are 'false' (left-associative, e.g., 5-3-1 = (5-3)-1).
// Power is 'true' (right-associative, e.g., 2**3**2 = 2**(3**2)).
map<string, bool> rightAssociative = {
    {"+", false}, {"-", false}, {"*", false}, {"/", false}, 
    {"#", true}, {"**", true}
};


// --- Helper Functions ---

// Checks if a token is one of the hard-coded mathematical function names.
bool isFunction(const string& token) {
    return token == "ln" || token == "exp" || token == "sin" ||
           token == "cos" || token == "tan" || token == "sqrt" ||
           token == "arcsin" || token == "arccos" || token == "arctan" ||
           token == "sinh" || token == "cosh" || token == "tanh";
}

// Checks if a token is an operator by seeing if it exists in the 'precedence' map.
bool isOperator(const string& token) {
    return precedence.count(token);
}

// Checks if a token is a number.
// It's a simple check: does it start with a digit, or a '.' (if not just ".")?
bool isNumber(const string& token) {
    return isdigit(token[0]) || (token[0] == '.' && token.length() > 1);
}

// Checks if a token is one of the defined constants.
bool isConstant(const string& token) {
    return token == "pi" || token == "e";
}


// --- Step 1: Tokenizer ---
// This function parses the raw input string into a list (vector) of tokens.
vector<string> tokenize(const string& expression) {
    vector<string> tokens;
    // This boolean tracks if the *next* minus sign should be a unary minus ('#').
    bool checkUnaryMinus = true; 
    
    for (int i = 0; i < expression.length(); ++i) {
        char c = expression[i];
        if (isspace(c)) continue; // 1. Skip whitespace.

        // 2. Read a full number (integer or decimal).
        else if (isdigit(c) || (c == '.' && i + 1 < expression.length() && isdigit(expression[i+1]))) {
            string number;
            number += c;
            // Keep reading as long as it's a digit or another '.'
            while (i + 1 < expression.length() && (isdigit(expression[i + 1]) || expression[i + 1] == '.')) {
                number += expression[++i];
            }
            tokens.push_back(number);
            checkUnaryMinus = false; // A number cannot be followed by a unary minus.
        } 
        // 3. Read a function name or constant (e.g., "sin", "pi").
        else if (isalpha(c)) {
            string mathematicalFunctions;
            mathematicalFunctions += c;
            while (i + 1 < expression.length() && isalpha(expression[i + 1])) {
                mathematicalFunctions += expression[++i];
            }
            tokens.push_back(mathematicalFunctions);
            checkUnaryMinus = false;
        } 
        // 4. Handle open parenthesis.
        else if (c == '(') {
            tokens.push_back("(");
            checkUnaryMinus = true; // A unary minus can follow '('.
        } 
        // 5. Handle close parenthesis.
        else if (c == ')') {
            tokens.push_back(")");
            checkUnaryMinus = false;
        } 
        // 6. Handle power ('**') and multiplication ('*').
        else if (c == '*') {
            if (i + 1 < expression.length() && expression[i + 1] == '*') {
                tokens.push_back("**"); i++; // Found power, skip next char.
            } else {
                tokens.push_back("*"); // Found multiplication.
            }
            checkUnaryMinus = true;
        }
        // 7. Handle unary minus ('#') vs. binary subtraction ('-').
        else if (c == '-') {
            // Use ternary operator: if checkUnaryMinus is true, push '#', else push '-'.
            tokens.push_back(checkUnaryMinus ? "#" : "-");
            checkUnaryMinus = true;
        }
        // 8. Handle other single-char operators ('+', '/').
        else {
            tokens.push_back(string(1, c)); // Convert char 'c' to string.
            checkUnaryMinus = true;
        }
    }
    return tokens;
}


// --- Step 2: Shunting-Yard Algorithm ---
// Converts the infix token list from tokenize() to a postfix (RPN) token list.
vector<string> shuntingYard(const vector<string>& tokens) {
    vector<string> outputQueue; // The final RPN list.
    stack<string> operatorStack; // Temporary stack for operators and functions.

    for (const string& token : tokens) {
        // Rule 1: If it's a number or constant, add it directly to the output.
        if (isNumber(token) || isConstant(token)) {
            outputQueue.push_back(token);
        } 
        // Rule 2: If it's a function, push it onto the operator stack.
        else if (isFunction(token)) {
            operatorStack.push(token);
        } 
        // Rule 3: If it's an operator...
        else if (isOperator(token)) {
            // ...while the operator on the stack has higher/equal precedence...
            while (!operatorStack.empty() && operatorStack.top() != "(" &&
                   (precedence[operatorStack.top()] > precedence[token] ||
                    (precedence[operatorStack.top()] == precedence[token] && !rightAssociative.at(token)))) {
                // ...pop from the stack to the output.
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
            // Finally, push the current operator onto the stack.
            operatorStack.push(token);
        } 
        // Rule 4: If it's an open parenthesis, push it onto the stack.
        else if (token == "(") {
            operatorStack.push(token);
        } 
        // Rule 5: If it's a close parenthesis...
        else if (token == ")") {
            // ...pop operators from stack to output until we find the '('.
            while (!operatorStack.empty() && operatorStack.top() != "(") {
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
            if (operatorStack.empty()) throw runtime_error("Mismatched parentheses");
            operatorStack.pop(); // Discard the '('.
            
            // If the token on the stack is now a function (e.g., "sin"), pop it to output.
            if (!operatorStack.empty() && isFunction(operatorStack.top())) {
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
        }
    }
    // After the loop, pop any remaining operators from the stack to the output.
    while (!operatorStack.empty()) {
        if (operatorStack.top() == "(") throw runtime_error("Mismatched parentheses");
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
    }
    return outputQueue;
}


// --- Step 3: RPN (Postfix) Evaluation ---
// Calculates the final result from the RPN token list.
double calculation(const vector<string>& rpnTokens) {
    stack<double> valueStack; // A stack to hold numbers (operands).

    for (const string& token : rpnTokens) {
        // Rule 1: If it's a number, convert to double and push onto the stack.
        if (isNumber(token)) {
            valueStack.push(stod(token)); // stod = string to double
        } 
        // Rule 2: If it's a constant, push its value.
        else if (token == "pi") valueStack.push(pi);
        else if (token == "e") valueStack.push(e);
        // Rule 3: If it's a function...
        else if (isFunction(token)) {
            if (valueStack.empty()) throw runtime_error("Invalid expression");
            double val = valueStack.top(); valueStack.pop(); // Pop one value.
            // Perform the correct math operation and push the result back.
            if (token == "ln") valueStack.push(log(val));
            else if (token == "exp") valueStack.push(exp(val));
            else if (token == "sin") valueStack.push(sin(val));
            // ... (etc. for all other functions)
            else if (token == "tanh") valueStack.push(tanh(val));
        }
        // Rule 4: If it's an operator...
        else if (isOperator(token)) {
            // Case 4a: Unary operator ('#')
            if (token == "#") { 
                if (valueStack.empty()) throw runtime_error("Invalid expression");
                double val = valueStack.top(); valueStack.pop(); // Pop one value.
                valueStack.push(-val); // Negate it and push back.
            } 
            // Case 4b: Binary operator ('+', '-', '*', '/', '**')
            else { 
                if (valueStack.size() < 2) throw runtime_error("Invalid expression");
                // Pop two values (order matters: val2, then val1).
                double val2 = valueStack.top(); valueStack.pop();
                double val1 = valueStack.top(); valueStack.pop();
                // Perform the operation and push the result back.
                if (token == "+") valueStack.push(val1 + val2);
                else if (token == "-") valueStack.push(val1 - val2);
                else if (token == "*") valueStack.push(val1 * val2);
                else if (token == "/") {
                    if (val2 == 0) throw runtime_error("Division by zero"); // Error check
                    valueStack.push(val1 / val2);
                }
                else if (token == "**") valueStack.push(pow(val1, val2));
            }
        }
        else {
             throw runtime_error("Unknown token: " + token);
        }
    }
    // After the loop, the stack should contain exactly one value: the final result.
    if (valueStack.size() != 1) throw runtime_error("Invalid expression");
    return valueStack.top();
}


// --- Main Program Logic ---

// This function handles a single calculation cycle.
void Solve() {
    string expression;
    // Use getline to read the entire line, including spaces.
    getline(cin, expression);
    
    // The 'try' block attempts to run the code that might fail.
    try {
        // Run the 3-step process: tokenize, convert, calculate.
        vector<string> tokens = tokenize(expression);
        vector<string> rpn = shuntingYard(tokens);
        double result = calculation(rpn);
        
        // Ask for and set output precision.
        cout << "Enter the number of significant decimal places you want to print: "; int x; cin >> x;
        cout.precision(x);
        cout << fixed << result << '\n'; // 'fixed' ensures precision is after the decimal point.
    } 
    // The 'catch' block runs *only if* a 'throw' happened inside the 'try' block.
    catch (const exception& e) {
        // Print the error message (e.g., "Division by zero").
        cerr << "Error: " << e.what() << '\n';
    }
}


// The main entry point of the program.
int main() { 
    bool irs = 1; // Flag to control the loop (irs = "is running").
    system("clear"); // Clear the console screen (Linux/macOS). Use "cls" for Windows.

    // Loop to allow multiple calculations.
    while(irs) {
        cout << "Please enter your expression: ";
        Solve(); // Call the main logic function.
        
        cout << "Do you want to continue? (Yes = 1; No = 0): "; 
        cin >> irs; // Read the user's choice (1 or 0).
        
        // --- FIX for cin/getline bug ---
        // cin >> irs leaves the '\n' (Enter key) in the input buffer.
        // This cin.ignore() consumes that '\n' so the next call to
        // getline() in Solve() will wait for new input instead of reading an empty line.
        cin.ignore(); 
        
        if(!irs) {
            cout << "Hope you have a great experience\n"; 
            return 0; // Exit the program.
        }
        else system("clear"); // Clear the screen for the next calculation.
    }
}