#pragma once

struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void GPFault_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void KeyboardInt_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void MouseInt_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void PITInt_Handler(interrupt_frame *frame);

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();