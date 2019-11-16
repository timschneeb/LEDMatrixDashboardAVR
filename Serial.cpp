#include <Arduino.h>
#include "Serial.h"
#include "MatrixPanel.h"
//Format: <AAAAAAABCDEFFFFFFFFFFFFFFFFFFFFFF...>
//        <                                 ...> - Packet Marker
//         PRINTLN                               - Command (char[7])
//                X                              - Parameter (char)
//                 Y                             - Parameter 2 (char)
//                  Z                            - Parameter 3 (char)
//                   1                           - Extra data included (bool)
//                    EXTRADATASTRING_______...  - Extra data (char[75])

void serial::recvWithStartEndMarkers()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();
        if (recvInProgress == true)
        {
            if (rc != endMarker)
            {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars)
                {
                    ndx = numChars - 1;
                }
                if (ndx > 110)
                {
                    receivedChars[ndx] = '>'; // handle corruption
                    recvInProgress = false;
                    ndx = 0;
                    newData = true;
                }
            }
            else
            {
                receivedChars[ndx] = '>'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker)
        {
            recvInProgress = true;
        }
    }
}

void serial::showNewData()
{
    if (newData == true)
    {
        recv.cmd = "";
        recv.extra = "";
        for (int i = 0; i < 7; i++)
        {
            recv.cmd += String(receivedChars[i]);
        }
        for (int i = 11; i < 111; i++)
        {
            if (receivedChars[i] == '>')
                break;
            recv.extra += String(receivedChars[i]);
        }
        recv.param = receivedChars[7];
        recv.param2 = receivedChars[8];
        recv.param3 = receivedChars[9];
        recv.extraIncluded = (receivedChars[10] == '1');

        Serial.println("CMD: " + recv.cmd);
        Serial.println("Param: " + String(recv.param));
        Serial.println("Param2: " + String(recv.param2));
        Serial.println("Param3: " + String(recv.param3));
        Serial.println("Extra?: " + String(recv.extraIncluded));
        Serial.println("Extra: " + recv.extra);

        ProcessRequest();
        newData = false;
    }
}
void serial::ProcessRequest()
{
    if (recv.cmd.indexOf("ADDQUE") == 0)
    {
        if (recv.extraIncluded)
            AddScrollQueue(recv.extra);
    }
    else
    {
        Serial.println("Command not recognized: " + recv.cmd);
        return;
    }
    Serial.print("Data recieved ... ");
    Serial.println(receivedChars);
}
