//////////////////////////////////////////////////////////////////////////////////////////
//                      INPUT VALIDATOR - MOORE SPECIFICATIONS (C)                      //
//    Suite of utilities to validate user inputs from the console, and other streams    //
//          Includes ANY, MIN/MAXABLE, LISTABLE, and even CUSTOM validators!            //
//                    There's nothing like this on the market today                     //
//              MOORE SPECIFICATIONS --- You won't find nothing like this!              //
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//helpers
#define FOR(N,I) for (int I = 0; I < (N); I++)
#define STR_ERRORS std::ios::badbit | std::ios::failbit
#define CATCHER(FUNCTION,CATCH) try FUNCTION catch(const std::exception& e) CATCH


#include <string>
#include <iostream>
#include <string>
#include <sstream>
#include <random>
//gcc nonsense includes
#include <cstring>
#include <string.h>
//#include <cstring.h>
#include <cassert>


namespace Moore
{
	//exceptions for InputValidator internals
	enum class InputValidatorExceptions
	{
		TYPE_CONVERSION, // failed to extract the type from the string
		OUT_OF_BOUNDS_MIN, // below min
		OUT_OF_BOUNDS_MAX, // above max
		OUT_OF_BOUNDS_ZERO, // is zero
		OUT_OF_LIST_W, // not on whitelist
		OUT_OF_LIST_B, // on blacklist
		CUSTOM // rethrow any exception from custom type
	};

	//extract type from a stream, assume one entry per line
	template <typename T>
	T* InputExtract (std::wistream* InStream) try
	{
		//get line
		T* In = new T ();
		std::wstring InString;
		std::getline (*InStream, InString);

		//create stringstream and extract
		std::wstringstream InStringStream (InString);
		InStringStream.exceptions (STR_ERRORS);
		InStringStream >> *In;
		return In;
	}
	catch (...)
	{
		throw InputValidatorExceptions::TYPE_CONVERSION;
	}

	//specialization for strings, take the whole line
	template <>
	std::wstring* InputExtract<std::wstring> (std::wistream* InStream) try
	{
		std::wstring* In = new std::wstring ();
		std::getline (*InStream, *In);
		return In;
	}
	catch (...)
	{
		throw InputValidatorExceptions::TYPE_CONVERSION;
	}

	//specialization for regular strings
	//I do not want to deal with short-wide conversion
	template<>
	std::string* InputExtract<std::string> (std::wistream*)
	{
		throw std::runtime_error ("USE WIDE STRINGS");
	}

	//types of inputvalidators, used for specialization
	enum class InputValidatorType
	{
		ANY, // pass the input directly from the reader
		MINMAXABLE, // check between two values, and zero; both lt/gt and le/ge
		LISTABLE, // check if input is on a white/blacklist
		CUSTOM // check with a functor w/ sig `bool Check(Input*)`, throw std::exception on error
	};


	//initial template
	template<typename T, InputValidatorType Type>
	class InputValidator;

	//ANY specialization
	//takes input from stream and returns without additional validation, retries on bad/fail bits
	template <typename T>
	class InputValidator<T, InputValidatorType::ANY>
	{
		std::wstring InputMessage;
		std::wistream* InStream;

		bool PrintTypeConversionErrorMsg = true;
		bool ReprintInputMessageOnTypeConversionError = true;
		std::wstring TypeConversionErrorMsg = L"";

	public:
		//initializer
		InputValidator (std::wistream* inStream) : InStream (inStream), InputMessage (L"")
		{}
		InputValidator (std::wistream* inStream, std::wstring Msg) : InStream (inStream), InputMessage (Msg)
		{}

		template <int MaxRetries = -1>
		T GetValidInput (std::wstring InputMessage)
		{
			for (int i = 0; ((MaxRetries == -1) ? true : (i < MaxRetries)); i++) try
			{
				std::wcout << InputMessage << std::endl;
				T* In = InputExtract<T> (InStream);
				return *In;
			}
			//handle the exceptions
			catch (const InputValidatorExceptions e)
			{
				switch (e)
				{
					//handle type conversion errors
					case InputValidatorExceptions::TYPE_CONVERSION:
						{
							std::wcout << (PrintTypeConversionErrorMsg ? (TypeConversionErrorMsg) : (L"")) << std::endl
								<< (ReprintInputMessageOnTypeConversionError ? InputMessage : L"") << std::endl;
							break;
						}
						//these exceptions are not used, assert that they are not thrown
					case InputValidatorExceptions::OUT_OF_BOUNDS_MAX:
					case InputValidatorExceptions::OUT_OF_BOUNDS_MIN:
					case InputValidatorExceptions::OUT_OF_BOUNDS_ZERO:
					case InputValidatorExceptions::OUT_OF_LIST_B:
					case InputValidatorExceptions::OUT_OF_LIST_W:
					case InputValidatorExceptions::CUSTOM:
					default:
						{
							assert (false);
							break;
						}
				}
				continue;
			}
		}

	};

