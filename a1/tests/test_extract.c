#include <stdio.h>
/*
 * Test harness for:
 *
 * int extractIPv4(const char* str, unsigned long* outAddress, int* outPort);
 *
 * Compile this file together with the file that contains extractIPv4().
 * Example:
 *     gcc -Wall -Wextra -std=c11 extract.c test_extract.c -o test_extract
 *
 * If extract.c also contains main(), temporarily remove/comment out that main()
 * or guard it so there is only one main() when compiling the tests.
 */
int extractIPv4(const char* str, unsigned long* outAddress, int* outPort);

typedef struct
{
    const char *description;
    const char *input;
    int expectedFound;
    unsigned long expectedAddress;
    int expectedPort;
} TestCase;

/* Build the expected 32-bit IPv4 value without using parsing functions. */
#define IPV4_VALUE(a, b, c, d) \
    (((unsigned long)(a) << 24) | \
     ((unsigned long)(b) << 16) | \
     ((unsigned long)(c) << 8)  | \
     (unsigned long)(d))

int main(void)
{
    TestCase tests[] =
    {
        /* ------------------------------------------------------------
           Sample-run test cases
           ------------------------------------------------------------ */
        {
            "Sample: ordinary embedded address",
            "connecting to 192.168.1.1 now",
            1, IPV4_VALUE(192,168,1,1), -1
        },
        {
            "Sample: address with port followed by letters",
            "server=10.0.0.255:8080end",
            1, IPV4_VALUE(10,0,0,255), 8080
        },
        {
            "Sample: garbage letter separates candidate tokens",
            "192a168.1.1.1",
            1, IPV4_VALUE(168,1,1,1), -1
        },
        {
            "Sample: stray period after otherwise valid address",
            "192.168.1.1.",
            0, 0, -1
        },
        {
            "Sample: address inside normal sentence",
            "Connection from 192.168.1.1 refused",
            1, IPV4_VALUE(192,168,1,1), -1
        },
        {
            "Sample: octet has leading zero",
            "192.168.01.1",
            0, 0, -1
        },
        {
            "Sample: port out of range",
            "1.2.3.4:99999",
            0, 0, -1
        },
        {
            "Sample: too few octets",
            "12.34.56",
            0, 0, -1
        },
        {
            "Sample: no address",
            "*no number here*",
            0, 0, -1
        },
        {
            "Sample: END is not itself an IPv4 address",
            "END",
            0, 0, -1
        },

        /* ------------------------------------------------------------
           Valid boundary cases
           ------------------------------------------------------------ */
        {
            "Minimum IPv4 address",
            "0.0.0.0",
            1, IPV4_VALUE(0,0,0,0), -1
        },
        {
            "Maximum IPv4 address",
            "255.255.255.255",
            1, IPV4_VALUE(255,255,255,255), -1
        },
        {
            "Minimum port",
            "1.2.3.4:0",
            1, IPV4_VALUE(1,2,3,4), 0
        },
        {
            "Maximum port",
            "1.2.3.4:65535",
            1, IPV4_VALUE(1,2,3,4), 65535
        },
        {
            "One-digit octets",
            "x1.2.3.4y",
            1, IPV4_VALUE(1,2,3,4), -1
        },
        {
            "Three-digit octets at valid limits",
            "100.200.250.255",
            1, IPV4_VALUE(100,200,250,255), -1
        },
        {
            "Valid token after invalid candidate",
            "999.1.1.1 garbage 8.8.8.8",
            1, IPV4_VALUE(8,8,8,8), -1
        },
        {
            "Valid token after punctuation garbage",
            "abc!!!172.16.0.1???",
            1, IPV4_VALUE(172,16,0,1), -1
        },

        /* ------------------------------------------------------------
           Invalid octet cases
           ------------------------------------------------------------ */
        {
            "First octet too large",
            "256.1.1.1",
            0, 0, -1
        },
        {
            "Middle octet too large",
            "1.300.1.1",
            0, 0, -1
        },
        {
            "Fourth octet too large",
            "1.1.1.256",
            0, 0, -1
        },
        {
            "Four-digit octet",
            "1000.1.1.1",
            0, 0, -1
        },
        {
            "Leading zero in first octet",
            "01.2.3.4",
            0, 0, -1
        },
        {
            "Leading zero in final octet",
            "1.2.3.004",
            0, 0, -1
        },
        {
            "All octets have invalid leading zeros",
            "00.00.00.00",
            0, 0, -1
        },
        {
            "Empty octet",
            "1..2.3.4",
            0, 0, -1
        },
        {
            "Missing first octet",
            ".1.2.3.4",
            0, 0, -1
        },
        {
            "Missing final octet",
            "1.2.3.",
            0, 0, -1
        },
        {
            "Too many octets",
            "1.2.3.4.5",
            0, 0, -1
        },

        /* ------------------------------------------------------------
           Invalid port cases
           ------------------------------------------------------------ */
        {
            "Port one above maximum",
            "1.2.3.4:65536",
            0, 0, -1
        },
        {
            "Port has leading zero",
            "1.2.3.4:080",
            0, 0, -1
        },
        {
            "Empty port",
            "1.2.3.4:",
            0, 0, -1
        },
        {
            "Six-digit port",
            "1.2.3.4:123456",
            0, 0, -1
        },
        {
            "Second colon",
            "1.2.3.4:80:90",
            0, 0, -1
        },
        {
            "Colon before fourth octet",
            "1.2.3:4",
            0, 0, -1
        },
        {
            "Extra period after valid port",
            "1.2.3.4:80.",
            0, 0, -1
        },

        /* ------------------------------------------------------------
           Full-token / adjacency tests
           ------------------------------------------------------------ */
        {
            "Period directly before address makes whole token invalid",
            ".192.168.1.1",
            0, 0, -1
        },
        {
            "Colon directly before address makes whole token invalid",
            ":192.168.1.1",
            0, 0, -1
        },
        {
            "Extra digit attached to first octet",
            "1192.168.1.1",
            0, 0, -1
        },
        {
            "Extra digit attached to final octet",
            "192.168.1.1000",
            0, 0, -1
        },
        {
            "Letters are garbage and may separate tokens",
            "bad192x10.20.30.40good",
            1, IPV4_VALUE(10,20,30,40), -1
        },
        {
            "Invalid token then valid token separated by letters",
            "1.2.3.4.5abc5.6.7.8",
            1, IPV4_VALUE(5,6,7,8), -1
        },

        /* ------------------------------------------------------------
           Stress / mixed-input cases
           ------------------------------------------------------------ */
        {
            "Long garbage before valid address",
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "!@#$%^&*()_+-=[]{};,<>?/ "
            "203.0.113.42:443 trailing text",
            1, IPV4_VALUE(203,0,113,42), 443
        },
        {
            "Several invalid candidates before valid one",
            "999.999.999.999 xx 01.2.3.4 yy 1.2.3.4:70000 zz 192.0.2.1",
            1, IPV4_VALUE(192,0,2,1), -1
        },
        {
            "Only punctuation candidates",
            "... ::: .:. ::..",
            0, 0, -1
        },
        {
            "Single number is not an address",
            "12345",
            0, 0, -1
        },
        {
            "Valid address immediately surrounded by letters",
            "abc127.0.0.1xyz",
            1, IPV4_VALUE(127,0,0,1), -1
        }
    };

    int testCount = (int)(sizeof(tests) / sizeof(tests[0]));
    int passed = 0;

    printf("Running %d extractIPv4 tests...\n\n", testCount);

    for (int i = 0; i < testCount; i++)
    {
        unsigned long address = 123456789UL;
        int port = 12345;

        int found = extractIPv4(tests[i].input, &address, &port);

        int success =
            found == tests[i].expectedFound &&
            address == tests[i].expectedAddress &&
            port == tests[i].expectedPort;

        printf("Test %2d: %-55s %s\n",
               i + 1,
               tests[i].description,
               success ? "PASS" : "FAIL");

        if (!success)
        {
            printf("         Input: \"%s\"\n", tests[i].input);
            printf("         Expected: found=%d, address=%lu, port=%d\n",
                   tests[i].expectedFound,
                   tests[i].expectedAddress,
                   tests[i].expectedPort);
            printf("         Actual:   found=%d, address=%lu, port=%d\n",
                   found, address, port);
        }
        else
        {
            passed++;
        }
    }

    printf("\n%d/%d tests passed.\n", passed, testCount);

    if (passed == testCount)
    {
        printf("All tests passed.\n");
        return 0;
    }

    printf("%d test(s) failed.\n", testCount - passed);
    return 1;
}