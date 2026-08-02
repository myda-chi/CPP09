#include "RPN.hpp"

RPN::RPN()
{
}

RPN::~RPN() 
{
}


bool RPN::isOperator(const std::string& token) const 
{
    return (token == "+" || token == "-" || token == "*" || token == "/");
}

int RPN::stringToInt(const std::string& str) const 
{
    std::istringstream iss(str);
    int value;
    iss >> value;
    return value;
}

int RPN::performOperation(const std::string& operation, int operand1, int operand2) const 
{
    if (operation == "+") return operand1 + operand2;
    if (operation == "-") return operand1 - operand2;
    if (operation == "*") return operand1 * operand2;
    if (operation == "/") 
    {
        if (operand2 == 0) 
        {
            throw std::runtime_error("Division by zero");
        }
        return operand1 / operand2;
    }
    throw std::runtime_error("Unknown operator: " + operation);
}

int RPN::evaluate(const std::string& expression) 
{
    std::istringstream iss(expression);
    std::string token;

    while (iss >> token) 
    {
        if (isOperator(token)) 
        {
            if (_stack.size() < 2) 
            {
                throw std::runtime_error("Insufficient operands for operation: " + token);
            }
            int operand2 = _stack.back(); _stack.pop_back();
            int operand1 = _stack.back(); _stack.pop_back();
            int result = performOperation(token, operand1, operand2);
            _stack.push_back(result);
        } 
        else 
        {
            try 
            {
                int value = stringToInt(token);
                _stack.push_back(value);
            } 
            catch (const std::exception&) 
            {
                throw std::runtime_error("Invalid token: " + token);
            }
        }
    }

    if (_stack.size() != 1) 
    {
        throw std::runtime_error("Invalid RPN expression");
    }

    return _stack.back();
}