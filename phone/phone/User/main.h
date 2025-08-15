#define WAIT_TIME_NUMBER   2000
#define MIN_TIME_RESET     9
#define MAX_TIME_RESET     12
#define PROGRAM_FLAT       255
void CALL_BEGIN();
void RESET_BEGIN();
typedef enum{
    STOP_CALL =0,
    START_CALL=1
} state_call;

typedef enum{
    SLEEP_MODE =0,
    NORMAL_MODE=1
} mode_mcu;