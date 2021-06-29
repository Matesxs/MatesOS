#include "stivale_main.h"

void stivale_enumerate(stivale2_struct *info)
{
  stivale2_tag *tag = (stivale2_tag *)info->tags;

  while (tag != NULL)
  {
    if (tag->identifier == STIVALE2_STRUCT_TAG_TERMINAL_ID)
    {
      stivale2_struct_tag_terminal *t = (stivale2_struct_tag_terminal *)tag;
      stivale2_print = (void (*)(const char *, size_t))t->term_write;
      break;
    }
    tag = (stivale2_tag *)tag->next;
  }

  e9_puts("Stivale2 info passed to the kernel:");
  e9_printf("Bootloader brand:   %s", info->bootloader_brand);
  e9_printf("Bootloader version: %s", info->bootloader_version);

  tag = (struct stivale2_tag *)info->tags;

  while (tag != NULL)
  {
    switch (tag->identifier)
    {
    case STIVALE2_STRUCT_TAG_CMDLINE_ID:
    {
      struct stivale2_struct_tag_cmdline *c = (struct stivale2_struct_tag_cmdline *)tag;
      e9_puts("Commandline tag:");
      e9_printf("\tCmdline: %s", (char *)(c->cmdline));
      break;
    }
    case STIVALE2_STRUCT_TAG_MEMMAP_ID:
    {
      struct stivale2_struct_tag_memmap *m = (struct stivale2_struct_tag_memmap *)tag;
      e9_puts("Memmap tag:");
      e9_printf("\tEntries: %d", m->entries);
      for (size_t i = 0; i < m->entries; i++)
      {
        struct stivale2_mmap_entry me = m->memmap[i];
        e9_printf("\t\t[%x+%x] %x", me.base, me.length, me.type);
      }
      break;
    }
    case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
    {
      struct stivale2_struct_tag_framebuffer *f = (struct stivale2_struct_tag_framebuffer *)tag;
      e9_puts("Framebuffer tag:");
      e9_printf("\tAddress: %x", f->framebuffer_addr);
      e9_printf("\tWidth:   %d", f->framebuffer_width);
      e9_printf("\tHeight:  %d", f->framebuffer_height);
      e9_printf("\tPitch:   %d", f->framebuffer_pitch);
      e9_printf("\tBPP:     %d", f->framebuffer_bpp);
      e9_printf("\tMemory model:    %d", f->memory_model);
      e9_printf("\tRed mask size:   %d", f->red_mask_size);
      e9_printf("\tRed mask size:   %d", f->red_mask_shift);
      e9_printf("\tGreen mask size: %d", f->green_mask_size);
      e9_printf("\tGreen mask size: %d", f->green_mask_shift);
      e9_printf("\tBlue mask size:  %d", f->blue_mask_size);
      e9_printf("\tBlue mask size:  %d", f->blue_mask_shift);
      break;
    }
    case STIVALE2_STRUCT_TAG_EDID_ID:
    {
      struct stivale2_struct_tag_edid *edid = (struct stivale2_struct_tag_edid *)tag;

      e9_printf("EDID information at %x:", edid->edid_information);
      e9_printf("\tSize: %d", edid->edid_size);
      break;
    }
    case STIVALE2_STRUCT_TAG_FB_MTRR_ID:
    {
      e9_puts("Framebuffer WC MTRR tag:");
      e9_puts("\tFramebuffer WC MTRR enabled");
      break;
    }
    case STIVALE2_STRUCT_TAG_TERMINAL_ID:
    {
      struct stivale2_struct_tag_terminal *term = (struct stivale2_struct_tag_terminal *)tag;

      e9_puts("Terminal tag:");
      e9_printf("\tTerminal write entry point at: %x", term->term_write);
      break;
    }
    case STIVALE2_STRUCT_TAG_MODULES_ID:
    {
      struct stivale2_struct_tag_modules *m = (struct stivale2_struct_tag_modules *)tag;
      e9_puts("Modules tag:");
      e9_printf("\tCount: %d", m->module_count);
      for (size_t i = 0; i < m->module_count; i++)
      {
        struct stivale2_module me = m->modules[i];
        e9_printf("\t\t[%x+%x] %s", me.begin, me.end, me.string);
      }
      break;
    }
    case STIVALE2_STRUCT_TAG_RSDP_ID:
    {
      struct stivale2_struct_tag_rsdp *r = (struct stivale2_struct_tag_rsdp *)tag;
      e9_puts("RSDP tag:");
      e9_printf("\tRSDP: %x", r->rsdp);
      break;
    }
    case STIVALE2_STRUCT_TAG_SMBIOS_ID:
    {
      struct stivale2_struct_tag_smbios *r = (struct stivale2_struct_tag_smbios *)tag;
      e9_puts("SMBIOS tag:");
      e9_printf("\tFlags: %x", r->flags);
      e9_printf("\tSMBIOS 32-bit entry point: %x", r->smbios_entry_32);
      e9_printf("\tSMBIOS 64-bit entry point: %x", r->smbios_entry_64);
      break;
    }
    case STIVALE2_STRUCT_TAG_EPOCH_ID:
    {
      struct stivale2_struct_tag_epoch *e = (struct stivale2_struct_tag_epoch *)tag;
      e9_puts("Epoch tag:");
      e9_printf("\tEpoch: %x", e->epoch);
      break;
    }
    case STIVALE2_STRUCT_TAG_FIRMWARE_ID:
    {
      struct stivale2_struct_tag_firmware *f = (struct stivale2_struct_tag_firmware *)tag;
      e9_puts("Firmware tag:");
      e9_printf("\tFlags: %x", f->flags);
      break;
    }
    case STIVALE2_STRUCT_TAG_EFI_SYSTEM_TABLE_ID:
    {
      struct stivale2_struct_tag_efi_system_table *t = (struct stivale2_struct_tag_efi_system_table *)tag;
      e9_printf("EFI system table at: %x", t->system_table);
      break;
    }
    case STIVALE2_STRUCT_TAG_KERNEL_FILE_ID:
    {
      struct stivale2_struct_tag_kernel_file *t = (struct stivale2_struct_tag_kernel_file *)tag;
      e9_printf("Raw kernel file loaded at: %x", t->kernel_file);
      break;
    }
    case STIVALE2_STRUCT_TAG_KERNEL_SLIDE_ID:
    {
      struct stivale2_struct_tag_kernel_slide *t = (struct stivale2_struct_tag_kernel_slide *)tag;
      e9_printf("Kernel slide: %x", t->kernel_slide);
      break;
    }
    case STIVALE2_STRUCT_TAG_SMP_ID:
    {
      struct stivale2_struct_tag_smp *s = (struct stivale2_struct_tag_smp *)tag;
      e9_puts("SMP tag:");
      e9_printf("\tFlags:        %x", s->flags);
      e9_printf("\tBSP LAPIC ID: %d", s->bsp_lapic_id);
      e9_printf("\tCPU Count:    %d", s->cpu_count);
      for (size_t i = 0; i < s->cpu_count; i++)
      {
        struct stivale2_smp_info *in = &s->smp_info[i];
        e9_printf("\t\tProcessor ID:   %d", in->processor_id);
        e9_printf("\t\tLAPIC ID:       %d", in->lapic_id);
        e9_printf("\t\tTarget Stack:   %x", in->target_stack);
        e9_printf("\t\tGOTO Address:   %x", in->goto_address);
        e9_printf("\t\tExtra Argument: %x", in->extra_argument);
      }
      break;
    }
    default:
      e9_printf("BUG: Unidentified tag %x", tag->identifier);
    }

    tag = (struct stivale2_tag *)tag->next;
  }
}