// Get a token to post a message using OAuth 
// https://arduino-tweet.appspot.com/
 #define TOKEN "<insert your token here>" 

// Twitter Proxy
#define LIB_DOMAIN "arduino-tweet.appspot.com"

// Twitter client
TCPClient client;

int bar0Map[10] = {0};
int bar1Map[10] = {0};
int bar2Map[10] = {0};
int bar3Map[10] = {0};
int bar4Map[10] = {0};

// The bars going across the elastic band
static int bar0 = D4;
static int bar1 = D1;
static int bar2 = D2;
static int bar3 = D3;
static int bar4 = D0;
static int NumberOfBars = 5;
static int barPins[5] = {bar0, bar1, bar2, bar3, bar4};

// The led lamp
static int red = D6;
static int blue = D7;
static int green = D5;
static int NumberOfLeds = 3;
static int ledPins[3] = {red, green, blue};

// ss0 - equals the first sense input where to keys are connected
static int ss0  = A0;
static int ss1  = A1;
static int ss2  = A2;
static int ss3  = A3;
static int ss4  = A4;
static int ss5  = A5;
static int ss6  = DAC;
static int ss7  = WKP;
static int ss8  = RX;
static int ss9 = TX;
static int sensePins[10] = {ss0, ss1, ss2, ss3, ss4, ss5, ss6, ss7, ss8, ss9};
static int NumberOfSense = 10;

// string used to output over wifi to computer
char message[140];

// funny keys
// Rick roll
char RICK_ROLL_URL[] = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";
char RICK_ROLL_KEY[] = "rickroll";

// boolean used for keypress
bool keyPressed = false;
bool shiftPressed = false;

// char within bars
char* bar0keys[] = {"w", "a", "<shift>", "1", "#", "q", "s", "z", "2", " "};
char* bar0Shiftkeys[] = {"W", "A", "<shift>", "§", "#", "Q", "S", "Z", "'", " "};
char* bar1keys[] = {"5", "c", "e", "d", "3", "4", "v", "f", "r", "x"};
char* bar1Shiftkeys[] = {"%", "C", "E", "D", "£", "$", "V", "F", "R", "X"};
char* bar2keys[] = {"u", "h", "b", "y", "t", "g", "j", "n", "7", "6"};
char* bar2Shiftkeys[] = {"U", "H", "B", "Y", "T", "G", "J", "N", "/", "&"};
char* bar3keys[] = {"0", ",", "i", "k", "8", "9", ".", "l", "o", "m"};
char* bar3Shiftkeys[] = {"=", ";", "I", "K", "(", ")", ":", "L", "O", "M"};
char* bar4keys[] = {"<backspace>", "æ", "+", "-", "p", "å", "<tweet>", "<shift>", "¨", "ø"};
char* bar4Shiftkeys[] = {"<backspace>", "Æ", "?", "_", "P", "Å", "<tweet>", "<shift>", "´", "Ø"};

// Setup method
void setup() {
    for (int i = 0; i < NumberOfSense; i++) {
        pinMode(sensePins[i], INPUT_PULLUP);
    }
    for (int i = 0; i < NumberOfLeds; i++) {
        pinMode(ledPins[i], OUTPUT);
    }
    updateLed(0, 1, 0);
    Spark.variable("message", message, STRING);
}

// function for changing the color of the led lamp
void updateLed(int r, int g, int b) {
    digitalWrite(red, r);
    digitalWrite(green, g);
    digitalWrite(blue, b);
}

// function for checking which key is pressed for given bar
// returns array of touching pins
void getSenseIndexActiveOnBar(int* barmap, int barNumberToCheck) {
    // Loop over all the bars pins and set all but the one to check as pull ups.
    // The one to check is set as OUTPUT, GND
    for (int i = 0; i < NumberOfBars; i++) {
        if (i == barNumberToCheck) { 
            pinMode(barPins[i], OUTPUT);
        }
        else {
            pinMode(barPins[i], INPUT_PULLUP); 
        }
    }
    // Wait for output and pullups to become active
    delay(5);

    // Now, check all sense on that bar and return if one is pressed
    for (int i = 0; i < NumberOfSense; i++) {
        // If key is touching the bar, ie. pressed it will be zero
        // If key is not pressed, it will be pulled high by the pull up on the sense pins
        if (digitalRead(sensePins[i])) {
            barmap[i] = 0;    
        } else {
            barmap[i] = 1; 
        }
    }

    return;
}

void appendChar(String c) {
    if (strlen(message) < 140) {
        strcat(message, c.c_str());
    }
    if (strlen(message) == 140) {
        updateLed(1, 0, 0);
    } else if (strlen(message) > 100) {
        updateLed(1, 1, 0);
    }
}

void deleteLastChar() {
    if (strlen(message) > 0) {
        message[strlen(message)-1] = 0;
    }
}

bool noKeyPressed() {
    if (keyPressedIn0Map(bar0Map)) {
        return false;
    } 
    if (keyPressedInMap(bar1Map)) {
        return false;
    } 
    if (keyPressedInMap(bar2Map)) {
        return false;
    } 
    if (keyPressedInMap(bar3Map)) {
        return false;
    } 
    if (keyPressedIn4Map(bar4Map)) {
        return false;
    }
    return true;
}

bool keyPressedInMap(int* map) {
    for (int i = 0; i < 10; i++) {
        if (map[i] == 1) {
            return true;
        }
    }
    return false;
}

bool keyPressedIn0Map(int* map) {
    for (int i = 0; i < 10; i++) {
        if (i == 2) {
            continue;
        }
        if (map[i] == 1) {
            return true;
        }
    }
    return false;
}

