/* Basic binary and decimal conversion program in C. The program will prompt for input strings,
   convert them between binary and decimal, and validate the conversions.
   author: Reese Delaney
   date: 11/1/2026
    */

//Includes
//Standard input and output functions
#include <stdio.h>
//Include string functions
#include <string.h>
//Include stdlib functions
#include <stdlib.h>
//Include Ctype functions
#include <ctype.h>
//Include wchar.h for wide character functions
#include <wchar.h>
//Include Locale functions
#include <locale.h>
//Include stdint for fixed-width integers
#include <stdint.h>
//Include float.h for floating-point information
#include <float.h>

//Include UChar for Apple systems
#ifdef __APPLE__
    typedef uint_least32_t char32_t; // char32_t is just an unsigned 32-bit int
#else
    #include <uchar.h>
#endif

//Global input string buffer
wchar_t input_array[100];

//Global binary input array
wchar_t binary_array[100];

//CodePoint input array
wchar_t codepoint_array[100];

//Function to extract sign, exponent, and mantissa from a float (Part 2e)
void extract_float_bits(float f)
{
    //Extract the bits of the float into an unsigned integer
    uint32_t bits;

    //Check that the float uses 32-bit IEEE 754 representation
    if (sizeof(float) != sizeof(uint32_t) ||
        FLT_RADIX != 2 || FLT_MANT_DIG != 24 || FLT_MAX_EXP != 128)
    {
        //Print an error message if the system does not support 32-bit IEEE 754 floats
        printf("This system does not support 32-bit IEEE 754 floats.\n");
        return;
    }

    //Copy the floating-point bits into an unsigned integer
    memcpy(&bits, &f, sizeof(bits));

    //Extract the sign, exponent, and mantissa from the bits
    uint32_t sign = (bits >> 31) & 1;
    printf("   i) Sign Bit: %u -> Number is %s\n",
           sign, (sign == 0) ? "Positive" : "Negative");

    //Extract the exponent from the bits
    uint32_t exponent = (bits >> 23) & 0xFF;
    printf("   ii) Exponent: %u (Unbiased exponent: %d)\n",
           exponent, (int)exponent - 127);

    //Extract the mantissa from the bits
    uint32_t mantissa = bits & 0x7FFFFF;
    printf("   iii) Mantissa: 0x%06X\n\n", mantissa);
}

// Function to convert an 8-bit binary string into an unsigned integer
unsigned int convert_binary_to_decimal(wchar_t binary[])
{
    //Initialize the result to 0
    unsigned int result = 0;

    // Loop through each element in the array until the null terminator (0)
    for (int i = 0; binary[i] != 0; i++) {
        // Step 1: Shift existing bits left by 1
        result = result << 1;

        // Step 2: If the character is '1', turn on the lowest bit
        if (binary[i] == L'1') {
            result = result | 1;
        }
    }

    //Return the converted decimal value
    return result;
}

//Function to convert a decimal number to binary
char* convert_decimal_to_binary(int number)
{
    // Allocate memory for a 32-bit binary string plus null terminator
    char* binary = malloc(33 * sizeof(char));

    //Check if memory allocation was successful
    if (binary == NULL)
        return NULL;

    //Convert the input number to an unsigned 32-bit integer
    uint32_t u_number = (uint32_t)number;

    // Convert all 32 bits, including leading zeros, into the binary string
    for (int i = 31; i >= 0; i--)
    {
        //Extract the i-th bit from the unsigned integer and convert it to a character '0' or '1'
        binary[31 - i] = (char)(((u_number >> i) & 1) + '0');
    }

    //Null-terminate the binary string
    binary[32] = '\0';

    return binary;
}

//Convert UTF-32 character to decimal code point
int convert_UTF32_to_decimal(char32_t c)
{
    //Cast the UTF-32 character to a 32-bit unsigned integer to get the code point
    uint32_t codepoint = (uint32_t)c;
    return codepoint;
}

