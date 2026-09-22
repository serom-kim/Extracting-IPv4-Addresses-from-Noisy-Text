# AI Disclosure

## 1. AI Tool Used

- Tool: ChatGPT
- Model/version: GPT-5.6 Sol
- Dates consulted: September 22, 2026
- Purpose: Assistance with generating, reviewing, debugging, and testing the IPv4 extraction program.

## 2. Prompts Used

### Prompt 1 - ExtractIPV4 File Prompt
Write a C program that reads a line of text and extracts a single valid IPv4 address — optionally followed by a port number — embedded anywhere in that text. Only digits, periods (.), and colons (:) are ever part of a valid token; every other character is garbage and is skipped. A candidate token must match the address grammar in full — no partial matches, no truncating to find a valid piece inside a longer run. An address is four octets separated by periods (octet.octet.octet.octet), each octet 1–3 digits, value 0–255, no leading zero unless the value is exactly 0. An optional :port may follow the fourth octet: 1–5 digits, value 0–65535, same leading-zero rule. If a colon is present, the port must be fully valid or the entire match — address included — is rejected.

The function returns whether a valid address was found, and delivers the address and port through two additional parameters.
C:
// Returns 1 if a valid address was found, 0 otherwise.
// On success: *outAddress holds the 32-bit value, and
// *outPort holds the port number, or -1 if no port was present.
// On failure: *outAddress is set to 0 and *outPort is set to -1.
int extractIPv4(const char* str, unsigned long* outAddress, int* outPort);

Do not use the following:
Any string-to-number conversion function: atoi, atol, atoll, strtol, strtoul, strtod, stoi, stol, stoul, sscanf, scanf with numeric conversions.
Any address-parsing library function: inet_aton, inet_pton, inet_addr, or equivalents.
Any regular-expression facility (std::regex, POSIX regex.h, or similar) — the parsing and validation logic must be your own character-by-character code, not a pattern matched by a library.
*Standard character-classification functions (isdigit, etc.) are fine to use.

Requirements:
Exactly one valid address may be extracted per input line; everything else in the line is either garbage (skipped) or part of a candidate token that fails validation.
Reject anything that does not exactly match the grammar above — wrong octet count, empty octet, out-of-range octet or port, a disallowed leading zero, a second colon, a colon not immediately after the fourth octet, or a stray period/colon directly adjacent to an otherwise-valid address.
On success, print: Extracted IPv4 address: A.B.C.D (decimal value: N, port: P) where N is the 32-bit decimal value and P is the port number or the literal text none.
Input loop: main continuously prompts the user for input until the user enters END (case-sensitive), then prints "Program terminated." and exits.
Extraction function: implement extractIPv4 exactly as prototyped above. All digit accumulation must be done by hand.
Display: main receives the result from extractIPv4 and formats the output exactly as specified above.

Here is a sample run:
Enter a string (or 'END' to quit): connecting to 192.168.1.1 now
Extracted IPv4 address: 192.168.1.1 (decimal value: 3232235777, port: none)

Enter a string (or 'END' to quit): server=10.0.0.255:8080end
Extracted IPv4 address: 10.0.0.255 (decimal value: 167772415, port: 8080)

Enter a string (or 'END' to quit): 192a168.1.1.1
Extracted IPv4 address: 168.1.1.1 (decimal value: 2818638081, port: none)

Enter a string (or 'END' to quit): 192.168.1.1.
Invalid input: no valid IPv4 address found

Enter a string (or 'END' to quit): Connection from 192.168.1.1 refused
Extracted IPv4 address: 192.168.1.1 (decimal value: 3232235777, port: none)

Enter a string (or 'END' to quit): 192.168.01.1
Invalid input: no valid IPv4 address found

Enter a string (or 'END' to quit): 1.2.3.4:99999
Invalid input: no valid IPv4 address found

Enter a string (or 'END' to quit): 12.34.56
Invalid input: no valid IPv4 address found

Enter a string (or 'END' to quit): *no number here*
Invalid input: no valid IPv4 address found

Enter a string (or 'END' to quit): END
Program terminated.

### Prompt 2 - Test Case Prompt

"Create a C file named "test_extract.c" that runs a program of the test cases used in the sample run given in the initial prompt. Make sure it includes all of the listed sample inputs, but also include some additional edge test cases to truly test the scalability and accuracy of the program."

### Prompt 3

"What does this error mean? Do not give the fix, just explain the error."
/usr/bin/ld: /tmp/cciQu6n4.o: in function main': test_extract.c:(.text+0x0): multiple definition of main'; /tmp/ccqLRaQF.o:extractIPV4.c:(.text+0x529): first defined here collect2: error: ld returned 1 exit status -bash: ./test: No such file or directory

## 3. AI-Generated Code

ChatGPT generated the entire extractIPV4.c file with the exception of the edits I made to lines 203 and 257 (see section 4).
ChatGPT also generated the entire test_extract.c file with the requirements I gave and added additional test cases.

## 4. Student-Written / Modified Code

At lines 203 and 257, I added "#ifndef TESTING" and "#endif", respectively, due to an error I was getting when attempting to compile and run the extract file.  

## 5. Problems Found in AI-Generated Code

After creating the test case file, because both the test file and extract file had a main(), I kept getting an error when compiling and running the test file. After getting an explanation from ChatGPT on what the error meant, I understood the issue, performed a Google search to confirm the solution I had in mind, and then implemented the solution by wrapping the main() in my extract file with the following lines: 
#ifndef TESTING
int main(void){
.
.
.
}
#endif

This made the main() function in my extract file conditional and essentially "ignore" it during testing so that my test file could compile and execute properly.

## 6. Testing and Verification

Initially, I manually tested my program by typing in each input from the given sample run in the assignment outline and compared my outputs with the outputs in the sample run. Once they all passed and aligned, I gave ChatGPT my Prompt #2 to create a program that would automatically do what I had just did and add in even more test cases. 

These tests included:
- Valid IPv4 addresses embedded within normal text
- Valid IPv4 addresses with and without port numbers
- Minimum and maximum valid IPv4 values
- Minimum and maximum valid port values
- Invalid octet values greater than 255
- Invalid port values greater than 65535
- Leading zeros in octets and ports
- Missing, extra, or empty octets
- Too many or too few digits in octets
- Missing, extra, or misplaced periods and colons
- Extra periods or colons directly adjacent to an otherwise valid address
- Multiple candidate tokens in one input line
- Invalid candidates followed by a valid IPv4 address
- Garbage characters before, after, and between candidate tokens
- Inputs containing no valid IPv4 address
- Long mixed-input strings to test robustness and scalability

## 7. Verification Statement

I understand every line of the submitted program and reviewed all
AI-generated code before including it in my submission.

I tested the program using both valid and invalid inputs and verified that it
behaves according to the assignment specification.

Known bugs or limitations: None
