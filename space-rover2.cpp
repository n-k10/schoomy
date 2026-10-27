// mineral_switch_controller v1.6 月面鉱物探査用、スイッチ3つのコントローラー（赤の信号）
// OUT A（左下）だけ=左回転 / OUT B（右下）だけ=右回転 / IN B（右上）だけ=前 / OUT AとOUT Bを両方=後ろ / どれも押さない=止まる
// 【つなぐ場所】赤外線送信=IN A（左上・19） / スイッチ=IN B（右上・15）・OUT A（左下・5）・OUT B（右下・10）
// 【v1.6】「まえ」は、まえ5回→止まる1回をくり返す（v1.5は3回）。ほかはv1.2と同じ
#define USECPERTICK 50
#include <SchooMyUtilities.h>
#include <IRremote.h>
SchooMyUtilities scmUtils = SchooMyUtilities();
IRsend irsend_19(19);   // IN A（左上）

const int SW_FWD   = 15;   // IN B（右上）
const int SW_LEFT  = 5;    // OUT A（左下）
const int SW_RIGHT = 10;   // OUT B（右下）
const int GAP_MS   = 40;   // 送ってから次に送るまでの待ち
const int FWD_IN_STEP = 5; // 「止まる」1回のあいだに送る「まえ」の回数（増やすとなめらか、減らすと止まりやすい）

uint16_t rawTurnLeft[]  = {9000,4400,600,550,650,500,650,550,600,550,600,550,600,550,600,550,600,550,600,1650,600,1650,600,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,550,600,550,600,550,600,550,600,1700,550,550,650,500,650,550,600,1650,600,1650,600,1650,600,1650,550,550,650,1650,600};
uint16_t rawTurnRight[] = {9000,4400,600,550,600,550,600,550,600,550,600,550,600,550,600,550,600,550,600,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,550,600,550,600,1700,550,550,600,550,600,550,600,1700,550,550,600,1700,550,1700,550,550,600,1700,550,1700,550,1700,550,550,600,1700,550};
uint16_t rawBack[]      = {8950,4450,600,550,600,550,600,500,650,500,650,500,600,550,600,550,600,550,600,1700,550,1700,550,1700,550,1650,600,1650,600,1650,600,1650,550,1700,550,1700,550,550,600,1650,600,550,600,1650,600,550,600,550,600,550,600,500,600,1700,550,550,600,1650,600,550,600,1650,600,1650,550,1700,550};
uint16_t rawForward[]   = {8900,4450,600,550,600,550,600,550,600,550,550,600,550,600,550,600,550,600,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1650,550,1700,550,600,550,1700,550,1700,550,550,600,550,600,550,600,1700,550,550,600,1650,550,600,550,600,550,1700,550,1700,550,1700,550,550,600,1650,550};
uint16_t rawStop[]      = {8900,4400,550,600,550,600,550,600,550,600,550,550,600,550,600,550,600,550,600,1650,550,1700,550,1700,550,1650,550,1700,550,1700,550,1700,550,1650,550,600,550,600,550,600,550,550,600,550,600,550,600,1650,550,600,550,1700,550,1650,550,1700,550,1700,550,1650,550,1700,550,550,600,1650,550};
#define LEN(a) (sizeof(a) / sizeof(a[0]))

// 0=止まる 1=前 2=左回転 3=右回転 4=後ろ（押すとLOW）
int decide() {
  bool f = !digitalRead(SW_FWD);
  bool l = !digitalRead(SW_LEFT);
  bool r = !digitalRead(SW_RIGHT);
  if (l && r) return 4;
  if (l) return 2;
  if (r) return 3;
  if (f) return 1;
  return 0;
}

// GAP_MS 待つ。途中で押し方が変わったら false を返す
bool waitGap(int cur) {
  unsigned long t = millis();
  while (millis() - t < (unsigned long)GAP_MS) {
    if (decide() != cur) return false;
    delay(5);
  }
  return true;
}

void setup() {
  Serial.begin(9600);
  pinMode(SW_FWD, INPUT);
  pinMode(SW_LEFT, INPUT);
  pinMode(SW_RIGHT, INPUT);
}

void loop() {
  int cur = decide();

  if (cur == 1) {
    // まえ FWD_IN_STEP 回 → 止まる1回
    for (int i = 0; i < FWD_IN_STEP; i++) {
      irsend_19.sendRaw(rawForward, LEN(rawForward), 38);
      if (!waitGap(cur)) return;
    }
    irsend_19.sendRaw(rawStop, LEN(rawStop), 38);
    waitGap(cur);
    return;
  }

  if      (cur == 2) irsend_19.sendRaw(rawTurnLeft,  LEN(rawTurnLeft),  38);
  else if (cur == 3) irsend_19.sendRaw(rawTurnRight, LEN(rawTurnRight), 38);
  else if (cur == 4) irsend_19.sendRaw(rawBack,      LEN(rawBack),      38);
  else               irsend_19.sendRaw(rawStop,      LEN(rawStop),      38);
  waitGap(cur);
}