//Function to print each character of the input string with its index
int process_string_input(wchar_t *inputString)
{
    // Safely strip out the trailing newline character
    inputString[wcscspn(inputString, L"\n")] = L'\0';

    //Iterate over each character in the input string
    for (size_t i = 0; i < wcslen(inputString); i++)
    {
        //Get the current character from the input string
        wchar_t currentChar = inputString[i];
        //Convert the current character to its code point
        int codepoint = (int)currentChar;

        //Convert the code point to a binary string
        char* binary = convert_decimal_to_binary(codepoint);

        //Check if the binary string was successfully allocated
        //If not, print an error message and return an error code
        if (binary == NULL)
        {
            printf("Memory allocation failed\n");
            return 1;
        }

        //Print the index, character, code point, and binary representation
        printf("%zu: %lc (Code point: %d, Binary: %s)\n",
               i, currentChar, codepoint, binary);

        //Free the allocated memory for the binary string
        free(binary);
    }

    //Print a newline after processing all characters
    printf("\n");
    return 0;
}

//Function to validate if the given codepoint matches the binary input
int validate_decimal_input(wchar_t* decimal_input, wchar_t* binary_input)
{
    //Convert the binary input to its decimal equivalent
    unsigned int binary_value = convert_binary_to_decimal(binary_input);

    //Convert the decimal input to a long integer
    long decimal_value = wcstol(decimal_input, NULL, 10);

    //Calculate the difference between the decimal input and the binary input converted to decimal
    long difference = decimal_value - (long)binary_value;

    //Print the decimal input and the binary input converted to decimal
    printf("Decimal input: %ld\n", decimal_value);
    printf("Binary converted to decimal: %u\n", binary_value);

    //Check if the difference is zero
    //If it is, print a success message and return 0
    //If it is not, print an error message and return 1
    if (difference == 0)
    {
        //Print a success message indicating that the binary input matches the decimal input
        printf("Binary input matches the decimal, difference = %ld\n",
               difference);
        return 0;
    }
    else
    {
        //Print an error message indicating that the binary input does not match the decimal input
        printf("Binary input does not match the decimal, difference = %ld\n",
               difference);
        printf("The difference is not zero because the decimal "
               "input does not match the value represented by "
               "the binary bit pattern.\n");
        return 1;
    }
}

//Function to check if the input is a float or an integer
int is_input_float_or_int(wchar_t* input)
{
    //Check if the input is a float or an integer
    int dot_count = 0;
    int digit_count = 0;
    size_t start = 0;

    // Allow an optional leading minus or plus sign
    if (input[0] == L'-' || input[0] == L'+')
    {
        start = 1;
    }

    // If the string is just "+" or "-" with nothing else, it's invalid
    if (wcslen(input) <= start)
    {
        return 0;
    }

    //Iterate through the input string to count digits and dots
    for (size_t i = start; i < wcslen(input); i++)
    {
        //Check if the current character is a dot or a digit
        if (input[i] == L'.')
        {
            dot_count++;

            if (dot_count > 1)
            {
                return 0;
            }
        }
        //If the current character is not a dot or a digit, the input is invalid
        else if (input[i] >= L'0' && input[i] <= L'9')
        {
            digit_count++;
        }
        //If the current character is a digit, increment the digit count
        else
        {
            return 0;
        }
    }

    //Make sure the input contains at least one digit
    if (digit_count == 0)
    {
        return 0;
    }

    // Check if it's a float or an integer
    if (dot_count == 1)
    {
        //If there is exactly one dot, it's a float
        return 2;
    }
    else
    {
        //If there is no dot, it's an integer
        return 1;
    }
}

//Function to check if the input is a binary number
//Returns 0 if the input is a valid binary number, 1 otherwise
int is_input_binary(wchar_t* input)
{
    for (size_t i = 0; i < wcslen(input); i++)
    {
        if (input[i] != L'0' && input[i] != L'1')
        {
            return 1;
        }
    }

    return 0;
}

//Function to get input from the user
wchar_t* get_input(wchar_t *input, size_t input_size)
{
    if (fgetws(input, input_size, stdin) == NULL)
    {
        return NULL;
    }

    return input;
}

