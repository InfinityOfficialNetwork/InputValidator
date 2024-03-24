// InputValidator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "InputValidator.h"

int main ()
{
	Moore::InputValidator<int, Moore::InputValidatorType::MINMAXABLE>::InputValidatorMinMaxArgs Args = {
		.Type = {.PrintError = true, .ReprintInputMsgOnError = true, .ErrorMsg = L"You entered something that is not a number"},
		.Max = {.MaxValue = 10, .CheckMax = true, .AllowEquality = true, .PrintError = true, .ReprintInputMsgOnError = true, .ErrorMsg = L"You entered something that is too big"},
		.Min = {.MinValue = 0, .CheckMin = true, .AllowEquality = false, .PrintError = true, .ReprintInputMsgOnError = true, .ErrorMsg = L"You entered somehing that is too small"},
		.Zero = {.ZeroValue = 0, .CheckZero = true, .PrintError = true, .ReprintInputMsgOnError = true, .ErrorMsg = L"You entered zero"}
	};

	Moore::InputValidator<int, Moore::InputValidatorType::MINMAXABLE> IntInputter (&std::wcin, Args);

	int a = IntInputter.GetValidInput (L"Please enter an int between 1 an 10: ");
	std::cout << a;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
