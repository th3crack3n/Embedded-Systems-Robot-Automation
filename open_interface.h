/** 
 * Open Interface API - Provides a set of functions for controlling the Create 
 * 
 * @author Much of this library comes from the "Robotics Primer Workbook" project hosted on SourceForge.Net
 * @date 07/06/2011
 */

#ifndef CREATE_H
#define CREATE_H

#define FOSC 16000000

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define OI_OPCODE_START            128
#define OI_OPCODE_BAUD             129
#define OI_OPCODE_CONTROL          130
#define OI_OPCODE_SAFE             131
#define OI_OPCODE_FULL             132
#define OI_OPCODE_POWER            133
#define OI_OPCODE_SPOT             134
#define OI_OPCODE_CLEAN            135
#define OI_OPCODE_MAX              136
#define OI_OPCODE_DRIVE            137
#define OI_OPCODE_MOTORS           138
#define OI_OPCODE_LEDS             139
#define OI_OPCODE_SONG             140
#define OI_OPCODE_PLAY             141
#define OI_OPCODE_SENSORS          142
#define OI_OPCODE_FORCEDOCK        143

#define OI_OPCODE_PWM_MOTORS       144
#define OI_OPCODE_DRIVE_WHEELS     145
#define OI_OPCODE_DRIVE_PWM        146
#define OI_OPCODE_OUTPUTS          147
#define OI_OPCODE_STREAM           148
#define OI_OPCODE_QUERY_LIST       149
#define OI_OPCODE_DO_STREAM        150
#define OI_OPCODE_SEND_IR_CHAR     151
#define OI_OPCODE_SCRIPT           152
#define OI_OPCODE_PLAY_SCRIPT      153
#define OI_OPCODE_SHOW_SCRIPT      154
#define OI_OPCODE_WAIT_TIME        155
#define OI_OPCODE_WAIT_DISTANCE    156
#define OI_OPCODE_WAIT_ANGLE       157
#define OI_OPCODE_WAIT_EVENT       158

// Contains Packets 7-26
#define OI_SENSOR_PACKET_GROUP0 0
// Contains Packets 7-16
#define OI_SENSOR_PACKET_GROUP1 1
// Contains Packets 17-20
#define OI_SENSOR_PACKET_GROUP2 2
// Contains Packets 21-26
#define OI_SENSOR_PACKET_GROUP3 3
// Contains Packets 27-34
#define OI_SENSOR_PACKET_GROUP4 4
// Contains Packets 35-42
#define OI_SENSOR_PACKET_GROUP5 5
// Contains Packets 7-42
#define OI_SENSOR_PACKET_GROUP6 6

#define OI_LINEAR_SPEED_MAX_MM_S 500
#define OI_RADIUS_MAX_MM 2000
#define OI_AXLE_LENGTH 263
#define OI_HALF_AXLE_LENGTH 131

#define MIN(a,b) ((a < b) ? (a) : (b))
#define MAX(a,b) ((a > b) ? (a) : (b))
#define NORMALIZE(z) atan2(sin(z), cos(z))

#define PIN_0 0x01
#define PIN_1 0x02
#define PIN_2 0x04
#define PIN_3 0x08
#define PIN_4 0x10
#define PIN_5 0x20
#define PIN_6 0x40
#define PIN_7 0x80

