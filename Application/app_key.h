#ifndef _KEY_H
#define _KEY_H

#ifdef TFN118A
#define jitter_delay 1310
//extern void key_init(void);//������ʼ��
extern void Key_Deal(void);
extern void onKeyEvent(void);
#endif

#endif
