//// Arduino.h
#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Adafruit_NeoPixel.h>

// 狀態機相關定義
typedef enum {
INIT,
SLEEP_STATE,
WORK_STATE,
NEED_LIGHT_STATE
} State;

typedef struct {
State stateMachine;
State lastState;
} StateManager;

// 函式原型宣告
void updateDisplay();
void debounce(State targetState, int* counter, State* lastResult);

#endif

//// ledstrip.cpp
#include "ledstrip.h"

// 定義變數
StateManager stateManager;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// 函式實作
void updateDisplay() {
// 更新 LED 顯示的程式碼
}

void debounce(State targetState, int* counter, State* lastResult) {
// debounce 機制程式碼
}

void setup() {
// 初始化
}

void loop() {
// 主要迴圈邏輯

// 呼叫 debounce
debounce(targetState, &debounceCount, &lastResult);

// 呼叫 updateDisplay
updateDisplay();
}