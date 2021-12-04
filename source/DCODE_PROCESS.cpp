#include "DCODE_PROCESS.h"
#include <iostream>
#include <fstream>
#include <string>

string*		DCODE_PROCESS	::	code = NULL;

long int	DCODE_PROCESS	::	size_stack = 0;

long int	DCODE_PROCESS	::	number_instructions = 0;

_register*  DCODE_PROCESS	::	memory_global_const = NULL;
_register*  DCODE_PROCESS	::	memory_global_variable = NULL;

long int	DCODE_PROCESS	::	number_global_const = 0;
long int	DCODE_PROCESS	::	number_global_variable = 0;

_arguments* DCODE_PROCESS	::	table_arguments = NULL;

string		DCODE_PROCESS	::	name_program =	"";

string		DCODE_PROCESS	::	source_code =	"";

bool		DCODE_PROCESS	::	first_load = false;

DCODE_PROCESS::DCODE_PROCESS(
	string _index_function,
	string _filename_code)
{
	index_function = _index_function;
	source_code = _filename_code;
	current_instruction = 0;
	top_stack = 0;

	string buff = "";

	fstream code_file(source_code);
		
	if (! code_file.is_open())
	{
		return_data = report(
			return_data, 
			1, 
			"err:CodeFile:IsNotFound");
	}
	else
	{
		while (code_file.eof() != true)
		{
			code_file >> buff;
			number_instructions++;
		}

		code_file.close();

		code = new string[number_instructions];

		code_file.open(source_code);

		long int loop = 0;
		
		while (loop < number_instructions)
		{
			code_file >> buff;
			code[loop] = buff;
			loop++;
		}
		
		code_file.close();
	}
}

_register DCODE_PROCESS::load_objects(
	_register *container,
	string _class_data,
	long int _number_registers
	)
{
	string buff = "";
	char index = '0';

	for (int loop = 0;
		loop < _number_registers;
		loop++)
	{
		container[loop].class_data = _class_data;
		container[loop].index_register = code[++current_instruction];
		container[loop].type_data = code[++current_instruction];

		if (container[loop].type_data == "~null")
		{
			container[loop].value_register = "null";
		}

		if (((index = container[loop].index_register[0]) == '$') & (table_arguments != nullptr))
		{
			for (int loop_arguments = 0; loop_arguments < number_arguments; loop_arguments++)
			{
				if ((table_arguments[loop_arguments].index == container[loop].index_register) & (container[loop].class_data != "const"))
				{
					container[loop].value_register = table_arguments[loop_arguments].argument.value_register;
					table_arguments[loop_arguments].is_used = true;
				}
			}
		}

		if (_class_data == "const")
		{
			if (container[loop].type_data == "~integer")
			{
				buff = code[++current_instruction];

				if (IsStringInt(buff) == true)
				{
					container[loop].value_register = buff;
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:Type:WrongData");
				}
			}
			else if (container[loop].type_data == "~float")
			{
				buff = code[++current_instruction];

				if (IsStringFloat(buff) == true)
				{
					container[loop].value_register = buff;
				}
				else
				{
					return return_data = report(
						return_data, 
						1,
						"err:Type:WrongData");
				}
			}
			else if (container[loop].type_data == "~bool")
			{
				buff = code[++current_instruction];

				if (IsStringBool(buff) == true)
				{
					container[loop].value_register = buff;
				}
				else
				{
					return return_data = report(
						return_data,
						1,
						"err:Type:WrongData");
				}
			}
			else if (container[loop].type_data == "~string")
			{
				container[loop].value_register = "";

				current_instruction++;

				string boarder_string_line = "";

				boarder_string_line = code[++current_instruction];

				while (boarder_string_line != "\"")
				{
					container[loop].value_register += boarder_string_line + " ";
					boarder_string_line = code[++current_instruction];
				}	
			}
			else
			{
				return return_data = report(
					return_data,
					1,
					"err:Type:IsNotDetermited");
			}
		}
	}

	if ((index == '$') & (table_arguments != nullptr))
	{
		for (int loop_arguments = 0; loop_arguments < number_arguments; loop_arguments++)
		{
			if (table_arguments[loop_arguments].is_used == false)
			{
				return return_data = report(
					return_data,
					1,
					"err:Function:SomeArgumentsAreNotExist");
			}
		}
	}

	return return_data = report(
		return_data,
		0,
		"suc:Registers:Loaded");
}

