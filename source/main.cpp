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

MicroBitImage image_print("\
        0 1 1 1 0\n\
        0 1 1 1 0\n\
        1 0 0 0 1\n\
        1 1 1 1 1\n\
        1 0 0 0 1\n"); 


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

MicroBitImage image_incomming("\
        0 0 1 0 0\n\
        0 1 0 1 0\n\
        1 1 0 1 1\n\
        1 1 1 1 1\n\
        1 1 0 1 1\n"); 


MicroBit uBit;

//Operation mode
typedef enum
{
    MODE_INSERT = 1,
    MODE_TELECOM = 2,
    MODE_INCOMMING = 3,

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
bool char_mode = false;

void char_init()
{
    if(!char_buf)
    {
        char_buf = new char[MED_INSERT_CHAR_BUF_LEN];
        char_pos = char_buf;
    }
    char_mode = false;
}

void char_bit(bool state)
{
    char_mode = true;
    if(state) *char_pos = '-';
    else *char_pos = '.'; 
    char_pos++;
}

void char_reset()
{
    memset(char_buf, 0, sizeof(char) * MED_INSERT_CHAR_BUF_LEN);
    char_pos = char_buf;
    char_mode = false;
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
char *telecom_recieve_buf = nullptr;
char *telecom_recieve_pos = nullptr;

void telecom_init()
{
    if(!telecom_recieve_buf)
    {
        telecom_recieve_buf = new char[MED_TELECOM_BUF_LEN];
        memset(telecom_recieve_buf,0, sizeof(char) * MED_TELECOM_BUF_LEN);
    }
    
    telecom_recieve_pos = nullptr;
}

void telecom_reset()
{
    telecom_recieve_pos = nullptr;
    memset(telecom_recieve_buf,0, sizeof(char) * MED_TELECOM_BUF_LEN);

}
void telecom_send(char *data, size_t len)
{
    uBit.radio.datagram.send(ManagedString(telecom_marker_begin));
    size_t send_len = (len < MED_TELECOM_BUF_LEN) ? len : MED_TELECOM_BUF_LEN;

    char packet_buf[2] = {'0', '\0'};
    for(size_t i = 0; i < send_len; i++)
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
        if(telecom_recieve_pos - telecom_recieve_buf < MED_TELECOM_BUF_LEN)
        {
            *telecom_recieve_pos = data;
            telecom_recieve_pos++;
        }
        else telecom_recieve_pos = nullptr;
    }
}

void handle_telecom(MicroBitEvent e)
{
    if(e.value == MICROBIT_RADIO_EVT_DATAGRAM)
    {
        //Retrieve Data From Radio
        ManagedString s = uBit.radio.datagram.recv();
        if(s == ManagedString(telecom_marker_begin))
        {
            telecom_reset();
            telecom_recieve_pos = telecom_recieve_buf;
        }
        else if(s == ManagedString(telecom_marker_end))
        {
            telecom_recieve_pos = nullptr;

            //Notify User About New Message & Activate Incomming Mode
            if(!char_mode)
            {
                uBit.display.print(image_incomming);
                MODE prev_mode = med_operation_mode;
                med_operation_mode = MODE_INCOMMING;
                fiber_sleep(1000);
                med_operation_mode = prev_mode;
                uBit.display.clear();
            }
        }
        else if(telecom_recieve_pos)
            telecom_recv(s.toCharArray()[0]);
        
    }
}

/*Button Wait Function to prevent multiple event handlers from running when
 Pressing more than one button (ie A+B should not raise the event handlers
 for A or B) */
bool button_wait_status = true;

bool button_wait()
{
    fiber_sleep(20);
    
    if(!button_wait_status) return false;
    else return true;
}

//Hardware Event Handlers
void handle_button_A(MicroBitEvent e)
{
    if(!button_wait()) return; //Prevent button event overlap

    if(med_operation_mode == MODE_INSERT)
    {
        //Choose Character Using Morse code from button A
        uBit.display.clear();
        if (e.value == MICROBIT_BUTTON_EVT_CLICK)
            char_bit(false);
        else if (e.value == MICROBIT_BUTTON_EVT_HOLD)
            char_bit(true);
        //Display current char so that the user know what he/she is typing
        uBit.display.printChar(char_get()); 
    }
    else if(med_operation_mode == MODE_TELECOM)
    {
        if (e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            //Broadcast currently inserted
            telecom_send(insert_data(), insert_size());

            uBit.display.print(image_upload);
            uBit.sleep(1000);
            uBit.display.clear();
        }
        else if (e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            //Print insert buffer
            uBit.display.print(image_print);
            uBit.sleep(500);
            uBit.display.clear();
            uBit.display.scroll(insert_data());
            uBit.display.clear();
        }
    }
    else if(med_operation_mode == MODE_INCOMMING)
    {
        if (e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
			uBit.display.scroll(telecom_recieve_buf);
        }
    }
}

void handle_button_B(MicroBitEvent e)
{
    if(!button_wait()) return; //Prevent button event overlap

    if(med_operation_mode == MODE_INSERT)
    {
        if (e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            //Insert Char entered
            uBit.display.scroll("|",35);

            insert_append(char_get());

            char_reset();
            uBit.display.clear();
        }
        else if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            //Backspace: Delete previously enetered character
            insert_delete();

            uBit.display.print(image_delete);
            uBit.sleep(250);
            uBit.display.clear();
        }
    }
    else if(med_operation_mode == MODE_TELECOM)
    {
		if (e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
			//Display Recieved
			uBit.display.print(image_download);
			uBit.sleep(500);
			uBit.display.clear();
			
			uBit.display.scroll(telecom_recieve_buf);
			uBit.display.clear();
		}
        else if (e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            //Delete Insert Buffer
            insert_reset();

            uBit.display.print(image_delete);
            uBit.sleep(500);
            uBit.display.clear();
            uBit.display.print(image_delete);
            uBit.sleep(500);
            uBit.display.clear();
        }
    }
}

void handle_button_AB(MicroBitEvent e)
{
    button_wait_status = false; //Mark possible button colision

    if(med_operation_mode == MODE_INSERT)
    {
        if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            //Switch to Telecom Mode
            uBit.display.clear();
            med_operation_mode = MODE_TELECOM;
            uBit.display.print(image_telecom_mode);
            uBit.sleep(1000);
            uBit.display.clear();
        }
        else if(e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            //Enter: Insert newline symbols.
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
            //Switch to Insert Mode
            uBit.display.clear();
            med_operation_mode = MODE_INSERT;
            uBit.display.print(image_insert_mode);
            uBit.sleep(1000);
            uBit.display.clear();
        }
    }
    
    button_wait_status = true; //Mark possible button colission
}

void handle_gesture(MicroBitEvent e)
{
    if(med_operation_mode == MODE_INSERT)
    {
        if(e.value == MICROBIT_ACCELEROMETER_EVT_SHAKE)
        {
            //Clear
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
    
    //Setup Event Handlers
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_EVT_ANY, handle_button_AB, MESSAGE_BUS_LISTENER_REENTRANT);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, handle_button_A, MESSAGE_BUS_LISTENER_REENTRANT);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, handle_button_B, MESSAGE_BUS_LISTENER_REENTRANT);
    uBit.messageBus.listen(MICROBIT_ID_GESTURE, MICROBIT_EVT_ANY, handle_gesture);

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, handle_telecom);
    uBit.radio.setGroup(MED_TELECOM_GROUP);
    if(uBit.radio.enable() != MICROBIT_OK) uBit.panic();

    char_init();
    insert_init();
    telecom_init();
    
    release_fiber();
}