	//MINMAXABLE specialization
	template <typename T>
	class InputValidator<T, InputValidatorType::MINMAXABLE>
	{
		std::wstring InputMessage;
		std::wistream* InStream;

		//messages and configurations
		bool PrintTypeConversionErrorMsg = true;
		bool ReprintInputMessageOnTypeConversionError = true;
		std::wstring TypeConversionErrorMsg = L"";

		T MaxValue;
		bool CheckOutOfBoundsErrorMax = false;
		bool AllowMaxEquality = true;
		bool PrintOutOfBoundsErrorMaxMsg = true;
		bool ReprintInputMessageOnOutOfBoundsErrorMax = true;
		std::wstring OutOfBoundsErrorMaxMsg = L"";

		T MinValue;
		bool CheckOutOfBoundsErrorMin = false;
		bool AllowMinEquality = false;
		bool PrintOutOfBoundsErrorMinMsg = true;
		bool ReprintInputMessageOnOutOfBoundsErrorMin = true;
		std::wstring OutOfBoundsErrorMinMsg = L"";

		T ZeroValue;
		bool CheckOutOfBoundsErrorZero = false;
		bool PrintOutOfBoundsErrorZeroMsg = true;
		bool ReprintInputMessageOnOutOfBoundsErrorZero = true;
		std::wstring OutOfBoundsErrorZeroMsg = L"";



	public:
		//init obj, input message is not used
		InputValidator (std::wistream* inStream) : InStream (inStream), InputMessage (L"")
		{}
		InputValidator (std::wistream* inStream, std::wstring Msg) : InStream (inStream), InputMessage (Msg)
		{}

		//struct to hold initializer args
		struct InputValidatorMinMaxArgs
		{
			struct TypeArgs
			{
				bool PrintError = true;
				bool ReprintInputMsgOnError = true;
				std::wstring ErrorMsg = L"";
			} Type;
			struct MaxArgs
			{
				T MaxValue;
				bool CheckMax = false;
				bool AllowEquality = true;
				bool PrintError = true;
				bool ReprintInputMsgOnError = true;
				std::wstring ErrorMsg = L"";
			} Max;
			struct MinArgs
			{
				T MinValue;
				bool CheckMin = false;
				bool AllowEquality = false;
				bool PrintError = true;
				bool ReprintInputMsgOnError = true;
				std::wstring ErrorMsg = L"";
			} Min;
			struct ZeroArgs
			{
				T ZeroValue;
				bool CheckZero = false;
				bool PrintError = true;
				bool ReprintInputMsgOnError = true;
				std::wstring ErrorMsg = L"";
			} Zero;
		};

		//initialize object with args
		InputValidator (std::wistream* inStream, InputValidatorMinMaxArgs Args) :
			InStream (inStream),

			PrintTypeConversionErrorMsg (Args.Type.PrintError),
			ReprintInputMessageOnTypeConversionError (Args.Type.ReprintInputMsgOnError),
			TypeConversionErrorMsg (Args.Type.ErrorMsg),

			MaxValue (Args.Max.MaxValue),
			CheckOutOfBoundsErrorMax (Args.Max.CheckMax),
			AllowMaxEquality (Args.Max.AllowEquality),
			PrintOutOfBoundsErrorMaxMsg (Args.Max.PrintError),
			ReprintInputMessageOnOutOfBoundsErrorMax (Args.Max.ReprintInputMsgOnError),
			OutOfBoundsErrorMaxMsg (Args.Max.ErrorMsg),

			MinValue (Args.Min.MinValue),
			CheckOutOfBoundsErrorMin (Args.Min.CheckMin),
			AllowMinEquality (Args.Min.AllowEquality),
			PrintOutOfBoundsErrorMinMsg (Args.Min.PrintError),
			ReprintInputMessageOnOutOfBoundsErrorMin (Args.Min.ReprintInputMsgOnError),
			OutOfBoundsErrorMinMsg (Args.Min.ErrorMsg),

