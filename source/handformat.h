#ifndef HANDFORMAT_H
#define HANDFORMAT_H

#include <string.h>
/*
 * This file contains the hand movements
 * translated to servo motors positions
 */

char* FormatMovement(const char* handID)
{
    char* CMD_Word;
    char* fist = "fist";
    char* stop = "stop";

    // More hand posititions can be put here

    if(strcmp(handID, fist) == 0)
    {
        CMD_Word = "F00P24"
                   "F02P25"
                   "F03P25"
                   "F04P25"
                   "F05P14";
    }

    if(strcmp(handID, stop) == 0)
    {
        CMD_Word = "F00P14"
                   "F02P14"
                   "F03P13"
                   "F04P14"
                   "F05P23";
    }

    /* Hand
     * Fully open
     * F0 10
     * F2 11
     * F3 9
     * F4 11
     * F5 11
     */

    /* Hand
     * Fully Closed
     * F0 22
     * F2 24
     * F3 22
     * F4 20
     * F5 24
     */

    return (CMD_Word);
}
#endif // HANDFORMAT_H
