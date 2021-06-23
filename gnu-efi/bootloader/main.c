#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

typedef struct
{
	void *BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanline;
} FrameBuffer;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct
{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct
{
	PSF1_HEADER *psf1_header;
	void *glyphBuffer;
} PSF1_FONT;

FrameBuffer frameBuffer;
FrameBuffer* InitializeGOP()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
	if (EFI_ERROR(status))
	{
		return NULL;
	}

	frameBuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	frameBuffer.BufferSize = gop->Mode->FrameBufferSize;
	frameBuffer.Width = gop->Mode->Info->HorizontalResolution;
	frameBuffer.Height = gop->Mode->Info->VerticalResolution;
	frameBuffer.PixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;

	return &frameBuffer;
}

EFI_FILE *LoadFile(EFI_FILE *directory, CHAR16 *path, EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
	EFI_FILE* loaded_file;

	EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
	system_table->BootServices->HandleProtocol(image_handle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	system_table->BootServices->HandleProtocol(loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&file_system);

	if (directory == NULL)
	{
		file_system->OpenVolume(file_system, &directory);
	}

	EFI_STATUS s = directory->Open(directory, &loaded_file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS)
	{
		return NULL;
	}

	return loaded_file;
}

PSF1_FONT* LoadPSF1Font(EFI_FILE *directory, CHAR16 *path, EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
	EFI_FILE* font = LoadFile(directory, path, image_handle, system_table);
	if (font == NULL) return NULL;

	PSF1_HEADER* fontHeader;
	system_table->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);

	if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1){
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if (fontHeader->mode == 1) { //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		system_table->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF1_FONT* finishedFont;
	system_table->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);
	finishedFont->psf1_header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont;
}

int memcmp(const void *aptr, const void *bptr, size_t n)
{
	const unsigned char *a = aptr, *b = bptr;

	for (size_t i = 0; i < n; i++)
	{
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}

	return 0;
}

typedef struct
{
	FrameBuffer *frameBuffer;
	PSF1_FONT *psf1Font;
	EFI_MEMORY_DESCRIPTOR *mMap;
	UINTN mMapSize;
	UINTN mMapDescriptorSize;
	void *rsdp;
} BootInfo;

UINTN strcmp(CHAR8 *a, CHAR8 *b, UINTN length)
{
	for (UINTN i = 0; i < length; i++)
	{
		if (*a != *b) return 0;
	}
	return 1;
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable);
	Print(L"Bootloader started\n\n\r");

	EFI_FILE* Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable); 
	if (Kernel == NULL)
	{
		Print(L"Could not find kernel\n\r");
		return EFI_LOAD_ERROR;
	}

	Print(L"Kernel found\n\r");

	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		Kernel->Read(Kernel, &size, &header);
	}

	if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
			header.e_ident[EI_CLASS] != ELFCLASS64 ||
			header.e_ident[EI_DATA] != ELFDATA2LSB ||
			header.e_type != ET_EXEC ||
			header.e_machine != EM_X86_64 ||
			header.e_version != EV_CURRENT)
	{
		Print(L"Bad kernel format\n\r");
		return EFI_COMPROMISED_DATA;
	}
	else
	{
		Print(L"Kernel header verified\n\r");
	}

	Elf64_Phdr *phdrs;
	{
		Kernel->SetPosition(Kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		Kernel->Read(Kernel, &size, phdrs);
	}

	for (Elf64_Phdr *phdr = phdrs; (char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize))
	{
		switch (phdr->p_type)
		{
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				Kernel->SetPosition(Kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				Kernel->Read(Kernel, &size, (void*)segment);
				break;
			}
		}
	}

	Print(L"Kernel loaded\n\r");

	PSF1_FONT *newFont = LoadPSF1Font(NULL, L"\\STATIC_SOURCES\\FONTS\\zap-light16.psf", ImageHandle, SystemTable);
	if (newFont == NULL)
	{
		Print(L"Font is not valid or not found\n\r");
		return EFI_LOAD_ERROR;
	}

	Print(L"Font found. Char size = %d\n\r", newFont->psf1_header->charsize);

	FrameBuffer *newBuffer = InitializeGOP();
	if (newBuffer == NULL)
	{
		Print(L"Failed to create new GOP");
		return EFI_LOAD_ERROR;
	}

	Print(L"GOP located\n\r");
	Print(L"\n\rGOP Stats:\n\rBase: 0x%x\n\rSize: 0x%x\n\rWidth: %d\n\rHeight: %d\n\rPPS: %d\n\n\r", newBuffer->BaseAddress, newBuffer->BufferSize, newBuffer->Width, newBuffer->Height, newBuffer->PixelsPerScanline);

	EFI_MEMORY_DESCRIPTOR *Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	{
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

		if (Map == NULL || MapSize == 0 || DescriptorSize == 0 || \
				DescriptorSize > MapSize)
		{
			Print(L"Failed to load memory");
			return EFI_LOAD_ERROR;
		}
	}

	Print(L"Memory map loaded\n\r");

	EFI_CONFIGURATION_TABLE *configTable = SystemTable->ConfigurationTable;
	void *rsdp = NULL;
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;

	for (UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++)
	{
		if (CompareGuid(&configTable[index].VendorGuid, &Acpi2TableGuid))
		{
			if (strcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable, 8))
			{
				rsdp = (void*)configTable->VendorTable;
			}
		}

		configTable++;
	}

	if (rsdp == NULL)
	{
		Print(L"RSDP not found\n\r");
	}
	else
	{
		Print(L"RSDP found\n\r");
	}

	void (*KernelStart)(BootInfo*) = ((__attribute__((sysv_abi)) void(*)(BootInfo*)) header.e_entry);

	BootInfo bootInfo;
	bootInfo.frameBuffer = newBuffer;
	bootInfo.psf1Font = newFont;
	bootInfo.mMap = Map;
	bootInfo.mMapSize = MapSize;
	bootInfo.mMapDescriptorSize = DescriptorSize;
	bootInfo.rsdp = rsdp;

	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	Print(L"Exiting boot service\n\r");
	
	KernelStart(&bootInfo);

	return EFI_SUCCESS; // Exit the UEFI application
}