//Main function
int main()
{
    setlocale(LC_ALL, "");

    //Part 1

    printf("Part 1 \n");

    printf("\n");

    printf("Enter the input string: \n");

    wchar_t* inputString = get_input(
        input_array,
        sizeof(input_array) / sizeof(input_array[0])
    );

    //Check whether input was successfully received
    if (inputString == NULL)
    {
        printf("Error reading input.\n");
        return 1;
    }

    //Check if the input is a valid number (integer or float)
    printf("Processing input string:\n");

    printf("\n");

    //Remove the newline before printing the string and its length
    inputString[wcscspn(inputString, L"\n")] = L'\0';

    printf("Input string: %ls\n", inputString);

    printf("\n");

    //Print the length of the input string
    printf("Length of input string: %zu\n", wcslen(inputString));

    printf("\n");

    process_string_input(inputString);

    // Divider
    printf("--------------------------------------------------\n");

    //Part 2: Binary to decimal conversion
    int binary_valid = 1;

    printf("Part 2 \n");

    printf("\n");

    //Initialize binary input and decimal input pointers
    wchar_t* binary_input = NULL;

    wchar_t* decimal_input = NULL;

    //Prompt the user for binary input until a valid binary number is entered
    while (binary_valid == 1)
    {
        printf("Enter binary input (exactly 8 digits): \n");

        binary_input = get_input(
            binary_array,
            sizeof(binary_array) / sizeof(binary_array[0])
        );

        //Check whether input was successfully received
        if (binary_input == NULL)
        {
            printf("Error reading input.\n");
            return 1;
        }

        //Remove the newline character at the end of the input
        binary_input[wcscspn(binary_input, L"\n")] = L'\0';

        //Check that the input contains exactly 8 characters
        if (wcslen(binary_input) != 8)
        {
            printf("Input must contain exactly 8 binary digits\n");
            continue;
        }

        //Check if all characters are binary digits
        if (is_input_binary(binary_input) == 0)
        {
            binary_valid = 0;
        }
        else
        {
            printf("Input is not a valid binary number\n");
        }
    }

    //Part 2b: Decimal input validation
    int is_digit = 1;
    int decimal_valid = 1;

    //Prompt the user for decimal input until a valid integer is entered
    while (decimal_valid == 1)
    {
        printf("Enter the decimal value that is equivalent to "
               "the binary input: \n");

        decimal_input = get_input(
            codepoint_array,
            sizeof(codepoint_array) / sizeof(codepoint_array[0])
        );

        //Check whether input was successfully received
        if (decimal_input == NULL)
        {
            printf("Error reading input.\n");
            return 1;
        }

        //Remove the newline character at the end of the input
        decimal_input[wcscspn(decimal_input, L"\n")] = L'\0';

        is_digit = is_input_float_or_int(decimal_input);

        //Check if the input is a valid number (integer or float)
        if (is_digit == 0)
        {
            printf("Input is not a valid number\n");
            continue;
        }
        //Check if the input is a floating-point number
        else if (is_digit == 2)
        {
            printf("Please enter an integer, not a floating-point number\n");
            continue;
        }
        else
        {
            //Convert the input to a decimal value
            long decimal_value = wcstol(decimal_input, NULL, 10);

            //Check that the decimal value is within the 8-bit range
            if (decimal_value < 0 || decimal_value > 255)
            {
                //Print an error message if the decimal value is out of range
                printf("Please enter a decimal value between 0 and 255.\n");
                continue;
            }

            //Mark the decimal input as valid
            decimal_valid = 0;
        }
    }

    //Validate the decimal input against the binary input
    validate_decimal_input(decimal_input, binary_input);

    //Part 2e: Extract IEEE 754 bits from a floating-point number
    //Call the function regardless of whether the entered number
    //contained a decimal point
    printf("Floating-point representation (IEEE 754):\n");

    //Extract the IEEE 754 bits from the decimal input
    extract_float_bits((float)wcstod(decimal_input, NULL));

    return 0;
}
