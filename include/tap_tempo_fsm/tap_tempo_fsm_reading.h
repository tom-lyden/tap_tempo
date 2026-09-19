//
// Created by tomly on 16/09/2026.
//

#ifndef TAP_TEMPO_FSM_READING_H
#define TAP_TEMPO_FSM_READING_H

void Reading_Enter(void* ctx);
void Reading_Update(void* ctx);
void Reading_Exit(void* ctx);

void Reading_ButtonPress(void* context);
void Reading_ButtonRelease(void* context);

#endif // TAP_TEMPO_FSM_READING_H