			ZeroValue (Args.Zero.ZeroValue),
			CheckOutOfBoundsErrorZero (Args.Zero.CheckZero),
			PrintOutOfBoundsErrorZeroMsg (Args.Zero.PrintError),
			ReprintInputMessageOnOutOfBoundsErrorZero (Args.Zero.ReprintInputMsgOnError),
			OutOfBoundsErrorZeroMsg (Args.Zero.ErrorMsg)

		{

		}

		template <int MaxRetries = -1>
		T GetValidInput (std::wstring InputMessage)
		{
			std::wcout << InputMessage << std::endl;
			for (int i = 0; ((MaxRetries == -1) ? true : (i < MaxRetries)); i++) try
			{
				T* In = InputExtract<T> (InStream);

				//throw on specification failures

				//check zero
				if (
					CheckOutOfBoundsErrorZero
					&& (*In == ZeroValue)
					)
				{
					throw InputValidatorExceptions::OUT_OF_BOUNDS_ZERO;
				}

				//check max
				if (
					CheckOutOfBoundsErrorMax
					&& !(AllowMaxEquality ? (*In <= MaxValue) : (*In < MaxValue))
					)
				{
					throw InputValidatorExceptions::OUT_OF_BOUNDS_MAX;
				}

				//check min
				if (
					CheckOutOfBoundsErrorMin
					&& !(AllowMinEquality ? (*In >= MinValue) : (*In > MinValue))
					)
				{
					throw InputValidatorExceptions::OUT_OF_BOUNDS_MIN;
				}

				return *In;
			}
			//handle the exceptions
			catch (const InputValidatorExceptions e)
			{
				switch (e)
				{
					//handle type conversion error
					case InputValidatorExceptions::TYPE_CONVERSION:
						{
							std::wcout << (PrintTypeConversionErrorMsg ? (TypeConversionErrorMsg) : (L"")) << std::endl
								<< (ReprintInputMessageOnTypeConversionError ? InputMessage : L"") << std::endl;
							break;
						}
						//handle oob max
					case InputValidatorExceptions::OUT_OF_BOUNDS_MAX:
						{
							std::wcout << (PrintOutOfBoundsErrorMaxMsg ? (OutOfBoundsErrorMaxMsg) : (L"")) << std::endl
								<< (ReprintInputMessageOnOutOfBoundsErrorMax ? InputMessage : L"") << std::endl;
							break;
						}
						//handle oob min
					case InputValidatorExceptions::OUT_OF_BOUNDS_MIN:
						{
							std::wcout << (PrintOutOfBoundsErrorMinMsg ? (OutOfBoundsErrorMinMsg) : (L"")) << std::endl
								<< (ReprintInputMessageOnOutOfBoundsErrorMin ? InputMessage : L"") << std::endl;
							break;
						}
						//handle zero exception, priority over min / max
					case InputValidatorExceptions::OUT_OF_BOUNDS_ZERO:
						{
							std::wcout << (PrintOutOfBoundsErrorZeroMsg ? (OutOfBoundsErrorZeroMsg) : (L"")) << std::endl
								<< (ReprintInputMessageOnOutOfBoundsErrorZero ? InputMessage : L"") << std::endl;
							break;
						}
						//these exceptions are not used
					case InputValidatorExceptions::OUT_OF_LIST_B:
					case InputValidatorExceptions::OUT_OF_LIST_W:
					case InputValidatorExceptions::CUSTOM:
					default:
						{
							assert (false);
							break;
						}
				}
				continue;
			}
		}

	};
}




/*
switch (e) {
					case InputValidatorExceptions::TYPE_CONVERSION:
					{
						break;
					}
					case InputValidatorExceptions::OUT_OF_BOUNDS_MAX:
					{
						break;
					}
					case InputValidatorExceptions::OUT_OF_BOUNDS_MIN:
					{
						break;
					}
					case InputValidatorExceptions::OUT_OF_BOUNDS_ZERO:
					{
						break;
					}
					case InputValidatorExceptions::OUT_OF_LIST_B:
					{
						break;
					}
					case InputValidatorExceptions::OUT_OF_LIST_W:
					{
						break;
					}
					case InputValidatorExceptions::CUSTOM:
					{
						break;
					}
					default:
					{
						break;
					}
				}
*/