/// \brief Create Data
typedef struct
{
  /// Boolean value for the right bumper
  uint8_t bumper_right;

  /// Boolean value for the left bumper
  uint8_t bumper_left;
  
  /// Boolean value for the right wheel
  uint8_t wheeldrop_right;

  /// Boolean value for the left wheel
  uint8_t wheeldrop_left;

  /// Boolean value for the castor
  uint8_t wheeldrop_caster;

  /// Boolean value for the wall detector
  uint8_t wall;

  /// Boolean value for the left cliff detector
  uint8_t cliff_left;

  /// Boolean value for the front left cliff detector
  uint8_t cliff_frontleft;

  /// Boolean value for the front right cliff detector
  uint8_t cliff_frontright;

  /// Boolean value for the right cliff detector
  uint8_t cliff_right;

  /// Boolean value for the wall detector
  uint8_t virtual_wall;

  /// Boolean value of the low side driver 0 overcurrent sensor
  uint8_t overcurrent_ld0;

  /// Boolean value of the low side driver 1 overcurrent sensor
  uint8_t overcurrent_ld1;

  /// Boolean value of the low side driver 2 overcurrent sensor
  uint8_t overcurrent_ld2;

  /// Boolean value of the right wheel overcurrent sensor
  uint8_t overcurrent_driveright;

  /// Boolean value of the left wheel overcurrent sensor
  uint8_t overcurrent_driveleft;

  /// IR opcode detected
  uint8_t remote_opcode;

  /// Boolean value for the play button
  uint8_t button_play;

  /// Boolean value for the advance button
  uint8_t button_advance;

  /// Meters traveled
  int16_t dist_m;

  /// Millimeters traveled
  int16_t dist_mm;

  /// Current angle
  int16_t angle;

  /// Charge state
  /// 0=not charging
  /// 1=reconditioning charging
  /// 2=full charging
  /// 3=trickle charging
  /// 4=waiting
  /// 5=chargin fault condition
  uint8_t charging_state;

  /// Voltage in millivolts
  uint16_t voltage;

  /// Current in milliamps
  uint16_t current;

  /// Battery temperature in degrees Celsius
  uint16_t temperature;
  
  /// Battery charge milliamp-hours
  uint16_t charge;

  /// Battery capacity milliamp-hours
  uint16_t capacity;

} oi_t;

/// \brief Allocated a new oi 
oi_t *oi_alloc(void);

/// \brief Destroy a oi 
void oi_free(oi_t *self);

/// \brief Initialize the Create. This must be called first.
void oi_init(oi_t *self);

/// \brief Update the Create. This will update all the sensor data. This
/// function should be called once every cycle
void oi_update(oi_t *self);

/// \brief Checks if the Create is On or Off.
/// \return 0=off, 1=on
uint8_t oi_is_on(void);

/// \brief Power on the Create.
void oi_power_on(void);

/// \brief Power off the Create
void oi_power_off(void);




/// \brief Set the LEDS on the Create
/// \param play_led 0=off, 1=on
/// \param advance_led 0=off, 1=on
/// \param power_color (0-255), 0=green, 255=red
/// \param power_intensity (0-255) 0=off, 255=full intensity
void oi_set_leds(uint8_t play_led, uint8_t advance_led, uint8_t power_color, 
                  uint8_t power_intensity);

/// \brief Set the speed of the robot.
/// \param linear_speed Linear speed in mm/s.
/// \param angular_speed Angular speed in milli-radians/s.
void oi_set_speed(int16_t linear_speed, int16_t angular_speed);

/// \brief Set direction and speed of the robot's wheels
/// \param linear velocity in mm/s values range from -500 -> 500 of right wheel
/// \param linear velocity in mm/s values range from -500 -> 500 of left wheel
void oi_set_wheels(int16_t right_wheel, int16_t left_wheel);

/// \brief Transmit a byte of data over the serial connection to the Create 
/// \param value 8-bit value to transmit to the Create
void oi_byte_tx(unsigned char value);

/// \brief Receive a byte of data from the Create serial connection. Blocks 
/// until a byte is received.
/// \return 8-bit value returned from the Create
unsigned char oi_byte_rx(void);

/// \brief Load song sequence
/// \param An integer value from 0 - 15 that acts as a label for note sequence
/// \param An integer value from 1 - 16 indicating the number of notes in the sequence
/// \param A pointer to a sequence of notes stored as integer values
/// \param A pointer to a sequence of durations that correspond to the notes
void oi_load_song(unsigned char song_index, unsigned char num_notes, unsigned char *notes, unsigned char *duration);


/// \brief Play song
/// \param An integer value from 0 - 15 that is a previously establish song index
void oi_play_song(unsigned char index);



// Calls in built in demo to send the iRobot to an open home base
// This will cause the iRobot to enter the Passive state
void go_charge(void);

void oi_clear_distance(oi_t *self);

void oi_clear_angle(oi_t *self);

int oi_current_distance(oi_t *self);

int oi_current_angle(oi_t *self);

//Returns bump sensor status
// 0 = no sensors pressed
// 1 = right sensor
// 2 = left sensor
// 3 = both sensors
char oi_bump_status(oi_t *self);

#endif
