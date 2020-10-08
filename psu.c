#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

static inline void init(void) {
    // unused
    DDRA  = 0x00;
    PORTA = 0xff;
    //      PS_ON  | PWR_OK | IRQ    | RESET
    DDRB  = (1<<0) | (0<<5) | (1<<6) | (1<<7);
    PORTB = (1<<0) | (1<<5) | (0<<6) | (0<<7);
    //      CTL0   | CTL1   | PWR_SW | RST_SW
    DDRD  = (0<<0) | (0<<1) | (0<<5) | (0<<6);
    PORTD = (0<<0) | (0<<1) | (1<<5) | (1<<6);
}

static inline char get_pwr_ok(void) {
    return PINB & (1<<5);
}

static inline char get_pwr_sw(void) {
    return PIND & (1<<5);
}

static inline char get_rst_sw(void) {
    return PIND & (1<<6);
}

static inline void set_ps_on(char val) {
    if (val)
        PORTB |= (1<<0);
    else
        PORTB &= ~(1<<0);
}

static inline void set_irq(char val) {
    if (val)
        PORTB |= (1<<6);
    else
        PORTB &= ~(1<<6);
}

// open drain output
static inline void set_reset(char val) {
    if (val)
        DDRB &= ~(1<<7);
    else
        DDRB |= (1<<7);
}

char booted = 0;
enum pwr_state {S_OFF, S_SW, S_OK};

void main(void) {
    enum pwr_state state = S_OFF;
    char pwr_sw_pre = 0, pwr_sw_post = 0;

    init();

    for (;;) {
        switch (state) {
        case S_OFF:
            set_reset(0);
            set_ps_on(1);
            booted = 0;
            if (!pwr_sw_pre && pwr_sw_post) {
                state = S_SW;
            }
            break;
        case S_SW:
            set_reset(0);
            set_ps_on(0);
            booted = 0;
            if (get_pwr_ok()) {
                state = S_OK;
            }
            break;
        case S_OK:
            set_reset(get_rst_sw());
            set_ps_on(0);
            booted = 1;
            if (!pwr_sw_pre && pwr_sw_post) {
                state = S_OFF;
            }
            break;
        default:
            set_reset(0);
            set_ps_on(1);
            booted = 0;
            state = S_OFF;
            break;
        }
        if (booted) {
            set_irq(0);
            set_irq(1);
            set_irq(0);
        }
	pwr_sw_pre = get_pwr_sw();
        _delay_ms(10);
	pwr_sw_post = get_pwr_sw();
    }
}
