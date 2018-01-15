/*
 * ureMorse
 * Morse Microbit Editor/Keyboard
*/
#include "MicroBit.h"
#include <cstring>
#include <cstdlib>

#define MED_INSERT_CHAR_BUF_LEN 8
#define MED_INSERT_BUF_LEN 100
#define MED_TELECOM_BUF_LEN 100
#define MED_TELECOM_GROUP 48

//Display Images
MicroBitImage image_download("\
        0 0 1 0 0\n\
        0 0 1 0 0\n\
        1 1 1 1 1\n\
        0 1 1 1 0\n\
        0 0 1 0 0\n"); 

MicroBitImage image_upload("\
        0 0 1 0 0\n\
        0 1 1 1 0\n\
        1 1 1 1 1\n\
        0 0 1 0 0\n\
        0 0 1 0 0\n"); 

MicroBitImage image_insert_mode("\
        1 0 0 0 1\n\
        1 1 0 1 1\n\
        1 1 0 1 1\n\
        1 1 0 1 1\n\
        1 0 0 0 1\n");


MicroBitImage image_global_mode("\
        0 0 1 1 1\n\
        0 1 0 0 0\n\
        0 1 0 1 1\n\
        0 1 0 0 1\n\
        0 0 1 1 0\n"); 


MicroBitImage image_telecom_mode("\
        0 1 1 1 0\n\
        1 0 0 0 1\n\
        0 0 1 0 0\n\
        0 1 0 1 0\n\
        0 0 1 0 0\n");
    
MicroBitImage image_delete("\
        1 1 1 1 1\n\
        1 0 1 0 1\n\
        1 1 0 1 1\n\
        1 0 1 0 1\n\
        1 1 1 1 1\n"); 

MicroBitImage image_return("\
        0 0 0 0 0\n\
        0 1 0 1 0\n\
        1 1 1 1 0\n\
        0 1 0 0 0\n\
        0 0 0 0 0\n"); 

MicroBitImage image_keyboard("\
        0 0 0 1 0\n\
        0 0 1 0 0\n\
        1 1 1 1 1\n\
        1 1 1 1 1\n\
        0 0 0 0 0\n"); 


MicroBit uBit;

//Operation mode
typedef enum
{
    MODE_INSERT = 1,
    MODE_TELECOM = 2,
}MODE;
MODE med_operation_mode = MODE_INSERT;