_register DCODE_PROCESS::start()
{
	if (return_data.class_data == "error")
	{
		return return_data;
	}

	string buff = "";

	buff = code[current_instruction = 0];
				 
	if (buff != "program_delta")
	{
		return return_data = report(
			return_data, 
			1,
			"err:CodeFile:IsNotDCode");
	}
	else
	{
		if (first_load == false)
		{
			while ((buff != "section:manifest") 
				& (buff != "section:global_const")
				& (buff != "section:global_variable")
				& (buff[0] != ':') 
				& (current_instruction < number_instructions))
			{
				buff = code[current_instruction++];
			}

			if (current_instruction >= number_instructions)
			{
				return return_data = report(
					return_data,
					1,
					"err:EOFDCode");
			}

			if ((buff == "section:global_const")
				|| (buff == "section:global_variable") 
				|| (buff[0] == ':'))
			{
				return return_data = report(
					return_data,
					1,
					"err:SectionMatifest:IsNotFound");
			}

			buff = code[current_instruction];
			
			while ((buff != "*name")
				& (buff != "section:global_const")
				& (buff != "section:global_variable")
				& (buff[0] != ':') 
				& (current_instruction < number_instructions))
			{
				buff = code[current_instruction++];
			}

			if (current_instruction >= number_instructions)
			{
				return return_data = report(
					return_data,
					1,
					"err:EOFDCode");
			}

			if ((buff == "section:global_const")
				|| (buff == "section:global_variable") 
				|| (buff[0] == ':'))
			{
				return return_data = report(
					return_data,
					1,
					"err:SectionMatifest:IsNotFound");
			}

			if (buff == "*name")
			{
				buff = code[current_instruction++];

				if (buff != "*size_stack")
				{
					name_program = buff;
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:SectionManifest:NameProgramIsNotFound");
				}
			}

			while ((buff != "*size_stack")
				& (buff != "section:global_const")
				& (buff != "section:global_variable")
				& (buff[0] != ':') 
				& (current_instruction < number_instructions))
			{
				buff = code[current_instruction++];
			}

			if (current_instruction >= number_instructions)
			{
				return return_data = report(
					return_data,
					1,
					"err:EOFDCode");
			}

			if ((buff == "section:global_const")
				|| (buff == "section:global_variable") 
				|| (buff[0] == ':'))
			{
				return return_data = report(
					return_data,
					1,
					"err:SectionMatifest:IsNotFound");
			}

			if (buff == "*size_stack")
			{
				buff = code[current_instruction++];

				if (IsStringInt(buff) == true)
				{
					size_stack = StringToInt(buff);
				}
				else
				{
					return return_data = report(
						return_data, 
						1,
						"err:SectionMatifest:SizeStackIsFault");
				}

				if ((size_stack <= 0) || 
					(size_stack > 100000))
				{
					return return_data = report(
						return_data,
						1, 
						"err:SectionMatifest:SizeStackIsFault");
				}

			}

			while ((buff != "section:global_const")
				& (buff != "section:global_variable")
				& (buff[0] != ':')
				& (current_instruction < number_instructions))
			{
				buff = code[current_instruction++];
			}

			if (current_instruction >= number_instructions)
			{
				return return_data = report(
					return_data,
					1, 
					"err:EOFDCode");
			}

			if ((buff == "section:global_variable")
				|| (buff[0] == ':'))
			{
				return return_data = report(
					return_data,
					1, 
					"err:SectionGlobalConst:IsNotFound");
			}

			number_global_const = StringToInt(code[current_instruction]);

			if ((number_global_const < 0) ||
				(number_global_const > (size_stack / 2)))
			{
				return return_data = report(
					return_data,
					1, 
					"err:SectionGlobalConst:NumberConst:WrongData");
			}
			else
			{
				memory_global_const = new _register[number_global_const];

				load_objects(memory_global_const, "const", number_global_const);

				if (return_data.class_data == "error")
				{
					return return_data;
				}
			}
		
			while ((buff != "section:global_variable")
				& (buff[0] != ':')
				& (current_instruction < number_instructions))
			{
				buff = code[current_instruction++];
			}

			if (current_instruction >= number_instructions)
			{
				return return_data = report(
					return_data, 
					1, 
					"err:EOFDCode");
			}

			if (buff[0] == ':')
			{
				return return_data = report(
					return_data, 
					1, 
					"err:SectionGlobalVariable:IsNotFound");
			}

			number_global_variable = StringToInt(code[current_instruction]);

			if ((number_global_variable < 0) ||
				(number_global_variable > (size_stack / 2)))
			{
				return return_data = report(
					return_data, 
					1, 
					"err:SectionGlobalVariable:NumberVariableLessNull");
			}
			else
			{
				memory_global_variable = new _register[number_global_variable];

				load_objects(memory_global_variable, "variable", number_global_variable);

				if (return_data.class_data == "error")
				{
					return return_data;
				}
			}

			table_arguments = nullptr;
			first_load = true;
		}

		stack = new _register[size_stack];

		while ((buff != index_function)
			& (current_instruction < number_instructions))
		{
			buff = code[current_instruction++];
		}
	
		if (current_instruction >= number_instructions)
		{
			return return_data = report(
				return_data, 
				1,
				"err:Function:IsNotFound");
		}
		
		type_return_data = code[current_instruction];

		buff = number_arguments = StringToInt(code[++current_instruction]);
		
		while ((buff != "section:const") 
			& (buff != "section:variable")
			& (buff[0] != ':')
			& (current_instruction < number_instructions))
		{
			buff = code[current_instruction++];
		}

		if (current_instruction >= number_instructions)
		{
			return return_data = report(return_data, 1, "err:EOFDCode");
		}

		if ((buff == "section:variable") ||
			(buff[0] == ':'))
		{
			return return_data = report(
				return_data,
				1, 
				"err:SectionConst:IsNotFound");
		}

		number_const = StringToInt(code[current_instruction]);

		if ((number_const < 0) ||
			(number_const > (size_stack / 2)))
		{
			return return_data = report(
				return_data,
				1,
				"err:SectionConst:NumberConst:WrongData");
		}
		else
		{
			memory_const = new _register[number_const];

			load_objects(memory_const, "const", number_const);

			if (return_data.class_data == "error")
			{
				return return_data;
			}
		}

		while ((buff != "section:variable") 
			& (buff[0] != ':')
			& (current_instruction <= number_instructions))
		{
			buff = code[current_instruction++];
		}

		if (current_instruction >= number_instructions)
		{
			return return_data = report(
				return_data,
				1,
				"err:EOFDCode");
		}

		if (buff[0] == ':')
		{
			return return_data = report(
				return_data, 
				1, 
				"err:SectionVariable:IsNotFound");
		}

		number_variable = StringToInt(code[current_instruction]);

		if ((number_variable < 0) ||
			(number_variable > (size_stack / 2)))
		{
			return return_data = report(
				return_data, 
				1, 
				"err:SectionVariable:NumberVariableLessNull");
		}
		else
		{
			memory_variable = new _register[number_variable];

			load_objects(memory_variable, "variable", number_variable);

			if (return_data.class_data == "error")
			{
				return return_data;
			}
		}

		while ((buff != "section:code") 
			& (buff[0] != ':')
			& (current_instruction < number_instructions))
		{
			buff = code[current_instruction++];
		}

		if (current_instruction >= number_instructions)
		{
			return return_data = report(
				return_data, 
				1, 
				"err:EOFDCode");
		}

		if (buff[0] == ':')
		{
			return return_data = report(
				return_data,
				1,
				"err:SectionCode:IsNotFound");
		}

		for (current_instruction; 
			current_instruction < number_instructions;
			current_instruction++)
		{
			buff = code[current_instruction];

			if (buff == "push")
			{
				buff = code[++current_instruction];

				if (find_object(buff).class_data == "error")
				{
					return return_data = report(
						return_data,
						1,
						 "err:PushInstruction:ObjectIsNotFound >> push " + buff);
				}
				else
				{
					if (((return_data = push(find_object(buff))).class_data) == "error")
					{
						return return_data;
					}
				}
			}

			if (buff == "pop")
			{
				buff = code[++current_instruction];

				if (find_object(buff).class_data == "error")
				{
					return return_data = report(
						return_data,
						1,
						"err:PopInstruction:ObjectIsNotFound >> pop " + buff);
				}
				else
				{
					_register register_temp = pop();

					if (((return_data = register_temp).class_data) == "error")
					{
						return return_data;
					}

					if (find_object(buff).class_data == "const")
					{
						return return_data = report(
							return_data,
							1,
							"err:PopInstruction:TryingToModifyConst >> pop " + buff);
					}

					if (find_object(buff).type_data != register_temp.type_data)
					{
						return return_data = report(
							return_data,
							1,
							"err:PopInstruction:ObjectsIsNotEqual >> pop " + find_object(buff).type_data + " <- " + register_temp.type_data);
					}

					find_object(buff).value_register = register_temp.value_register;
				}
			}

			if (buff == "add")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data, 
						1,
						"err:AddInstruction:WrongArgs");
				}

				if (first_operand.type_data == "~integer" 
					& second_operand.type_data == "~integer")
				{
					if (((return_data = push(add_i(first_operand, second_operand))).class_data) == "error")
					{
						return return_data;
					}
				}
				else if (first_operand.type_data == "~float"
					& second_operand.type_data == "~float")
				{
					if ((return_data = push(add_f(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else if (first_operand.type_data == "~string"
					& second_operand.type_data == "~string")
				{
					if ((return_data = push(add_s(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:AddInstruction:WrongArgs");
				}
			}

			if (buff == "sub")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:SubInstruction:WrongArgs");
				}

				if (first_operand.type_data == "~integer" 
					& second_operand.type_data == "~integer")
				{
					if ((return_data = push(sub_i(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else if (first_operand.type_data == "~float"
					& second_operand.type_data == "~float")
				{
					if ((return_data = push(sub_f(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else
				{
					return return_data = report(
						return_data,
						1,
						"err:SubInstruction:WrongArgs");
				}
			}

			if (buff == "div")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") || 
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:DivInstruction:WrongArgs");
				}

				if (first_operand.type_data == "~integer" 
					& second_operand.type_data == "~integer")
				{
					if ((return_data = push(div_i(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else if (first_operand.type_data == "~float" 
					& second_operand.type_data == "~float")
				{
					if ((return_data = push(div_f(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else
				{
					return return_data = report(
						return_data,
						1,
						"err:DivInstruction:WrongArgs");
				}
			}

			if (buff == "mul")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1, 
						"err:MulInstruction:WrongArgs");
				}

				if (first_operand.type_data == "~integer" 
					& second_operand.type_data == "~integer")
				{
					if ((return_data = push(mul_i(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else if (first_operand.type_data == "~float" 
					& second_operand.type_data == "~float")
				{
					if ((return_data = push(mul_f(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:MulInstruction:WrongArgs");
				}
			}

			if (buff == "mod")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:ModInstruction:WrongArgs");
				}

				if (first_operand.type_data == "~integer"
					& second_operand.type_data == "~integer")
				{
					if ((return_data = push(mod(first_operand, second_operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else
				{
					return return_data = report(
						return_data,
						1,
						"err:ModInstruction:WrongArgs");
				}
			}

			if (buff == "inc")
			{
				_register operand = pop();

				if (operand.class_data == "error")
				{
					return return_data = report(
						return_data, 
						1, 
						"err:IncInstruction:WrongArg");
				}

				if (operand.type_data == "~integer")
				{
					if ((return_data = push(inc(operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:IncInstruction:WrongArg");
				}
			}

			if (buff == "dec")
			{
				_register operand = pop();

				if ((operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:DecInstruction:WrongArg");
				}

				if (operand.type_data == "~integer")
				{
					if ((return_data = push(dec(operand))).class_data == "error")
					{
						return return_data;
					}
				}
				else
				{
					return return_data = report(
						return_data,
						1,
						"err:DecInstruction:WrongArg");
				}
			}

			if (buff == "out")
			{
				out(pop());
			}

			if (buff == "out_nl")
			{
				out_nl();
			}

			if (buff == "inp")
			{
				if ((return_data = push(inp())).class_data == "error")
				{
					return return_data;
				}
			}

			if (buff == "jmp")
			{
				buff = code[++current_instruction];

				jmp(buff);
			}

			if (buff == "jmp_e")
			{
				buff = code[++current_instruction];

				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_EInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data &
					second_operand.type_data != "~null" &
					first_operand.type_data != "~null")
				{
					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register == first_operand.value_register)
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) == StringToInt(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) == StringToFloat(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) == StringToBool(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else 
					{
						return return_data = report(
							return_data, 
							1, 
							"err:Jmp_EInstruction:WrongArgs");
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:Jmp_EInstruction:WrongArgs");
				}
			}

			if (buff == "jmp_ne")
			{
				buff = code[++current_instruction];

				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_NEInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data &
					second_operand.type_data != "~null" &
					first_operand.type_data != "~null")
				{
					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register != first_operand.value_register)
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) != StringToInt(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) != StringToFloat(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) != StringToBool(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else 
					{
						return return_data = report(
							return_data, 
							1, 
							"err:Jmp_NEInstruction:WrongArgs");
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:Jmp_NEInstruction:WrongArgs");
				}
			}

			if (buff == "jmp_l")
			{
				buff = code[++current_instruction];

				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_LInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data &
					second_operand.type_data != "~null" &
					first_operand.type_data != "~null")
				{
					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register > first_operand.value_register)
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) > StringToInt(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) > StringToFloat(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) > StringToBool(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else 
					{
						return return_data = report(
							return_data, 
							1, 
							"err:Jmp_LInstruction:WrongArgs");
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:Jmp_LInstruction:WrongArgs");
				}
			}

			if (buff == "jmp_nl")
			{
				buff = code[++current_instruction];

				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_NLInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data &
					second_operand.type_data != "~null" &
					first_operand.type_data != "~null")
				{
					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register <= first_operand.value_register)
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) <= StringToInt(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) <= StringToFloat(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) <= StringToBool(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else 
					{
						return return_data = report(
							return_data, 
							1, 
							"err:Jmp_NLInstruction:WrongArgs");
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:Jmp_NLInstruction:WrongArgs");
				}
			}

			if (buff == "jmp_g")
			{
				buff = code[++current_instruction];

				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_GInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data &
					second_operand.type_data != "~null" &
					first_operand.type_data != "~null")
				{
					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register < first_operand.value_register)
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) < StringToInt(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) < StringToFloat(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) < StringToBool(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else 
					{
						return return_data = report(
							return_data, 
							1, 
							"err:Jmp_GInstruction:WrongArgs");
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:Jmp_GInstruction:WrongArgs");
				}
			}

			if (buff == "jmp_ng")
			{
				buff = code[++current_instruction];

				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_NGInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data &
					second_operand.type_data != "~null" &
					first_operand.type_data != "~null")
				{
					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register >= first_operand.value_register)
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) >= StringToInt(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) >= StringToFloat(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) >= StringToBool(first_operand.value_register))
						{
							jmp(buff);
						}
					}
					else 
					{
						return return_data = report(
							return_data, 
							1, 
							"err:Jmp_NGInstruction:WrongArgs");
					}
				}
				else
				{
					return return_data = report(
						return_data, 
						1, 
						"err:Jmp_NGInstruction:WrongArgs");
				}
			}

			if (buff == "less")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:LessInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data)
				{		
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register < first_operand.value_register)
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) < StringToInt(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) < StringToFloat(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) < StringToBool(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:LessInstruction:WrongArgs");
				}
			}
			if (buff == "greater")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:GreaterInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data)
				{		
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register > first_operand.value_register)
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) > StringToInt(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) > StringToFloat(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) > StringToBool(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:GreaterInstruction:WrongArgs");
				}
			}
			if (buff == "equal")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:EqualInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data)
				{		
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register == first_operand.value_register)
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) == StringToInt(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) == StringToFloat(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) == StringToBool(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:EqualInstruction:WrongArgs");
				}
			}
			if (buff == "n_less")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:NLessInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data)
				{		
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register >= first_operand.value_register)
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) >= StringToInt(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) >= StringToFloat(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) >= StringToBool(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:NLessInstruction:WrongArgs");
				}
			}
			if (buff == "n_greater")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:NGreaterInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data)
				{		
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register <= first_operand.value_register)
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) <= StringToInt(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) <= StringToFloat(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) <= StringToBool(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:NGreaterInstruction:WrongArgs");
				}
			}

			if (buff == "n_equal")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:NEqualInstruction:WrongArgs");
				}

				if (second_operand.type_data == first_operand.type_data)
				{		
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (second_operand.type_data == "~string")
					{
						if (second_operand.value_register != first_operand.value_register)
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~integer")
					{
						if (StringToInt(second_operand.value_register) != StringToInt(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~float")
					{
						if (StringToFloat(second_operand.value_register) != StringToFloat(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					if (second_operand.type_data == "~bool")
					{
						if (StringToBool(second_operand.value_register) != StringToBool(first_operand.value_register))
						{
							result_operation.value_register = "1";
						}
						else
						{
							result_operation.value_register = "0";
						}
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:NEqualInstruction:WrongArgs");
				}
			}
			if (buff == "jmp_t")
			{
				buff = code[++current_instruction];

				_register operand = pop();

				if (operand.class_data == "error")
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_TInstruction:WrongArg");
				}

				if (operand.type_data == "~bool")
				{
					if (StringToBool(operand.value_register) == 1)
					{
						jmp(buff);
					}
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:Jmp_TInstruction:WrongArg");
				}
			}

			if (buff == "jmp_f")
			{
				buff = code[++current_instruction];

				_register operand = pop();

				if (operand.class_data == "error")
				{
					return return_data = report(
						return_data,
						1,
						"err:Jmp_FInstruction:WrongArg");
				}

				if (operand.type_data == "~bool")
				{
					if (StringToBool(operand.value_register) == 0)
					{
						jmp(buff);
					}
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:Jmp_FInstruction:WrongArg");
				}
			}

			if (buff == "and")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:AndInstruction:WrongArgs");
				}

				if ((second_operand.type_data == "~bool")
					& (first_operand.type_data == "~bool"))
				{
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if ((StringToBool(first_operand.value_register) == true)
						& (StringToBool(second_operand.value_register) == true))
					{
						result_operation.value_register = "1";
					}
					else
					{
						result_operation.value_register = "0";
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:AndInstruction:WrongArgs");
				}
			}

			if (buff == "xor")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:XorInstruction:WrongArgs");
				}

				if ((second_operand.type_data == "~bool")
					& (first_operand.type_data == "~bool"))
				{
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (((StringToBool(first_operand.value_register) == true)
						& (StringToBool(second_operand.value_register) == false)) ||
						((StringToBool(first_operand.value_register) == false) 
						& (StringToBool(second_operand.value_register) == true)))
					{
						result_operation.value_register = "1";
					}
					else
					{
						result_operation.value_register = "0";
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:XorInstruction:WrongArgs");
				}
			}

			if (buff == "or")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:OrInstruction:WrongArgs");
				}

				if ((second_operand.type_data == "~bool")
					& (first_operand.type_data == "~bool"))
				{
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if ((StringToBool(first_operand.value_register) == true) ||
						(StringToBool(second_operand.value_register) == true))
					{
						result_operation.value_register = "1";
					}
					else
					{
						result_operation.value_register = "0";
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:OrInstruction:WrongArgs");
				}
			}

			if (buff == "not")
			{
				_register operand = pop();

				if (operand.class_data == "error")
				{
					return return_data = report(
						return_data,
						1,
						"err:NotInstruction:WrongArgs");
				}

				if (operand.type_data == "~bool")
				{
					_register result_operation;
					result_operation.class_data = "variable";
					result_operation.index_register = "";
					result_operation.type_data = "~bool";

					if (StringToBool(operand.value_register) == true)
					{
						result_operation.value_register = "0";
					}
					else
					{
						result_operation.value_register = "1";
					}

					push(result_operation);
				}
				else
				{
					return return_data = report(
						return_data,
						1, 
						"err:NotInstruction:WrongArgs");
				}
			}

			if (buff == "neg")
			{
				_register operand = pop();

				if (operand.type_data != "~integer")
				{
					operand.value_register = IntToString(StringToInt(operand.value_register) * -1);
				}
				else if (operand.type_data != "~float")
				{
					operand.value_register = FloatToString(StringToFloat(operand.value_register) * -1.0);
				}
				else if (operand.type_data != "~bool")
				{
					operand.value_register = BoolToString(! StringToBool(operand.value_register));
				}
				else
				{
					return return_data = report(return_data, 1, "err:NegInstruction:WrongArg");
				}

				push(operand);
			}

			if (buff == "drop")
			{
				_register operand = pop();

				if (operand.class_data == "error")
				{
					return return_data = report(
						return_data,
						1,
						"err:DropInstruction:WrongArg");
				}
			}

			if (buff == "dup")
			{
				_register operand = pop();

				if (operand.class_data == "error")
				{
					return return_data = report(
						return_data,
						1,
						"err:DupInstruction:WrongArg");
				}

				push(operand);

				if ((return_data = push(operand)).class_data == "error")
					{
						return return_data;
					}
			}

			if (buff == "swap")
			{
				_register second_operand = pop();
				_register first_operand = pop();

				if ((first_operand.class_data == "error") ||
					(second_operand.class_data == "error"))
				{
					return return_data = report(
						return_data,
						1,
						"err:SwapInstruction:WrongArgs");
				}

				push(second_operand);
				push(first_operand);
			}

			if (buff == "int<")
			{
				_register operand = pop();

				if (operand.type_data == "~string")
				{
					if (IsStringInt(operand.value_register) == true)
					{
						operand.type_data = "~integer";
					}
				}

				if (operand.type_data == "~float")
				{
					operand.value_register = IntToString(FloatToInt(StringToFloat(operand.value_register)));
					operand.type_data = "~integer";
				}

				if (operand.type_data == "~bool")
				{
					operand.value_register = IntToString(BoolToInt(StringToBool(operand.value_register)));
					operand.type_data = "~bool";
				}

				push(operand);
			}

			if (buff == "float<")
			{
				_register operand = pop();

				if (operand.type_data == "~string")
				{
					if (IsStringFloat(operand.value_register) == true)
					{
						operand.type_data = "~float";
					}
				}

				if (operand.type_data == "~integer")
				{
					operand.value_register = FloatToString(IntToFloat(StringToInt(operand.value_register)));
					operand.type_data = "~float";
				}

				if (operand.type_data == "~bool")
				{
					operand.value_register = FloatToString(BoolToFloat(StringToBool(operand.value_register)));
					operand.type_data = "~float";
				}

				push(operand);
			}

			if (buff == "bool<")
			{
				_register operand = pop();

				if (operand.type_data == "~string")
				{
					if (IsStringBool(operand.value_register) == true)
					{
						operand.type_data = "~bool";
					}
				}

				if (operand.type_data == "~float")
				{
					operand.value_register = BoolToString(FloatToBool(StringToFloat(operand.value_register)));
					operand.type_data = "~bool";
				}

				if (operand.type_data == "~integer")
				{
					operand.value_register = BoolToString(IntToBool(StringToInt(operand.value_register)));
					operand.type_data = "~bool";
				}

				push(operand);
			}

			if (buff == "str<")
			{
				_register operand = pop();

				operand.type_data = "~string";

				push(operand);
			}

			if (buff == "call")
			{
				buff = code[++current_instruction];

				long int current_instruction_call = 0;

				while ((code[current_instruction_call++] != (":" + buff))
					& (current_instruction_call < number_instructions))
				{
				}

				if (current_instruction_call >= number_instructions)
				{
					return return_data = report(
						return_data, 
						1,
						"err:CallInstruction:FunctionIsNotFound -> :" + buff);
				}
		
				number_arguments = StringToInt(code[++current_instruction_call]);

				if (number_arguments > 0)
				{
					table_arguments = new _arguments[number_arguments];

					for (int loop = 0; 
						loop < number_arguments; 
						loop++)
					{
						table_arguments[loop].index = code[++current_instruction_call];

						if ((table_arguments[loop].argument = pop()).class_data == "error")
						{
							return return_data = table_arguments[loop].argument;
						}

						table_arguments[loop].is_used = false;
					}
				}
				else
				{
					table_arguments = nullptr;
				}

				DCODE_PROCESS process((":" + buff), source_code);

				_register returnX = process.start();

				if (returnX.class_data == "error")
				{
					return return_data = returnX;
				}

				if (returnX.type_data != "~null")
				{
					push(returnX);
				}
			}

			if (buff == "ret")
			{
				if (type_return_data == "~null")
				{
					return_data.index_register = "";
					return_data.type_data = "~null";
					return_data.value_register = "~null";

					return return_data;
				}
				else
				{
					_register result = pop();
	
					if (result.class_data != "error")
					{
						return_data.class_data = "success";
					}
					else
					{
						return return_data = result;
					}

					if (result.type_data != type_return_data)
					{
						return return_data = report(
							return_data, 
							1,
							"err:RetInstruction:TypeReturnDataIsFault");
					}

					{
						return_data.index_register = "";
						return_data.type_data = result.type_data;
						return_data.value_register = result.value_register;
					}

					return return_data;
				}
			}
		}

		return return_data = report(
			return_data,
			1, 
			"err:Function:EmptyRequest");
	}
}

_register DCODE_PROCESS::pop()
{
	if (top_stack > 0)
	{
		return stack[--top_stack];
	}
	else
	{
		return return_data = report(
			return_data,
			1,
			"err:Stack:IsEmpty");
	}
}

_register DCODE_PROCESS::push(
	_register _data)
{
	if (_data.class_data == "error")
	{
		return _data;
	}
	else
	{
		if (top_stack < size_stack)
		{
			stack[top_stack++] = _data;
	
			return return_data = report(
				return_data,
				0,
				"suc:Stack:DataIsPushed");
		}
		else
		{
			return return_data = report(
				return_data, 
				1, 
				"err:Stack:OverFlow");
		}
	}
}

_register DCODE_PROCESS::add_i(
	_register _first_object, 
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = IntToString(StringToInt(_first_object.value_register) 
										+ StringToInt(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::add_f(
	_register _first_object,
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = FloatToString(StringToFloat(_first_object.value_register) 
										+ StringToFloat(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::add_s(
	_register _first_object,
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = _first_object.value_register
							+ _second_object.value_register;
	return result;
}

_register DCODE_PROCESS::sub_i(
	_register _first_object,
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = IntToString(StringToInt(_first_object.value_register)
										- StringToInt(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::sub_f(
	_register _first_object,
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = FloatToString(StringToFloat(_first_object.value_register) 
										- StringToFloat(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::div_i(
	_register _first_object, 
	_register _second_object)
{
	_register result(_first_object);

	if (StringToInt(_second_object.value_register) == 0)
	{
		return return_data = report(
			return_data,
			1, 
			"err:DivInstruction:DivisionOnZero");
	}

	result.value_register = IntToString(StringToInt(_first_object.value_register) 
										/ StringToInt(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::div_f(
	_register _first_object,
	_register _second_object)
{
	_register result(_first_object);

	if (StringToFloat(_second_object.value_register) == 0.0)
	{
		return return_data = report(
			return_data,
			1, 
			"err:DivInstruction:DivisionOnZero");
	}

	result.value_register = FloatToString(StringToFloat(_first_object.value_register) 
										/ StringToFloat(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::mul_i(
	_register _first_object, 
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = IntToString(StringToInt(_first_object.value_register) 
										* StringToInt(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::mul_f(
	_register _first_object, 
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = FloatToString(StringToFloat(_first_object.value_register) 
										* StringToFloat(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::mod(
	_register _first_object,
	_register _second_object)
{
	_register result(_first_object);
	result.value_register = IntToString(StringToInt(_first_object.value_register) 
										% StringToInt(_second_object.value_register));
	return result;
}

_register DCODE_PROCESS::inc(
	_register _object)
{
	_register result(_object);
	result.value_register = IntToString(StringToInt(_object.value_register) + 1);
	return result;
}

_register DCODE_PROCESS::dec(
	_register _object)
{
	_register result(_object);
	result.value_register = IntToString(StringToInt(_object.value_register) - 1);
	return result;
}

void DCODE_PROCESS::nop()
{
}

void DCODE_PROCESS::jmp(
	string _label)
{
	for (current_instruction = 1; 
		current_instruction < number_instructions; 
		current_instruction++)
	{
		if (code[current_instruction] == index_function)
		{
			break;
		}
	}

	for (current_instruction; 
		current_instruction < number_instructions;
		current_instruction++)
	{
		if ((code[current_instruction] == "^" + _label)
			& (code[current_instruction - 1] != "jmp") 
			& (code[current_instruction - 1] != "jmp_e")
			& (code[current_instruction - 1] != "jmp_ne") 
			& (code[current_instruction - 1] != "jmp_l")
			& (code[current_instruction - 1] != "jmp_nl") 
			& (code[current_instruction - 1] != "jmp_g") 
			& (code[current_instruction - 1] != "jmp_ng"))
		{
			break;
		}
	}
}

void DCODE_PROCESS::out(
	_register _object)
{
	if (_object.type_data == "~string")
	{
		cout << _object.value_register;
	}

	if (_object.type_data == "~integer")
	{
		cout << StringToInt(_object.value_register);
	}

	if (_object.type_data == "~float")
	{
		cout << StringToFloat(_object.value_register);
	}

	if (_object.type_data == "~bool")
	{
		cout << StringToBool(_object.value_register);
	}

	if (_object.type_data == "~null")
	{
		cout << "~null";
	}
}

void DCODE_PROCESS::out_nl()
{
	cout << endl;
}

_register DCODE_PROCESS::inp()
{
	_register output_register;

	output_register.class_data = "";
	output_register.index_register = "";
	output_register.type_data = "~string";

	cin >> output_register.value_register;

	return output_register;
}

_register& DCODE_PROCESS::report(
	_register &_back_result, 
	bool _error, 
	string _class_report)
{
	if (_error == true)
	{
		_back_result.class_data = "error";
	}
	else
	{
		_back_result.class_data = "success";
	}
	_back_result.index_register = "~null";
	_back_result.type_data = "~null";
	_back_result.value_register = _class_report;

	return _back_result;
}

_register& DCODE_PROCESS::find_object(
	string _adress)
{
	int loop = 0;
	bool found = false;

	switch (_adress[0])
	{
		case '@':
		{
			for (loop = 0;
				loop < number_const; 
				loop++)
			{
				if (memory_const[loop].index_register == _adress)
				{
					found = true;
					break;
				}
			}
			if (found == true)
			{
				return memory_const[loop];
			}
			else
			{
				return return_data = report(
					return_data,
					1, 
					"err:Const:IsNotFound");
			}
			break;
		}
		case '$':
		{
			for (loop = 0;
				loop < number_variable;
				loop++)
			{
				if (memory_variable[loop].index_register == _adress)
				{
					found = true;
					break;
				}
			}
			if (found == true)
			{
				return memory_variable[loop];
			}
			else
			{
				return return_data = report(
					return_data, 
					1,
					"err:Variable:IsNotFound");
			}
			break;
		}
		case 'g':
		{
			switch (_adress[1])
			{
				case '@':
				{
					for (loop = 0;
						loop < number_global_const;
						loop++)
					{
						if (memory_global_const[loop].index_register == _adress)
						{
							found = true;
							break;
						}
					}
					if (found == true)
					{
						return memory_global_const[loop];
					}
					else
					{
						return return_data = report(
							return_data, 
							1, 
							"err:GlobalConst:IsNotFound");
					}
					break;
				}
				case '$':
				{
					for (loop = 0;
						loop < number_global_variable;
						loop++)
					{
						if (memory_global_variable[loop].index_register == _adress)
						{
							found = true;
							break;
						}
					}
					if (found == true)
					{
						return memory_global_variable[loop];
					}
					else
					{
						return return_data = report(
							return_data,
							1,
							"err:GlobalVariable:IsNotFound");
					}
					break;
				}
				default:
				{
					return return_data = report(
						return_data,
						1, 
						"err:Register:WrongIndex");
					break;
				}
			}
		}
		default:
		{
			return return_data = report(
				return_data,
				1,
				"err:Register:WrongIndex");
			break;
		}
	}
}

int DCODE_PROCESS::StringToInt(
	string _data)
{
	int minus = 0;

	if (_data[0] == '-')
	{
		minus = 1;
	}

	int loop_size = 0;

	for (loop_size = 0; 
		(_data[loop_size] != '\0') 
		& (loop_size <= 8);
		loop_size++)
	{
	}

	int result = 0;
	char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	for (int loop = minus;
		loop < loop_size;
		loop++)
	{
		bool access = false;
		int rank = 1;

		for (int loop_rank = loop_size - 1; 
			loop_rank > loop; 
			loop_rank--)
		{
			rank *= 10;
		}

		for (int loop_x = 0;
			(loop_x < 10)
			& (access == false); 
			loop_x++)
		{
			if (_data[loop] == digits[loop_x])
			{
				result += (digits[loop_x] - '0') * rank;
				access = true;
			}
		}
		
		if (access == false)
		{
			return -42;
		}
	}

	if( minus == 1)
	{
		result *= -1;
	}

	return result;
}

bool DCODE_PROCESS::StringToBool(
	string _data)
{
	bool result = 0;
	bool access = false;

	if (_data == "0" || 
		_data == "false")
	{
		result = false;
		access = true;
	}

	if (_data == "1" || 
		_data == "true")
	{
		result = true;
		access = true;
	}
	if (access == false)
	{
		return false;
	}

	return result;
}

float DCODE_PROCESS::StringToFloat(
	string _data)
{
	int minus = 0;

	if (_data[0] == '-')
	{
		minus = 1;
	}

	int loop_size_int_part = 0;
	int loop_size_float_part = 0;

	for (loop_size_int_part = 0; 
		(_data[loop_size_int_part] != '\0') 
		& (loop_size_int_part + loop_size_float_part < 10) 
		& (_data[loop_size_int_part] != '.');
		loop_size_int_part++)
	{
	}

	if (_data[loop_size_int_part] != '.')
	{
		_data += ".0";
	}

	for (loop_size_float_part = 0; 
		(_data[loop_size_int_part + loop_size_float_part + 1] != '\0') 
		& (loop_size_int_part + loop_size_float_part <= 8);
		loop_size_float_part++)
	{
	}

	float result = 0;
	char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	bool dot_flag = false;

	for (int loop = minus; 
		loop < (loop_size_int_part + loop_size_float_part + 1); 
		loop++)
	{
		bool access = false;
		float rank = 1.0;

		if ((loop_size_int_part - 1 - loop) >= 0)
		{
			for (int loop_rank = loop_size_int_part - 1;
				loop_rank > loop; 
				loop_rank--)
			{
				rank *= 10.0;
			}
		}
		else
		{
			for (int loop_rank = loop_size_int_part; 
				loop_rank < loop; 
				loop_rank++)
			{
				rank *= 0.1;
			}
		}

		for (int loop_x = 0;
			(loop_x < 10)
			& (access == false); 
			loop_x++)
		{
			if (_data[loop] == digits[loop_x])
			{
				result += (digits[loop_x] - '0') * rank;
				access = true;
			}

			if ((_data[loop] == '.')
				& (dot_flag == false))
			{
				dot_flag = true;
				access = true;
			}
		}
		
		if (access == false)
		{
			return -42.0;
		}
	}

	if ( minus == 1)
	{
		result *= -1.0;
	}

	return result;
}

string DCODE_PROCESS::IntToString(
	int _data)
{
	string result = "";

    char digit;
	int integer = _data;

	if (integer < 0)
	{
		integer *= -1;
	}
    
	if (integer == 0)
	{
		result = "0";
	}
	else
	{
		while (integer > 0)
		{
		    digit = integer % 10 + '0';
			result = digit + result;
			integer /= 10;
		}
	}

	if (_data < 0)
	{
		result = "-" + result;
	}

	return result;
}

float DCODE_PROCESS::IntToFloat(
	int _data)
{
	return (float) _data;
}

bool DCODE_PROCESS::IntToBool(
	int _data)
{
	if (_data > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string DCODE_PROCESS::BoolToString(
	bool _data)
{
	if (_data == true ||
		_data == 1)
	{
		return "1";
	}
	else
	{
		return "0";
	}
}

float DCODE_PROCESS::BoolToFloat(
	bool _data)
{
		if (_data == true ||
		_data == 1)
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

int DCODE_PROCESS::BoolToInt(
	bool _data)
{
	if (_data == true ||
		_data == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

string DCODE_PROCESS::FloatToString(
	float _data)
{
	string result = "";
    char digit;
 
    int integer_part = _data;

	if (integer_part < 0)
	{
		integer_part *= -1;
	}
    
	if (integer_part == 0)
	{
		result = "0";
	}
	else
	{
		while (integer_part > 0)
		{
		    digit = integer_part % 10 + '0';
			result = digit + result;
		 integer_part /= 10;
		}
	}

    result = result + '.';



    float fract_part = _data - (int) _data;

	if (fract_part < 0)
	{
		fract_part *= -1.0;
	}

	int accuracy = 0;

    while (accuracy < 7)
	{
        fract_part *= 10;
        digit = (int) fract_part + '0';
        result = result + digit;
        fract_part = fract_part - (int) fract_part;
		accuracy++;
    }

	if (_data < 0)
	{
		result = "-" + result;
	}

	return result;
}

int DCODE_PROCESS::FloatToInt(
	float _data)
{
	return (int) _data;
}

bool DCODE_PROCESS::FloatToBool(
	float _data)
{
	if (_data > 0.0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DCODE_PROCESS::IsStringInt(
	string _data)
{
	int minus = 0;

	if (_data[0] == '-')
	{
		minus = 1;
	}

	int loop_size = 0;

	for (loop_size = 0; 
		(_data[loop_size] != '\0') 
		& (loop_size <= 8);
		loop_size++)
	{
	}

	char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	for (int loop = minus;
		loop < loop_size; 
		loop++)
	{
		bool access = false;
		int rank = 1;

		for (int loop_x = 0; 
			(loop_x < 10) 
			& (access == false); 
			loop_x++)
		{
			if (_data[loop] == digits[loop_x])
			{
				access = true;
			}
		}
		
		if (access == false)
		{
			return false;
		}
	}

	return true;
}

bool DCODE_PROCESS::IsStringBool(
	string _data)
{
	if (_data == "0" || 
		_data == "false" ||
		_data == "1" || 
		_data == "true")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DCODE_PROCESS::IsStringFloat(
	string _data)
{
	int minus = 0;

	if (_data[0] == '-')
	{
		minus = 1;
	}

	int loop_size_int_part = 0;
	int loop_size_float_part = 0;

	for (loop_size_int_part = 0; 
		(_data[loop_size_int_part] != '\0') 
		& (loop_size_int_part + loop_size_float_part < 10) 
		& (_data[loop_size_int_part] != '.');
		loop_size_int_part++)
	{
	}

	if (_data[loop_size_int_part] != '.')
	{
		_data += ".0";
	}

	for (loop_size_float_part = 0;
		(_data[loop_size_int_part + loop_size_float_part + 1] != '\0') 
		& (loop_size_int_part + loop_size_float_part <= 8); 
		loop_size_float_part++)
	{
	}

	char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	bool dot_flag = false;

	for (int loop = minus; 
		loop < (loop_size_int_part + loop_size_float_part + 1);
		loop++)
	{
		bool access = false;
		float rank = 1.0;

		for (int loop_x = 0; 
			(loop_x < 10) 
			& (access == false);
			loop_x++)
		{
			if (_data[loop] == digits[loop_x])
			{
				access = true;
			}

			if ((_data[loop] == '.') 
				& (dot_flag == false))
			{
				dot_flag = true;
				access = true;
			}
		}
		
		if (access == false)
		{
			return false;
		}
	}

	return true;
}

DCODE_PROCESS::~DCODE_PROCESS(
	void)
{
}