bool keyPressedIn4Map(int* map) {
    for (int i = 0; i < 10; i++) {
        if (i == 7) {
            continue;
        }
        if (map[i] == 1) {
            return true;
        }
    }
    return false;
}

bool spaceIsPressed(int* map) {
    return map[9] == 1;
}

bool backspaceIsPressed(int* map) {
    return map[0] == 1;
}

bool tweetIsPressed(int* map) {
    return map[6] == 1;
}

bool shiftIsPressed(int* map0, int* map4) {
    if(map0[2] == 1) {
        return true;
    } else if(map4[7] == 1) {
        return true;
    }
    return false;
}

int getIndexFromBar(int* map) {
    for (int i = 0; i < 10; i++) {
        if (map[i] == 1) {
            return i;
        }
    }
    return -1;
}

int getIndexFromBar0(int* map) {
    for (int i = 0; i < 10; i++) {
        if (i == 2) {
            continue;
        }
        if (map[i] == 1) {
            return i;
        }
    }
    return -1;
}

int getIndexFromBar4(int* map) {
    for (int i = 0; i < 10; i++) {
        if (i == 7) {
            continue;
        }
        if (map[i] == 1) {
            return i;
        }
    }
    return -1;
}

void checkChar() {
    if (keyPressed) {
        return;
    }
    
    // check for space
    if (spaceIsPressed(bar0Map)) {
        int index0 = getIndexFromBar0(bar0Map);
        char* pressedChar = bar0keys[index0];
        appendChar(pressedChar);
        keyPressed = true;
        return;
    }
    
    // check for backspace
    if (backspaceIsPressed(bar4Map)) {
        deleteLastChar();
        keyPressed = true;
        return;
    }
    
    // check for twitter-key
    if (tweetIsPressed(bar4Map)) {
        postTwitterMsg();
        keyPressed = true;
        return;
    }
    
    // check for shift-key
    if (shiftIsPressed(bar0Map, bar4Map)) {
        shiftPressed = true;
    } else {
        shiftPressed = false;
    }
    
    if (keyPressedIn0Map(bar0Map)) {
        int index = getIndexFromBar0(bar0Map);
        if (shiftPressed) {
            char* pressedChar = bar0Shiftkeys[index];
            appendChar(pressedChar);
        } else {
            char* pressedChar = bar0keys[index];
            appendChar(pressedChar);
        }
        keyPressed = true;
        return;
    }
    if (keyPressedInMap(bar1Map)) {
        int index = getIndexFromBar(bar1Map);
        if (shiftPressed) {
            char* pressedChar = bar1Shiftkeys[index];
            appendChar(pressedChar);
        } else {
            char* pressedChar = bar1keys[index];
            appendChar(pressedChar);
        }
        keyPressed = true;
        return;
    }
    if (keyPressedInMap(bar2Map)) {
        int index = getIndexFromBar(bar2Map);
        if (shiftPressed) {
            char* pressedChar = bar2Shiftkeys[index];
            appendChar(pressedChar);
        } else {
            char* pressedChar = bar2keys[index];
            appendChar(pressedChar);
        }
        keyPressed = true;
        return;
    }
    if (keyPressedInMap(bar3Map)) {
        int index = getIndexFromBar(bar3Map);
        if (shiftPressed) {
            char* pressedChar = bar3Shiftkeys[index];
            appendChar(pressedChar);
        } else {
            char* pressedChar = bar3keys[index];
            appendChar(pressedChar);
        }
        keyPressed = true;
        return;
    }
    if (keyPressedIn4Map(bar4Map)) {
        int index = getIndexFromBar4(bar4Map);
        if (shiftPressed) {
            char* pressedChar = bar4Shiftkeys[index];
            appendChar(pressedChar);
        } else {
            char* pressedChar = bar4keys[index];
            appendChar(pressedChar);
        }
        keyPressed = true;
        return;
    }
}

void loop() {
    while(1) {
         getSenseIndexActiveOnBar(bar0Map, 0);
         getSenseIndexActiveOnBar(bar1Map, 1);
         getSenseIndexActiveOnBar(bar2Map, 2);
         getSenseIndexActiveOnBar(bar3Map, 3);
         getSenseIndexActiveOnBar(bar4Map, 4);

        if (noKeyPressed()) {
            keyPressed = false;
        } else {
            checkChar();
        }
    
        delay(15);
    }
}

void postTwitterMsg() {
    // if message.size > 0, continue (else do not post to twitter)
    if (strlen(message) == 0) {
        redLedShow();
    } else {
        client.connect(LIB_DOMAIN, 80);
        client.println("POST /update HTTP/1.0");
        client.println("Host: " LIB_DOMAIN);
        client.print("Content-Length: ");
        if (strcmp(message,RICK_ROLL_KEY) == 0) {
            client.println(strlen(RICK_ROLL_URL)+strlen(TOKEN)+14);
            client.println();
            client.print("token=");
            client.print(TOKEN);
            client.print("&status=");
            client.println(RICK_ROLL_URL);
        } else {
            client.println(strlen(message)+strlen(TOKEN)+14);
            client.println();
            client.print("token=");
            client.print(TOKEN);
            client.print("&status=");
            client.println(message);
        }
        ledShow();
        message[0] = (char)0;
    }
    updateLed(0, 1, 0);
}

void ledShow() {
    for (int i = 0; i < 5; i++) {
        delay(250);
        updateLed(1, 0, 0);
        delay(250);
        updateLed(0, 1, 0);
        delay(250);
        updateLed(0, 0, 1);
    }
}

void redLedShow() {
    for (int i = 0; i < 5; i++) {
        delay(250);
        updateLed(1, 0, 0);
        delay(250);
        updateLed(0, 0, 0);
    }
}
