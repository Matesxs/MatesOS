#include "utils/kernelUtil.h"
#include "renderer/basic_renderer.h"
#include "memory_management/heap.h"
#include "scheduling/pit/pit.h"
#include "renderer/stat_logger.h"
#include "utils/helpers.h"
#include "library/memory.h"

#include "facp/facp.h"

extern "C" void _start(BootInfo *bootInfo)
{
  InitializeKernel(bootInfo);

  // PIT::SetDivisor(70);
  // PIT::Sleep(5000);

  // FACP::Reboot();

  // char *string1 = new char[50];
  // char *string2 = new char[50];

  // BasicRenderer::g_Renderer.SetCursor(1000, 100);
  // BasicRenderer::g_Renderer.Print(to_hstring((uint64_t)(void*)string1));
  // BasicRenderer::g_Renderer.PutChar('\n');
  // BasicRenderer::g_Renderer.Print(to_hstring((uint64_t)(void*)string2));
  // BasicRenderer::g_Renderer.PutChar('\n');

  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.Print(string1);
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.PutChar('\n');
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.Print(string2);
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.PutChar('\n');

  // memcpy(string1, "Test string 1\0", 14);
  // memcpy(string2, "Test string 2\0", 14);

  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.Print(string1);
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.PutChar('\n');
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.Print(string2);
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.PutChar('\n');

  // memmove(string2, string1, 14);

  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.Print(string1);
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.PutChar('\n');
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.Print(string2);
  // BasicRenderer::g_Renderer.PutChar('"');
  // BasicRenderer::g_Renderer.PutChar('\n');

  // delete [] string1;
  // delete [] string2;

  // PIT::SetDivisor(60);
  // PIT::Sleep(10000);

  // FACP::Reboot();
  // FACP::Shutdown();
  
  // // Allocated some random memory
  // void *mem = memory::malloc(sizeof(uint64_t) * 1000000);
  // ShowOSStats(1100, 250);

  // // Free that memory
  // memory::free(mem);
  // ShowOSStats(1100, 450);

  // // Allocate random memory again
  // mem = memory::malloc(sizeof(uint64_t) * 1000000);
  // ShowOSStats(1100, 650);

  // // And another one
  // void *mem2 = memory::malloc(sizeof(uint32_t) * 200000);
  // ShowOSStats(1100, 850);

  // // Free both
  // memory::free(mem);
  // memory::free(mem2);
  // ShowOSStats(1500, 50);

  halt();
}