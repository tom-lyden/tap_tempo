//
// Created by tomly on 16/09/2026.
//

#ifndef TAP_TEMPO_FSM_RUNNING_H
#define TAP_TEMPO_FSM_RUNNING_H

void Running_Enter(void* ctx);
void Running_Update(void* ctx);
void Running_Exit(void* ctx);

void Running_ButtonPress(void* context);
void Running_ButtonRelease(void* context);

#endif // TAP_TEMPO_FSM_RUNNING_H
