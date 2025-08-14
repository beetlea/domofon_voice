#define WAIT_TIME_NUMBER   2000
#define MIN_TIME_RESET     200
#define MAX_TIME_RESET     2000

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