#include "stivale/stivale_main.h"
#include "utils/kernelUtil.h"
#include "renderer/basic_renderer.h"
#include "memory_management/heap.h"
#include "scheduling/pit/pit.h"
#include "renderer/stat_logger.h"
#include "utils/helpers.h"
#include "library/memory.h"

#include "facp/facp.h"

extern "C" void _start(stivale2_struct *bootloader_info)
{
  stivale_enumerate(bootloader_info);

  stivale2_struct_tag_framebuffer *fb = (stivale2_struct_tag_framebuffer *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	stivale2_struct_tag_rsdp *rsdp = (stivale2_struct_tag_rsdp *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);
	stivale2_struct_tag_memmap *memory_map = (stivale2_struct_tag_memmap *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	stivale2_struct_tag_smp *smp = (stivale2_struct_tag_smp *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);
  stivale2_struct_tag_modules *modules = (stivale2_struct_tag_modules *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MODULES_ID);

  stivale2_module fontModule;
  stivale2_get_module(modules, &fontModule, "font");

  FrameBuffer frameBuffer = {
    .BaseAddress=(void*)fb->framebuffer_addr,
    .BufferSize=fb->framebuffer_pitch * fb->framebuffer_height,
    .Width=fb->framebuffer_width, 
    .Height=fb->framebuffer_height
  };

  BootInfo bootInfo = {
    .framebuffer=&frameBuffer,
  };

  e9_printf("Framebuffer address: %x\nFramebuffer resolution: %dx%d\nFramebuffer size: %d",
            bootInfo.framebuffer->BaseAddress,
            bootInfo.framebuffer->Width, bootInfo.framebuffer->Height,
            frameBuffer.BufferSize);

  e9_printf("Bootloader info loaded");

  // InitializeKernel(&bootInfo);

  halt();
}