//Morse Code Translation
static const char MORSE_TO_CHAR[128] = {
    '\0', '\0', 'E', 'T', 'I', 'N', 'A', 'M',
    'S', 'D', 'R', 'G', 'U', 'K', 'W', 'O',
    'H', 'B', 'L', 'Z', 'F', 'C', 'P', '\0',
    'V', 'X', '\0', 'Q', '\0', 'Y', 'J', '\0',
    '5', '6', '\0', '7', '\0', '\0', '\0', '8',
    '\0', '/', '\0', '\0', '\0', '(', '\0', '9',
    '4', '=', '\0', '\0', '\0', '\0', '\0', '\0',
    '3', '\0', '\0', '\0', '2', '\0', '1', '0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', ':',
    '\0', '\0', '\0', '\0', '?', '\0', '\0', '\0',
    '\0', '\0', '"', '\0', '\0', '\0', '@', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\'', '\0',
    '\0', '-', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '.', '\0', '_', ')', '\0', '\0',
    '\0', '\0', '\0', ',', '\0', '!', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

char translate_morse(const char *str)
{
    unsigned char sum = 0, bit;

    for (bit = 1; bit; bit <<= 1) {
            switch (*str++) {
            case 0:
                    return MORSE_TO_CHAR[sum | bit];
            default:
                    return MORSE_TO_CHAR[0];
            case '-':
                    sum |= bit;
                    /* FALLTHROUGH */
            case '.':
                    break;
            }
    }
    

    return MORSE_TO_CHAR[0];
}

//Insert Mode
static char *char_buf = nullptr;
static char *char_pos = nullptr;

void char_init()
{
    if(!char_buf)
    {
        char_buf = new char[MED_INSERT_CHAR_BUF_LEN];
        char_pos = char_buf;
    }
}

void char_bit(bool state)
{
    if(state) *char_pos = '-';
    else *char_pos = '.';
    
    char_pos++;
}

void char_reset()
{
    memset(char_buf, 0, sizeof(char) * MED_INSERT_CHAR_BUF_LEN);
    char_pos = char_buf;
}

char char_get()
{
    if(!char_buf) return '\0';
    if(char_pos == char_buf) return ' ';
    *char_pos = '\0';
    char result = translate_morse(char_buf);

    return result;
}

static char *insert_buf = nullptr;
static char *insert_pos = nullptr;

void insert_init()
{
    if(!insert_buf) 
    {
        insert_buf = new char[MED_INSERT_BUF_LEN];
        memset(insert_buf, 0, sizeof(char) * MED_INSERT_BUF_LEN);
        insert_pos = insert_buf;
    }
}

void insert_append(char c)
{
    *insert_pos = c;
    insert_pos++;
}

void insert_delete()
{
    if(!insert_buf) return;

    if(insert_pos == insert_buf) return; //Do nothing if nothing in buf
    insert_pos--;
    if(*insert_pos == '\n') //New line comprises of 2 characters
    {
        *insert_pos = '\0';
        insert_pos --; 
    }

    *insert_pos = '\0';
}

void insert_reset()
{
    memset(insert_buf, 0, sizeof(char) * MED_INSERT_BUF_LEN);
    insert_pos = insert_buf;
}

char *insert_data()
{
    if(!insert_buf) return nullptr;
    *insert_pos = '\0';
    return insert_buf;
}

size_t insert_size()
{
    return insert_pos - insert_buf;
}

//Telecom
const char *telecom_marker_begin = "^@BEGIN@^";
const char *telecom_marker_end = "^@END@^";
char *telecom_buf = nullptr;
char *telecom_recieve_pos = nullptr;

void telecom_init()
{
    if(!telecom_buf)
    {
        telecom_buf = new char[MED_TELECOM_BUF_LEN];
        memset(telecom_buf,0, sizeof(char) * MED_TELECOM_BUF_LEN);
    }
    
    telecom_recieve_pos = nullptr;
}

void telecom_send(char *data, size_t len)
{
    uBit.radio.datagram.send(ManagedString(telecom_marker_begin));

    char packet_buf[2] = {'0', '0'};
    for(size_t i = 0; i < len; i++)
    {
        packet_buf[0] = *(data + i);
        uBit.radio.datagram.send(ManagedString(packet_buf));
    }
    
    uBit.radio.datagram.send(ManagedString(telecom_marker_end));
}

void telecom_recv(char data)
{
    if(telecom_recieve_pos) 
    {
        *telecom_recieve_pos = data;
        telecom_recieve_pos++;
    }
}

void handle_telecom(MicroBitEvent e)
{
    if(e.value == MICROBIT_RADIO_EVT_DATAGRAM)
    {
        ManagedString s = uBit.radio.datagram.recv();
        if(s == ManagedString(telecom_marker_begin))
            telecom_recieve_pos = telecom_buf;
        else if(s == ManagedString(telecom_marker_end))
            telecom_recieve_pos = nullptr;
        else if(telecom_recieve_pos)
            telecom_recv(s.toCharArray()[0]);
    }
}

//Hardware Event Handlers
bool button_syn_status = true;

bool button_syn()
{
    fiber_sleep(20);
    
    if(!button_syn_status) return false;
    else return true;
}

void handle_button_A(MicroBitEvent e)
{
    if(!button_syn()) return;

    if(med_operation_mode == MODE_INSERT)
    {
        uBit.display.clear();
        //Morse Code Insert
        if (e.value == MICROBIT_BUTTON_EVT_CLICK)
            char_bit(false);
        else if (e.value == MICROBIT_BUTTON_EVT_HOLD)
            char_bit(true);
        uBit.display.printChar(char_get());
    }
}

void handle_button_B(MicroBitEvent e)
{
    if(!button_syn()) return;

    if(med_operation_mode == MODE_INSERT)
    {
        if (e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            //Insert Char
            uBit.display.scroll("|",35);

            insert_append(char_get());

            char_reset();
            uBit.display.clear();
        }
        else if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            insert_delete();

            uBit.display.print(image_delete);
            uBit.sleep(250);
            uBit.display.clear();
        }
    }
}

void handle_button_AB(MicroBitEvent e)
{
    button_syn_status = false;

    if(med_operation_mode == MODE_INSERT)
    {
        if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            med_operation_mode = MODE_TELECOM;
            uBit.display.print(image_telecom_mode);
            uBit.sleep(1000);
            uBit.display.clear();
        }
        else if(e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            insert_append('\r'); 
            insert_append('\n'); 

            uBit.display.print(image_return);
            uBit.sleep(500);
            uBit.display.clear();
        }
    }
    else if(med_operation_mode == MODE_TELECOM)
    {
        if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            med_operation_mode = MODE_INSERT;
            uBit.display.print(image_insert_mode);
            uBit.sleep(1000);
            uBit.display.clear();
        }
    }
    
    
    button_syn_status = true;
}

void handle_gesture(MicroBitEvent e)
{
    if(med_operation_mode == MODE_INSERT)
    {
        if(e.value == MICROBIT_ACCELEROMETER_EVT_SHAKE)
        {
            char_reset();
            uBit.display.clear();
            uBit.display.stopAnimation();
        }
    }
    
}

//Core
int main()
{
    uBit.init();
    
    uBit.serial.baud(115200);
    
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_EVT_ANY, handle_button_AB, MESSAGE_BUS_LISTENER_REENTRANT);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, handle_button_A, MESSAGE_BUS_LISTENER_REENTRANT);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, handle_button_B, MESSAGE_BUS_LISTENER_REENTRANT);
    uBit.messageBus.listen(MICROBIT_ID_GESTURE, MICROBIT_EVT_ANY, handle_gesture, MESSAGE_BUS_LISTENER_REENTRANT);

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, handle_telecom);
    uBit.radio.setGroup(MED_TELECOM_GROUP);
    if(uBit.radio.enable() != MICROBIT_OK) uBit.panic();

    char_init();
    insert_init();
    telecom_init();
    
    release_fiber();
}
