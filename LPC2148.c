#include <LPC214x.h>
#include <stdio.h>
#include <string.h>

// Define macros for LCD control and sensor inputs
#define RS (1 << 10)
#define RW (1 << 12)
#define EN (1 << 13)
#define LCD_DATA (0xFF << 16)
#define HALL_SENSOR_PIN (1 << 2)   // GPIO pin P0.16 for Hall sensor input
#define IR_SENSOR1_PIN (1 << 3)    // GPIO pin P0.3 for IR sensor 1 input
#define IR_SENSOR2_PIN (1 << 7)    // GPIO pin P0.4 for IR sensor 2 input
#define BUZZER_PIN (1 << 15)       // GPIO pin P0.15 for buzzer output
#define WHEEL_CIRCUMFERENCE_CM 23  // Total circumference in cm

// Function prototypes
void delay_ms(unsigned int ms);
void LCD_command(unsigned char a);
void LCD_data(unsigned char a);
void LCD_init(void);
void LCD_display(char *str);
void setDAC(unsigned int value);
void displayADCValue(int value);
void updateDistance(void);
void controlMotorSpeed(void);
void checkIRSensor(void);
void init_uart0(void);
void display_uart(char *str);

// Global variables
unsigned int total_distance_cm = 0;
unsigned int prev_distance_cm = 0;
int adc_result = 0;
int motor_speed = 0;
int ir_sensor1_triggered = 0;
int ir_sensor2_triggered = 0;
char distance_str[16];
int distance_transmitted = 0;  // Flag to track if distance has been transmitted

// Function to delay in milliseconds
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 6000; j++); // Adjust this delay for your system clock frequency
}

// Function to initialize UART0
void init_uart0() {
    PINSEL0 |= (1 << 0) | (1 << 2); // P0.0 and P0.1 for UART0 TXD and RXD
    U0LCR = 0x83; // 8-bit, 1 Stop Bit, DLAB=1
    U0DLM = 0x00;
    U0DLL = 0x61; // Baud rate 9600 with 12 MHz PCLK (assuming 9600 baud rate)
    U0LCR = 0x03; // 8-bit, 1 Stop Bit, DLAB=0
}

// Function to send a string to UART0
void display_uart(char *str) {
    char *ptr = str;
    while (*ptr) {
        while (!(U0LSR & 0x20)); // Check whether transmit buffer is empty
        U0THR = *ptr;
        ptr++;
        delay_ms(100);
    }
}

// Function to send a command to LCD
void LCD_command(unsigned char a) {
    IOPIN0 = (IOPIN0 & ~LCD_DATA) | ((a << 16) & LCD_DATA); // Clear and set data lines
    IOCLR0 = RS; // RS should be 0 for command
    IOCLR0 = RW;
    IOSET0 = EN;
    delay_ms(1);
    IOCLR0 = EN;
    delay_ms(1);
}

// Function to send data to LCD
void LCD_data(unsigned char a) {
    IOPIN0 = (IOPIN0 & ~LCD_DATA) | ((a << 16) & LCD_DATA); // Clear and set data lines
    IOSET0 = RS; // RS=1 for data
    IOCLR0 = RW;
    IOSET0 = EN;
    delay_ms(1);
    IOCLR0 = EN;
    delay_ms(1);
}

// Function to initialize LCD
void LCD_init(void) {
    IODIR0 |= EN | RS | RW | LCD_DATA; // Set control and data pins as output
    delay_ms(20);
    LCD_command(0x38); // Initialize 16x2 LCD in 8-bit mode
    LCD_command(0x0E); // Display on, cursor off, blink off
    LCD_command(0x01); // Clear display
    LCD_command(0x06); // Increment cursor
    LCD_command(0x80); // Set cursor position to line 1, position 0
}

// Function to display a string on LCD
void LCD_display(char *str) {
    while (*str) {
        LCD_data(*str);
        str++;
    }
}

// Function to set DAC value
void setDAC(unsigned int value) {
    DACR = (value & 0x3FF) << 6; // Set DACR value, ensuring it's within 10-bit range and left-aligned
}

