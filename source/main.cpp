/*
 * ureMorse
 * Morse Microbit Editor/Keyboard
*/
#include "MicroBit.h"
#include <cstring>
#include <cstdlib>

#define MED_INSERT_CHAR_BUF_LEN 8
#define MED_WORK_BUF_LEN 400

MicroBit uBit;

typedef enum
{
    MODE_INSERT = 1,
    MODE_GLOBAL = 2,
}MODE;

MODE med_operation_mode = MODE_INSERT;

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


MicroBitImage image_transmit_mode("\
        1 1 0 0 0\n\
        1 0 1 1 1\n\
        1 0 1 0 0\n\
        1 0 1 1 0\n\
        1 1 0 0 1\n"); 


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

//Morse Code Translater
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
static char *insert_char_buf = nullptr;
static char *insert_char_pos = nullptr;
bool insert_keyboard_mode = false;


void insert_char_init()
{
    if(!insert_char_buf)
    {
        insert_char_buf = new char[MED_INSERT_CHAR_BUF_LEN];
        insert_char_pos = insert_char_buf;
    }
}

void insert_char_bit(bool state)
{
    insert_char_init();
    if(state) *insert_char_pos = '-';
    else *insert_char_pos = '.';
    
    insert_char_pos++;
}

void insert_char_reset()
{
    insert_char_init();
    memset(insert_char_buf, 0, sizeof(char) * MED_INSERT_CHAR_BUF_LEN);
    insert_char_pos = insert_char_buf;
}

char insert_char_get()
{
    if(!insert_char_buf) return '\0';
    if(insert_char_pos == insert_char_buf) return ' ';
    *insert_char_pos = '\0';
    char result = translate_morse(insert_char_buf);

    return result;
}

static char *work_buf = nullptr;
static char *insert_pos = nullptr;

void work_init()
{
    if(!work_buf) 
    {
        work_buf = new char[MED_WORK_BUF_LEN];
        memset(work_buf, 0, sizeof(char) * MED_WORK_BUF_LEN);
        
        insert_pos = work_buf;
    }

}

void insert_append(char c)
{
    work_init();

    *insert_pos = c;
    insert_pos++;
}

void insert_delete()
{
    if(!work_buf) return;

    if(insert_pos == work_buf) return; //Do nothing if nothing in buf
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
    work_init();

    memset(work_buf, 0, sizeof(char) * MED_WORK_BUF_LEN);
    insert_pos = work_buf;
}

char *insert_data()
{
    if(!work_buf) return nullptr;
    *insert_pos = '\0';
    return work_buf;
}

//Event Handlers
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
            insert_char_bit(false);
        else if (e.value == MICROBIT_BUTTON_EVT_HOLD)
            insert_char_bit(true);
        uBit.display.printChar(insert_char_get());
    }
    else if(med_operation_mode == MODE_GLOBAL)
    {
        if(e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            uBit.display.clear();
            uBit.display.scroll(insert_data());
        }
        else if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            uBit.serial.printf("%s\r\n", insert_data());
            uBit.display.print(image_upload);
            uBit.sleep(1000);
            uBit.display.clear();
        }
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

            insert_append(insert_char_get());
            if(insert_keyboard_mode) uBit.serial.printf("%c",insert_char_get());

            insert_char_reset();
            uBit.display.clear();
        }
        else if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            insert_delete();
            if(insert_keyboard_mode) uBit.serial.printf("\b");

            uBit.display.print(image_delete);
            uBit.sleep(250);
            uBit.display.clear();
        }
    }
    else if(med_operation_mode == MODE_GLOBAL)
    {
        if(e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            insert_reset();
            for(int i = 0; i < 4; i ++)
            {
                uBit.display.print(image_delete);
                uBit.sleep(250);
                uBit.display.clear();
                uBit.sleep(250);
            }
        }
        else if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            uBit.display.clear();
            uBit.display.print(image_download);
            uBit.serial.clearRxBuffer();
            insert_reset();

            size_t len = 0;
            
            while(len < MED_WORK_BUF_LEN)
            {
                int c = uBit.serial.read(ASYNC);
                if(c == MICROBIT_NO_DATA) continue;
                if((char)c == '\04') break; //EOF
                insert_append((char)c);
                len ++;
            }

            uBit.display.scroll("OK");
        }
    }
}

void handle_button_AB(MicroBitEvent e)
{
    button_syn_status = false;

    if(med_operation_mode == MODE_INSERT)
    {
        if (e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            med_operation_mode = MODE_GLOBAL;
            uBit.display.print(image_transmit_mode);
            uBit.sleep(1000);
            uBit.display.clear();
        }
        else if(e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            insert_append('\r'); 
            insert_append('\n'); 
            if(insert_keyboard_mode) uBit.serial.printf("\r\n");

            uBit.display.print(image_return);
            uBit.sleep(500);
            uBit.display.clear();
        }
    }
    else if(med_operation_mode == MODE_GLOBAL)
    {
        if(e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            med_operation_mode = MODE_INSERT;
            uBit.display.print(image_insert_mode);
            uBit.sleep(1000);
            uBit.display.clear();
        }
        else if(e.value == MICROBIT_BUTTON_EVT_CLICK)
        {
            insert_keyboard_mode = !insert_keyboard_mode;
            uBit.display.print(image_keyboard);
            uBit.sleep(500);
            if(insert_keyboard_mode) uBit.display.scroll("ON");
            else uBit.display.scroll("OFF");
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
            insert_char_reset();
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

    // We don't want to drop out of main!
    while(1)
        uBit.sleep(100);
}
