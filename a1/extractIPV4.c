#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1000

/*
 * Validates one complete candidate token.
 * start points to the first character of the token.
 * end points one character past the end of the token.
 */
int validateToken(const char *start, const char *end, unsigned long *address, int *port)
{
    const char *p = start;
    int octets[4];

    /* Read exactly four octets. */
    for (int i = 0; i < 4; i++)
    {
        int value = 0;
        int digits = 0;

        /* Each octet must begin with a digit. */
        if (p >= end || !isdigit((unsigned char)*p))
            return 0;

        /* Leading zero is allowed only for the value 0. */
        if (*p == '0')
        {
            p++;
            digits = 1;

            /* Another digit after 0 means a leading zero. */
            if (p < end && isdigit((unsigned char)*p))
                return 0;

            value = 0;
        }
        else
        {
            /* Accumulate the octet value by hand. */
            while (p < end &&
                   isdigit((unsigned char)*p) &&
                   digits < 3)
            {
                value = value * 10 + (*p - '0');
                digits++;
                p++;
            }

            /* More than 3 digits in this octet. */
            if (p < end && isdigit((unsigned char)*p))
                return 0;

            if (value > 255)
                return 0;
        }

        octets[i] = value;

        /* First three octets must be followed by '.'. */
        if (i < 3)
        {
            if (p >= end || *p != '.')
                return 0;

            p++;
        }
    }

    /* Default: no port. */
    *port = -1;

    /*
     * Anything remaining after the fourth octet must be
     * exactly ":port".
     */
    if (p < end)
    {
        if (*p != ':')
            return 0;

        p++;

        /* Colon must be followed by at least one digit. */
        if (p >= end || !isdigit((unsigned char)*p))
            return 0;

        int portValue = 0;
        int digits = 0;

        /* Leading zero rule for port. */
        if (*p == '0')
        {
            p++;
            digits = 1;

            if (p < end && isdigit((unsigned char)*p))
                return 0;

            portValue = 0;
        }
        else
        {
            while (p < end && isdigit((unsigned char)*p) && digits < 5)
            {
                portValue = portValue * 10 + (*p - '0');
                digits++;
                p++;
            }

            /* More than 5 digits. */
            if (p < end && isdigit((unsigned char)*p))
                return 0;

            if (portValue > 65535)
                return 0;
        }

        *port = portValue;
    }

    /*
     * The ENTIRE candidate token must have been consumed.
     * This rejects extra periods, colons, etc.
     */
    if (p != end)
        return 0;

    /* Build the 32-bit IPv4 value. */
    *address =
        ((unsigned long)octets[0] << 24) |
        ((unsigned long)octets[1] << 16) |
        ((unsigned long)octets[2] << 8)  |
        (unsigned long)octets[3];

    return 1;
}
/*
 * Returns 1 if a valid address was found, 0 otherwise.
 *
 * On success:
 *   *outAddress = 32-bit IPv4 value
 *   *outPort = port number, or -1 if no port
 *
 * On failure:
 *   *outAddress = 0
 *   *outPort = -1
 */
int extractIPv4(const char *str, unsigned long *outAddress, int *outPort)
{
    const char *p = str;

    *outAddress = 0;
    *outPort = -1;

    while (*p != '\0')
    {
        /*
         * Skip garbage. Only digits, '.', and ':' can
         * belong to a candidate token.
         */
        while (*p != '\0' && !isdigit((unsigned char)*p) && *p != '.' && *p != ':')
        {
            p++;
        }

        if (*p == '\0')
            break;

        /* Beginning of the candidate token. */
        const char *start = p;

        /*
         * Find the end of the complete run of characters
         * that could belong to a token.
         */
        while (*p != '\0' && (isdigit((unsigned char)*p) || *p == '.' || *p == ':'))
        {
            p++;
        }

        const char *end = p;

        /*
         * Validate the WHOLE candidate.
         * No partial matching is attempted.
         */
        unsigned long address;
        int port;

        if (validateToken(start, end, &address, &port))
        {
            *outAddress = address;
            *outPort = port;
            return 1;
        }
    }

    return 0;
}

#ifndef TESTING
int main(void)
{
    char input[MAX_INPUT];

    while (1)
    {
        printf("Enter a string (or 'END' to quit): ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        /* Remove the newline added by fgets. */
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "END") == 0 || strcmp(input, "end") == 0 || strcmp(input, "End") == 0)
        {
            printf("Program terminated.\n");
            break;
        }

        unsigned long address;
        int port;

        if (extractIPv4(input, &address, &port))
        {
            unsigned int a = (address >> 24) & 0xFF;
            unsigned int b = (address >> 16) & 0xFF;
            unsigned int c = (address >> 8) & 0xFF;
            unsigned int d = address & 0xFF;

            if (port == -1)
            {
                printf("Extracted IPv4 address: %u.%u.%u.%u "
                       "(decimal value: %lu, port: none)\n",
                       a, b, c, d, address);
            }
            else
            {
                printf("Extracted IPv4 address: %u.%u.%u.%u "
                       "(decimal value: %lu, port: %d)\n",
                       a, b, c, d, address, port);
            }
        }
        else
        {
            printf("Invalid input: no valid IPv4 address found\n");
        }

        printf("\n");
    }

    return 0;
}
#endif