// Function to display ADC value on LCD
void displayADCValue(int value) {
    char adc_str[16];
    LCD_command(0xC0); // Set cursor to line 2, position 0
    sprintf(adc_str, "ADC: %d", value);
    LCD_display(adc_str);
}

// Function to update distance based on hall sensor input
void updateDistance(void) {
    int current_sensor_state = (IO0PIN & HALL_SENSOR_PIN) == 0;
    static int last_sensor_state = 0; // Initially set to low state

    // Only update distance if there's a change from not triggered to triggered
    if (current_sensor_state && !last_sensor_state) {
        total_distance_cm += WHEEL_CIRCUMFERENCE_CM; // Increment distance by 23 cm
        LCD_command(0x80); // Set cursor to line 1, position 0
        sprintf(distance_str, "%d", total_distance_cm);
       
LCD_command(0x80);
LCD_display("Dist:");
LCD_command(0x86);
    LCD_display(distance_str);
LCD_command(0x8A);
LCD_display("cm");

        // Check if distance has changed
        if (total_distance_cm != prev_distance_cm) {
            distance_transmitted = 0; // Reset the transmission flag if distance changes
        }
        prev_distance_cm = total_distance_cm; // Update the previous distance
    }

    last_sensor_state = current_sensor_state;
}

// Function to read ADC value and control motor speed via DAC
void controlMotorSpeed() {
    AD0CR |= (1 << 24); // Start Conversion
    while (!(AD0GDR & 0x80000000)); // Wait till DONE
    adc_result = AD0GDR;
    adc_result = (adc_result >> 6) & 0x000003FF; // Extract 10-bit ADC result

    displayADCValue(adc_result); // Display ADC value on LCD

    if (ir_sensor1_triggered) {
        motor_speed = adc_result / 2; // Reduce speed to half for IR sensor 1
        if (!distance_transmitted) {
            display_uart(distance_str);
            distance_transmitted = 1; // Set flag to indicate distance has been transmitted
display_uart("*");
        }
    } else if (ir_sensor2_triggered) {
        motor_speed = adc_result / 4; // Reduce speed to quarter for IR sensor 2
        if (!distance_transmitted) {
            display_uart(distance_str);
            distance_transmitted = 1; // Set flag to indicate distance has been transmitted
display_uart("&");
        }
    } else {
        motor_speed = adc_result; // Set normal speed
    }

    setDAC(motor_speed); // Set DAC value based on ADC result to control motor speed
}

// Function to check IR sensor inputs and update motor speed
void checkIRSensor(void) {
    if ((IO0PIN & IR_SENSOR1_PIN) == 0) { // If IR sensor 1 detects object
        ir_sensor1_triggered = 0;
    } else {
        ir_sensor1_triggered = 1;
    }

    if ((IO0PIN & IR_SENSOR2_PIN) == 0) { // If IR sensor 2 detects object
        ir_sensor2_triggered = 0;
    } else {
        ir_sensor2_triggered = 1;
    }
}

int main() {
    PINSEL1 |= (1 << 19); // Configure P0.25 as DAC output
    PINSEL1 |= (1 << 26); // P0.29 as AD0.2
    AD0CR = 0x00200304; // ADC setup (AD0.2, 10-bit, ADC clock divider 1)
    IODIR0 &= ~(IR_SENSOR1_PIN | IR_SENSOR2_PIN | HALL_SENSOR_PIN); // Set P0.3, P0.4, and P0.16 as inputs for IR sensors and Hall sensor
    IO0CLR |= HALL_SENSOR_PIN;
    LCD_init();
    init_uart0(); // Initialize UART0 for debugging
LCD_command(0x80);
LCD_display("Dist:");
LCD_command(0x86);
    LCD_display("0");
LCD_command(0x8A);
LCD_display("cm");

    while (1) {
        updateDistance(); // Update distance based on hall sensor input
        checkIRSensor(); // Check IR sensor inputs
        controlMotorSpeed(); // Read ADC value and control motor speed
        delay_ms(100); // Add a delay for stable operation
    }

    return 0;
}

